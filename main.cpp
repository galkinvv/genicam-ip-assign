#include <iostream>
#include <stdexcept>

#include "genicam-ip-assign.h"

const int kExpectedCamerasCount = 4;
const int kExtraTriesOnFailure = 4;
const int kWaitMs = 1000;
const Ip4Octets kHostAddr = {192, 168, 1, 1};
const Ip4Octets kSubNetMask = {255, 255, 255, 0};

int main(int argc, char *argv[]) {
    bool all_fine = true;
    std::string camera_iteration_progress = "enumeratng cameras";
    all_fine &= HandleExceptions(
        [&] {
            SystemCameras cameras(kExpectedCamerasCount, kWaitMs, std::cout);
            if (cameras.cameras_ordered_by_mac.size() > kExpectedCamerasCount) {
                throw std::runtime_error("Too much cameras discovered:" +
                                         std::to_string(cameras.cameras_ordered_by_mac.size()));
            }
            if (cameras.cameras_ordered_by_mac.empty()) {
                throw std::runtime_error("No any cameras discovered");
            }
            Ip4Octets device_addr = kHostAddr;
            for (auto &info : cameras.cameras_ordered_by_mac) {
                ++device_addr[3]; // increment last ip part for next device
                camera_iteration_progress = "setting " + IpAsString(device_addr);

                // If more then one device would require retries
                // the overall iteration process may have many waits between retries.
                // In theory this can be speed up by retrying all failed cameras after common wait,
                // but it would comlicate logic for non-common scenario
                std::string progress_reported = "pre open";
                bool set_ok = HandleExceptions(
                    [&] {
                        std::cout << "Configuring device with MAC=" << info.MacAddressStr() << "..." << std::endl;
                        progress_reported = "OpenDevice";
                        auto device_nodes = cameras.OpenDevice(info);
                        std::cout << "Device opened..." << std::endl;
                        device_nodes.ConfigureIpAndResetDevice(std::cout, progress_reported, device_addr, kSubNetMask,
                                                               kHostAddr);
                    },
                    kExtraTriesOnFailure, kWaitMs, std::cout, progress_reported);
                if (!set_ok) {
                    std::cout << "Failed configuring device with MAC=" << info.MacAddressStr() << std::endl;
                    all_fine = false;
                }
            }
        },
        0, 0, std::cerr, camera_iteration_progress);
    if (!all_fine) {
        std::cerr << "Exiting with error status, no more retries done\n";
        return 1;
    }
    std::cerr << "All done, exiting with ok status\n";
    return 0;
}
