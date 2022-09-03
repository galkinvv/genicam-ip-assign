This application configures 4 GenICam GigEVision cameras supporting Arena API to fixed IP adresses in range 192.168.1.2-192.168.1.5/24

## Prepare env & build
Build requires
* docker on the host
* Arena SDK – Ubuntu 18.04, 64-bit from https://thinklucid.com/downloads-hub It must be extracted to parent directory of source - conf file to `../ArenaSDK_Linux_x64/Arena_SDK_Linux_x64.conf`

To build run `./build-in-docker.sh`. The resulting binary is created in `../artifacts/bin/genicam_ip_assign` with required .so libraries in the same folder.

Expected build output (note: /opt/... paths are inside-docker)
```
$ ./build-in-docker.sh
Sending build context to Docker daemon  157.7kB
Step 1/4 : FROM ubuntu:bionic-20220801
 ---> 8d5df41c547b
Step 2/4 : RUN apt-get update && apt-get install -y apt-utils
 ---> Using cache
 ---> 7b5f3aa6c969
Step 3/4 : RUN apt-get install -y build-essential cmake ninja-build
 ---> Using cache
 ---> d9b67ab6555a
Step 4/4 : RUN apt-get install -y libgtest-dev
 ---> Using cache
 ---> 49ae0601d5fd
Successfully built 49ae0601d5fd
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
-- Looking for pthread.h
-- Looking for pthread.h - found
-- Looking for pthread_create
-- Looking for pthread_create - not found
-- Looking for pthread_create in pthreads
-- Looking for pthread_create in pthreads - not found
-- Looking for pthread_create in pthread
-- Looking for pthread_create in pthread - found
-- Found Threads: TRUE
-- Configuring done
-- Generating done
-- Build files have been written to: /opt/artifacts/build
[8/9] Install the project...
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
-- Installing: /opt/artifacts/bin/genicam_ip_assign_smoke_test
-- Set runtime path of "/opt/artifacts/bin/genicam_ip_assign_smoke_test" to "$ORIGIN"
Running main() from gtest_main.cc
[==========] Running 8 tests from 4 test cases.
[----------] Global test environment set-up.
[----------] 2 tests from SystemCameras
[ RUN      ] SystemCameras.HasTLTypeGigEVision
[       OK ] SystemCameras.HasTLTypeGigEVision (13 ms)
[ RUN      ] SystemCameras.OpenStubDeviceInfo
[       OK ] SystemCameras.OpenStubDeviceInfo (120 ms)
[----------] 2 tests from SystemCameras (133 ms total)

[----------] 1 test from DeviceNodeMap
[ RUN      ] DeviceNodeMap.CallConfigureForStub
[       OK ] DeviceNodeMap.CallConfigureForStub (120 ms)
[----------] 1 test from DeviceNodeMap (120 ms total)

[----------] 2 tests from Ip4Octets
[ RUN      ] Ip4Octets.IpAsUint32ForLucidGenTL
[       OK ] Ip4Octets.IpAsUint32ForLucidGenTL (0 ms)
[ RUN      ] Ip4Octets.IpAsString
[       OK ] Ip4Octets.IpAsString (0 ms)
[----------] 2 tests from Ip4Octets (0 ms total)

[----------] 3 tests from HandleExceptions
[ RUN      ] HandleExceptions.Retry4Times
[       OK ] HandleExceptions.Retry4Times (120 ms)
[ RUN      ] HandleExceptions.Retry1TimeGenICamException
[       OK ] HandleExceptions.Retry1TimeGenICamException (30 ms)
[ RUN      ] HandleExceptions.NoRetries
[       OK ] HandleExceptions.NoRetries (0 ms)
[----------] 3 tests from HandleExceptions (151 ms total)

[----------] Global test environment tear-down
[==========] 8 tests from 4 test cases ran. (404 ms total)
[  PASSED  ] 8 tests.
```

## Run
The resulting binary can be executed inside any `ld-linux-x86-64.so.2`-environmanet with backward-compatible to ubuntu 18.04 versions of
`libc.so.6`, `libstdc++.so.6`, `libm.so.6`, `libdl.so.2`, `libgcc_s.so.1`, `libpthread.so.0`

Example of successfull execution with real hardware is not available yet.

Execution inside a docker without connected cameras
```
$ docker run --rm -it -v $(pwd)/../artifacts:/opt/GenicamArtifacts u1804-cpp-dev:latest /opt/GenicamArtifacts/bin/genicam_ip_assign
Opened Arena::ISystem LUCID_GenTL_1_5 0.1.59.0 from /opt/GenicamArtifacts/bin/libgentl.so.0 TLType=GigEVision
1 network interfaces discovered
Performing longer searching since only 0 devices found out of expected 4
0 devices found
stage enumeratng cameras throwed std exception of type St13runtime_error: No any cameras discovered
Exiting with error status, no more retries done
```

Execution on a host debian 11 system without connected cameras
```
$ ../artifacts/bin/genicam_ip_assign
Opened Arena::ISystem LUCID_GenTL_1_5 0.1.59.0 from /media/data/j/artifacts/bin/libgentl.so.0 TLType=GigEVision
4 network interfaces discovered
Performing longer searching since only 0 devices found out of expected 4
0 devices found
stage enumeratng cameras throwed std exception of type St13runtime_error: No any cameras discovered
Exiting with error status, no more retries done

```
