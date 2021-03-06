#include "common.h"

#include <net/database.h>
#include <operations/common.h>
#include <operations/installer.h>
#include <scrapers/amscraper.h>
#include <scrapers/ghscraper.h>
#include <scrapers/ltscraper.h>

namespace smam
{
auto command::Install(const LoggerPtr&  logger,
                      const OptionsPtr& options) noexcept -> ExitCode
{
    auto exec  = Executor<CommonContext>(logger, options);
    auto error = exec.Run<CheckAddons>()
                     .Run<GoToSMRoot>()
                     .Run<LoadAddons>(".smamdata.json")
                     .GetError();

    if (error)
    {
        logger->Error() << error.message << cr;
        return error.code;
    }

    const auto& ids  = options->Addons();
    const auto& url  = options->DatabaseUrl();
    const auto  meta = db::Fetch(logger, url, ids);

    logger->Debug("Fetched metadata: ", meta);
    if (!meta) return ExitCode::DatabaseFailure;

    static_assert(std::tuple_size<ScraperArray>::value == 3);
    auto scrapers   = std::make_shared<ScraperArray>();
    scrapers->at(0) = std::make_unique<AMScraper>();
    scrapers->at(1) = std::make_unique<LTScraper>();
    scrapers->at(2) = std::make_unique<GHScraper>();

    for (const auto& id : ids)
    {
        error =
            Executor<InstallerContext>(logger, id, meta)
                .Run<CheckPending>()
                .Run<ParseMetadata>()
                .Run<MarkExplicit>()
                .Run<CheckInstalled>(options->Force())
                .Run<BeginTransaction>()
                .Run<InstallDependencies>(scrapers, options->NoDeps())
                .Run<InstallAddon>(scrapers)
                .Run<CommitTransaction>()
                .GetError();

        if (error)
        {
            logger->Error() << error.message << cr;
        }
    }

    error = exec.Run<SaveAddons>(".smamdata.json").GetError();

    if (error)
    {
        logger->Error() << error.message << cr;
        return error.code;
    }

    return ExitCode::OK;
}
}  // namespace smam
