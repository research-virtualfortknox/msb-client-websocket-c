cmake_minimum_required(VERSION 2.8)
project(MsbClientC_CppWrapper_Test)

set(JSON-C_INCLUDE_PATH /usr/local/include/json-c CACHE PATH "Path to JSON-C")
set(MSB_C_CLIENT_INCLUDE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../src CACHE PATH "Path to MSB-C-Client")

set(JSON-C_LIBRARY_PATH /usr/local/lib CACHE PATH "Path to library JSON-C")
set(MSB_C_CLIENT_LIBRARY_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../cmake-build-debug CACHE PATH "Path to MSB-C-Client cpp wrapper library")

include_directories(
        ${MSB_C_CLIENT_INCLUDE_PATH}
        ${JSON-C_INCLUDE_PATH}
)

set(sources_lib
        MsbClientC_CppWrapper.hpp
        MsbClientC_CppWrapper.cpp
        )

set(sources
        MsbClientC_CppWrapper_Test.cpp
)

link_directories(
        ${JSON-C_LIBRARY_PATH}
        ${MSB_C_CLIENT_LIBRARY_PATH}
)

link_libraries(
        json-c
        uuid
        #MsbClientC_CppWrapper
)

add_library(MsbClientC_CppWrapper ${sources_lib})
add_executable(MsbClientC_CppWrapper_Test ${sources})
