/*============================================================================================+
 | Game System (GS) Library                                                                   |
 |--------------------------------------------------------------------------------------------|
 | FILES: gs_app.cpp, gs_app.h                                                                |
 |--------------------------------------------------------------------------------------------|
 | CLASS: GS_Application                                                                      |
 |--------------------------------------------------------------------------------------------|
 | ABOUT: ...                                                                                 |
 |--------------------------------------------------------------------------------------------|
 |                                                                                    08/2003 |
 +============================================================================================*/


#ifndef GS_APPLICATION_H
#define GS_APPLICATION_H


//==============================================================================================
// Defines enabling certain features. Simply comment out the features to disable.
// ---------------------------------------------------------------------------------------------
#define GS_ENABLE_ACCELERATORS
#define GS_ENABLE_MODESWITCH
#define GS_ENABLE_MENU
// ---------------------------------------------------------------------------------------------
#define GS_DEFAULT_WIDTH  640
#define GS_DEFAULT_HEIGHT 480
#define GS_DEFAULT_DEPTH  32
#define GS_DEFAULT_MODE   1
#define GS_DEFAULT_FPS    60.0f
//==============================================================================================


//==============================================================================================
// Include platform abstraction header files.
// ---------------------------------------------------------------------------------------------
#include "gs_platform.h"
//==============================================================================================

#ifdef GS_PLATFORM_WINDOWS
//==============================================================================================
// Include Windows header files (Windows only).
 // Already included via gs_platform.h
//==============================================================================================
#endif


//==============================================================================================
// Include standard C library header files.
// ---------------------------------------------------------------------------------------------
#include <stdio.h>
//==============================================================================================


//==============================================================================================
// Include resource header files.
// ---------------------------------------------------------------------------------------------
#include "gs_resource.h"
//==============================================================================================


//==============================================================================================
// Include Game System (GS) header files.
// ---------------------------------------------------------------------------------------------
#include "gs_error.h"
#include "gs_timer.h"
#include "gs_object.h"
//==============================================================================================


////////////////////////////////////////////////////////////////////////////////////////////////

class GS_Application : public GS_Object
{

private:

#ifdef GS_PLATFORM_WINDOWS
    HINSTANCE m_hInstance;
    HWND      m_hWnd;
    HDC       m_hDC;
    HMENU     m_hMenu;
    HACCEL    m_hAccel;
#else
    SDL_Window*   m_pWindow;
    SDL_GLContext m_glContext;
#endif

    BOOL m_bIsActive;         // Whether application is currently active.
    BOOL m_bIsWindowed;       // Whether application is in windowed or fullscreen modes.
    BOOL m_bIsReady;          // Whether application has been successfully initialized.
    BOOL m_bIsPaused;         // Whether application is paused or not.

    int m_nDisplayWidth;      // Display width in pixels.
    int m_nDisplayHeight;     // Display height in pixels.
    int m_nColorDepth;        // Display color depth in bits (8, 16, 24 or 32).

    DWORD m_dwWindowStyle;    // Style of the applications window.
    DWORD m_dwWindowStyleEx;  // Extended style of the applications window.
    RECT  m_rcWindowBounds;   // Screen coordinates of entire window area.
    RECT  m_rcWindowClient;   // Coordinates of window's client area.

    GS_FrameTimer m_gsFrameTimer; // Frame timer object.
    float         m_fFrameRate;   // Frame-rate of application.
    float         m_fFrameTime;   // Time it took to complete the last frame.
    float         m_fWaitTime;    // Time to wait between frames.

protected:

    virtual BOOL GameInit()
    {
        return TRUE;
    }
    virtual BOOL GameShutdown()
    {
        return TRUE;
    }
    virtual BOOL GameRelease()
    {
        return TRUE;
    }
    virtual BOOL GameRestore()
    {
        return TRUE;
    }
    virtual BOOL GameLoop()
    {
        return TRUE;
    }
    virtual void OnChangeMode()
    {
        return;
    }


public:

    GS_Application();
    ~GS_Application();

    BOOL Create(int nWidth, int nHeight, int nDepth, BOOL bIsWindowed);
    BOOL SetMode(int nWidth, int nHeight, int nDepth, BOOL bIsWindowed);
    void Destroy();

    INT Run();
    virtual LRESULT MsgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

    void SetFrameRate(float fFrameRate);
    void ShowFrameRate();

#ifdef GS_PLATFORM_WINDOWS
    HINSTANCE& GetInstance()
    {
        return m_hInstance;
    }
    HWND&      GetWindow()
    {
        return m_hWnd;
    }
    HDC&       GetDevice()
    {
        return m_hDC;
    }
#else
    void* GetInstance()
    {
        return NULL;
    }
    SDL_Window* GetWindow()
    {
        return m_pWindow;
    }
    SDL_Window* GetDevice()
    {
        return m_pWindow;
    }
#endif

    int GetWidth()
    {
        // return (m_rcWindowClient.right - m_rcWindowClient.left);
        return m_nDisplayWidth;
    }
    int GetHeight()
    {
        // return (m_rcWindowClient.bottom - m_rcWindowClient.top);
        return m_nDisplayHeight;
    }
    int GetColorDepth()
    {
        // if (m_bIsWindowed) return GetDeviceCaps(m_hDC, BITSPIXEL);
        // else return m_nColorDepth;
        return m_nColorDepth;
    }

    float GetCurrentFrameRate()
    {
        return m_gsFrameTimer.GetFrameRate();
    }
    float GetFrameRate()
    {
        return m_fFrameRate;
    }
    float GetFrameTime()
    {
        return m_fFrameTime;
    }

    void SetTitle(LPCTSTR lpString)
    {
#ifdef GS_PLATFORM_WINDOWS
        SetWindowText(m_hWnd, lpString);
#else
        SDL_SetWindowTitle(m_pWindow, lpString);
#endif
    }

    BOOL IsActive()
    {
        return m_bIsActive;
    }
    BOOL IsReady()
    {
        return m_bIsReady;
    }
    BOOL IsPaused()
    {
        return m_bIsPaused;
    }
    BOOL IsWindowed()
    {
        return m_bIsWindowed;
    }

    void Pause(BOOL bPause)
    {
        m_bIsPaused = bPause;
    }
    void Quit()
    {
#ifdef GS_PLATFORM_WINDOWS
        PostQuitMessage(0);
#else
        // Push SDL_QUIT event to signal the event loop to exit
        SDL_Event quitEvent;
        quitEvent.type = SDL_QUIT;
        SDL_PushEvent(&quitEvent);
#endif
    }
};

////////////////////////////////////////////////////////////////////////////////////////////////

#endif
