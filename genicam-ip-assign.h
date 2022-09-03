#include <array>
#include <functional>
#include <iostream>
#include <memory>
#include <vector>

#include "Arena/ArenaApi.h"

struct DeviceAndNodeMap {
    // Note that lifetime of the pointers is bound to smart-pointer containing DeviceAndNodeMap,
    // so they should be copied outside of a structure
    Arena::IDevice *device;
    GenApi::INodeMap *nodes;
};

struct SystemCameras {
    std::shared_ptr<Arena::ISystem> system;
    std::vector<Arena::DeviceInfo> cameras_ordered_by_mac;
};

typedef std::array<uint8_t, 4> Ip4Octets;
SystemCameras DiscoverCameras(int expected_device_count, int max_timeout_ms, std::ostream &log);

std::shared_ptr<DeviceAndNodeMap> OpenDevice(std::shared_ptr<Arena::ISystem> &asystem, const Arena::DeviceInfo &info);

void ConfigurePersistentStaticIp(std::shared_ptr<DeviceAndNodeMap> &device_nodes, std::ostream &log,
                                 const Ip4Octets &ip, const Ip4Octets &mask, const Ip4Octets &gateway);

// Try executing call_to_try with additional extra_tries retries on exception with retry_ms wait between retries
// Returns true if the call eventually succeeded.
bool HandleExceptions(const std::function<void()> &call_to_try, int extra_tries, int retry_ms, std::ostream &log);

uint32_t IpAsUint32ForLucidGenTL(const Ip4Octets &octets);
