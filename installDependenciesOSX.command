#!/bin/bash
DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"

# get llvm
if [ -d "$DIR/libraries/stooling/libraries/llvm" ]; then
  echo "Skip intallation of llvm, because the '$DIR/libraries/stooling/libraries/llvm' directory already exists."
else
  set -x #echo on
  curl http://llvm.org/releases/3.4/clang+llvm-3.4-x86_64-apple-darwin10.9.tar.gz > llvm.tar.gz
  tar xzf llvm.tar.gz
  mv clang+llvm-3.4-x86_64-apple-darwin10.9 $DIR/libraries/stooling/libraries/llvm
  rm llvm.tar.gz
  set +x #echo off
fi

if [ -d "$DIR/libraries/gtest" ]; then
  echo "Skip intallation of gtest, because the '$DIR/libraries/gtest' directory already exists."
else
  set -x #echo on
  curl http://googletest.googlecode.com/files/gtest-1.7.0.zip > gtest-1.7.0.zip
  unzip -q gtest-1.7.0.zip
  mv gtest-1.7.0 $DIR/libraries/gtest
  rm gtest-1.7.0.zip
  set +x #echo off
fi

