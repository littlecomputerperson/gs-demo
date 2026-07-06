/*============================================================================================+
 | Game System (GS) Library                                                                   |
 |--------------------------------------------------------------------------------------------|
 | FILES: gs_platform.cpp, gs_platform.h                                                      |
 |--------------------------------------------------------------------------------------------|
 | CLASS: None                                                                                |
 |--------------------------------------------------------------------------------------------|
 | ABOUT: Platform abstraction layer implementation for non-Windows platforms. Provides       |
 |        SDL2-based implementations of Windows API functions for cross-platform support.     |
 |--------------------------------------------------------------------------------------------|
 |                                                                                    01/2026 |
 +============================================================================================*/


//==============================================================================================
// Include platform abstraction header files.
// ---------------------------------------------------------------------------------------------
#include "gs_platform.h"
//==============================================================================================


//==============================================================================================
// Include standard C library header files.
// ---------------------------------------------------------------------------------------------
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifndef GS_PLATFORM_WINDOWS
    #include <unistd.h> // Only the non-Windows implementation below (guarded) needs this.
#endif
#include <iostream>
//==============================================================================================


#ifdef GS_PLATFORM_WINDOWS

////////////////////////////////////////////////////////////////////////////////////////////////
// Platform Utility Functions (native Win32 implementations) /////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////

namespace GS_Platform {

unsigned long GetTickCount() {
    return ::GetTickCount();
}

void Sleep(unsigned long milliseconds) {
    ::Sleep(milliseconds);
}

bool GetClientRect(HWND hwnd, RECT* rect) {
    return ::GetClientRect(hwnd, rect) != 0;
}

void SetRect(RECT* rect, int left, int top, int right, int bottom) {
    ::SetRect(rect, left, top, right, bottom);
}

bool PtInRect(const RECT* rect, POINT pt) {
    return ::PtInRect(rect, pt) != 0;
}

// windows.h #defines this to OutputDebugStringA, so this definition's
// name is macro-expanded the same way as the header's declaration.
void OutputDebugString(const char* message) {
    ::OutputDebugStringA(message);
}

// windows.h #defines this to MessageBoxA, matching the header's declaration.
int MessageBox(void* hwnd, const char* text, const char* caption, unsigned int type) {
    return ::MessageBoxA((HWND)hwnd, text, caption, type);
}

// windows.h #defines this to GetCurrentDirectoryA, matching the header's
// declaration. Deliberately NOT the literal Win32 GetCurrentDirectory (the
// process's cwd, which depends on how the app was launched) -- to match
// the SDL_GetBasePath()-based behavior used on other platforms, this
// returns the directory containing the running executable instead, so
// data/config paths resolve consistently regardless of launch context.
void GetCurrentDirectory(int buflen, char* buffer) {
    if (!buffer || buflen <= 0) return;

    char modulePath[MAX_PATH];
    DWORD len = ::GetModuleFileNameA(NULL, modulePath, MAX_PATH);
    if (len == 0 || len == MAX_PATH) {
        buffer[0] = '\0';
        return;
    }

    char* lastSlash = strrchr(modulePath, '\\');
    if (lastSlash) *lastSlash = '\0';

    strncpy(buffer, modulePath, buflen - 1);
    buffer[buflen - 1] = '\0';
}

// Paths are already in native Windows format on this platform; nothing to normalize.
void NormalizePath(char* path) {
    (void)path;
}

} // namespace GS_Platform

#endif // GS_PLATFORM_WINDOWS


#ifndef GS_PLATFORM_WINDOWS

// Undefine macros before implementing the functions
#undef MessageBox
#undef GetCurrentDirectory
#undef SetRect
#undef SetCursor
#undef SwapBuffers


////////////////////////////////////////////////////////////////////////////////////////////////
// Platform Utility Functions //////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_Platform::GetTickCount():
// ---------------------------------------------------------------------------------------------
// Purpose: Returns the number of milliseconds since SDL initialization.
// ---------------------------------------------------------------------------------------------
// Returns: Number of milliseconds elapsed.
//==============================================================================================

