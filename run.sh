#!/usr/bin/env bash
set -euo pipefail

cd "$(dirname "$0")"
mkdir -p build results

MODE="${1:-matmul}"

if [ "$MODE" = "cache" ]; then
    g++ -std=c++17 -O2 -Wall -Wextra -Wpedantic \
        src/main.cpp -o build/step1
    ./build/step1

elif [ "$MODE" = "matmul" ]; then
    g++ -std=c++17 -O2 -Wall -Wextra -Wpedantic \
        src/matmul.cpp -o build/matmul

    if [ "$#" -ge 2 ]; then
        printf '%s\n' "$2" | ./build/matmul
    else
        ./build/matmul
    fi

else
    echo "Usage: ./run.sh [cache|matmul] [N]"
    exit 1
fi