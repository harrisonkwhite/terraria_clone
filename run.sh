#!/bin/bash
set -e
mkdir -p build
cd build
cmake -DCMAKE_EXPORT_COMPILE_COMMANDS=ON ..
cmake --build .
cp compile_commands.json ..
cd ..
./build/terraria_clone
