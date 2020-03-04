#!/bin/sh -e

# Install ninja
sudo apt-get install -y ninja-build

# Install boost
sudo apt-get install -y libboost-all-dev

cd

# Install yaml-cpp
git clone https://github.com/jbeder/yaml-cpp.git
cd yaml-cpp
mkdir build
cd build
cmake ..
make -j$(nproc --all)
sudo make install
cd ../..
sudo rm -r -f yaml-cpp

# Libfranka
sudo apt install build-essential cmake git libpoco-dev libeigen3-dev
git clone --recursive https://github.com/frankaemika/libfranka
mkdir build
cd build
cmake -DCMAKE_BUILD_TYPE=Release ..
cmake --build .
sudo make install

