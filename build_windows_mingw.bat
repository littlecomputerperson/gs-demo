@echo off
REM filepath: /mnt/HD/Projects/GameSystem/SDL2/Demo/build_windows_mingw.bat
REM Windows MinGW/GCC build script for Cyber Snake

echo ========================================
echo Cyber Snake - MinGW Build Script
echo ========================================
echo.

REM Check if CMake is installed
where cmake >nul 2>nul
if %ERRORLEVEL% NEQ 0 (
    echo ERROR: CMake not found in PATH
    echo Please install CMake from https://cmake.org/download/
    pause
    exit /b 1
)

REM Check if MinGW is installed
where g++ >nul 2>nul
if %ERRORLEVEL% NEQ 0 (
    echo ERROR: g++ not found in PATH
    echo Please install MinGW-w64 or MSYS2 with GCC
    pause
    exit /b 1
)

REM Check if mingw32-make is available
where mingw32-make >nul 2>nul
if %ERRORLEVEL% EQU 0 (
    set MAKE_CMD=mingw32-make
) else (
    where make >nul 2>nul
    if %ERRORLEVEL% EQU 0 (
        set MAKE_CMD=make
    ) else (
        echo ERROR: Neither mingw32-make nor make found in PATH
        pause
        exit /b 1
    )
)

echo Using make command: %MAKE_CMD%
echo.

REM Create build directory
if not exist build mkdir build

cd build

REM Configure with CMake for MinGW
echo.
echo Configuring project for MinGW...
echo.
cmake .. -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Release

if %ERRORLEVEL% NEQ 0 (
    echo.
    echo ERROR: CMake configuration failed
    echo.
    echo Troubleshooting:
    echo 1. Ensure MinGW-w64 or MSYS2 is properly installed
    echo 2. Make sure SDL2 development libraries are in MinGW path
    echo 3. For MSYS2, install: pacman -S mingw-w64-x86_64-SDL2 mingw-w64-x86_64-SDL2_mixer
    cd ..
    pause
    exit /b 1
)

REM Build the project
echo.
echo Building project...
echo.
%MAKE_CMD% -j4

if %ERRORLEVEL% NEQ 0 (
    echo.
    echo ERROR: Build failed
    cd ..
    pause
    exit /b 1
)

cd ..

echo.
echo ========================================
echo Build completed successfully!
echo ========================================
echo.
echo Executable location: build\Demo.exe
echo.
echo NOTE: Make sure SDL2.dll and SDL2_mixer.dll are in the same
echo       directory as the executable or in your system PATH.
echo.
echo To run the game:
echo   cd build
echo   Demo.exe
echo.
pause
