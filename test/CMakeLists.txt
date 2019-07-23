cmake_minimum_required(VERSION 2.8)
project(MsbClientC_Test)

add_compile_options(-fPIC -g3)
set(CMAKE_C_STANDARD 99)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fprofile-arcs -ftest-coverage")
set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -fprofile-arcs -ftest-coverage")

set(JSON-C_INCLUDE_PATH /usr/local/include/json-c CACHE PATH "Path to JSON-C")
set(MSB_C_CLIENT_INCLUDE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../src CACHE PATH "Path to Msb-C-Client")
set(MSB_C_CLIENT_HEADER_INCLUDE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../cmake-build-debug CACHE PATH "Path to Msb-C-Client library header")
set(XML_INCLUDE_PATH /usr/local/include/libxml2 CACHE PATH "Path to libxml2")

set(JSON-C_LIBRARY_PATH /usr/local/lib CACHE PATH "Path to library JSON-C")
set(MSB_C_CLIENT_LIBRARY_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../cmake-build-debug CACHE PATH "Path to library Msb-C-Client")

include_directories(
        ${JSON-C_INCLUDE_PATH}
        ${MSB_C_CLIENT_INCLUDE_PATH}
		${MSB_C_CLIENT_HEADER_INCLUDE_PATH}
		${XML_INCLUDE_PATH}
)

set(JSON-C_LIB json-c)
set(MSB_C_CLIENT_LIB MsbClientC)

set(sources_test
		src/sput-1.4.0/sput.h
		src/main.c
)

set(sources_example
		src/example.c
		)

link_directories(
        ${JSON-C_LIBRARY_PATH}
        ${MSB_C_CLIENT_LIBRARY_PATH}
)

link_libraries(
		curl
        ${JSON-C_LIB}
        ${MSB_C_CLIENT_LIB}
        uuid
        xml2
		m
)

add_executable(MsbClientC_Test ${sources_test})
add_executable(MsbClientC_Example ${sources_example})

configure_file(integration_flow.json integration_flow.json COPYONLY)
