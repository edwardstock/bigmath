#!/usr/bin/env bash

if [ ! -d "/tmp/pkgs" ]; then
  mkdir -p /tmp/pkgs
fi

# fetching cmake

if [ ! -f "/tmp/pkgs/cmake.sh" ]; then
  wget -O /tmp/pkgs/cmake.sh https://github.com/Kitware/CMake/releases/download/v3.14.5/cmake-3.14.5-Linux-x86_64.sh
fi

cmake_bin=$(which cmake)

if  [ "${cmake_bin}" == "" ] && [ ! -f "/usr/bin/cmake" ]; then
  sh /tmp/pkgs/cmake.sh --skip-license --prefix=/usr
fi