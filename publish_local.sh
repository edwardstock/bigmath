#!/usr/bin/env bash

export CONAN_LOCAL=1
export NOUPLOAD=1
chmod +x .circleci/deploy.sh

rm -rf ~/.conan/data/bigmath
bash .circleci/deploy.sh
