cmake_minimum_required(VERSION 3.10)
project(Demo)

# Set C++ standard
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Add compiler definitions
add_definitions(-DGS_USE_SDL_MIXER)

# Find SDL2
find_package(SDL2 REQUIRED)

# Tell CMake to use the modern behavior for finding OpenGL libraries
cmake_policy(SET CMP0072 NEW)

# LEGACY to ensure compatibility with older systems
set(OpenGL_GL_PREFERENCE LEGACY)

# GLVND for modern systems
#set(OpenGL_GL_PREFERENCE GLVND)

find_package(OpenGL REQUIRED)

# Find OpenGL (includes both GL and GLU on most systems)
find_package(OpenGL REQUIRED)

# Verify GLU is available (use different checks for different platforms)
if(APPLE)
    # macOS: GLU is part of the OpenGL framework
    if(NOT OPENGL_INCLUDE_DIR)
        message(FATAL_ERROR "OpenGL not found on macOS")
    endif()
    message(STATUS "Using macOS OpenGL framework (includes GLU)")
elseif(UNIX)
    # Linux: Check for GLU explicitly
    if(NOT OPENGL_GLU_FOUND AND NOT OPENGL_glu_LIBRARY)
        message(FATAL_ERROR "GLU library not found. Please install libglu1-mesa-dev (Ubuntu/Debian) or mesa-libGLU-devel (Fedora/RHEL)")
    endif()
    message(STATUS "Found GLU: ${OPENGL_glu_LIBRARY}")
elseif(WIN32)
    # Windows: GLU is typically included with OpenGL
    message(STATUS "Using Windows OpenGL (includes GLU)")
endif()

# Find SDL2_mixer using pkg-config
find_package(PkgConfig REQUIRED)
pkg_check_modules(SDL2_MIXER REQUIRED SDL2_mixer)

# Include directories
include_directories(
    ${SDL2_INCLUDE_DIRS}
    ${OPENGL_INCLUDE_DIRS}
    ${SDL2_MIXER_INCLUDE_DIRS}
)

# Add library directories from pkg-config
link_directories(${SDL2_MIXER_LIBRARY_DIRS})

# Source files
set(SOURCES
    gs_app.cpp
    gs_error.cpp
    gs_file.cpp
    gs_ini_file.cpp
    gs_keyboard.cpp
    gs_main.cpp
    gs_mouse.cpp
    gs_object.cpp
    gs_timer.cpp
    gs_ogl_collide.cpp
    gs_ogl_display.cpp
    gs_ogl_font.cpp
    gs_ogl_image.cpp
    gs_ogl_map.cpp
    gs_ogl_menu.cpp
    gs_ogl_particle.cpp
    gs_ogl_sprite.cpp
    gs_ogl_sprite_ex.cpp
    gs_ogl_texture.cpp
    gs_platform.cpp
    gs_sdl_controller.cpp
    gs_sdl_mixer_sound.cpp
    gs_demo.cpp
)

# Create executable - as macOS bundle on Apple platforms
if(APPLE)
    set(MACOSX_BUNDLE_BUNDLE_NAME "Demo")
    set(MACOSX_BUNDLE_EXECUTABLE_NAME "Demo")
    
    add_executable(Demo MACOSX_BUNDLE ${SOURCES})
    
    set_target_properties(Demo PROPERTIES
        MACOSX_BUNDLE TRUE
        MACOSX_BUNDLE_INFO_PLIST "${CMAKE_CURRENT_SOURCE_DIR}/Info.plist.in"
    )
    
    # Copy data directory into the bundle's Resources
    if(EXISTS ${CMAKE_SOURCE_DIR}/data)
        add_custom_command(TARGET Demo POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${CMAKE_SOURCE_DIR}/data
            $<TARGET_BUNDLE_CONTENT_DIR:Demo>/Resources/data
            COMMENT "Copying data directory to bundle"
        )
    endif()
    
    # Copy the contents of the templates directory to bundle's Resources
    if(EXISTS ${CMAKE_SOURCE_DIR}/templates)
        add_custom_command(TARGET Demo POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${CMAKE_SOURCE_DIR}/templates
            $<TARGET_BUNDLE_CONTENT_DIR:Demo>/Resources/
            COMMENT "Copying templates to bundle"
        )
    endif()
    
    # Link CoreFoundation framework for bundle resource path lookup
    # On macOS, OpenGL::GL includes both GL and GLU
    target_link_libraries(Demo
        SDL2::SDL2
        OpenGL::GL
        ${SDL2_MIXER_LIBRARIES}
        "-framework CoreFoundation"
    )
    
    message(STATUS "Build complete! Application bundle: ${CMAKE_BINARY_DIR}/Demo.app")
else()
    add_executable(Demo ${SOURCES})
    
    # Copy data directory to build directory
    if(EXISTS ${CMAKE_SOURCE_DIR}/data)
        add_custom_command(TARGET Demo POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${CMAKE_SOURCE_DIR}/data
            ${CMAKE_BINARY_DIR}/data
            COMMENT "Copying data directory to build"
        )
    endif()
    
    # Copy contents of templates directory to build directory
    if(EXISTS ${CMAKE_SOURCE_DIR}/templates)
        add_custom_command(TARGET Demo POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${CMAKE_SOURCE_DIR}/templates
            ${CMAKE_BINARY_DIR}/
            COMMENT "Copying templates to build"
        )
    endif()
    
    # Link libraries for non-Apple platforms
    # Try to use OpenGL::GLU if available, otherwise fall back to GL only
    if(TARGET OpenGL::GLU)
        target_link_libraries(Demo
            SDL2::SDL2
            OpenGL::GL
            OpenGL::GLU
            ${SDL2_MIXER_LIBRARIES}
        )
    else()
        # Fallback: link GLU library directly
        target_link_libraries(Demo
            SDL2::SDL2
            OpenGL::GL
            ${OPENGL_glu_LIBRARY}
            ${SDL2_MIXER_LIBRARIES}
        )
    endif()
    
    message(STATUS "Build complete! Executable: ${CMAKE_BINARY_DIR}/Demo")
endif()

