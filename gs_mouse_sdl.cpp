/*============================================================================================+
 | Game System (GS) Library                                                                   |
 |--------------------------------------------------------------------------------------------|
 | FILES: gs_mouse.cpp, gs_mouse.h, gs_mouse_sdl.cpp                                          |
 |--------------------------------------------------------------------------------------------|
 | CLASS: GS_Mouse                                                                            |
 |--------------------------------------------------------------------------------------------|
 | ABOUT: SDL2 implementation of mouse input handling. This file provides SDL-specific        |
 |        mouse functionality for Linux and macOS platforms, including cursor management      |
 |        and button state tracking.                                                          |
 |--------------------------------------------------------------------------------------------|
 |                                                                                    01/2026 |
 +============================================================================================*/


#ifndef GS_PLATFORM_WINDOWS


//==============================================================================================
// Include Game System (GS) header files.
// ---------------------------------------------------------------------------------------------
#include "gs_mouse.h"
#include "gs_platform.h"
//==============================================================================================


//==============================================================================================
// Include SDL2 header files.
// ---------------------------------------------------------------------------------------------
#include <SDL2/SDL.h>
//==============================================================================================


////////////////////////////////////////////////////////////////////////////////////////////////
// Mouse State Update Methods //////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_Mouse::Update():
// ---------------------------------------------------------------------------------------------
// Purpose: Updates the mouse state by querying SDL's mouse position and button states. This
//          method retrieves the current mouse coordinates and button press states.
// ---------------------------------------------------------------------------------------------
// Parameters:
//    hWnd - Window handle (unused in SDL2 version, kept for API compatibility)
// ---------------------------------------------------------------------------------------------
// Returns: Nothing.
// ---------------------------------------------------------------------------------------------
// Notes: This method should be called once per frame to keep mouse state current. Mouse
//        events are automatically processed by SDL_PollEvent() in the main event loop.
//==============================================================================================

void GS_Mouse::Update(HWND hWnd)
{
    // Window handle not used in SDL2 implementation.
    (void)hWnd;
    
    Uint32 buttons;
    
    // Get the current mouse state from SDL (position and button states).
    // SDL_GetMouseState() returns a bitmask of button states and fills in the coordinates.
    buttons = SDL_GetMouseState(&m_nMouseX, &m_nMouseY);
    
    // Update button states by testing bits in the button mask.
    // SDL_BUTTON(n) creates a bitmask for button n (1=left, 2=middle, 3=right).
    m_bIsLeftPressed   = (buttons & SDL_BUTTON(SDL_BUTTON_LEFT))   != 0;
    m_bIsRightPressed  = (buttons & SDL_BUTTON(SDL_BUTTON_RIGHT))  != 0;
    m_bIsMiddlePressed = (buttons & SDL_BUTTON(SDL_BUTTON_MIDDLE)) != 0;
}


////////////////////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////////////////////
// Cursor Visibility Methods ///////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_Mouse::HideCursor():
// ---------------------------------------------------------------------------------------------
// Purpose: Shows or hides the mouse cursor within the game window using SDL's cursor
//          visibility functions.
// ---------------------------------------------------------------------------------------------
// Parameters:
//    bHideCursor - TRUE to hide cursor, FALSE to show cursor
// ---------------------------------------------------------------------------------------------
// Returns: Nothing.
// ---------------------------------------------------------------------------------------------
// Notes: The cursor state is tracked in m_bIsCursorHidden to prevent redundant SDL calls.
//        SDL_ShowCursor(SDL_DISABLE) hides the cursor globally, not just in the game window.
//==============================================================================================

void GS_Mouse::HideCursor(BOOL bHideCursor)
{
    // Should we hide the cursor?
    if (bHideCursor && !m_bIsCursorHidden)
    {
        // Hide the mouse cursor.
        SDL_ShowCursor(SDL_DISABLE);
        
        // Remember that cursor is now hidden.
        m_bIsCursorHidden = TRUE;
    }
    // Should we show the cursor?
    else if (!bHideCursor && m_bIsCursorHidden)
    {
        // Show the mouse cursor.
        SDL_ShowCursor(SDL_ENABLE);
        
        // Remember that cursor is now visible.
        m_bIsCursorHidden = FALSE;
    }
}


////////////////////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////////////////////
// Mouse Position Test Methods /////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_Mouse::IsInWindow():
// ---------------------------------------------------------------------------------------------
// Purpose: Determines whether the mouse cursor is within the bounds of the game window.
// ---------------------------------------------------------------------------------------------
// Parameters:
//    hWnd - SDL window handle (SDL_Window* cast to HWND for API compatibility)
// ---------------------------------------------------------------------------------------------
// Returns: TRUE if mouse is within window bounds, FALSE otherwise.
// ---------------------------------------------------------------------------------------------
// Notes: Checks if mouse coordinates are within the window dimensions (0,0) to (width,height).
//==============================================================================================

BOOL GS_Mouse::IsInWindow(HWND hWnd)
{
    // Is window handle invalid?
    if (!hWnd)
    {
        return FALSE;
    }
    
    // Cast HWND back to SDL_Window* (platform abstraction).
    SDL_Window* window = (SDL_Window*)hWnd;
    
    int w, h;
    
    // Get the window dimensions.
    SDL_GetWindowSize(window, &w, &h);
    
    // Check if mouse coordinates are within window bounds.
    return (m_nMouseX >= 0 && m_nMouseX < w && 
            m_nMouseY >= 0 && m_nMouseY < h);
}


////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_Mouse::IsInClient():
// ---------------------------------------------------------------------------------------------
// Purpose: Determines whether the mouse cursor is within the client area of the window.
// ---------------------------------------------------------------------------------------------
// Parameters:
//    hWnd - SDL window handle (SDL_Window* cast to HWND for API compatibility)
// ---------------------------------------------------------------------------------------------
// Returns: TRUE if mouse is within client area, FALSE otherwise.
// ---------------------------------------------------------------------------------------------
// Notes: On SDL2 platforms, the entire window is the client area (no title bar like Windows),
//        so this is equivalent to IsInWindow(). Provided for API compatibility with Windows.
//==============================================================================================

BOOL GS_Mouse::IsInClient(HWND hWnd)
{
    // On SDL2 platforms, the client area is the same as the window area.
    return IsInWindow(hWnd);
}


////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_Mouse::IsInRect():
// ---------------------------------------------------------------------------------------------
// Purpose: Determines whether the mouse cursor is within a specified rectangular area.
// ---------------------------------------------------------------------------------------------
// Parameters:
//    rcTemp - RECT structure defining the test area (left, top, right, bottom)
// ---------------------------------------------------------------------------------------------
// Returns: TRUE if mouse is within the rectangle, FALSE otherwise.
// ---------------------------------------------------------------------------------------------
// Notes: Uses the platform abstraction layer's PtInRect() function for cross-platform
//        point-in-rectangle testing.
//==============================================================================================

BOOL GS_Mouse::IsInRect(RECT rcTemp)
{
    // Create a POINT structure from current mouse coordinates.
    POINT pt = { m_nMouseX, m_nMouseY };
    
    // Use platform-independent point-in-rectangle test.
    return GS_Platform::PtInRect(&rcTemp, pt);
}


////////////////////////////////////////////////////////////////////////////////////////////////


#endif // !GS_PLATFORM_WINDOWS
