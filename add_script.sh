#!/bin/sh -e
PATH=`pwd`



COMMAND="alias deploy='"${PATH}"/deploy.sh""'"

echo ${COMMAND} >> ~/.bashrc
echo "Added: "${COMMAND}"  =to=> ~/.bashrc"

