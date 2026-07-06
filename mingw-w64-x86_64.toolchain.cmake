# CMake toolchain file for cross-compiling this project from Linux to Windows (x86_64) using
# mingw-w64. Used by build_windows_cross.sh -- see that script and README.md's "Cross-compiling
# for Windows + Wine" section for the one-time host setup this depends on.

set(CMAKE_SYSTEM_NAME Windows)
set(CMAKE_SYSTEM_PROCESSOR x86_64)

set(TOOLCHAIN_PREFIX x86_64-w64-mingw32)

# The "-posix" variant is required: it provides the C++11 threading support (std::mutex,
# std::thread, etc. via winpthreads) that the plain (win32-threads) variant lacks.
set(CMAKE_C_COMPILER ${TOOLCHAIN_PREFIX}-gcc-posix)
set(CMAKE_CXX_COMPILER ${TOOLCHAIN_PREFIX}-g++-posix)
set(CMAKE_RC_COMPILER ${TOOLCHAIN_PREFIX}-windres)

set(CMAKE_FIND_ROOT_PATH
    /usr/x86_64-w64-mingw32
    $ENV{HOME}/mingw-sdl2/sysroot/x86_64-w64-mingw32
)

set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE BOTH)
