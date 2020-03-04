cd

# Install ninja
sudo apt-get install ninja-build

# Install boost
sudo apt-get install libboost-all-dev

# Install yaml-cpp
git clone https://github.com/jbeder/yaml-cpp.git
cd yaml-cpp
mkdir build
cd build
cmake ..
sudo make install
cd
rm -r -f yaml-cpp



QT Creator:

sudo apt-get install build-essential
sudo apt-get install qtcreator
sudo apt-get install qt5-default

sudo apt-get install git
git clone git@github.com:TheGaertner/cryopreservation-panda.git


# Libfranka
sudo apt install build-essential cmake git libpoco-dev libeigen3-dev
git clone --recursive https://github.com/frankaemika/libfranka
mkdir build
cd build
cmake -DCMAKE_BUILD_TYPE=Release ..
cmake --build .
sudo make install




git clone https://github.com/opencv/opencv.git
git clone https://github.com/opencv/opencv_contrib.git
