#!/usr/bin/env bash
DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"

echo "Install libraries from source?"
echo "(y)es or (n)o "
read ANSWER
if [[ "$ANSWER" == 'y' ]]; then

  if [ -d "$DIR/libraries/stooling/libraries/llvm" ]; then
    echo "Skip intallation of llvm, because the 'libraries/stooling/libraries/llvm' directory already exists."
  else

    # Build LLVM 3.3 from source.
    NPROC=$(nproc)
    BUILD_THREADS=$((NPROC+1))

    LLVM_VERSION=3.3

    LLVM=llvm-$LLVM_VERSION
    LLVM_DIR=libraries/llvm
    LLVM_BUILD=$LLVM_DIR/build
    LLVM_TARGET=$LLVM_DIR/install

    CLANG=cfe-$LLVM_VERSION
    CLANG_DIR=$LLVM_DIR/tools/clang

    if [ ! -f "$LLVM.src.tar.gz" ]; then
      set -ex #echo on
      wget "http://llvm.org/releases/$LLVM_VERSION/$LLVM.src.tar.gz"
      set +ex #echo off
    fi

    if [ ! -d "$LLVM_DIR" ]; then
      set -ex #echo on
      tar -zxf "$LLVM.src.tar.gz"
      mv "$LLVM.src" "$LLVM_DIR"
      set +ex #echo off
    fi

    if [ ! -f "$CLANG.src.tar.gz" ]; then
      set -ex #echo on
      wget "http://llvm.org/releases/$LLVM_VERSION/$CLANG.src.tar.gz"
      set +ex #echo off
    fi

    if [ ! -d "$CLANG_DIR" ]; then
      set -ex #echo on
      tar -zxf "$CLANG.src.tar.gz"
      mv "$CLANG.src" "$CLANG_DIR"
      set +ex #echo off
    fi

    if [ ! -d "$LLVM_BUILD" ]; then
      set -ex #echo on
      mkdir "$DIR/$LLVM_BUILD"
      cd "$DIR/$LLVM_BUILD"
      cmake -DPYTHON_EXECUTABLE=$(which python2.7) -DCMAKE_INSTALL_PREFIX="$DIR/$LLVM_TARGET" ..
      make -j$BUILD_THREADS all install
      set +ex #echo off
    fi

    set -ex #echo on
    cd $DIR
    rm -f "$LLVM.src.tar.gz" "$CLANG.src.tar.gz"
    mkdir $DIR/libraries/stooling/libraries/llvm
    mkdir $DIR/libraries/stooling/libraries/llvm/bin
    ln -s "$DIR/$LLVM_TARGET/bin/llvm-config" "$DIR/libraries/stooling/libraries/llvm/bin/llvm-config"
    ln -s "$DIR/$LLVM_TARGET/include" "$DIR/libraries/stooling/libraries/llvm/include"
    ln -s "$DIR/$LLVM_TARGET/lib" "$DIR/libraries/stooling/libraries/llvm/lib"
    set +ex #echo off
  fi

  if [ -d "$DIR/libraries/gtest" ]; then
    echo "Skip intallation of gtest, because the 'libraries/gtest' directory already exists."
  else
    set -ex #echo on
    wget http://googletest.googlecode.com/files/gtest-1.7.0.zip
    unzip -q gtest-1.7.0.zip
    mv gtest-1.7.0 $DIR/libraries/gtest
    rm gtest-1.7.0.zip
    set +ex #echo off
  fi
fi
