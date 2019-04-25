#!/bin/bash

PREV_DIR=$(pwd)
SCRIPTS_FOLDER="$( cd "$(dirname "$0")" ; pwd -P )"
source "$SCRIPTS_FOLDER/bashrc"
RSYN_FOLDER="$(dirname $SCRIPTS_FOLDER)"
BUILD_FOLDER="$RSYN_FOLDER/build"
NUM_CPUS=$(grep ^cpu\\scores /proc/cpuinfo | uniq | awk '{print $4}')

cd $RSYN_FOLDER
if [[ -d $BUILD_FOLDER ]]; then
	echo "Removing build folder"
	rm -rf $BUILD_FOLDER
fi

if [ $# -eq 0 ]; then
	MKOPTS="-j"$NUM_CPUS
else
	MKOPTS=$1
fi

mkdir $BUILD_FOLDER
cd $BUILD_FOLDER

cmake ..
make $MKOPTS

cd $PREV_DIR
