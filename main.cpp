#include <exception>
#include <iostream>
#include <stdexcept>
#include <typeinfo>

#include "Base/GCException.h"
#include "genicam-ip-assign.h"

const int kExpectedCamerasCount = 4;
const int kMaxWaitMs = 1000;

int main(int argc, char *argv[]) {
    try {

        auto cameras = DiscoverCameras(kExpectedCamerasCount, kMaxWaitMs, std::cout);
        if (cameras.cameras_ordered_by_mac.size() > kExpectedCamerasCount) {
            throw std::runtime_error("Too much cameras discovered:" +
                                     std::to_string(cameras.cameras_ordered_by_mac.size()));
        }
        if (cameras.cameras_ordered_by_mac.empty()) {
            throw std::runtime_error("No any cameras discovered");
        }
        return 0;
    } catch (const GenICam::GenericException &e) { // doesn't inherit std::exception
        std::cerr << "Unexpected GenICam exception of type " << typeid(e).name() << "   " << e.what() << std::endl;
        return 1;
    } catch (const std::exception &e) {
        std::cerr << "Unexpected std exception of type " << typeid(e).name() << "   " << e.what() << std::endl;
        return 1;
    }
}
