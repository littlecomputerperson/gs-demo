/*============================================================================================+
 | Game System (GS) Library                                                                   |
 |--------------------------------------------------------------------------------------------|
 | FILES: gs_mouse.cpp, gs_mouse.h                                                            |
 |--------------------------------------------------------------------------------------------|
 | CLASS: GS_Mouse                                                                            |
 |--------------------------------------------------------------------------------------------|
 | ABOUT: ...                                                                                 |
 |--------------------------------------------------------------------------------------------|
 |                                                                                    02/2003 |
 +============================================================================================*/


//==============================================================================================
// Include Game System (GS) header files.
// ---------------------------------------------------------------------------------------------
#include "gs_mouse.h"
//==============================================================================================


////////////////////////////////////////////////////////////////////////////////////////////////
// Constructor/Destructor Methods //////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_Mouse::GS_Mouse():
// ---------------------------------------------------------------------------------------------
// Purpose: The constructor, initializes class data when class object is created.
// ---------------------------------------------------------------------------------------------
// Returns: Nothing.
//==============================================================================================

GS_Mouse::GS_Mouse()
{

    m_nMouseX = 0;
    m_nMouseY = 0;

    m_bIsLeftPressed   = FALSE;
    m_bIsMiddlePressed = FALSE;
    m_bIsRightPressed  = FALSE;

    m_bIsCursorHidden = FALSE;
}


////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_Mouse::~GS_Mouse():
// ---------------------------------------------------------------------------------------------
// Purpose: The destructor, de-initializes class data when class object is destroyed.
// ---------------------------------------------------------------------------------------------
// Returns: Nothing.
//==============================================================================================

GS_Mouse::~GS_Mouse()
{
#ifdef GS_PLATFORM_WINDOWS
    if (m_bIsCursorHidden)
    {
        if (ShowCursor(FALSE)<0)
        {
            while (ShowCursor(TRUE)<0);
        }
        m_bIsCursorHidden = FALSE;
    }
#else
    if (m_bIsCursorHidden)
    {
        SDL_ShowCursor(SDL_ENABLE);
        m_bIsCursorHidden = FALSE;
    }
#endif
}


////////////////////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////////////////////
// Update/Reset Methods ////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_Mouse::Update():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Returns: Nothing.
//==============================================================================================

