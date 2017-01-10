cmake_minimum_required(VERSION 3.6)
project(ExternalMergeSort)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES ext_msort.cpp)
add_executable(ExternalMergeSort ${SOURCE_FILES})