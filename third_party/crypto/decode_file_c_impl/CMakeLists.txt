cmake_minimum_required (VERSION 2.8)
project (decode_file)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O0 -ggdb")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -lz")

get_filename_component(CMAKE_SOURCE_DIR
         "../third_party/"
         ABSOLUTE)

include_directories(${CMAKE_SOURCE_DIR})
add_executable(decode_file decode_file.c ../third_party/micro-ecc-master/uECC.c)