#!/bin/sh -e

# Install git
sudo apt-get install -y git

# Install QT Creator
sudo apt-get install -y build-essential
sudo apt-get install -y qtcreator
sudo apt-get install -y qt5-default


COMMAND="alias pushall='git add .; git commit -m "Update" ; git push'"

echo ${COMMAND} >> ~/.bashrc
echo "Added: "${COMMAND}"  =to=> ~/.bashrc"

