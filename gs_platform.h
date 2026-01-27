/*============================================================================================+
 | Game System (GS) Library                                                                   |
 |--------------------------------------------------------------------------------------------|
 | FILES: gs_platform.cpp, gs_platform.h                                                      |
 |--------------------------------------------------------------------------------------------|
 | CLASS: None                                                                                |
 |--------------------------------------------------------------------------------------------|
 | ABOUT: Platform abstraction layer for cross-platform compatibility between Windows and     |
 |        SDL2. Provides type definitions, constants, and utility functions that wrap         |
 |        platform-specific APIs.                                                             |
 |--------------------------------------------------------------------------------------------|
 |                                                                                    01/2026 |
 +============================================================================================*/


#ifndef GS_PLATFORM_H
#define GS_PLATFORM_H

// Platform detection
#if defined(_WIN32) || defined(_WIN64)
    #define GS_PLATFORM_WINDOWS
#elif defined(__linux__)
    #define GS_PLATFORM_LINUX
#elif defined(__APPLE__)
    #define GS_PLATFORM_MACOS
#endif

// Include appropriate headers
#ifdef GS_PLATFORM_WINDOWS
    #define WIN32_LEAN_AND_MEAN
    #include <windows.h>
#else
    #include <SDL2/SDL.h>
    #include <stdint.h>
    #include <limits.h>
    #include <string.h>
    
    // Type definitions to match Windows types
    typedef uint8_t  BYTE;
    typedef uint32_t DWORD;
    typedef int32_t  BOOL;
    typedef int32_t  INT;
    typedef uint32_t UINT;
    typedef int16_t  SHORT;
    typedef uint16_t USHORT;
    typedef int64_t  LONG;
    typedef uint64_t ULONG;
    typedef void*    HWND;
    typedef void*    HDC;
    typedef void*    HINSTANCE;
    typedef void*    HANDLE;
    typedef void*    HMENU;
    typedef void*    HACCEL;
    typedef void*    LPVOID;
    typedef char*    LPSTR;
    typedef const char* LPCTSTR;
    typedef wchar_t  WCHAR;
    typedef uint32_t WPARAM;
    typedef int32_t  LPARAM;
    typedef int32_t  LRESULT;
    typedef unsigned short WORD;
    typedef unsigned long ULONG_PTR;
    
    #define TRUE  1
    #define FALSE 0
    #ifndef NULL
        #define NULL  0
    #endif
    #define INVALID_HANDLE_VALUE ((HANDLE)-1)
    
    #ifndef _MAX_PATH
        #define _MAX_PATH PATH_MAX
    #endif
    
    // Additional Windows constants
    #define GENERIC_READ     0x80000000
    #define GENERIC_WRITE    0x40000000
    #define CREATE_ALWAYS    2
    #define OPEN_EXISTING    3
    #define FILE_ATTRIBUTE_NORMAL 0x00000080
    #define FILE_BEGIN       0
    #define FILE_CURRENT     1
    #define FILE_END         2
    
    // MessageBox constants
    #define MB_OK            0x00000000
    #define MB_ICONERROR     0x00000010
    #define MB_ICONINFORMATION 0x00000040
    #define MB_YESNO         0x00000004
    #define MB_ICONQUESTION  0x00000020
    
    // MessageBox return values
    #define IDOK             1
    #define IDCANCEL         2
    #define IDYES            6
    #define IDNO             7
    
    // Windows message constants (for compatibility)
    #define WM_CHAR          0x0102
    #define WM_KEYDOWN       0x0100
    #define WM_KEYUP         0x0101
    #define WM_SYSKEYDOWN    0x0104
    #define WM_SYSKEYUP      0x0105
    #define WM_SETCURSOR     0x0020
    #define WM_MOUSEMOVE     0x0200
    #define WM_LBUTTONDOWN   0x0201
    #define WM_LBUTTONUP     0x0202
    #define WM_MBUTTONDOWN   0x0207
    #define WM_MBUTTONUP     0x0208
    #define WM_RBUTTONDOWN   0x0204
    #define WM_RBUTTONUP     0x0205
    
    // Command and menu constants
    #define WM_COMMAND       0x0111
    
    // Window styles (not used on non-Windows)
    #define WS_OVERLAPPED    0x00000000
    #define WS_POPUP         0x80000000
    #define WS_CAPTION       0x00C00000
    #define WS_SYSMENU       0x00080000
    #define WS_MINIMIZEBOX   0x00020000
    #define WS_VISIBLE       0x10000000
    #define WS_CLIPCHILDREN  0x02000000
    #define WS_CLIPSIBLINGS  0x04000000
    #define WS_EX_APPWINDOW  0x00040000
    #define WS_EX_TOPMOST    0x00000008
    
    // ShowWindow constants
    #define SW_HIDE          0
    #define SW_SHOWNORMAL    1
    
    // GetWindowLong constants
    #define GWL_STYLE        (-16)
    
    // System metrics
    #define SM_CXSCREEN      0
    #define SM_CYSCREEN      1
    
    // Message constants (additional)
    #define PM_NOREMOVE      0x0000
    
    // Class styles
    #define CS_VREDRAW       0x0001
    #define CS_HREDRAW       0x0002
    #define CS_OWNDC         0x0020
    
    // Cursors
    #define IDC_ARROW        ((const char*)32512)
    
    // Colors
    #define COLOR_BACKGROUND 1
    
    // System commands
    #define SC_MAXIMIZE      0xF030
    #define SC_MOVE          0xF010
    #define SC_SIZE          0xF000
    #define SC_KEYMENU       0xF100
    #define SC_MONITORPOWER  0xF170
    
    // Macros
    #define LOWORD(l)        ((WORD)(((DWORD)(l)) & 0xffff))
    #define HIWORD(l)        ((WORD)((((DWORD)(l)) >> 16) & 0xffff))
    #define MAKEINTRESOURCE(i) ((LPSTR)((ULONG_PTR)((WORD)(i))))
    
    // Callback definition
    #define CALLBACK
    #define APIENTRY
    
    // String type
    typedef char TCHAR;
    
    // Calling convention macro
    #define __cdecl
    
    // Memory functions
    #define ZeroMemory(dest, len) memset(dest, 0, len)
    #define CopyMemory(dest, src, len) memcpy(dest, src, len)
    
    // String functions
    #define lstrlen(s) ((s) ? strlen(s) : 0)
    #define lstrcpy(d, s) strcpy(d, s)
    #define lstrcat(d, s) strcat(d, s)
    
    // RECT structure
    typedef struct tagRECT {
        int32_t left;
        int32_t top;
        int32_t right;
        int32_t bottom;
    } RECT;
    
    // POINT structure
    typedef struct tagPOINT {
        int32_t x;
        int32_t y;
    } POINT;
    
    // MSG structure
    typedef struct tagMSG {
        HWND   hwnd;
        UINT   message;
        WPARAM wParam;
        LPARAM lParam;
        DWORD  time;
        POINT  pt;
    } MSG;
