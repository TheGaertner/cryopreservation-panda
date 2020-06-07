#!/bin/sh -e

# Install git
sudo apt-get install -y git

# Add pushall command to ~/.bashrc to easily push everything
COMMAND="alias pushall='git add .; git commit -m "Update" ; git push'"

echo ${COMMAND} >> ~/.bashrc
echo "Added: "${COMMAND}"  =to=> ~/.bashrc"

# Install QT Creator
sudo apt-get install -y build-essential
sudo apt-get install -y qtcreator
sudo apt-get install -y qt5-default

# Install doxygen
sudo apt-get install -y doxygen
