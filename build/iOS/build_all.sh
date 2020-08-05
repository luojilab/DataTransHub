#!/bin/bash
rm -rf build
rm -rf libs/*
./build_i386.sh
./build_x86_64.sh
./build_arm.sh

mkdir -p libs/all/DataTransHub.framework
lipo -create libs/arm/DataTransHub.framework/DataTransHub libs/i386/DataTransHub.framework/DataTransHub libs/x86_64/DataTransHub.framework/DataTransHub -output libs/all/DataTransHub.framework/DataTransHub
cp -rf libs/arm/DataTransHub.framework/Headers libs/all/DataTransHub.framework

