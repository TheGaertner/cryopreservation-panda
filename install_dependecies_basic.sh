#!/bin/sh -e

sudo apt-get install -y libeigen3-dev build-essential git libpoco-dev
sudo apt-get install -y gstreamer1.0-plugins-bad

# Install ninja
sudo apt-get install -y ninja-build

# Install boost
sudo apt-get install -y libboost-all-dev

cd

# cmake (3.16.3)
wget http://www.cmake.org/files/v3.16/cmake-3.16.3.tar.gz
tar -xvzf cmake-3.16.3.tar.gz
cd cmake-3.16.3/
./configure
make -j$(nproc --all)
sudo make install
cd ..
sudo rm -r cmake-3.16.3

# yaml-cpp
git clone https://github.com/jbeder/yaml-cpp.git
cd yaml-cpp
mkdir build
cd build
cmake ..
make -j$(nproc --all)
sudo make install
cd ../..
sudo rm -r -f yaml-cpp

# libfranka
git clone --recursive https://github.com/frankaemika/libfranka
cd libfranka
mkdir build
cd build
cmake -DCMAKE_BUILD_TYPE=Release ..
cmake --build .
sudo make install
cd ../..
sudo rm -r libfranka

# http lib
git clone https://github.com/yhirose/cpp-httplib.git
cd cpp-httplib
mkdir build
cd build
cmake -DCMAKE_BUILD_TYPE=Release ..
make -j$(nproc --all)
sudo make install
cd ../..
sudo rm -r cpp-httplib

# nlohman json

git clone https://github.com/nlohmann/json.git
cd json
mkdir build
cd build
cmake -DCMAKE_BUILD_TYPE=Release ..
make -j$(nproc --all)
sudo make install
cd ../..
sudo rm -r json

# json rpc
git clone https://github.com/jsonrpcx/json-rpc-cxx.git
cd json-rpc-cxx
mkdir build
cd build
cmake -DCMAKE_BUILD_TYPE=Release ..
make -j$(nproc --all)
sudo make install
cd ../..
sudo rm -r json-rpc-cxx
