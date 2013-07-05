#!/bin/bash

platform='unknown'
unamestr=`uname -s`
if [[ "$unamestr" == 'Darwin' ]]; then
  platform='osx'
elif [[ "$unamestr" == 'Linux' ]]; then
  platform='ubuntu' #assume ubuntu as linux
fi

if [[ $platform == 'ubuntu' ]]; then
  # get and extract libclang (Step 2 from the INSTALL file)
  wget http://llvm.org/releases/3.3/clang+llvm-3.3-amd64-Ubuntu-12.04.2.tar.gz
  tar xzf clang+llvm-3.3-amd64-Ubuntu-12.04.2.tar.gz
  mv clang+llvm-3.3-amd64-Ubuntu-12.04.2 libraries/stooling/libraries/llvm
  rm clang+llvm-3.3-amd64-Ubuntu-12.04.2.tar.gz
fi

if [[ $platform == 'osx' ]]; then
  # get and extract libclang (Step 2 from the INSTALL file)
  wget http://llvm.org/releases/3.3/clang+llvm-3.3-x86_64-apple-darwin12.tar.gz
  tar xzf clang+llvm-3.3-x86_64-apple-darwin12.tar.gz
  mv clang+llvm-3.3-x86_64-apple-darwin12 libraries/stooling/libraries/llvm
  rm clang+llvm-3.3-x86_64-apple-darwin12.tar.gz
fi

# get and extract gtest (Step 3)
wget http://googletest.googlecode.com/files/gtest-1.6.0.zip
unzip -q gtest-1.6.0.zip
mv gtest-1.6.0 libraries/gtest
rm gtest-1.6.0.zip

# make build directory (Step 4)
mkdir build

# call cmake (Step 5)
(cd build; cmake ..)

# call make (Step 6)
(cd build; make -j4)

# call all tests (Step 7)
(cd build; make test)

