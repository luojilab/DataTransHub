#!/bin/bash
rm -rf build/x86_64
rm -rf ../../libs/x86_64
mkdir -p build/x86_64
mkdir -p libs/x86_64
cd build/x86_64
cmake -DDEBUG=OFF -DNDEBUG=1 -DCMAKE_BUILD_TYPE=Release -DCMAKE_TOOLCHAIN_FILE=../../ios.toolchain.cmake -DENABLE_ARC=1 -DIOS_PLATFORM=SIMULATOR64 ../../
make
mv -f DataTransHub.framework ../../libs/x86_64/
