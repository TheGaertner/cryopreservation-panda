cd

# Install ninja
sudo apt-get install ninja-build

# Install yaml-cpp
git clone https://github.com/jbeder/yaml-cpp.git
cd yaml-cpp
mkdir build
cd build
cmake -G Ninja ..
sudo make install
cd
rm -r -f yaml-cpp


