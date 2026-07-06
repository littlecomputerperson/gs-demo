# GameSystem OpenGL Demo

A comprehensive demonstration of the GameSystem (GS) Library - a cross-platform 2D/3D game development framework built with OpenGL and SDL2.

## Description

This demo showcases the capabilities of the GameSystem library through 12 interactive demonstrations, each highlighting different features of the framework. The library provides a complete abstraction layer for game development, making it easy to write games that compile on both Windows and SDL2-based platforms (Linux, macOS).

### Demo Features

The application cycles through the following demonstrations:

1. **Clear Demo** - Basic screen clearing and OpenGL setup
2. **Polygon Demo** - Drawing triangles and quads
3. **Color Demo** - Smooth color blending across vertices
4. **Rotation Demo** - 3D rotation on multiple axes
5. **3D Shapes Demo** - Pyramid and cube with transparency
6. **Texture Demo** - Texture mapping on 3D objects with lighting
7. **Sprite Demo** - 2D sprite rendering with rotation and scaling
8. **Font Demo** - Bitmap font rendering and text display
9. **Collision Demo** - Sprite-based collision detection
10. **Menu Demo** - Interactive menu system with mouse/keyboard input
11. **Map Demo** - Tile-based map rendering and scrolling
12. **Particle Demo** - Particle effects system

### Interactive Controls

- **Space** - Next demonstration
- **P** - Pause/Resume
- **ESC** - Quit application
- **Arrow Keys** - Demo-specific controls (rotation, movement, etc.)
- **Page Up/Down** - Adjust transparency
- **Home** - Reset current demo settings
- **F1-F5** - Change resolution (320x240, 400x300, 640x480, 800x600, 1024x768)
- **1-5** - Play sound samples
- **+/-** - Adjust master volume
- **B** - Toggle blending
- **L** - Toggle lighting
- **V** - Toggle VSync
- **T** - Toggle frame rate cap

## Tech Stack

### Core Technologies
- **C++** (C++98/C++11 compatible)
- **OpenGL 2.1** - Fixed-function pipeline graphics
- **SDL2** - Cross-platform window management and input
- **FMOD** - Audio playback (legacy, optional)
- **SDL2_mixer** - Modern audio playback (recommended)

### Build System
- **CMake 3.10+** - Cross-platform build configuration
- **GCC/Clang/MSVC** - Compiler support

### Platform Abstraction Layer
The `gs_platform.*` files provide seamless compatibility between Windows API and SDL2, allowing the same codebase to compile on multiple platforms with minimal changes.

## File Structure

### Core Framework Files
```
gs_app.cpp/h              - Application framework and main loop
gs_main.cpp/h             - Entry point (WinMain/main)
gs_platform.cpp/h         - Platform abstraction (Windows/SDL2)
gs_demo.cpp/h             - Demo application implementation
```

### Game System Library Components
```
gs_error.cpp/h            - Error reporting and logging
gs_file.cpp/h             - File I/O with cross-platform paths
gs_ini_file.cpp/h         - INI file parsing
gs_keyboard.cpp/h         - Keyboard input handling
gs_mouse.cpp/h            - Mouse input and cursor management
gs_object.cpp/h           - Base object class
gs_timer.cpp/h            - Frame timing and FPS monitoring
```

### OpenGL Rendering System
```
gs_ogl_collide.cpp/h      - 2D collision detection
gs_ogl_display.cpp/h      - OpenGL context and rendering setup
gs_ogl_font.cpp/h         - Bitmap font rendering
gs_ogl_image.cpp/h        - TGA image loading
gs_ogl_map.cpp/h          - Tile-based map system
gs_ogl_menu.cpp/h         - Interactive menu system
gs_ogl_particle.cpp/h     - Particle effects
gs_ogl_sprite.cpp/h       - Basic sprite rendering
gs_ogl_sprite_ex.cpp/h    - Animated sprite rendering
gs_ogl_texture.cpp/h      - Texture loading and management
gs_ogl_color.cpp/h        - Color utilities
```

### Audio System
```
gs_sdl_mixer_sound.cpp/h  - SDL2_mixer implementation (recommended)
gs_fmod_sound.cpp/h       - FMOD implementation (legacy)
```

### Configuration & Resources
```
gs_resource.h             - Resource ID definitions
settings.ini              - Display and audio settings
data/                     - Demo assets (textures, sounds, maps)
```

## Platform Support

### Windows

#### Prerequisites
- Visual Studio 2017+ or MinGW-w64
- CMake 3.10+
- SDL2 development libraries
- SDL2_mixer development libraries

