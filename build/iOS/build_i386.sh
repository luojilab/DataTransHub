#!/bin/bash
rm -rf  build/i386
rm -rf ../../libs/i386
mkdir -p build/i386
mkdir -p libs/i386
cd build/i386
cmake -DDEBUG=OFF -DNDEBUG=1 -DCMAKE_BUILD_TYPE=Release -DCMAKE_TOOLCHAIN_FILE=../../ios.toolchain.cmake -DENABLE_ARC=1 -DIOS_PLATFORM=SIMULATOR ../../
make
mv -f DataTransHub.framework ../../libs/i386/
