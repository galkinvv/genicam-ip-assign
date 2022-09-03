#include "gtest/gtest.h"
#include <sstream>

#include "Base/GCException.h"
#include "genicam-ip-assign.h"


TEST(DiscoverCameras, HasTLTypeGigEVision) {
    std::ostringstream log;
    DiscoverCameras(1, 1, log);
    
    EXPECT_TRUE(log.str().find("TLType=GigEVision") != std::string::npos) << "Log was: \n" << log.str();
}

TEST(IpAsUint32ForLucidGenTL, HandlesDifferentIps) {
    EXPECT_EQ(IpAsUint32ForLucidGenTL({192,168,0,1}), 0xC0A80001);
    EXPECT_EQ(IpAsUint32ForLucidGenTL({0,0,0,0}), 0);
}