namespace GS_Platform {

unsigned long GetTickCount() {
    return SDL_GetTicks();
}


////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_Platform::Sleep():
// ---------------------------------------------------------------------------------------------
// Purpose: Delays execution for the specified number of milliseconds.
// ---------------------------------------------------------------------------------------------
// Returns: Nothing.
//==============================================================================================

void Sleep(unsigned long milliseconds) {
    SDL_Delay(milliseconds);
}


////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_Platform::GetClientRect():
// ---------------------------------------------------------------------------------------------
// Purpose: Gets the dimensions of the client area of the specified window.
// ---------------------------------------------------------------------------------------------
// Returns: True if successful, false otherwise.
//==============================================================================================

bool GetClientRect(HWND hwnd, RECT* rect) {
    if (!rect) return false;
    SDL_Window* window = (SDL_Window*)hwnd;
    if (!window) return false;
    
    int w, h;
    SDL_GetWindowSize(window, &w, &h);
    rect->left = 0;
    rect->top = 0;
    rect->right = w;
    rect->bottom = h;
    return true;
}


////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_Platform::SetRect():
// ---------------------------------------------------------------------------------------------
// Purpose: Sets the coordinates of a rectangle structure.
// ---------------------------------------------------------------------------------------------
// Returns: Nothing.
//==============================================================================================

void SetRect(RECT* rect, int left, int top, int right, int bottom) {
    if (!rect) return;
    rect->left = left;
    rect->top = top;
    rect->right = right;
    rect->bottom = bottom;
}


////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_Platform::PtInRect():
// ---------------------------------------------------------------------------------------------
// Purpose: Determines whether a point lies within a rectangle.
// ---------------------------------------------------------------------------------------------
// Returns: True if the point is within the rectangle, false otherwise.
//==============================================================================================

bool PtInRect(const RECT* rect, POINT pt) {
    if (!rect) return false;
    return (pt.x >= rect->left && pt.x < rect->right &&
            pt.y >= rect->top && pt.y < rect->bottom);
}


////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_Platform::OutputDebugString():
// ---------------------------------------------------------------------------------------------
// Purpose: Outputs a debug message to the console.
// ---------------------------------------------------------------------------------------------
// Returns: Nothing.
//==============================================================================================

void OutputDebugString(const char* message) {
    std::cout << message << std::endl;
}


////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_Platform::MessageBox():
// ---------------------------------------------------------------------------------------------
// Purpose: Displays a modal message box using SDL.
// ---------------------------------------------------------------------------------------------
// Returns: ID of the button pressed (IDOK, IDYES, IDNO, IDCANCEL).
//==============================================================================================

int MessageBox(void* hwnd, const char* text, const char* caption, unsigned int type) {
    (void)hwnd; // Unused
    
    // Determine button configuration
    SDL_MessageBoxButtonData buttons[2];
    int numButtons = 0;
    
    if (type & MB_YESNO) {
        buttons[0].flags = SDL_MESSAGEBOX_BUTTON_RETURNKEY_DEFAULT;
        buttons[0].buttonid = IDYES;
        buttons[0].text = "Yes";
        buttons[1].flags = SDL_MESSAGEBOX_BUTTON_ESCAPEKEY_DEFAULT;
        buttons[1].buttonid = IDNO;
        buttons[1].text = "No";
        numButtons = 2;
    } else {
        buttons[0].flags = SDL_MESSAGEBOX_BUTTON_RETURNKEY_DEFAULT;
        buttons[0].buttonid = IDOK;
        buttons[0].text = "OK";
        numButtons = 1;
    }
    
    // Determine icon type
    SDL_MessageBoxFlags flags = SDL_MESSAGEBOX_INFORMATION;
    if (type & MB_ICONERROR) {
        flags = SDL_MESSAGEBOX_ERROR;
    } else if (type & MB_ICONQUESTION) {
        flags = SDL_MESSAGEBOX_WARNING;
    }
    
    SDL_MessageBoxData messageboxdata = {
        flags,
        NULL,
        caption,
        text,
        numButtons,
        buttons,
        NULL
    };
    
    int buttonid;
    if (SDL_ShowMessageBox(&messageboxdata, &buttonid) < 0) {
        return IDCANCEL;
    }
    
    return buttonid;
}


////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_Platform::GetCurrentDirectory():
// ---------------------------------------------------------------------------------------------
// Purpose: Gets the directory where the executable is located using SDL_GetBasePath().
// ---------------------------------------------------------------------------------------------
// Returns: Nothing (result stored in buffer).
//==============================================================================================

void GetCurrentDirectory(int buflen, char* buffer) {
    if (!buffer || buflen <= 0) return;
    
    // Get the directory where the executable is located
    char* basePath = SDL_GetBasePath();
    if (basePath) {
        strncpy(buffer, basePath, buflen - 1);
        buffer[buflen - 1] = '\0';
        
        // Remove trailing path separator if present
        size_t len = strlen(buffer);
        if (len > 0 && (buffer[len - 1] == '/' || buffer[len - 1] == '\\')) {
            buffer[len - 1] = '\0';
        }
        
        SDL_free(basePath);
    } else {
        // Fallback to current working directory
        char* cwd = getcwd(buffer, buflen);
        if (!cwd) {
            buffer[0] = '\0';
        }
    }
}


////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_Platform::NormalizePath():
// ---------------------------------------------------------------------------------------------
// Purpose: Converts Windows-style backslashes to forward slashes for cross-platform paths.
// ---------------------------------------------------------------------------------------------
// Returns: Nothing (modifies path in place).
//==============================================================================================

void NormalizePath(char* path) {
    if (!path) return;
    
    // Convert Windows backslashes to forward slashes on non-Windows platforms
    for (char* p = path; *p; ++p) {
        if (*p == '\\') {
            *p = '/';
        }
    }
}


////////////////////////////////////////////////////////////////////////////////////////////////

} // namespace GS_Platform


//==============================================================================================
// PostQuitMessage():
// ---------------------------------------------------------------------------------------------
// Purpose: Posts a quit message to the SDL event queue.
// ---------------------------------------------------------------------------------------------
// Returns: Nothing.
//==============================================================================================

void PostQuitMessage(int nExitCode) {
    (void)nExitCode; // Unused parameter
    SDL_Event quitEvent;
    quitEvent.type = SDL_QUIT;
    SDL_PushEvent(&quitEvent);
}


////////////////////////////////////////////////////////////////////////////////////////////////

#endif // !GS_PLATFORM_WINDOWS
