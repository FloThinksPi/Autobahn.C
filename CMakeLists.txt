cmake_minimum_required(VERSION 2.8.4)
project(Autobahn_C)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=gnu99")

set(SOURCE_FILES main.c)
add_executable(Autobahn_C ${SOURCE_FILES})