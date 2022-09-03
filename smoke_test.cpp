#include "gtest/gtest.h"
#include <sstream>
#include <stdexcept>

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

TEST(SystemCameras, HasTLTypeGigEVision) {
    std::ostringstream log;
    SystemCameras cameras(1, 1, log);

    EXPECT_TRUE(ExpectSubstringCount(log.str(), "TLType=GigEVision", 1));
}

TEST(SystemCameras, OpenStubDeviceInfo) {
    std::ostringstream log;
    SystemCameras cameras(0, 110, log);
    log.str(""); // clear log
    std::string stage = "test";

    bool opened = HandleExceptions([&] { cameras.OpenDevice({}); }, 0, 0, log, stage);
    EXPECT_FALSE(opened);
    EXPECT_TRUE(ExpectSubstringCount(log.str(), "Camera Interface not found", 1));
}

TEST(DeviceNodeMap, CallConfigureForStub) {
    std::ostringstream log;
    SystemCameras cameras(0, 110, log);
    log.str(""); // clear log
    std::string stage = "test";
    DeviceNodeMap stub_nodes;
    // use global nodes as stub device nodes
    stub_nodes.device_nodes = std::shared_ptr<GenApi::INodeMap>{cameras.system, cameras.system->GetTLSystemNodeMap()};

    bool configured =
        HandleExceptions([&] { stub_nodes.ConfigureIpAndResetDevice(log, stage, {}, {}, {}); }, 0, 0, log, stage);
    EXPECT_FALSE(configured);
    EXPECT_TRUE(ExpectSubstringCount(log.str(), "stage GevPersistentIPAddress throwed GenICam exception", 1));
}
TEST(Ip4Octets, IpAsUint32ForLucidGenTL) {
    EXPECT_EQ(IpAsUint32ForLucidGenTL({192, 168, 0, 1}), 0xC0A80001);
    EXPECT_EQ(IpAsUint32ForLucidGenTL({0, 0, 0, 0}), 0);
    EXPECT_EQ(IpAsUint32ForLucidGenTL({255, 0, 0, 0}), 0xFF000000);
}

TEST(Ip4Octets, IpAsString) {
    EXPECT_EQ(IpAsString({192, 168, 0, 1}), "192.168.0.1");
    EXPECT_EQ(IpAsString({0, 0, 0, 0}), "0.0.0.0");
    EXPECT_EQ(IpAsString({255, 0, 0, 0}), "255.0.0.0");
}

TEST(HandleExceptions, Retry4Times) {
    std::string stage = "test";
    std::ostringstream log;
    int kTries = 4;
    int tries_done = 0;
    bool succeed_try = HandleExceptions(
        [&] {
            ++tries_done;
            throw std::runtime_error("Mock error for retrying");
        },
        kTries, 30, log, stage);
    EXPECT_FALSE(succeed_try);
    EXPECT_EQ(tries_done, kTries + 1);
    EXPECT_TRUE(ExpectSubstringCount(log.str(), "Waiting and retrying...\n", kTries));
}

TEST(HandleExceptions, Retry1TimeGenICamException) {
    std::string stage = "test";
    std::ostringstream log;
    int kTries = 1;
    int tries_done = 0;
    bool succeed_try = HandleExceptions(
        [&] {
            ++tries_done;
            throw GENERIC_EXCEPTION("Mock GenICam error for retrying");
        },
        kTries, 30, log, stage);
    EXPECT_FALSE(succeed_try);
    EXPECT_EQ(tries_done, kTries + 1);
    EXPECT_TRUE(ExpectSubstringCount(log.str(), "Waiting and retrying...\n", kTries));
}

TEST(HandleExceptions, NoRetries) {
    std::string stage = "test";
    std::ostringstream log;
    int kTries = 4;
    int tries_done = 0;
    bool succeed_try = HandleExceptions([&] { ++tries_done; }, kTries, 30, log, stage);
    EXPECT_TRUE(succeed_try);
    EXPECT_EQ(tries_done, 1);
    EXPECT_TRUE(ExpectSubstringCount(log.str(), "Waiting and retrying...\n", 0));
}
