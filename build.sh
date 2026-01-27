#!/bin/bash
# filepath: /mnt/HD/Projects/GameSystem/Other/Demo/build.sh

mkdir -p build
cd build

# Clean previous builds
rm -rf *

# Configure with SDL2
cmake .. -DUSE_SDL2=ON -DUSE_SDL_MIXER=OFF

# Build
cmake --build . -j$(nproc)

echo "Build complete! Executable: build/Demo"
