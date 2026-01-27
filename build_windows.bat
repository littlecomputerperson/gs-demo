@echo off
REM filepath: /mnt/HD/Projects/GameSystem/SDL2/Demo/build_windows.bat
REM Windows build script for Cyber Snake

echo ========================================
echo Cyber Snake - Windows Build Script
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

REM Create build directory
if not exist build mkdir build

cd build

REM Configure with CMake
echo.
echo Configuring project...
echo.

REM Check if vcpkg toolchain is available
if defined VCPKG_ROOT (
    echo Using vcpkg toolchain from: %VCPKG_ROOT%
    cmake .. -DCMAKE_TOOLCHAIN_FILE=%VCPKG_ROOT%\scripts\buildsystems\vcpkg.cmake
) else (
    echo WARNING: VCPKG_ROOT not set. Attempting standard CMake configuration...
    echo If build fails, install SDL2 via vcpkg and set VCPKG_ROOT environment variable.
    cmake ..
)

if %ERRORLEVEL% NEQ 0 (
    echo.
    echo ERROR: CMake configuration failed
    echo.
    echo Troubleshooting:
    echo 1. Install vcpkg: git clone https://github.com/Microsoft/vcpkg.git
    echo 2. Run: vcpkg\bootstrap-vcpkg.bat
    echo 3. Install dependencies: vcpkg install sdl2:x64-windows sdl2-mixer:x64-windows
    echo 4. Set VCPKG_ROOT environment variable to vcpkg directory
    cd ..
    pause
    exit /b 1
)

REM Build the project
echo.
echo Building project (Release)...
echo.
cmake --build . --config Release

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
echo Executable location: build\Release\Demo.exe
echo.
echo To run the game:
echo   cd build\Release
echo   Demo.exe
echo.
pause
