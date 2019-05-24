#pragma once

#include <functional>
#include <string>
#include <vector>

#include "database.h"
#include "downloader.h"

#include "scrapers/scraper.h"

namespace Installer
{
using Setup      = std::function<bool(const File&)>;
using FileVector = std::vector<File>;

void initScrapers(Downloader&) noexcept;

auto files(const std::string& id, const Database&) noexcept
    -> FileVector;

bool setup(const std::string& id, const Database&, Setup) noexcept;
};  // namespace Installer
