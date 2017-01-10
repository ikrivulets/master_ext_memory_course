cmake_minimum_required(VERSION 3.6)
project(transposition_ext_memory)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES main.cpp generate.cpp)
add_executable(transposition_ext_memory ${SOURCE_FILES} generate.cpp)