void GS_Mouse::Update(HWND hWnd)
{
#ifdef GS_PLATFORM_WINDOWS
    MSG msg;
    while (PeekMessage(&msg, hWnd, 0, 0, PM_NOREMOVE))
    {
        if(!GetMessage(&msg, hWnd, 0, 0))
        {
            return;
        }
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
#else
    Uint32 buttons = SDL_GetMouseState(&m_nMouseX, &m_nMouseY);
    
    // Invert Y coordinate for OpenGL
    if (hWnd) {
        SDL_Window* window = (SDL_Window*)hWnd;
        int h;
        SDL_GetWindowSize(window, NULL, &h);
        m_nMouseY = h - m_nMouseY - 1;
    }
    
    m_bIsLeftPressed = (buttons & SDL_BUTTON(SDL_BUTTON_LEFT)) != 0;
    m_bIsRightPressed = (buttons & SDL_BUTTON(SDL_BUTTON_RIGHT)) != 0;
    m_bIsMiddlePressed = (buttons & SDL_BUTTON(SDL_BUTTON_MIDDLE)) != 0;
#endif
}


////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_Mouse::Reset():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Returns: Nothing.
//==============================================================================================

void GS_Mouse::Reset()
{
    m_nMouseX = 0;
    m_nMouseY = 0;

    m_bIsLeftPressed   = FALSE;
    m_bIsMiddlePressed = FALSE;
    m_bIsRightPressed  = FALSE;

#ifdef GS_PLATFORM_WINDOWS
    if (m_bIsCursorHidden)
    {
        if (ShowCursor(FALSE)<0)
        {
            while (ShowCursor(TRUE)<0);
        }
        m_bIsCursorHidden = FALSE;
    }
#else
    if (m_bIsCursorHidden)
    {
        SDL_ShowCursor(SDL_ENABLE);
        m_bIsCursorHidden = FALSE;
    }
#endif
}


////////////////////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_Mouse::IsInRect():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Returns: TRUE if mouse coordinates are within a specified rectangle, FALSE if not.
//==============================================================================================

BOOL GS_Mouse::IsInRect(RECT rcTemp)
{
#ifdef GS_PLATFORM_WINDOWS
    POINT ptMouseCoords;
    GetCursorPos(&ptMouseCoords);
    if (PtInRect(&rcTemp, ptMouseCoords))
    {
        return TRUE;
    }
    return FALSE;
#else
    POINT ptMouseCoords = { m_nMouseX, m_nMouseY };
    return GS_Platform::PtInRect(&rcTemp, ptMouseCoords);
#endif
}


////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_Mouse::IsInWindow():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Returns: TRUE if mouse coordinates are within the specified window, FALSE if not.
//==============================================================================================

BOOL GS_Mouse::IsInWindow(HWND hWnd)
{
#ifdef GS_PLATFORM_WINDOWS
    POINT ptMouseCoords;
    RECT  rcWindow;
    GetCursorPos(&ptMouseCoords);
    GetWindowRect(hWnd, &rcWindow);
    if (PtInRect(&rcWindow, ptMouseCoords))
    {
        return TRUE;
    }
    return FALSE;
#else
    if (!hWnd) return FALSE;
    
    SDL_Window* window = (SDL_Window*)hWnd;
    int w, h;
    SDL_GetWindowSize(window, &w, &h);
    
    return (m_nMouseX >= 0 && m_nMouseX < w && 
            m_nMouseY >= 0 && m_nMouseY < h);
#endif
}


////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_Mouse::IsInClient():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Returns: TRUE if mouse coordinates are within the client area of the specified window,
//          FALSE if not.
//==============================================================================================

BOOL GS_Mouse::IsInClient(HWND hWnd)
{
#ifdef GS_PLATFORM_WINDOWS
    POINT ptMouseCoords;
    RECT  rcClient;
    GetCursorPos(&ptMouseCoords);
    GetClientRect(hWnd, &rcClient);
    POINT ptClientTop = { rcClient.left,  rcClient.top    };
    POINT ptClientBot = { rcClient.right, rcClient.bottom };
    ClientToScreen(hWnd, &ptClientTop);
    ClientToScreen(hWnd, &ptClientBot);
    SetRect(&rcClient, ptClientTop.x, ptClientTop.y, ptClientBot.x, ptClientBot.y);
    if (PtInRect(&rcClient, ptMouseCoords))
    {
        return TRUE;
    }
    return FALSE;
#else
    return IsInWindow(hWnd); // Same for SDL
#endif
}


////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_Mouse::HideCursor():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Returns: Nothing.
//==============================================================================================

void GS_Mouse::HideCursor(BOOL bHideCursor)
{
#ifdef GS_PLATFORM_WINDOWS
    if (bHideCursor)
    {
        if (!m_bIsCursorHidden)
        {
            if (ShowCursor(FALSE)>=0)
            {
                while (ShowCursor(FALSE)>=0);
            }
            m_bIsCursorHidden = TRUE;
        }
    }
    else
    {
        if (m_bIsCursorHidden)
        {
            if (ShowCursor(FALSE)<0)
            {
                while (ShowCursor(TRUE)<0);
            }
            m_bIsCursorHidden = FALSE;
        }
    }
#else
    if (bHideCursor)
    {
        SDL_ShowCursor(SDL_DISABLE);
        m_bIsCursorHidden = TRUE;
    }
    else
    {
        SDL_ShowCursor(SDL_ENABLE);
        m_bIsCursorHidden = FALSE;
    }
#endif
}


////////////////////////////////////////////////////////////////////////////////////////////////
