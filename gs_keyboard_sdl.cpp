/*============================================================================================+
 | Game System (GS) Library                                                                   |
 |--------------------------------------------------------------------------------------------|
 | FILES: gs_keyboard.cpp, gs_keyboard.h, gs_keyboard_sdl.cpp                                 |
 |--------------------------------------------------------------------------------------------|
 | CLASS: GS_Keyboard                                                                         |
 |--------------------------------------------------------------------------------------------|
 | ABOUT: SDL2 implementation of keyboard input handling. This file provides SDL-specific     |
 |        keyboard functionality for Linux and macOS platforms, mapping SDL scancodes to      |
 |        the unified GS keyboard virtual key codes.                                          |
 |--------------------------------------------------------------------------------------------|
 |                                                                                    01/2026 |
 +============================================================================================*/


#ifndef GS_PLATFORM_WINDOWS


//==============================================================================================
// Include Game System (GS) header files.
// ---------------------------------------------------------------------------------------------
#include "gs_keyboard.h"
//==============================================================================================


//==============================================================================================
// Include SDL2 header files.
// ---------------------------------------------------------------------------------------------
#include <SDL2/SDL.h>
//==============================================================================================


////////////////////////////////////////////////////////////////////////////////////////////////
// SDL Key Mapping Methods /////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_Keyboard::MapSDLKey():
// ---------------------------------------------------------------------------------------------
// Purpose: Maps SDL scancode values to GameSystem virtual key codes (GSK_*). This provides
//          a unified key code system across Windows and SDL2 platforms.
// ---------------------------------------------------------------------------------------------
// Parameters:
//    scancode - SDL_Scancode value from SDL keyboard events
// ---------------------------------------------------------------------------------------------
// Returns: Corresponding GSK_* virtual key code, or GSK_NONE if not mapped.
//==============================================================================================

int GS_Keyboard::MapSDLKey(SDL_Scancode scancode)
{
    switch (scancode)
    {
        // Special keys.
        case SDL_SCANCODE_ESCAPE:    return GSK_ESCAPE;
        case SDL_SCANCODE_RETURN:    return GSK_ENTER;
        case SDL_SCANCODE_SPACE:     return GSK_SPACE;
        case SDL_SCANCODE_BACKSPACE: return GSK_BACK;
        case SDL_SCANCODE_TAB:       return GSK_TAB;
        
        // Arrow keys.
        case SDL_SCANCODE_LEFT:  return GSK_LEFT;
        case SDL_SCANCODE_RIGHT: return GSK_RIGHT;
        case SDL_SCANCODE_UP:    return GSK_UP;
        case SDL_SCANCODE_DOWN:  return GSK_DOWN;
        
        // Letter keys (A-Z).
        case SDL_SCANCODE_A: return GSK_A;
        case SDL_SCANCODE_B: return GSK_B;
        case SDL_SCANCODE_C: return GSK_C;
        case SDL_SCANCODE_D: return GSK_D;
        case SDL_SCANCODE_E: return GSK_E;
        case SDL_SCANCODE_F: return GSK_F;
        case SDL_SCANCODE_G: return GSK_G;
        case SDL_SCANCODE_H: return GSK_H;
        case SDL_SCANCODE_I: return GSK_I;
        case SDL_SCANCODE_J: return GSK_J;
        case SDL_SCANCODE_K: return GSK_K;
        case SDL_SCANCODE_L: return GSK_L;
        case SDL_SCANCODE_M: return GSK_M;
        case SDL_SCANCODE_N: return GSK_N;
        case SDL_SCANCODE_O: return GSK_O;
        case SDL_SCANCODE_P: return GSK_P;
        case SDL_SCANCODE_Q: return GSK_Q;
        case SDL_SCANCODE_R: return GSK_R;
        case SDL_SCANCODE_S: return GSK_S;
        case SDL_SCANCODE_T: return GSK_T;
        case SDL_SCANCODE_U: return GSK_U;
        case SDL_SCANCODE_V: return GSK_V;
        case SDL_SCANCODE_W: return GSK_W;
        case SDL_SCANCODE_X: return GSK_X;
        case SDL_SCANCODE_Y: return GSK_Y;
        case SDL_SCANCODE_Z: return GSK_Z;
        
        // Number keys (0-9).
        case SDL_SCANCODE_0: return GSK_0;
        case SDL_SCANCODE_1: return GSK_1;
        case SDL_SCANCODE_2: return GSK_2;
        case SDL_SCANCODE_3: return GSK_3;
        case SDL_SCANCODE_4: return GSK_4;
        case SDL_SCANCODE_5: return GSK_5;
        case SDL_SCANCODE_6: return GSK_6;
        case SDL_SCANCODE_7: return GSK_7;
        case SDL_SCANCODE_8: return GSK_8;
        case SDL_SCANCODE_9: return GSK_9;
        
        // Function keys (F1-F12).
        case SDL_SCANCODE_F1:  return GSK_F1;
        case SDL_SCANCODE_F2:  return GSK_F2;
        case SDL_SCANCODE_F3:  return GSK_F3;
        case SDL_SCANCODE_F4:  return GSK_F4;
        case SDL_SCANCODE_F5:  return GSK_F5;
        case SDL_SCANCODE_F6:  return GSK_F6;
        case SDL_SCANCODE_F7:  return GSK_F7;
        case SDL_SCANCODE_F8:  return GSK_F8;
        case SDL_SCANCODE_F9:  return GSK_F9;
        case SDL_SCANCODE_F10: return GSK_F10;
        case SDL_SCANCODE_F11: return GSK_F11;
        case SDL_SCANCODE_F12: return GSK_F12;
        
        // Modifier keys (map both left and right variants to same GSK code).
        case SDL_SCANCODE_LSHIFT:
        case SDL_SCANCODE_RSHIFT:
            return GSK_SHIFT;
            
        case SDL_SCANCODE_LCTRL:
        case SDL_SCANCODE_RCTRL:
            return GSK_CONTROL;
            
        case SDL_SCANCODE_LALT:
        case SDL_SCANCODE_RALT:
            return GSK_ALT;
        
        // Unrecognized key.
        default:
            return GSK_NONE;
    }
}


