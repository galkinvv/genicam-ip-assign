#include <iostream>
#include <memory>
#include <vector>
#include <functional>

#include "Arena/ArenaApi.h"

struct DeviceAndNodeMap
{
    //Note that lifetime of the pointers is bound to smart-pointer containing DeviceAndNodeMap,
    //so they should be copied outside of a structure
    Arena::IDevice* device;
    GenApi::INodeMap* nodes;
};

struct SystemCameras
{
    std::shared_ptr<Arena::ISystem> system;
    std::vector<Arena::DeviceInfo> cameras_ordered_by_mac;
};

SystemCameras DiscoverCameras(int expected_device_count, int max_timeout_ms, std::ostream& log);

std::shared_ptr<DeviceAndNodeMap> OpenDevice(std::shared_ptr<Arena::ISystem> &asystem, const Arena::DeviceInfo& info);

void ConfigurePersistentStaticIp(std::shared_ptr<DeviceAndNodeMap>& device_nodes, std::ostream& log);

void RetryNTimesOnException(const std::function<void()>& call_to_try, int times, std::ostream& log);
