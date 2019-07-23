#!/bin/bash

PATH=""

while getopts ":p:" opt; do
  case $opt in
    p) PATH="$OPTARG"
    ;;
    *) echo "Available parameters: -p (path) Example: -p '/home/ci-vfk'"
    ;;
  esac
done

if [ "$PATH" == '' ]
then
	echo "The path has to be set (use -p option)"
    exit 1
fi

sudo apt-get install git=1:1.9.1-1ubuntu0.8 \
autoconf=2.69-6 \
automake=1:1.14.1-2ubuntu1 \
libtool=2.4.2-1.7ubuntu1 \
build-essential=11.6ubuntu6 \
cmake=2.8.12.2-ubuntu3 \
libssl-dev=1.0.1f-1ubuntu2.26 \
libcurl4-openssl-dev=7.35.0-1ubuntu2.16 \
service_uuid-dev=2.20.1-5.1ubuntu20.9 \
python-dev=2.7.5-5ubuntu3

cd $PATH

if [ -d "git" ]
then

else
    mkdir git
fi

cd git

if [ -d "json-c" ]
then
    echo "Remove json-c folder content"
    rm -rf json-c
fi

if [ -d "nopoll" ]
then
    echo "Remove nopoll folder content"
    rm -rf nopoll
fi

if [ -d "uriparser" ]
then
    echo "Remove uriparser folder content"
    rm -rf uriparser
fi

if [ -d "libxml2" ]
then
    echo "Remove libxml2 folder content"
    rm -rf libxml2
fi

git clone https://github.com/json-c/json-c.git
cd json-c
git checkout json-c-0.13
sh autogen.sh
./configure
make && sudo make install

cd ..

git clone https://github.com/ASPLes/nopoll.git
cd nopoll
git checkout tags/0.4.6
sh autogen.sh
make && sudo make install

cd ..

git clone https://github.com/uriparser/uriparser.git
cd uriparser
git checkout tags/uriparser-0.8.6
sh autogen.sh
./configure --disable-test --disable-doc
make && sudo make install

cd ..

git clone https://gitlab.gnome.org/GNOME/libxml2.git
cd libxml2
git checkout tags/v2.9.8
sh autogen.sh
make && sudo make install