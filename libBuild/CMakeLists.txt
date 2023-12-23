cmake_minimum_required(VERSION 3.5)
project(easyLogLib VERSION 0.1.0 LANGUAGES C CXX)

# Set output directories

# Set compiler flags

# Set include directories

# Add source files

# Add library
option(BUILD_SHARED_LIB "Build shared library" ON)
option(BUILD_STATIC_LIB "Build static library" OFF)
option(BUILD_MODULE_LIB "Build module library" OFF)

if(BUILD_SHARED_LIB)
    add_library(easyLogLib SHARED EasyLog.cpp)
endif()

if(BUILD_STATIC_LIB)
    add_library(easyLogLib STATIC EasyLog.cpp)
endif()

if(BUILD_MODULE_LIB)
    add_library(easyLogLib MODULE EasyLog.cpp)
endif()

# Add executable

# Link libraries

# Set target properties

# Set target compile features
if(BUILD_SHARED_LIB)
target_compile_features(easyLogLib PRIVATE cxx_std_20)
endif()

if(BUILD_STATIC_LIB)
    target_compile_features(easyLogLib PRIVATE cxx_std_20)
endif()

if(BUILD_MODULE_LIB)
    target_compile_features(easyLogLib PRIVATE cxx_std_20)
endif()

# Set target compile definitions

# Set target include directories

# Set target compile options

# Set target link options

# Set target pre-build commands

# Set target post-build commands

# Add subdirectories