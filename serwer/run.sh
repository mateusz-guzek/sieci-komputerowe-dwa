#!/usr/bin/bash

cmake -B build
cmake --build build
exec ./out/sk-serwer
