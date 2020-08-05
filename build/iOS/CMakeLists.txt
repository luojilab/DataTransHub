# For more information about using CMake with Android Studio, read the
# documentation: https://d.android.com/studio/projects/add-native-code.html

# Sets the minimum version of CMake required to build the native library.

cmake_minimum_required(VERSION 3.4.1)

# Creates and names a library, sets it as either STATIC
# or SHARED, and provides the relative paths to its source code.
# You can define multiple libraries, and CMake builds them for you.
# Gradle automatically packages shared libraries with your APK.
#

project(DataTransHub)

#if (CMAKE_BUILD_TYPE MATCHES "Debug")
#option(MEMORY_CHECK         "Set to switch to build use SSE"  ON)
#add_definitions(-D MEMORY_CHECK)
#endif()

get_filename_component(CMAKE_SOURCE_DIR
         "../../src"
         ABSOLUTE)

get_filename_component(CMAKE_CURRENT_SOURCE_DIR
         "../../src"
         ABSOLUTE)
		 
get_filename_component(THIRD_PARTY_SOURCE_DIR
        "../../third_party"
        ABSOLUTE)

get_filename_component(CMAKE_CURRENT_DIR
        "."
        ABSOLUTE)
		 
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")

MESSAGE( STATUS "src_dir = ${CMAKE_SOURCE_DIR}.")
MESSAGE( STATUS "current_src_dir = ${CMAKE_CURRENT_SOURCE_DIR}.")

if(DEFINED NDEBUG)
  add_definitions(-DNDEBUG)
endif()


function(extract_dirs root_dir out_arg)
    if (IS_DIRECTORY ${root_dir})               # 当前路径是一个目录吗，是的话就加入到包含目录
        get_filename_component(DIR_NAME ${root_dir} NAME)
        list(APPEND new_source_list ${root_dir})
    endif()

    file(GLOB all_sub RELATIVE ${root_dir} ${root_dir}/*) # 获得当前目录下的所有文件，让如ALL_SUB列表中
    foreach(sub ${all_sub})
        if (IS_DIRECTORY ${root_dir}/${sub})
            extract_dirs(${root_dir}/${sub} new_source_list) # 对子目录递归调用，包含
        endif()
    endforeach()
    set(${out_arg} ${new_source_list} PARENT_SCOPE)
endfunction()

function(extract_ios_include include_list out_arg)
    set (new_source_list)
    foreach (source_name ${${include_list}})
        if (NOT source_name MATCHES "(/android/)")
            #message(STATUS "source_name =${source_name}")
			if(NOT MEMORY_CHECK AND source_name MATCHES "(/libleaktracer/)")
				continue()
            endif()
            list(APPEND new_source_list ${source_name})
        endif()
    endforeach()
    set(${out_arg} ${new_source_list} PARENT_SCOPE)
endfunction()

function(extract_ios_source source_list out_arg)
    set (new_source_list)
    foreach (source_name ${${source_list}})
        if (NOT source_name MATCHES "(/android/)")
            #message(STATUS "source_name =${source_name}")
            if(NOT MEMORY_CHECK AND source_name MATCHES "(/libleaktracer/)")
                continue()
            endif()
            list(APPEND new_source_list ${source_name})
        endif()
    endforeach()
    set(${out_arg} ${new_source_list} PARENT_SCOPE)
endfunction()

extract_dirs(${CMAKE_SOURCE_DIR} data_trans_hub_header_files)
extract_ios_include(data_trans_hub_header_files data_trans_hub_ios_header_files)
include_directories(${data_trans_hub_ios_header_files})

extract_dirs(${THIRD_PARTY_SOURCE_DIR} third_party_header_files)
extract_ios_include(third_party_header_files third_party_ios_header_files)
include_directories(${third_party_ios_header_files})

file(GLOB_RECURSE SOURCES ${CMAKE_SOURCE_DIR}/*.cpp ${CMAKE_SOURCE_DIR}/*.c ${CMAKE_SOURCE_DIR}/*.cc ${CMAKE_SOURCE_DIR}/*.mm ${CMAKE_SOURCE_DIR}/*.h ${THIRD_PARTY_SOURCE_DIR}/*.cpp ${THIRD_PARTY_SOURCE_DIR}/*.c ${THIRD_PARTY_SOURCE_DIR}/*.cc)
extract_ios_source(SOURCES SOURCE_FILES)

#source_group(TREE ${CMAKE_SOURCE_DIR} FILES  ${SOURCE_FILES} )

add_definitions(-DHAVE_EXPAT_CONFIG_H )

add_library( # Sets the name of the library.
             DataTransHub
             # Sets the library as a shared library.
             STATIC
             ${SOURCE_FILES} )

# Searches for a specified prebuilt library and stores the path as a
# variable. Because CMake includes system libraries in the search path by
# default, you only need to specify the name of the public NDK library
# you want to add. CMake verifies that the library exists before
# completing its build.

#find_library( # Sets the name of the path variable.
#              log-lib
              # Specifies the name of the NDK library that
              # you want CMake to locate.
#              log )

# Specifies libraries CMake should link to your target library. You
# can link multiple libraries, such as libraries you define in this
# build script, prebuilt third-party libraries, or system libraries.


target_link_libraries( # Specifies the target library.
                       DataTransHub
                       #${log-lib}
		)
		
set_target_properties(DataTransHub PROPERTIES
    FRAMEWORK TRUE
    MACOSX_FRAMEWORK_IDENTIFIER com.igetget.data-trans-hub
    MACOSX_FRAMEWORK_INFO_PLIST ${CMAKE_CURRENT_DIR}/Info.plist
    VERSION 1.0.0
    SOVERSION 1.0.0
    PUBLIC_HEADER "${CMAKE_SOURCE_DIR}/call_interface/iOS/DataTransHub.h;${CMAKE_SOURCE_DIR}/call_interface/iOS/LogHub.h;${CMAKE_SOURCE_DIR}/call_interface/iOS/LogHubLever.h"
    XCODE_ATTRIBUTE_IPHONEOS_DEPLOYMENT_TARGET "10.0"
    XCODE_ATTRIBUTE_CLANG_ENABLE_OBJC_ARC YES
    XCODE_ATTRIBUTE_ENABLE_BITCODE "NO"
)
