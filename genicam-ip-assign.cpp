#include "genicam-ip-assign.h"
#include <cstdint>

#include <algorithm>
#include <chrono>
#include <exception>
#include <iostream>
#include <set>
#include <thread>
#include <typeinfo>

#include "Arena/ArenaApi.h"
#include "Base/GCException.h"
#include "GenApi/INodeMap.h"

const int kDefaultLucidResponceDelayMs = 100;
void system_deleter(Arena::ISystem *asystem) {
    // docs doesn't say if nullptr ISystem* value should be closed, so close it without check according to example
    Arena::CloseSystem(asystem);
};

SystemCameras::SystemCameras(int expected_device_count, int max_timeout_ms, std::ostream &log) {
    system = {Arena::OpenSystem(), system_deleter};
    auto get_node_str = [&](auto key) {
        return Arena::GetNodeValue<GenICam::gcstring>(system->GetTLSystemNodeMap(), key);
    };

    log << "Opened Arena::ISystem " << get_node_str("TLID") << " " << get_node_str("TLVersion") << " from "
        << get_node_str("TLPath") << " TLType=" << get_node_str("TLType") << "\n";
    log << system->GetInterfaces().size() << " network interfaces discovered\n";

    int first_timeout_ms = kDefaultLucidResponceDelayMs * 2;
    if (first_timeout_ms > max_timeout_ms / 2) {
        first_timeout_ms = max_timeout_ms; // use smaller first timeout only if next search timeout would be longer
    }

    system->UpdateDevices(first_timeout_ms);
    cameras_ordered_by_mac = system->GetDevices();

    if (first_timeout_ms < max_timeout_ms && static_cast<int>(cameras_ordered_by_mac.size()) < expected_device_count) {
        log << "Performing longer searching since only " << cameras_ordered_by_mac.size()
            << " devices found out of expected " << expected_device_count << "\n";
        system->UpdateDevices(max_timeout_ms - first_timeout_ms);
        cameras_ordered_by_mac = system->GetDevices();
    }

    auto cmp_by_mac = [](Arena::DeviceInfo &dinfo1, Arena::DeviceInfo &dinfo2) {
        return dinfo1.MacAddress() < dinfo2.MacAddress();
    };
    std::sort(cameras_ordered_by_mac.begin(), cameras_ordered_by_mac.end(), cmp_by_mac);

    log << cameras_ordered_by_mac.size() << " devices found\n";
}

DeviceNodeMap SystemCameras::OpenDevice(const Arena::DeviceInfo &info) const {
    // capture by copy for use system in deleter
    auto device_deleter = [*this](Arena::IDevice *device) {
        if (device != nullptr) {
            system->DestroyDevice(device);
        }
    };
    std::shared_ptr<Arena::IDevice> owned_device = {system->CreateDevice(info), device_deleter};

    // construct to shared_ptr to subobject - INodeMap is a subobject IDevice in lifetime terms
    std::shared_ptr<GenApi::INodeMap> owned_device_nodes{owned_device, owned_device->GetNodeMap()};
    if (owned_device_nodes == nullptr) {
        throw std::runtime_error("GetNodeMap is nullptr");
    }
    return {owned_device_nodes};
}

void DeviceNodeMap::ConfigureIpAndResetDevice(std::ostream &log, std::string &progress_reported, const Ip4Octets &ip,
                                              const Ip4Octets &mask, const Ip4Octets &gateway) {
    progress_reported = "GevPersistentIPAddress";
    Arena::SetNodeValue<int64_t>(device_nodes.get(), "GevPersistentIPAddress", IpAsUint32ForLucidGenTL(ip));

    progress_reported = "GevPersistentSubnetMask";
    Arena::SetNodeValue<int64_t>(device_nodes.get(), "GevPersistentSubnetMask", IpAsUint32ForLucidGenTL(mask));

    progress_reported = "GevPersistentDefaultGateway";
    Arena::SetNodeValue<int64_t>(device_nodes.get(), "GevPersistentDefaultGateway", IpAsUint32ForLucidGenTL(gateway));

    progress_reported = "GevCurrentIPConfigurationPersistentIP";
    Arena::SetNodeValue<bool>(device_nodes.get(), "GevCurrentIPConfigurationPersistentIP", true);

    progress_reported = "GevCurrentIPConfigurationDHCP";
    Arena::SetNodeValue<bool>(device_nodes.get(), "GevCurrentIPConfigurationDHCP", false);

    progress_reported = "DeviceResetViaSetNode";
    auto device_reset_node = device_nodes->GetNode("DeviceReset");

    // DeviceReset node type is not yet known for specific used device. So handle both Command and value variants
    if (device_reset_node && device_reset_node->GetPrincipalInterfaceType() == GenApi::intfICommand) {
        progress_reported = "DeviceResetViaCommand";
        Arena::ExecuteNode(device_nodes.get(), "DeviceReset");
    } else {
        Arena::SetNodeValue<bool>(device_nodes.get(), "DeviceReset", true);
    }
    std::cout << "Device configured to ip=" << IpAsString(ip) << " mask=" << IpAsString(mask)
              << " gateway=" << IpAsString(gateway) << std::endl;
}

bool HandleExceptions(const std::function<void()> &call_to_try, int extra_tries, int retry_ms, std::ostream &log,
                      std::string &progress_reported_to_log_on_exception) {
    for (int try_num = 0; try_num <= extra_tries; ++try_num) {
        if (try_num > 0) {
            log << "Waiting and retrying...\n";
            std::this_thread::sleep_for(std::chrono::milliseconds(retry_ms));
        }
        try {
            call_to_try();
            return true;
        } catch (const GenICam::GenericException &e) { // GenICam base exception doesn't inherit std::exception
            log << "stage " << progress_reported_to_log_on_exception << " throwed GenICam exception of type "
                << typeid(e).name() << ": " << e.what() << std::endl;
        } catch (const std::exception &e) {
            log << "stage " << progress_reported_to_log_on_exception << " throwed std exception of type "
                << typeid(e).name() << ": " << e.what() << std::endl;
        } catch (...) {
            log << "Unexpected exception with unknown base type, rethrowing " << std::endl;
            throw;
        }
    }
    return false;
}

uint32_t IpAsUint32ForLucidGenTL(const Ip4Octets &octets) {
    return uint32_t(octets[0]) << 24 | uint32_t(octets[1]) << 16 | uint32_t(octets[2]) << 8 | uint32_t(octets[3]);
}

std::string IpAsString(const Ip4Octets &octets) {
    return std::to_string(octets[0]) + "." + std::to_string(octets[1]) + "." + std::to_string(octets[2]) + "." +
           std::to_string(octets[3]);
}
