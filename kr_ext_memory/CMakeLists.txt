cmake_minimum_required(VERSION 3.6)
project(kr_ext_memory)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES main.cpp aplusb.cpp generate.cpp long_sum.cpp connected_component.cpp matrix_mult_vector.cpp)
add_executable(kr_ext_memory ${SOURCE_FILES} aplusb.cpp generate.cpp long_sum.cpp connected_component.cpp matrix_mult_vector.cpp)