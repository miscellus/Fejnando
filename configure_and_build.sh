#!/usr/bin/env sh
set -e
mkdir -p build
cd $_
cmake ..
make
