#u1804-cpp-dev
FROM ubuntu:bionic-20220801
RUN apt-get update && apt-get install -y apt-utils
RUN apt-get install -y build-essential cmake ninja-build
