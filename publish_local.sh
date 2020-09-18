#!/usr/bin/env bash
rm -rf ~/.conan/data/bigmath
NOUPLOAD=1 sh .circleci/deploy.sh
