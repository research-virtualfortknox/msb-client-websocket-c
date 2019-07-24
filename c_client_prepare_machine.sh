#!/bin/sh

while getopts ":p:" opt; do
  case $opt in
    p) WORKPATH="$OPTARG"
    ;;
    *) echo "Available parameters: -p (workpath) Example: -p '/home/ci-vfk'"
    ;;
  esac
done

if [ "$WORKPATH" = '' ]
then
	echo "The work path has to be set (use -p option)"
    exit 1
fi

apt update
apt --force-yes install git \
autoconf \
automake \
libtool \
build-essential \
cmake \
libssl-dev \
libcurl4-openssl-dev \
uuid-dev \
python-dev \
lcov

cd $WORKPATH

if [ -d "git" ]
then
    :
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
#git checkout tags/0.4.6
git checkout master
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