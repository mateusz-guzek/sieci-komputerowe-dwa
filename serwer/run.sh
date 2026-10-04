#!/usr/bin/bash

if [ ! -d build ]; then
    cmake -B build || exit 1
fi

if cmake --build build; then
    exec ./out/sk-serwer
fi