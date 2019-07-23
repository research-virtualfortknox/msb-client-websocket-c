#!/bin/sh

VERSION=""

while getopts ":v:" opt; do
  case $opt in
    v) VERSION="$OPTARG"
    ;;
    *) echo "Available parameters: -v (vfk.platform project version) Example: -v '1.4.0-SNAPSHOT'"
    ;;
  esac
done

if [ "$VERSION" = '' ]
then
	echo "The project version has to be set (use -v option)"
    exit 1
fi

VERSIONSHORT="$(echo $VERSION | grep -o "[0-9]\.[0-9]\.[0-9]")"

if [ "$VERSIONSHORT" = '' ]
then
	echo "The project version $VERSION could not be transformed to a short version number"
    exit 1
fi

VERSIONSHORT=$VERSIONSHORT".0"
echo "Short version number: $VERSIONSHORT"


# Build c client lib
echo "Start processing vfk.msb.client.library.websocket.c libMsbClientC"

if [ -d "output" ]
then
    echo "Remove output folder content"
    rm -rf output/*
else
    mkdir output
fi

# Clean build folder
if [ -d "cmake-build-debug" ]
then
    echo "Remove build folder content"
    rm -rf cmake-build-debug/*
else
    mkdir cmake-build-debug
fi

cd cmake-build-debug
echo "Loading Cmake project"
cmake ..

echo "Make"
make -j4

echo "Copying library to /usr/local/lib"
cp libMsbClientC.so /usr/local/lib/libMsbClientC.so

echo "Copying library and header to output folder"
cp libMsbClientC.so ../output/libMsbClientC.so
cp libMsbClientC.h ../output/libMsbClientC.h
cp LICENSE ../output/LICENSE

# Build c client test
echo "Start processing vfk.msb.client.library.websocket.c MsbClientCTest"
cd ../test

# Clean build folder
if [ -d "cmake-build-debug" ]
then
    echo "Remove build folder content"
    rm -rf cmake-build-debug/*
else
    mkdir cmake-build-debug
fi

cd cmake-build-debug
echo "Loading Cmake project"
cmake ..

echo "Make"
make -j4

echo "Copying test and integrationflow file to output folder"
cp MsbClientC_Test ../../output/MsbClientC_Test
cp ../integration_flow.json ../../output/integration_flow.json

echo "Copying Doxyfile to output folder"
cd ../../output
cp ../Doxyfile .

echo "Creating documentation"
doxygen

echo "Done"
