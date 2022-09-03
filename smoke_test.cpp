#include "gtest/gtest.h"
#include <sstream>

#include "Base/GCException.h"
#include "genicam-ip-assign.h"
testing::AssertionResult ExpectSubstringCount(const std::string &all_text, const std::string &substring,
                                              int expected_count) {
    int found_count = 0;
    if (!substring.empty()) {
        std::string::size_type pos = 0;
        for (;;) {
            pos = all_text.find(substring, pos);
            if (pos == std::string::npos)
                break;
            ++found_count;
            ++pos;
        }
    }
    if (found_count == expected_count) {
        return testing::AssertionSuccess()
               << " ok count " << found_count << " for substring '" << substring << "' in the text:\n"
               << all_text;
    }
    return testing::AssertionFailure() << " wrong count " << found_count << " instead of " << expected_count
                                       << " for substring '" << substring << "' in the text:\n"
                                       << all_text;
}

TEST(DiscoverCameras, HasTLTypeGigEVision) {
    std::ostringstream log;
    DiscoverCameras(1, 1, log);

    EXPECT_TRUE(ExpectSubstringCount(log.str(), "TLType=GigEVision", 1));
}

TEST(IpAsUint32ForLucidGenTL, HandlesDifferentIps) {
    EXPECT_EQ(IpAsUint32ForLucidGenTL({192, 168, 0, 1}), 0xC0A80001);
    EXPECT_EQ(IpAsUint32ForLucidGenTL({0, 0, 0, 0}), 0);
    EXPECT_EQ(IpAsUint32ForLucidGenTL({255, 255, 255, 0}), 0xFFFFFF00);
}

TEST(HandleExceptions, Retry4Times) {
    std::ostringstream log;
    int kTries = 4;
    int tries_done = 0;
    bool succeed_try = HandleExceptions(
        [&] {
            ++tries_done;
            throw std::runtime_error("Mock error for retrying");
        },
        kTries, 30, log);
    EXPECT_FALSE(succeed_try);
    EXPECT_EQ(tries_done, kTries + 1);
    EXPECT_TRUE(ExpectSubstringCount(log.str(), "Waiting and retrying...\n", kTries));
}

TEST(HandleExceptions, NoRetries) {
    std::ostringstream log;
    int kTries = 4;
    int tries_done = 0;
    bool succeed_try = HandleExceptions([&] { ++tries_done; }, kTries, 30, log);
    EXPECT_TRUE(succeed_try);
    EXPECT_EQ(tries_done, 1);
    EXPECT_TRUE(ExpectSubstringCount(log.str(), "Waiting and retrying...\n", 0));
}
