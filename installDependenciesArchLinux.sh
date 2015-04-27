#!/usr/bin/env bash
DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"

install_pkgs() {
  for package in $@; do
    set -x #echo on
    sudo pacman -S $package
    set +x #echo off
  done
}

if [ ! -f "/etc/arch-release" ]; then
  echo "Not an Arch Linux release. Aborting"
  exit 1
fi

echo "Install essential tools and the build environment? (requires sudo access)"
echo "(y)es (n)o "
read ANSWER
if [[ "$ANSWER" == 'y' ]]; then
  install_pkgs base-devel unzip glu python2
fi

./installDependencies.sh
