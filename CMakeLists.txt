cmake_minimum_required(VERSION 2.8.4)
project(Autobahn_C)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=gnu99")

set(
    CMAKE_RUNTIME_OUTPUT_DIRECTORY
    ${CMAKE_HOME_DIRECTORY}/bin
    )

FILE(GLOB SOURCE_FILES src/*.c)
FILE(GLOB HEADER_FILES src/*.h)

add_executable(Autobahn_C ${SOURCE_FILES} ${HEADER_FILES})


