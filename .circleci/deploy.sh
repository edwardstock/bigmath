#!/usr/bin/env bash
set -e
set -x

VERS=$(cat version | tr -d "\n")
echo $VERS
exit

sysname=$(uname)

stdlibname="libstdc++"
if [ "${sysname}" == "Darwin" ]; then
  stdlibname="libc++"
fi

## Deploy in latest channel
if [ "${sysname}" == "Linux" ]; then
  CONAN_LOCAL=1 conan create . edwardstock/latest -s compiler.libcxx=${stdlibname}11 -s build_type=Debug --build=missing
  CONAN_LOCAL=1 conan create . edwardstock/latest -s compiler.libcxx=${stdlibname}11 -s build_type=Release --build=missing
fi

CONAN_LOCAL=1 conan create . edwardstock/latest -s compiler.libcxx=${stdlibname} -s build_type=Debug --build=missing
CONAN_LOCAL=1 conan create . edwardstock/latest -s compiler.libcxx=${stdlibname} -s build_type=Release --build=missing

if [ "${NOUPLOAD}" != "1" ]; then
  CONAN_LOCAL=1 conan upload bigmath/${VERS}@edwardstock/latest --all -r=edwardstock
fi
