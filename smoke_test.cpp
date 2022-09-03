#include "gtest/gtest.h"
#include <sstream>

#include "Base/GCException.h"
#include "genicam-ip-assign.h"

TEST(DiscoverCameras, HasTLTypeGigEVision) {
    std::ostringstream log;
    DiscoverCameras(1, 1, log);
    
    EXPECT_TRUE(log.str().find("TLType=GigEVision") != std::string::npos) << "Log was: \n" << log.str();
}
