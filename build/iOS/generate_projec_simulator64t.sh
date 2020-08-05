#!/bin/bash
mkdir build
cd build
cmake -DCMAKE_TOOLCHAIN_FILE=../ios.toolchain.cmake -DENABLE_ARC=1 -DCMAKE_IOS_DEVELOPER_ROOT=/Applications/Xcode.app/Contents/Developer/Platforms/iPhoneOS.platform/Developer/ -DCMAKE_IOS_SDK_ROOT=/Applications/Xcode.app/Contents/Developer/Platforms/iPhoneOS.platform/Developer/SDKs/iPhoneOS.sdk/ -DIOS_PLATFORM=SIMULATOR64 -G Xcode ../