#!/usr/bin/env bash

conan install mpir/3.0.0@ -s build_type=Debug --build=missing
conan install mpir/3.0.0@ -s build_type=Release --build=missing
conan install mpdecimal/2.4.2@ -s build_type=Debug --build=missing
conan install mpdecimal/2.4.2@ -s build_type=Release --build=missing