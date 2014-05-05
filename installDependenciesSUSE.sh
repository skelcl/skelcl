#!/bin/bash
DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"

echo "Install essential tools and the build environment? (requires sudo access)"
echo "(y)es (n)o "
read ANSWER
if [[ "$ANSWER" == 'y' ]]; then
  set -x #echo on
  sudo zypper install gcc-c++ cmake
  set +x #echo off
fi

echo "Install SkelCL dependencies? (requires sudo access)"
echo "(y)es or (n)o "
read ANSWER
if [[ "$ANSWER" == 'y' ]]; then
  set -x #echo on
  sudo zypper install libopenssl-devel Mesa-libGL-devel
  set +x #echo off

  if [ -d "$DIR/libraries/stooling/libraries/llvm" ]; then
    echo "Skip intallation of llvm, because the 'libraries/stooling/libraries/llvm' directory already exists."
  else
    set -x #echo on
    sudo zypper install llvm-clang-devel llvm-devel libLLVM
    set +x #echo off
  fi

  if [ -d "$DIR/libraries/gtest" ]; then
    echo "Skip intallation of gtest, because the 'libraries/gtest' directory already exists."
  else
    set -x #echo on
    wget http://googletest.googlecode.com/files/gtest-1.7.0.zip
    unzip -q gtest-1.7.0.zip
    mv gtest-1.7.0 $DIR/libraries/gtest
    rm gtest-1.7.0.zip
    set +x #echo off
  fi
fi

