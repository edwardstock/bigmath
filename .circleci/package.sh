#!/usr/bin/env bash
set -e
set -x

rootDir=$(pwd)
mkdir -p output
mkdir -p _build_package && cd _build_package

rm -rf output/*
rm -rf _build_package/*

cmake .. -DCMAKE_BUILD_TYPE=Debug -DENABLE_CONAN=Off -DENABLE_TEST=Off -DCMAKE_INSTALL_PREFIX=$(pwd)/_install
cmake --build . -- -j4
cmake --build . --target install

cmake .. -DCMAKE_BUILD_TYPE=Debug -DENABLE_SHARED=On -DENABLE_CONAN=Off -DENABLE_TEST=Off -DCMAKE_INSTALL_PREFIX=$(pwd)/_install
cmake --build . -- -j4
cmake --build . --target install


_to_lower() {
  echo ${1} | tr '[:upper:]' '[:lower:]'
}

tar_bin=$(which tar | tr -d "\n")
if [ "${tar_bin}" == "" ]; then
  echo "Tar executable not found!"
  exit 1
fi

if [ -f "version" ]; then
  VERS=$(cat version | tr -d "\n")
fi

if [ -f "../version" ]; then
  VERS=$(cat ../version | tr -d "\n")
fi

if [ "${1}" != "" ]; then
  VERS=${1}
fi

arch=$(uname -m)
sysname=$(uname)
sysname=$(_to_lower ${sysname})
gvers=$(git tag | sort -r | head -n 1)
ghash=$(git rev-parse --short=8 HEAD)
fname_sufix="v${gvers}-${ghash}-${sysname}-${arch}"
fname="libbigmath-${fname_sufix}.tar.gz"

mv $(pwd)/_install "$(pwd)/libbigmath"

tar -zcvf "${fname}" "libbigmath"

mv "${fname}" ${rootDir}/output/
cd ${rootDir}/output

ls -lsa $(pwd)

ghr ${VERS} .