#### Building with Visual Studio + vcpkg
```cmd
vcpkg install sdl2:x64-windows sdl2-mixer:x64-windows
mkdir build && cd build
cmake .. -DCMAKE_TOOLCHAIN_FILE=[vcpkg]/scripts/buildsystems/vcpkg.cmake
cmake --build . --config Release
```

#### Building with MinGW/MSYS2
```bash
# Install MSYS2 from https://www.msys2.org/
pacman -S mingw-w64-x86_64-gcc mingw-w64-x86_64-cmake \
          mingw-w64-x86_64-SDL2 mingw-w64-x86_64-SDL2_mixer \
          mingw-w64-x86_64-make

mkdir build && cd build
cmake .. -G "MinGW Makefiles"
mingw32-make -j$(nproc)
```

#### Running
```cmd
cd build
Demo.exe
```

### Linux

#### Prerequisites
```bash
# Ubuntu/Debian
sudo apt-get install build-essential cmake libsdl2-dev libsdl2-mixer-dev \
                     libgl1-mesa-dev libglu1-mesa-dev

# Fedora/RHEL
sudo dnf install gcc-c++ cmake SDL2-devel SDL2_mixer-devel \
                 mesa-libGL-devel mesa-libGLU-devel

# Arch Linux
sudo pacman -S base-devel cmake sdl2 sdl2_mixer mesa
```

#### Building
```bash
chmod +x build.sh
./build.sh
```

Or manually:
```bash
mkdir build && cd build
cmake ..
make -j$(nproc)
```

#### Running
```bash
./build/Demo
```

### macOS

#### Prerequisites
```bash
# Install Homebrew if not already installed
/bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"

# Install dependencies
brew install cmake pkg-config sdl2 sdl2_mixer
```

#### Building
```bash
chmod +x build_macos.sh
./build_macos.sh
```

Or manually:
```bash
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
cmake --build . -j$(sysctl -n hw.ncpu)
cd ..
```

#### Running
```bash
open build/Demo.app
```

Or from Finder, navigate to the `build` directory and double-click `Demo.app`.

## Automated Builds & Releases

`.github/workflows/release.yml` builds Linux, macOS, and Windows binaries automatically on every push of a `v*` tag (e.g. `v1.0.0`), or on a manual run from the Actions tab (`workflow_dispatch`). Each platform's build is attached to a GitHub Release as a ready-to-run download — no compiler, SDK, or dev libraries required on the end user's machine.

### What each platform produces

