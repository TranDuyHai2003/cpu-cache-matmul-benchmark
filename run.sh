#!/usr/bin/env bash
set -e

mkdir -p build

g++ -std=c++17 -O2 -Wall -Wextra -Wpedantic \
    src/main.cpp -o build/step1

./build/step1