#endif

#ifndef GS_PLATFORM_WINDOWS
    // Bitmap structures for image loading
    #pragma pack(push, 1)
    typedef struct tagBITMAPFILEHEADER {
        uint16_t bfType;
        uint32_t bfSize;
        uint16_t bfReserved1;
        uint16_t bfReserved2;
        uint32_t bfOffBits;
    } BITMAPFILEHEADER;
    
    typedef struct tagBITMAPINFOHEADER {
        uint32_t biSize;
        int32_t  biWidth;
        int32_t  biHeight;
        uint16_t biPlanes;
        uint16_t biBitCount;
        uint32_t biCompression;
        uint32_t biSizeImage;
        int32_t  biXPelsPerMeter;
        int32_t  biYPelsPerMeter;
        uint32_t biClrUsed;
        uint32_t biClrImportant;
    } BITMAPINFOHEADER;
    #pragma pack(pop)
#endif

// Platform-agnostic utility functions
namespace GS_Platform {
    unsigned long GetTickCount();
    void Sleep(unsigned long milliseconds);
    bool GetClientRect(HWND hwnd, RECT* rect);
    void SetRect(RECT* rect, int left, int top, int right, int bottom);
    bool PtInRect(const RECT* rect, POINT pt);
    
    void OutputDebugString(const char* message);
    int MessageBox(void* hwnd, const char* text, const char* caption, unsigned int type);
    void GetCurrentDirectory(int buflen, char* buffer);
    
    // Path normalization for cross-platform compatibility
    void NormalizePath(char* path);
} // namespace GS_Platform

#ifndef GS_PLATFORM_WINDOWS
    // PostQuitMessage needs to be available globally on non-Windows platforms
    void PostQuitMessage(int nExitCode);
#endif

// Wrapper macros for non-Windows platforms
#ifndef GS_PLATFORM_WINDOWS
    // Wrap Windows API calls to use GS_Platform namespace
    #define MessageBox(hwnd, text, caption, type) GS_Platform::MessageBox(hwnd, text, caption, type)
    #define GetCurrentDirectory(buflen, buffer) GS_Platform::GetCurrentDirectory(buflen, buffer)
    #define SetRect(rect, l, t, r, b) GS_Platform::SetRect(rect, l, t, r, b)
    #define SetCursor(cursor) SDL_ShowCursor((cursor) ? SDL_ENABLE : SDL_DISABLE)
    
    // SwapBuffers needs special handling - redirect to SDL_GL_SwapWindow
    // Note: This assumes g_pGSApp is available globally
    #define SwapBuffers(hdc) SDL_GL_SwapWindow((SDL_Window*)g_pGSApp->GetWindow())
#endif

// Forward declaration for global app pointer
class GS_Application;
extern GS_Application* g_pGSApp;

#endif // GS_PLATFORM_H