////////////////////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////////////////////
// Keyboard State Update Methods ///////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_Keyboard::Update():
// ---------------------------------------------------------------------------------------------
// Purpose: Updates the keyboard state by polling SDL's keyboard state. This method queries
//          all keyboard keys and updates the internal key state array (m_bKeyPressed[]).
// ---------------------------------------------------------------------------------------------
// Parameters:
//    hWnd - Window handle (unused in SDL2 version, kept for API compatibility)
// ---------------------------------------------------------------------------------------------
// Returns: Nothing.
// ---------------------------------------------------------------------------------------------
// Notes: This method should be called once per frame to keep keyboard state current. SDL's
//        keyboard state is automatically updated by SDL_PollEvent() in the main event loop.
//==============================================================================================

void GS_Keyboard::Update(HWND hWnd)
{
    // Window handle not used in SDL2 implementation.
    (void)hWnd;
    
    // Get the current keyboard state from SDL (array of Uint8, one per scancode).
    const Uint8* keystate = SDL_GetKeyboardState(NULL);
    
    // Loop through all SDL scancodes and update corresponding GSK key states.
    for (int i = 0; i < SDL_NUM_SCANCODES; i++)
    {
        // Map SDL scancode to GameSystem virtual key code.
        int gsk = MapSDLKey((SDL_Scancode)i);
        
        // Is this a valid mapped key within our key array bounds?
        if (gsk != GSK_NONE && gsk < 256)
        {
            // Update the key state (TRUE if pressed, FALSE if released).
            m_bKeyPressed[gsk] = keystate[i];
        }
    }
}


////////////////////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////////////////////
// Key Code Conversion Methods /////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_Keyboard::KeyCodeToAscii():
// ---------------------------------------------------------------------------------------------
// Purpose: Converts a virtual key code to its ASCII character representation. This is a
//          simplified implementation for SDL2 platforms.
// ---------------------------------------------------------------------------------------------
// Parameters:
//    uVirtualKey - GameSystem virtual key code (GSK_*)
//    sAsciiKey   - Pointer to receive the ASCII character code
// ---------------------------------------------------------------------------------------------
// Returns: 1 if successful, 0 if conversion failed.
// ---------------------------------------------------------------------------------------------
// Notes: SDL handles text input differently via SDL_TEXTINPUT events. This simplified version
//        provides basic compatibility with the Windows implementation. For proper text input,
//        use SDL_TEXTINPUT events in your message handler.
//==============================================================================================

int GS_Keyboard::KeyCodeToAscii(unsigned int uVirtualKey, unsigned short* sAsciiKey)
{
    // SDL handles text input via SDL_TEXTINPUT events, not virtual key conversion.
    // This is a simplified compatibility implementation.
    *sAsciiKey = (unsigned short)uVirtualKey;
    
    return 1;
}


////////////////////////////////////////////////////////////////////////////////////////////////


#endif // !GS_PLATFORM_WINDOWS
