#include <array>
#include <functional>
#include <iostream>
#include <memory>
#include <vector>

#include "Arena/ArenaApi.h"

typedef std::array<uint8_t, 4> Ip4Octets;

struct DeviceNodeMap {
    std::shared_ptr<GenApi::INodeMap> device_nodes;
    void ConfigureIpAndResetDevice(std::ostream &log, std::string &progress_reported, const Ip4Octets &ip,
                                   const Ip4Octets &mask, const Ip4Octets &gateway);
};

struct SystemCameras {
    SystemCameras(int expected_device_count, int max_timeout_ms, std::ostream &log);

    DeviceNodeMap OpenDevice(const Arena::DeviceInfo &info) const;

    std::shared_ptr<Arena::ISystem> system;
    std::vector<Arena::DeviceInfo> cameras_ordered_by_mac;
};

// Try executing call_to_try with additional extra_tries retries on exception with retry_ms wait between retries
// Returns true if the call eventually succeeded.
// progress_reported_to_log_on_exception is logged on exception to indicate which stage it occured
bool HandleExceptions(const std::function<void()> &call_to_try, int extra_tries, int retry_ms, std::ostream &log,
                      std::string &progress_reported_to_log_on_exception);

uint32_t IpAsUint32ForLucidGenTL(const Ip4Octets &octets);
std::string IpAsString(const Ip4Octets &octets);
