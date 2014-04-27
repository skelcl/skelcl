#!/bin/bash

# get llvm
if [ -d "libraries/stooling/libraries/llvm" ]; then
  wget http://llvm.org/releases/3.4/clang+llvm-3.4-x86_64-apple-darwin10.9.tar.gz
  curl http://llvm.org/releases/3.4/clang+llvm-3.4-x86_64-apple-darwin10.9.tar.gz > llvm.tar.gz
  tar xzf llvm.tar.gz
  mv clang+llvm-3.4-x86_64-apple-darwin10.9 libraries/stooling/libraries/llvm
  rm llvm.tar.gz
fi

if [ -d "libraries/gtest" ]; then
  curl http://googletest.googlecode.com/files/gtest-1.7.0.zip > gtest-1.7.0.zip
  unzip -q gtest-1.7.0.zip
  mv gtest-1.7.0 libraries/gtest
  rm gtest-1.7.0.zip
fi

