#include <gtest/gtest.h>

#include "../../src/scrapers/scraper.h"

inline void compare(const Scraper::Data& expected,
                    const Scraper::Data& actual) noexcept
{
    ASSERT_EQ(expected.size(), actual.size());

    for (const auto& [name, url] : actual)
    {
        auto entry = expected.find(name);
        ASSERT_NE(expected.end(), entry) << "Could not find " << name;
        EXPECT_EQ(entry->second, url);
    }

    EXPECT_EQ(expected.website, actual.website);
}
