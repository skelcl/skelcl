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

./installDependencies.sh
