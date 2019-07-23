cmake_minimum_required(VERSION 2.8)
project(MsbClientC)

add_compile_options(-fPIC -g3)
set(CMAKE_C_STANDARD 99)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fprofile-arcs -ftest-coverage")
set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -fprofile-arcs -ftest-coverage")

set(JSON-C_INCLUDE_PATH /usr/local/include/json-c CACHE PATH "Path to JSON-C")
set(NOPOLL_INCLUDE_PATH /usr/local/include/nopoll CACHE PATH "Path to nopoll")
set(URIPARSER_INCLUDE_PATH /usr/local/include/uriparser CACHE PATH "Path to uriparser")

set(JSON-C_LIBRARY_PATH /usr/local/lib CACHE PATH "Pfad to JSON-C library")
set(NOPOLL_LIBRARY_PATH /usr/local/lib CACHE PATH "Path to nopoll library")
set(URIPARSER_LIBRARY_PATH /usr/local/lib CACHE PATH "Path to uriparser library")

include_directories(
        ${JSON-C_INCLUDE_PATH}
        ${NOPOLL_INCLUDE_PATH}
        ${URIPARSER_INCLUDE_PATH}
)

set(sources
        src/MsbClientC.c
        src/MsbClientC.h
        src/rest/helpers.h
        src/rest/uri.c
        src/MsbObject.c
        src/websocket/websocket.c
        src/websocket/websocket.h
)

link_directories(
        ${JSON-C_LIBRARY_PATH}
        ${NOPOLL_LIBRARY_PATH}
        ${URIPARSER_LIBRARY_PATH}
)

link_libraries(
        pthread
        m
        uriparser
        json-c
        nopoll
        uuid
)

add_library(MsbClientC SHARED ${sources})

configure_file(src/MsbClientC.h libMsbClientC.h COPYONLY)
configure_file(LICENSE LICENSE COPYONLY)