| Platform | Artifact | How it's made self-contained |
|---|---|---|
| Linux | `Demo-linux-x86_64.AppImage` (single file) | [`linuxdeploy`](https://github.com/linuxdeploy/linuxdeploy) bundles SDL2/SDL2_mixer and their full transitive dependency chain (FLAC, mpg123, opus, vorbis, ~25 libs total) into the AppImage |
| macOS | `Demo-macOS.zip` (contains `Demo.app`) | [`dylibbundler`](https://github.com/auriamg/macdylibbundler) copies Homebrew's SDL2/SDL2_mixer dylibs into `Demo.app/Contents/Frameworks` and rewrites the executable's load paths, so it doesn't depend on Homebrew being installed |
| Windows | `Demo-windows-x64.zip` (contains `Demo.exe` + DLLs) | Built via vcpkg's dynamic `x64-windows` triplet; every DLL vcpkg produces is copied next to `Demo.exe` so nothing needs to be installed separately |

Each platform job also runs an automated smoke test (headless via Xvfb on Linux, a real launch-and-check on macOS, `Start-Process` on Windows) before uploading its artifact, to catch a build that compiles but doesn't actually run.

### Why macOS also bundles `libSDL3.dylib`

Homebrew's `sdl2` formula is actually [`sdl2-compat`](https://github.com/libsdl-org/sdl2-compat) — a thin compatibility shim that implements the SDL2 API on top of the real SDL3 library, loaded at runtime via `dlopen()` rather than a normal linked dependency. `dylibbundler` only follows `otool -L` dependencies, so it never bundles SDL3, and the resulting app fails on launch with `Failed loading SDL3 library.` on any machine that does not happen to have Homebrew's SDL3 installed. sdl2-compat's dlopen search (visible in its own binary's embedded strings) includes `@loader_path/libSDL3.dylib` — i.e. right next to `libSDL2`, which is exactly where `dylibbundler` places things — so the workflow copies it there explicitly after the normal bundling step. Confirmed by reproducing the crash locally (installing `sdl2-compat`/`sdl3` via Homebrew and launching the bundled, unfixed app) and then confirming the fix resolves it.

### Cutting a release

```bash
git tag v1.0.0
git push origin v1.0.0
```

This triggers all three builds; once they finish, the release (with all three files attached) appears automatically on the repo's Releases page.

### Unsigned binaries

None of these builds are code-signed (that requires a paid Apple Developer ID / Microsoft certificate), so:
- **macOS**: Gatekeeper will say the app is from an "unidentified developer" — right-click → Open the first time.
- **Windows**: SmartScreen will show a warning — click "More info" → "Run anyway".

This is normal for free/indie-distributed builds and doesn't indicate a problem with the download.

### Why Windows uses dynamic linking (not a single static .exe)

An earlier version of this workflow tried statically linking SDL2/SDL2_mixer via vcpkg's `x64-windows-static` triplet, for a single self-contained `.exe` with no DLLs at all. That failed at link time: SDL2_mixer's static library pulls in a long, version-dependent chain of optional codec libraries (FLAC, mpg123, opus, vorbis, wavpack, ...) that are only exposed via pkg-config's `Libs.private`, which the CMakeLists.txt here doesn't request. Dynamic linking (the current approach) sidesteps this entirely and mirrors the same "bundle whatever the tool produces" strategy already used for Linux and macOS.

### Why `pkg-config` comes from vcpkg, not Chocolatey

The Windows job originally installed `pkg-config` via `choco install pkgconfiglite`, which downloads from a SourceForge mirror -- one that's become unreliable (404s / reset connections), causing intermittent CI failures unrelated to anything in this repo. Since the job already depends on vcpkg for SDL2/SDL2_mixer, it now installs vcpkg's own `pkgconf` port instead, removing that third-party dependency entirely. vcpkg's port only produces `pkgconf.exe` (no `pkg-config` alias), so the workflow copies it to both names, since which one CMake's `FindPkgConfig` looks for has varied across CMake versions.

### The Windows platform abstraction bridge

Getting a real Windows build working also required finishing `gs_platform.h`/`gs_platform.cpp`'s native Win32 implementations — `GS_Platform::GetTickCount`, `Sleep`, `GetClientRect`, `SetRect`, `PtInRect`, `OutputDebugString`, `MessageBox`, `GetCurrentDirectory`, and `NormalizePath` previously had no Windows-side implementation at all (only the SDL/non-Windows branch was ever finished), so any Windows build — MSVC or MinGW — would have failed to link the moment those functions were actually called. If you add new `GS_Platform` functions, make sure both the `#ifdef GS_PLATFORM_WINDOWS` and non-Windows branches in `gs_platform.cpp` are implemented, not just one.

One subtlety worth knowing if you touch this file: `windows.h` `#define`s several of these names to their `A`-suffixed forms (`OutputDebugStringA`, `MessageBoxA`, `GetCurrentDirectoryA`), and since the call sites are namespace-qualified (`GS_Platform::OutputDebugString(...)`), that macro substitution applies to the declaration and definition too — so the Windows implementations are written under the same (post-macro-expansion) names, explicitly calling through to the real global function via `::`.

### Why `add_executable()` needs the `WIN32` keyword

`gs_main.cpp` defines `WinMain` on Windows (a native Win32 GUI entry point) rather than `main`. CMake's `add_executable(Demo ${SOURCES})` defaults to the console subsystem, whose CRT startup expects `main` — without the `WIN32` keyword, MSVC fails to link with `unresolved external symbol main`. `add_executable(Demo WIN32 ${SOURCES})` fixes this by selecting the GUI subsystem instead. This is a documented no-op on non-Windows platforms, so it doesn't affect the Linux/macOS builds. (GCC/mingw doesn't have this problem — its linker auto-selects the console vs. GUI CRT startup stub based on which entry symbol is actually defined, which is why a manual mingw-w64 cross-compile can succeed even without this keyword, while MSVC strictly requires it.)

## Platform Differences

### Windows-Specific Features

The Windows build includes additional features not available on SDL2 platforms:

- **Menu Bar** - Access to display settings, sound controls, and program options
- **Accelerator Keys** - Alt+X to quit, Alt+Enter to toggle fullscreen
- **Native Window Chrome** - Standard Windows title bar and controls

### SDL2 Platforms (Linux/macOS)

SDL2 builds use keyboard-only controls since native menu bars aren't supported:

**All Controls Available via Keyboard:**
- F1-F5: Change resolution
- 1-5: Play sound samples  
- +/-: Adjust volume
- B: Toggle blending
- L: Toggle lighting
- V: Toggle VSync
- T: Toggle turbo mode
- P: Pause
- ESC: Quit
- Space: Next demo
- Home: Reset current demo

The demo is fully functional on all platforms; only the menu UI is Windows-specific.

## Configuration

### Display Settings (settings.ini)
```ini
[Display]
DisplayWidth=640        ; Window width
DisplayHeight=480       ; Window height
ColorDepth=32          ; Bits per pixel
WindowMode=1           ; 0=Fullscreen, 1=Windowed
```

### Audio Settings
- **MusicVolume** - 0-255 (default: 127)
- **EffectsVolume** - 0-255 (default: 255)

## Using the GameSystem Library

The GameSystem library is designed to make game development straightforward:

### Basic Application Structure
```cpp
#include "gs_main.h"

class MyGame : public GS_Application {
protected:
    BOOL GameInit() override {
        // Initialize your game here
        return TRUE;
    }
    
    BOOL GameLoop() override {
        // Main game logic goes here
        return TRUE;
    }
    
    BOOL GameShutdown() override {
        // Cleanup
        return TRUE;
    }
};

MyGame g_myGame;  // Create global instance
```

### Key Features

#### Automatic Platform Abstraction
```cpp
// Works on both Windows and SDL2 platforms
MessageBox(NULL, "Hello!", "Title", MB_OK);
GetCurrentDirectory(buffer, size);
```

#### OpenGL Rendering Made Easy
```cpp
GS_OGLDisplay display;
display.Create(hDC, 640, 480, 32);
display.EnableVSync(TRUE);
display.BeginRender2D(hWnd);
// Draw sprites, fonts, etc.
display.EndRender2D();
```

#### Sprite Management
```cpp
GS_OGLSprite sprite;
sprite.Create("image.tga");
sprite.SetDestXY(100, 100);
sprite.SetRotateZ(45.0f);
sprite.Render();
```

#### Collision Detection
```cpp
GS_OGLCollide collide;
if (collide.IsRectOnRect(rect1, rect2)) {
    // Handle collision
}
```

## Development

### Adding Custom Demos

Modify `gs_demo.cpp` to add new demonstrations:

```cpp
BOOL GS_Demo::MyCustomDemo() {
    // Your demo code here
    return TRUE;
}
```

Add to the game loop switch statement in `GameLoop()`.

### Creating Games with GameSystem

1. Inherit from `GS_Application`
2. Override `GameInit()`, `GameLoop()`, `GameShutdown()`
3. Use GS classes for rendering, input, collision, etc.
4. Compile with CMake on any supported platform

## Architecture

### Design Patterns
- **Inheritance-based framework** - Extend GS_Application
- **Platform abstraction** - Single codebase, multiple platforms
- **Object-oriented OpenGL** - C++ wrappers around OpenGL
- **Resource management** - Automatic texture/sprite cleanup

### Rendering Pipeline
1. `BeginRender2D()` - Set up 2D orthographic projection
2. Render sprites, fonts, particles, maps
3. `EndRender2D()` - Restore 3D projection
4. `SwapBuffers()` - Display frame

## Future Platform Support

### Potential Targets
- **Android** - High feasibility via SDL2 Android support
- **iOS** - High feasibility via SDL2 iOS support
- **WebAssembly** - Medium-high feasibility via Emscripten
- **Nintendo Switch** - Would require SDL2 homebrew support

The SDL2 foundation makes porting to new platforms straightforward.

## Troubleshooting

### Linux: "Failed to initialize SDL"
- Ensure SDL2 packages are installed
- Check X11/Wayland is running

### macOS: "Library not loaded"
- Verify SDL2 frameworks are in the correct path
- Use `install_name_tool` if needed

### Windows: "Cannot find SDL2.dll"
- Copy SDL2.dll and SDL2_mixer.dll to executable directory

### Performance Issues
- Disable VSync (press 'V')
- Reduce resolution (F1-F5 keys)
- Check GPU drivers are up to date

## License

This project demonstrates the GameSystem library originally developed in 2003.

Libraries used:
- **SDL2** - zlib License
- **SDL2_mixer** - zlib License
- **OpenGL** - Vendor-specific implementations

## Credits

- **Original Development** - Little Computer Person (2003)
- **SDL2 Port** - 2026
- **Demo Music** - "Stomp Rock for Champions" by Musictown
- **Demo Sounds** - Various samples

## Version History

**v1.48b** (Current - SDL2 Port)
- Cross-platform support via SDL2
- CMake build system
- Improved scaling for modern displays
- Bug fixes and stability improvements

**v1.0** (2003 - Original)
- Windows-only release
- FMOD audio support
- 12 demonstration modes
- OpenGL 2.1 fixed-function pipeline

## Learn More

This demo showcases the GameSystem library's capabilities. The library provides:
- Complete game framework
- Cross-platform compatibility
- OpenGL rendering abstraction
- Input handling (keyboard/mouse)
- Audio playback
- Collision detection
- Resource management
- Tile-based maps
- Particle systems
- Menu systems

Perfect for 2D games, educational projects, and retro-style game development!

