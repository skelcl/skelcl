#!/bin/bash

# get and extract libclang (Step 2 from the INSTALL file)
wget http://llvm.org/releases/3.1/clang+llvm-3.1-x86_64-linux-ubuntu_12.04.tar.gz
tar xzf clang+llvm-3.1-x86_64-linux-ubuntu_12.04.tar.gz
mv clang+llvm-3.1-x86_64-linux-ubuntu_12.04 libraries/ssedit/libraries/libclang
rm clang+llvm-3.1-x86_64-linux-ubuntu_12.04.tar.gz

# get and extract gtest (Step 3)
wget http://googletest.googlecode.com/files/gtest-1.6.0.zip
unzip -q gtest-1.6.0.zip
mv gtest-1.6.0 libraries/gtest
rm gtest-1.6.0.zip

# symlink gtest (Step 4)
ln -s ../../gtest libraries/ssedit/libraries/gtest

# make build directory (Step 5)
mkdir build

# call cmake (Step 6)
(cd build; cmake ...)

# call make (Step 7)
(cd build; make -j4)

# call all tests (Step 8)
(cd build; make test)

