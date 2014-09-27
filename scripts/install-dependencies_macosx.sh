#!/bin/bash

set -e

#
# Download and build dependencies
# You will need to install gcc-4.6 or greater for mpir on 64bits system, I have tested on gcc49
# also, to make the new gcc49 as default, you need to run
# This will install several binaries with the -mp-4.9 (as gcc-mp-4.9). You can then activate gcc 4.9 as default with
#		sudo port select gcc mp-gcc49
#   	hash gcc
# To go back to the Apple LLVM compiler
#		sudo port select gcc none
#		hash gcc
#

mkdir -p lib
cd lib

# Download libs

if [ ! -f "gmp-6.0.0a.tar.lz" ]; then
    wget https://gmplib.org/download/gmp/gmp-6.0.0a.tar.lz
fi

if [ ! -f "flint-1.6.tgz" ]; then
    wget http://www.flintlib.org/flint-1.6.tgz
fi

if [ ! -f "mpir-2.6.0.tar.bz2" ]; then
    wget http://www.mpir.org/mpir-2.6.0.tar.bz2
fi

if [ ! -f "mpfr-3.1.1.tar.bz2" ]; then
    wget https://ftp.gnu.org/gnu/mpfr/mpfr-3.1.1.tar.bz2
fi

# Install GMP

if [ ! -d "gmp-6.0.0" ]; then
    lzip -d gmp-6.0.0a.tar.lz
    tar xf gmp-6.0.0a.tar
fi

cd gmp-6.0.0
./configure
make
make check
sudo make install
sudo update_dyld_shared_cache #ldconfig
cd ..

# Install mpfr

if [ ! -d "mpfr-3.1.1" ]; then
    tar xjf mpfr-3.1.1.tar.bz2
fi

cd mpfr-3.1.1
./configure
make
make check
sudo make install
sudo update_dyld_shared_cache #ldconfig
cd ..

# Install mpir

if [ ! -d "mpir-2.6.0" ]; then
    tar xjf mpir-2.6.0.tar.bz2
fi


cd mpir-2.6.0
#cp ../../x86_64.patch ./
#if [ $(uname -m) = "x86_64" ]; then
#    nohup patch -p0 -N --dry-run --silent < x86_64.patch 2>/dev/null
#    #If the patch has not been applied then the $? which is the exit status 
#    #for last command would have a success status code = 0
#    if [ $? -eq 0 ];
#    then
#       #apply the patch
#       patch -p0 -N < x86_64.patch
#    fi
#fi
./configure
make
make check
sudo make install
sudo update_dyld_shared_cache #ldconfig
cd ..

# Install flint (ok, this is perhaps quite a bit strange)

if [ ! -d "flint-1.6" ]; then
    tar xzf flint-1.6.tgz
fi

cd flint-1.6
. flint_env
make library
sudo cp libflint.dylib /usr/local/lib
sudo cp *.h /usr/local/include
sudo mkdir -p /usr/local/include/zn_poly/src
sudo cp zn_poly/include/*.h /usr/local/include/zn_poly/src/
sudo update_dyld_shared_cache #ldconfig
cd ..

