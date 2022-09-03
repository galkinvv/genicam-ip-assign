#include "genicam-ip-assign.h"
#include <cstdint>

#include <algorithm>
#include <exception>
#include <iostream>
#include <set>
#include <typeinfo>

#include "Arena/ArenaApi.h"
#include "Arena/GenApiCustom.h"
#include "GenApi/INodeMap.h"

const int kDefaultLucidResponceDelayMs = 100;

SystemCameras DiscoverCameras(int expected_device_count, int max_timeout_ms, std::ostream &log) {
    SystemCameras result;
    auto system_deleter = [](Arena::ISystem *asystem) {
        // docs doesn't say if nullptr ISystem* value should be closed, so close it without check according to example
        Arena::CloseSystem(asystem);
    };
    result.system = {Arena::OpenSystem(), system_deleter};
    auto get_node_str = [&](auto key) {
        return Arena::GetNodeValue<GenICam::gcstring>(result.system->GetTLSystemNodeMap(), key);
    };
    log << "Opened Arena::ISystem " << get_node_str("TLID") << " " << get_node_str("TLVersion") << " from "
        << get_node_str("TLPath") << " TLType=" << get_node_str("TLType") << "\n";
    log << result.system->GetInterfaces().size() << " network interfaces discovered\n";
    int first_timeout_ms = kDefaultLucidResponceDelayMs * 2;
    if (first_timeout_ms > max_timeout_ms / 2)
    {
        first_timeout_ms = max_timeout_ms; //use smaller first timeout only if next search timeout would be longer
    }
    result.system->UpdateDevices(first_timeout_ms);
    result.cameras_ordered_by_mac = result.system->GetDevices();
    
    if (first_timeout_ms < max_timeout_ms && result.cameras_ordered_by_mac.size() < expected_device_count)
    {
        log << "Performing longer searching since only " << result.cameras_ordered_by_mac.size() << " devices found out of expected " << expected_device_count << "\n";
        result.system->UpdateDevices(max_timeout_ms - first_timeout_ms);
        result.cameras_ordered_by_mac = result.system->GetDevices();        
    }
    auto cmp_by_mac = [](Arena::DeviceInfo& dinfo1, Arena::DeviceInfo& dinfo2) {
        return dinfo1.MacAddress() < dinfo2.MacAddress();
    };
    std::sort(result.cameras_ordered_by_mac.begin(), result.cameras_ordered_by_mac.end(), cmp_by_mac);
    log << result.cameras_ordered_by_mac.size() << " devices found\n";
    return result;
}

uint32_t IpAsUint32ForLucidGenTL(const Ip4Octets& octets)
{
    return uint32_t(octets[0]) << 24 | uint32_t(octets[1]) << 16 | uint32_t(octets[2]) << 8 | uint32_t(octets[3]);
}
