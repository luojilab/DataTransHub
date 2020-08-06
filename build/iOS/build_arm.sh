#!/bin/bash
rm -rf build/arm
rm -rf ../../libs/arm
mkdir -p build/arm
mkdir -p libs/arm
cd build/arm
cmake -DDEBUG=OFF -DNDEBUG=1 -DCMAKE_BUILD_TYPE=Release -DCMAKE_TOOLCHAIN_FILE=../../ios.toolchain.cmake -DENABLE_ARC=1 -DIOS_PLATFORM=OS ../../
make -j4
mv -f DataTransHub.framework ../../libs/arm/
