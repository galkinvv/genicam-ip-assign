## Prepare env & build
Build requires
* docker on the host
* Arena SDK – Ubuntu 18.04, 64-bit from https://thinklucid.com/downloads-hub It must be extracted to parent directory of source - conf file to `../ArenaSDK_Linux_x64/Arena_SDK_Linux_x64.conf`

To build run `./build-in-docker.sh`. The resulting binary is created in `../artifacts/bin/genicam_ip_assign` with required .so libraries in the same folder.

Expected build output (note: /opt/... paths are inside-docker)
```
 % ./build-in-docker.sh
Sending build context to Docker daemon  75.26kB
Step 1/3 : FROM ubuntu:bionic-20220801
 ---> 8d5df41c547b
Step 2/3 : RUN apt-get update && apt-get install -y apt-utils
 ---> Using cache
 ---> 7b5f3aa6c969
Step 3/3 : RUN apt-get install -y build-essential cmake ninja-build
 ---> Using cache
 ---> d9b67ab6555a
Successfully built d9b67ab6555a
Successfully tagged u1804-cpp-dev:latest
-- Setting build type to RelWithDebInfo as none was specified.
-- The C compiler identification is GNU 7.5.0
-- The CXX compiler identification is GNU 7.5.0
-- Check for working C compiler: /usr/bin/cc
-- Check for working C compiler: /usr/bin/cc -- works
-- Detecting C compiler ABI info
-- Detecting C compiler ABI info - done
-- Detecting C compile features
-- Detecting C compile features - done
-- Check for working CXX compiler: /usr/bin/c++
-- Check for working CXX compiler: /usr/bin/c++ -- works
-- Detecting CXX compiler ABI info
-- Detecting CXX compiler ABI info - done
-- Detecting CXX compile features
-- Detecting CXX compile features - done
-- Found GenicamArena: /opt/ArenaSDK_Linux_x64/lib64/libarena.so.0
-- Configuring done
-- Generating done
-- Build files have been written to: /opt/artifacts/build
[2/3] Install the project...
-- Install configuration: "RelWithDebInfo"
-- Installing: /opt/artifacts/bin/genicam_ip_assign
-- Set runtime path of "/opt/artifacts/bin/genicam_ip_assign" to "$ORIGIN"
-- Installing: /opt/artifacts/bin/libGCBase_gcc54_v3_3_LUCID.so
-- Installing: /opt/artifacts/bin/libGenApi_gcc54_v3_3_LUCID.so
-- Installing: /opt/artifacts/bin/libLog_gcc54_v3_3_LUCID.so
-- Installing: /opt/artifacts/bin/libMathParser_gcc54_v3_3_LUCID.so
-- Installing: /opt/artifacts/bin/libNodeMapData_gcc54_v3_3_LUCID.so
-- Installing: /opt/artifacts/bin/libXmlParser_gcc54_v3_3_LUCID.so
-- Installing: /opt/artifacts/bin/liblog4cpp_gcc54_v3_3_LUCID.so
-- Installing: /opt/artifacts/bin/liblucidlog.so.0
-- Installing: /opt/artifacts/bin/liblucidlog.so.0.1.59
-- Installing: /opt/artifacts/bin/libgentl.so.0
-- Installing: /opt/artifacts/bin/libgentl.so.0.1.59
-- Installing: /opt/artifacts/bin/libarena.so.0
-- Installing: /opt/artifacts/bin/libarena.so.0.1.59
```
