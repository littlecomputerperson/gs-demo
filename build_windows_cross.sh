#!/bin/bash
# Cross-compiles a Windows (x86_64) build of this project on Linux using mingw-w64, for quick
# local testing/debugging under Wine without needing a Windows machine or a CI round-trip.
#
# One-time host setup (see README.md's "Cross-compiling for Windows + Wine" section):
#   - mingw-w64 (the "-posix" threading variant) and wine installed
#   - A mingw-w64 SDL2 + SDL2_mixer "sysroot" built from the official SDL mingw-devel release
#     archives, defaulting to ~/mingw-sdl2/sysroot/x86_64-w64-mingw32
#     (override with GS_MINGW_SYSROOT if you keep it elsewhere)
#
# Usage:
#   ./build_windows_cross.sh          # configure + build only
#   ./build_windows_cross.sh --run    # ...then launch the result under Wine

set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
BUILD_DIR="$SCRIPT_DIR/build-win"
MINGW_SYSROOT="${GS_MINGW_SYSROOT:-$HOME/mingw-sdl2/sysroot/x86_64-w64-mingw32}"
TOOLCHAIN_FILE="$SCRIPT_DIR/mingw-w64-x86_64.toolchain.cmake"

if [ ! -f "$TOOLCHAIN_FILE" ]; then
    echo "ERROR: toolchain file not found at $TOOLCHAIN_FILE" >&2
    exit 1
fi

if [ ! -d "$MINGW_SYSROOT" ]; then
    echo "ERROR: mingw SDL2 sysroot not found at $MINGW_SYSROOT" >&2
    echo "See README.md's 'Cross-compiling for Windows + Wine' section to set it up (one-time)." >&2
    exit 1
fi

mkdir -p "$BUILD_DIR"

PKG_CONFIG_PATH="$MINGW_SYSROOT/lib/pkgconfig" \
PKG_CONFIG_LIBDIR="$MINGW_SYSROOT/lib/pkgconfig" \
cmake -S "$SCRIPT_DIR" -B "$BUILD_DIR" \
    -DCMAKE_TOOLCHAIN_FILE="$TOOLCHAIN_FILE" \
    -DSDL2_DIR="$MINGW_SYSROOT/lib/cmake/SDL2" \
    -DCMAKE_BUILD_TYPE=Release

cmake --build "$BUILD_DIR" -j"$(nproc)"

# Copy the runtime DLLs the .exe needs alongside it (Wine, and a real Windows machine, both
# resolve DLLs from the executable's own directory first).
cp -f "$MINGW_SYSROOT/bin/SDL2.dll" "$BUILD_DIR/" 2>/dev/null || true
cp -f "$MINGW_SYSROOT/bin/SDL2_mixer.dll" "$BUILD_DIR/" 2>/dev/null || true
WINPTHREAD_DLL="$(find /usr/x86_64-w64-mingw32/lib -maxdepth 1 -name 'libwinpthread-1.dll' 2>/dev/null | head -1)"
[ -n "$WINPTHREAD_DLL" ] && cp -f "$WINPTHREAD_DLL" "$BUILD_DIR/"
LIBSTDCPP_DLL="$(find /usr/lib/gcc/x86_64-w64-mingw32 -name 'libstdc++-6.dll' 2>/dev/null | head -1)"
[ -n "$LIBSTDCPP_DLL" ] && cp -f "$LIBSTDCPP_DLL" "$BUILD_DIR/"
LIBGCC_DLL="$(find /usr/lib/gcc/x86_64-w64-mingw32 -name 'libgcc_s_seh-1.dll' 2>/dev/null | head -1)"
[ -n "$LIBGCC_DLL" ] && cp -f "$LIBGCC_DLL" "$BUILD_DIR/"

EXE="$(find "$BUILD_DIR" -maxdepth 1 -name '*.exe' | head -1)"

echo ""
echo "Build complete: $EXE"
echo "Run it under Wine with:  wine \"$EXE\""

if [ "$1" == "--run" ] && [ -n "$EXE" ]; then
    echo "Launching under Wine..."
    wine "$EXE"
fi
