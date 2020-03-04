#!/bin/sh -e

# openCV with aruco and gstreamer
sudo apt-get install -y cmake git libgtk2.0-dev pkg-config libavcodec-dev libavformat-dev libswscale-dev python-dev python-numpy libjpeg-dev libpng-dev

sudo apt install libgstreamer1.0-dev libgstreamer-plugins-base1.0-dev

cd
git clone https://github.com/opencv/opencv.git
git clone https://github.com/opencv/opencv_contrib.git

cd opencv
mkdir build
cd build

cmake -D CMAKE_BUILD_TYPE=RELEASE \
      -D OPENCV_GENERATE_PKGCONFIG=ON \
      -D CMAKE_INSTALL_PREFIX=/usr/local \
      -D WITH_V4L=ON \
      -D WITH_GSTREAMER=ON \
      -D WITH_QT=ON \
      -D WITH_OPENGL=ON \
      -D OPENCV_EXTRA_MODULES_PATH=../../opencv_contrib/modules ..

make -j$(nproc --all)
sudo make install

sudo ldconfig

cd ../..
rm -r opencv opencv_contrib
