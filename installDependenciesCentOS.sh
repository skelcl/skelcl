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

  ./installDependencies.sh
fi

