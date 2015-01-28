cmake_minimum_required(VERSION 2.8.4)
project(Autobahn_C)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=gnu99")

set(
    CMAKE_RUNTIME_OUTPUT_DIRECTORY
    ${CMAKE_HOME_DIRECTORY}/bin
    )

#Source Dir
FILE(GLOB SOURCE_FILES src/*.c)
FILE(GLOB SOURCE_HEADER_FILES src/*.h)

#Menu Library
FILE(GLOB LIB_MENU_FILES lib/menu/*.c)
FILE(GLOB LIB_MENU_HEADER_FILES lib/menu/*.h)

#TextAusgabe Library(Selbstgeschrieben)
FILE(GLOB LIB_PRINTUI_FILES lib/printui/*.c)
FILE(GLOB LIB_PRINTUI_HEADER_FILES lib/printui/*.h)

#CMD/Terminal Anpassungs Library(Selbstgeschrieben)
FILE(GLOB LIB_WIN32ADAPTER_FILES src/consoleAdaption/*.c)
FILE(GLOB LIB_WIN32ADAPTER_HEADER_FILES src/consoleAdaption/*.h)


#Include
add_executable(Autobahn_C ${SOURCE_FILES} ${HEADER_FILES} ${LIB_MENU_FILES} ${LIB_MENU_HEADER_FILES}
                ${LIB_PRINTUI_FILES} ${LIB_PRINTUI_HEADER_FILES}
                ${LIB_WIN32ADAPTER_FILES} ${LIB_WIN32ADAPTER_HEADER_FILES})


