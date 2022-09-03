#include <iostream>
#include <stdexcept>

#include "genicam-ip-assign.h"

const int kExpectedCamerasCount = 4;
const int kWaitMs = 1000;

int main(int argc, char *argv[]) {
    bool all_fine = true;
    all_fine &= HandleExceptions(
        [] {
            auto cameras = DiscoverCameras(kExpectedCamerasCount, kWaitMs, std::cout);
            if (cameras.cameras_ordered_by_mac.size() > kExpectedCamerasCount) {
                throw std::runtime_error("Too much cameras discovered:" +
                                         std::to_string(cameras.cameras_ordered_by_mac.size()));
            }
            if (cameras.cameras_ordered_by_mac.empty()) {
                throw std::runtime_error("No any cameras discovered");
            }
        },
        0, 0, std::cerr);
    if (!all_fine) {
        std::cerr << "Exiting with error ststus, no more retries done\n";
        return 1;
    }
    std::cerr << "All done, exiting with ok status\n";
    return 0;
}
