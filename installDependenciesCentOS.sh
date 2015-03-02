#!/bin/bash
DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"

echo "Install modern developer toolset (devtools-1.1)? (requires root access)"
echo "(y)es (n)o"
read ANSWER
if [[ "$ANSWER" == 'y' ]]; then
  set -x #echo on
  su -c 'wget http://people.centos.org/tru/devtools-1.1/devtools-1.1.repo -O /etc/yum.repos.d/devtools-1.1.repo && yum install devtoolset-1.1'
  echo "To enable the developer toolset permernently add 'scl enable devtoolset-1.1 bash' to your ~/.bashrc"
  set +x #echo off
fi

echo "Add Extra Packages for Enterprise Linux (EPEL) repository? (requires root access)"
echo "(y)es (n)o"
read ANSWER
if [[ "$ANSWER" == 'y' ]]; then
  set -x #echo on
  wget http://download.fedoraproject.org/pub/epel/6/x86_64/epel-release-6-8.noarch.rpm
  su -c 'rpm -ivh epel-release-6-8.noarch.rpm'
  rm epel-release-6-8.noarch.rpm
  set +x #echo off
fi

echo "Install SkelCL dependencies? (requires sudo access)"
echo "(y)es or (n)o "
read ANSWER
if [[ "$ANSWER" == 'y' ]]; then
  set -x #echo on
  su -c 'yum install cmake28 openssl-devel mesa-libGL-devel'
  set +x #echo off

  if [ -d "$DIR/libraries/stooling/libraries/llvm" ]; then
    echo "Skip intallation of llvm, because the 'libraries/stooling/libraries/llvm' directory already exists."
  else
    set -x #echo on
    su -c 'yum install llvm-devel llvm-static clang-devel'
    mkdir $DIR/libraries/stooling/libraries/llvm
    mkdir $DIR/libraries/stooling/libraries/llvm/bin
    ln -s `which llvm-config` $DIR/libraries/stooling/libraries/llvm/bin/llvm-config
    ln -s `llvm-config --includedir` $DIR/libraries/stooling/libraries/llvm/include
    ln -s `llvm-config --libdir` $DIR/libraries/stooling/libraries/llvm/lib
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

