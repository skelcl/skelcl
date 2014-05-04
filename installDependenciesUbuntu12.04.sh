#!/bin/bash

echo "Install essential tools and the build environment? (requires sudo access)"
echo "(y)es (n)o "
read ANSWER
if [[ "$ANSWER" == 'y' ]]; then
  set -x #echo on
  sudo apt-get install build-essential git cmake cmake-curses-gui
  set +x #echo off
fi

echo "Install SkelCL dependencies? (requires sudo access)"
echo "(y)es or (n)o "
read ANSWER
if [[ "$ANSWER" == 'y' ]]; then
  if [ -d "libraries/stooling/libraries/llvm" ]; then
    echo "Skip intallation of llvm, because the 'libraries/stooling/libraries/llvm' directory already exists."
  else
    set -x #echo on
    sudo apt-get install libssl-dev libglu1-mesa-dev llvm-3.3-dev clang-3.3 libclang-3.3-dev
    mkdir libraries/stooling/libraries/llvm
    ln -s `llvm-config-3.3 --includedir` libraries/stooling/libraries/llvm/include
    ln -s `llvm-config-3.3 --libdir` libraries/stooling/libraries/llvm/lib
    set +x #echo off
  fi

  if [ -d "libraries/gtest" ]; then
    echo "Skip intallation of gtest, because the 'libraries/gtest' directory already exists."
  else
    set -x #echo on
    wget http://googletest.googlecode.com/files/gtest-1.7.0.zip
    unzip -q gtest-1.7.0.zip
    mv gtest-1.7.0 libraries/gtest
    rm gtest-1.7.0.zip
    set +x #echo off
  fi
fi

