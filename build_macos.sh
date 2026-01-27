#!/bin/bash
# filepath: /mnt/HD/Projects/GameSystem/SDL2/Demo/build_macos.sh
# macOS build script for Cyber Snake

set -e  # Exit on error

echo "========================================"
echo "Demo - macOS Build Script"
echo "========================================"
echo ""

# Check if CMake is installed
if ! command -v cmake &> /dev/null; then
    echo "ERROR: CMake not found"
    echo "Please install CMake using Homebrew:"
    echo "  brew install cmake"
    exit 1
fi

# Check if Homebrew is installed
if ! command -v brew &> /dev/null; then
    echo "WARNING: Homebrew not found"
    echo "Install Homebrew from https://brew.sh"
    echo "Then install dependencies:"
    echo "  brew install cmake pkg-config sdl2 sdl2_mixer"
    exit 1
fi

# Check for pkg-config
if ! brew list pkg-config &> /dev/null; then
    echo "Installing pkg-config..."
    brew install pkg-config
fi

# Check for SDL2
if ! brew list sdl2 &> /dev/null; then
    echo "Installing SDL2..."
    brew install sdl2
fi

# Check for SDL2_mixer
if ! brew list sdl2_mixer &> /dev/null; then
    echo "Installing SDL2_mixer..."
    brew install sdl2_mixer
fi

# Create build directory
mkdir -p build

cd build

# Clean previous builds
echo "Cleaning previous build..."
rm -rf CMakeCache.txt CMakeFiles

# Configure with CMake
echo ""
echo "Configuring project..."
echo ""
cmake .. -DCMAKE_BUILD_TYPE=Release

# Build
echo ""
echo "Building project..."
echo ""
cmake --build . -j$(sysctl -n hw.ncpu)

cd ..

echo ""
echo "========================================"
echo "Build completed successfully!"
echo "========================================"
echo ""
echo "Application bundle: build/Demo.app"
echo ""
echo "Verifying bundle contents..."
if [ -d "build/Demo.app/Contents/Resources/data" ]; then
    echo "✓ Data folder copied to bundle"
else
    echo "✗ Data folder NOT found in bundle"
fi

if [ "$(ls -A build/Demo.app/Contents/Resources/*.ini 2>/dev/null)" ]; then
    echo "✓ Template files copied to bundle"
else
    echo "✗ Template files NOT found in bundle"
fi
echo ""
echo "To run the game:"
echo "  open build/Demo.app"
echo "  or double-click Demo.app in Finder"
echo ""
