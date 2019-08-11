#include "common.h"

#include <smfs/addon.h>
#include <utils/report.h>

class Remover final
{
    using Type         = Report::Type;
    using StringVector = std::vector<std::string>;

    const StringVector& ids;

    bool forceRemove;
    bool noDeps;

    Report report;

public:
    Remover(const StringVector& addons, bool forceRemove,
            bool noDeps = false) noexcept;

    /*
     * Remove every addon in the `addons` vector.
     */
    auto removeAll() noexcept -> Report;

private:
    /*
     * Remove a single specific addon that has been precached by the
     * database.
     */
    auto removeSingle(const std::string& addon) noexcept
        -> Report::Type;
};

auto Command::remove(const Opts& opts) noexcept -> ExitCode
{
    auto addons = opts.getAddons();

    if (Common::noAddons(addons)) return ExitCode::NoAddons;
    if (Common::noSMRoot(opts)) return ExitCode::NoSMRoot;
    if (auto ret = Common::load(); ret) return ret;

    auto remover = Remover(addons, opts.force(), opts.noDeps());

    const auto& report = remover.removeAll();

    if (!Common::save()) return ExitCode::WriteError;

    out(Ch::Info) << "Removal complete." << cr;

    report.print();

    return ExitCode::OK;
}

Remover::Remover(const StringVector& ids, bool forceRemove,
                 bool noDeps) noexcept
    : ids(ids), forceRemove(forceRemove), noDeps(noDeps)
{
}

auto Remover::removeAll() noexcept -> Report
{
    for (const auto& id : ids)
    {
        if (auto addon = Addon::get(id))  // addon installed
        {
            removeSingle(id);
            report.insert(Type::Removed, id);
        }
        else
        {
            out(Ch::Warn) << "Addon not installed: " << id << cr;
            report.insert(Type::Skipped, id);
        }

        out << cr;
    }

    return report;
}

auto Remover::removeSingle(const std::string& id) noexcept -> Type
{
    assert(Addon::get(id).has_value());  // checked before call

    auto addon = Addon::get(id).value();

    out(Ch::Info) << Col::yellow << "Removing " << addon->getId()
                  << "..." << Col::reset << cr;

    addon->remove([](const auto& file, const auto& error) {
        out();  // create newline
        if (!error.empty()) out << "Skipping " << error << " file: ";
        out << file->raw() << cr;
    });

    addon->forEachDep([&](const auto& depId) {
        if (addon->findByDep(depId).size() > 0) return true;

        if (auto dep = Addon::get(depId))
        {
            if (!dep.value()->isExplicit())
            {
                if (noDeps)
                {
                    report.remark(id, depId, Type::Dangling);
                    return true;
                }

                out << cr;
                report.remark(id, depId, removeSingle(depId));
            }
        }

        return true;
    });

    return Type::Removed;
}
