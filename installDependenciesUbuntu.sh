#!/bin/bash
DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"

echo "Install essential tools and the build environment? (requires sudo access)"
echo "(y)es (n)o "
read ANSWER
if [[ "$ANSWER" == 'y' ]]; then
  set -x #echo on
  sudo apt-get install build-essential git cmake cmake-curses-gui unzip
  set +x #echo off
fi

echo "Install SkelCL dependencies? (requires sudo access)"
echo "(y)es or (n)o "
read ANSWER
if [[ "$ANSWER" == 'y' ]]; then
  if [ -d "$DIR/libraries/stooling/libraries/llvm" ]; then
    echo "Skip intallation of llvm, because the 'libraries/stooling/libraries/llvm' directory already exists."
  else
    set -x #echo on
    sudo apt-get install libssl-dev libglu1-mesa-dev llvm-3.3-dev clang-3.3 libclang-3.3-dev
    mkdir $DIR/libraries/stooling/libraries/llvm
    mkdir $DIR/libraries/stooling/libraries/llvm/bin
    ln -s `which llvm-config-3.3` $DIR/libraries/stooling/libraries/llvm/bin/llvm-config
    ln -s `llvm-config-3.3 --includedir` $DIR/libraries/stooling/libraries/llvm/include
    ln -s `llvm-config-3.3 --libdir` $DIR/libraries/stooling/libraries/llvm/lib
    set +x #echo off
  fi

  if [ -d "$DIR/libraries/gtest" ]; then
    echo "Skip intallation of gtest, because the 'libraries/gtest' directory already exists."
  else
    set -x #echo on
    wget -O gtest-1.7.0.zip https://github.com/google/googletest/archive/release-1.7.0.zip
    unzip -q gtest-1.7.0.zip
    mv gtest-1.7.0 $DIR/libraries/gtest
    rm gtest-1.7.0.zip
    set +x #echo off
  fi
fi

