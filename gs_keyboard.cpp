/*============================================================================================+
 | Game System (GS) Library                                                                   |
 |--------------------------------------------------------------------------------------------|
 | FILES: gs_keyboard.cpp, gs_keyboard.h                                                      |
 |--------------------------------------------------------------------------------------------|
 | CLASS: GS_Keyboard                                                                         |
 |--------------------------------------------------------------------------------------------|
 | ABOUT: ...                                                                                 |
 |--------------------------------------------------------------------------------------------|
 |                                                                                    02/2003 |
 +============================================================================================*/


//==============================================================================================
// Include Game System (GS) header files.
// ---------------------------------------------------------------------------------------------
#include "gs_keyboard.h"
//==============================================================================================


////////////////////////////////////////////////////////////////////////////////////////////////


const char* GS_Keyboard::KeyCodeDescription[256] =
{
    0,"Left Mouse","Right Mouse","Control+Break","Middle Mouse",0,0,0,"Backspace","Tab",0,0,
    "Clear","Enter",0,0,"Shift","Control","Alt","Pause","Caps Lock",0,0,0,0,0,0,"Escape",0,0,0,0,
    "Space","Page Up","Page Down","End","Home","Left","Up","Right","Down","Select",0,"Execute",
    "Print Screen","Insert","Delete","Help","0","1","2","3","4","5","6","7","8","9",0,0,0,0,0,0,
    0,"A","B","C","D","E","F","G","H","I","J","K","L","M","N","O","P","Q","R","S","T","U","V",
    "W","X","Y","Z","Left Window","Right Window","AppMenu",0,0,"Num Pad 0","Num Pad 1",
    "Num Pad 2","Num Pad 3","Num Pad 4","Num Pad 5","Num Pad 6","Num Pad 7","Num Pad 8",
    "Num Pad 9","*","+","Separator","-",".","/","F1","F2","F3","F4","F5","F6","F7","F8","F9",
    "F10","F11","F12","F13","F14","F15","F16","F17","F18","F19","F20","F21","F22","F23","F24",0,
    0,0,0,0,0,0,0,"Num Lock","Scroll Lock",0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,":","+","<","-",">","?","`",0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,"[","|","]","\"",0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,"Attn","CrSel",
    "ExCel","Erase","Erase EOF","Play","Zoom",0,"Pa1","Clear"
};


////////////////////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////////////////////
// Constructor/Destructor Methods //////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_Keyboard::GS_Keyboard():
// ---------------------------------------------------------------------------------------------
// Purpose: The constructor, initializes class data when class object is created.
// ---------------------------------------------------------------------------------------------
// Returns: Nothing.
//==============================================================================================

GS_Keyboard::GS_Keyboard()
{

    for (int nLoop = 0; nLoop < 256; nLoop++)
    {
        m_bKeyPressed[nLoop] = FALSE;
    }

    ZeroMemory(m_nKeyBuffer, sizeof(m_nKeyBuffer));
    m_nItemsInBuffer = 0;
}


////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_Keyboard::~GS_Keyboard():
// ---------------------------------------------------------------------------------------------
// Purpose: The destructor, de-initializes class data when class object is destroyed.
// ---------------------------------------------------------------------------------------------
// Returns: Nothing.
//==============================================================================================

GS_Keyboard::~GS_Keyboard()
{

    this->ClearBuffer();
}


////////////////////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////////////////////
// Update/Reset Methods ////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_Keyboard::Update():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Returns: Nothing.
//==============================================================================================

void GS_Keyboard::Update(HWND hWnd)
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
    // For SDL, we don't need to do anything here since events are processed
    // in the main event loop and passed to MsgProc
    (void)hWnd; // Unused
#endif
}


////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_Keyboard::Reset():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Returns: Nothing.
//==============================================================================================

void GS_Keyboard::Reset()
{

    for (int nLoop = 0; nLoop < 256; nLoop++)
    {
        m_bKeyPressed[nLoop] = FALSE;
    }

    ZeroMemory(m_nKeyBuffer, sizeof(m_nKeyBuffer));
    m_nItemsInBuffer = 0;
}


////////////////////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////////////////////
// Methods for Getting Keyboard Input //////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_Keyboard::IsKeyDown():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Returns: TRUE or FALSE depending on wether specified key is pressed or not.
//==============================================================================================

BOOL GS_Keyboard::IsKeyDown(int nKeyCode)
{

    // Check if valid key code.
    if ((nKeyCode < 0) || (nKeyCode >255))
    {
        return FALSE;
    }

    return m_bKeyPressed[nKeyCode];
}


////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_Keyboard::IsCharDown():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Returns: TRUE or FALSE depending on wether specified key is pressed or not.
//==============================================================================================

BOOL GS_Keyboard::IsCharDown(unsigned short sAsciiKey)
{

    unsigned short sChar;

    // Check each key to see wether it is pressed or not.
    for (int nLoop=0; nLoop<256; nLoop++)
    {
        // Has a key been pressed?
        if (m_bKeyPressed[nLoop])
        {
            // Convert key code to ascii.
            this->KeyCodeToAscii(nLoop, &sChar);
            // Has the selected key been pressed?
            if (sAsciiKey == sChar)
            {
                return TRUE;
            }
        }
    }

    return FALSE;
}


////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_Keyboard::AreKeysDown():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Returns: TRUE or FALSE depending on wether specified keys are pressed or not.
//==============================================================================================

BOOL GS_Keyboard::AreKeysDown(int nNumKeys, int* pKeys)
{

    // Exit if wrong number of keys.
    if (nNumKeys < 1)
    {
        return FALSE;
    }

    // Check each key to see wether it is pressed or not.
    for (int nLoop=0; nLoop<nNumKeys; nLoop++)
    {
        // Has the specified key not been pressed?
        if (!m_bKeyPressed[*(pKeys+nLoop)])
        {
            return FALSE;
        }
    }

    // All of the selected keys were pressed.
    return TRUE;
}


////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_Keyboard::AreKeysUp():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Returns: TRUE or FALSE depending on wether specified keys are released or not.
//==============================================================================================

BOOL GS_Keyboard::AreKeysUp(int nNumKeys, int* pKeys)
{

    // Exit if wrong number of keys.
    if (nNumKeys < 1)
    {
        return FALSE;
    }

    // Check each key to see wether it is pressed or not.
    for (int nLoop=0; nLoop<nNumKeys; nLoop++)
    {
        // Has the specified key been pressed?
        if (m_bKeyPressed[*(pKeys+nLoop)])
        {
            return FALSE;
        }
    }

    // None of the selected keys were pressed.
    return TRUE;
}


////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_Keyboard::GetKeyPressed():
// ---------------------------------------------------------------------------------------------
// Purpose:
// ---------------------------------------------------------------------------------------------
// Returns: The virtual key code for the key that is pressed, or 0 if no key is pressed.
//==============================================================================================

int GS_Keyboard::GetKeyPressed()
{

    // Check each key to see wether it is pressed or not.
    for (int nLoop=0; nLoop<256; nLoop++)
    {
        // Has a key been pressed?
        if (m_bKeyPressed[nLoop])
        {
            // Return the virtual key code of the first key pressed.
            return nLoop;
        }
    }

    return 0;
}


////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_Keyboard::GetKeyPressed():
// ---------------------------------------------------------------------------------------------
// Purpose:
// ---------------------------------------------------------------------------------------------
// Returns: The virtual key code for the key that is pressed, or 0 if no key is pressed.
//==============================================================================================

int GS_Keyboard::GetKeyPressed(int nExcludeKey)
{

    // Check each key to see wether it is pressed or not.
    for (int nLoop=0; nLoop<256; nLoop++)
    {
        // Has a key been pressed that is not excluded?
        if ((m_bKeyPressed[nLoop]) && (nLoop != nExcludeKey))
        {
            // Return the virtual key code of the key pressed.
            return nLoop;
        }
    }

    return 0;
}


////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_Keyboard::GetKeyPressed():
// ---------------------------------------------------------------------------------------------
// Purpose:
// ---------------------------------------------------------------------------------------------
// Returns: The virtual key code for the key that is pressed, or 0 if no key is pressed.
//==============================================================================================

int GS_Keyboard::GetKeyPressed(int nFromKey, int nToKey)
{


    // Switch keys if necessary.
    if (nToKey < nFromKey)
    {
        int nTempKey = nFromKey;
        nFromKey = nToKey;
        nToKey = nTempKey;
    }

    // Make sure keys are within range (0-255);
    if ((nFromKey < 0) || (nFromKey >255))
    {
        nFromKey = 0;
    }

    if ((nToKey < 0) || (nToKey >255))
    {
        nToKey = 255;
    }

    // Check each key in range to see wether it is pressed or not.
    for (int nLoop = nFromKey; nLoop <= nToKey; nLoop++)
    {
        // Has a key been pressed?
        if (m_bKeyPressed[nLoop])
        {
            // Return the virtual key code of the key pressed.
            return nLoop;
        }
    }


    return 0;
}


////////////////////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////////////////////
// Buffered Input Methods //////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_Keyboard::AddKeyToBuffer():
// ---------------------------------------------------------------------------------------------
// Purpose:
// ---------------------------------------------------------------------------------------------
// Returns: TRUE if there was enough space in buffer, FALSE if a key was lost.
//==============================================================================================

BOOL GS_Keyboard::AddKeyToBuffer(int nVirtualKeyCode)
{

    BOOL bBufferFull = FALSE;

    // Is buffer full?
    if (m_nItemsInBuffer >= GS_MAX_BUFFER_SIZE)
    {
        // Remove oldest item from the buffer and shift the rest one down.
        for (int nLoop = 0; nLoop < m_nItemsInBuffer - 1; nLoop++)
        {
            m_nKeyBuffer[nLoop] = m_nKeyBuffer[nLoop+1];
        }
        // Decrease number of items in the buffer.
        m_nItemsInBuffer--;
        // Set last item in buffer to 0.
        m_nKeyBuffer[m_nItemsInBuffer] = 0;
        // Buffer is full, key was lost.
        bBufferFull = TRUE;
    }

    // Set the last postion in the buffer to the key code.
    m_nKeyBuffer[m_nItemsInBuffer] = nVirtualKeyCode;
    // Increase number of items in buffer.
    m_nItemsInBuffer++;

    return(!bBufferFull);
}


//==============================================================================================
// GS_Keyboard::ClearBuffer():
// ---------------------------------------------------------------------------------------------
// Purpose:
// ---------------------------------------------------------------------------------------------
// Returns: Nothing.
//==============================================================================================

void GS_Keyboard::ClearBuffer()
{

    // Remove all items from the keyboard buffer.
    ZeroMemory(m_nKeyBuffer, sizeof(m_nKeyBuffer));
    // Reset the number of items in the buffer.
    m_nItemsInBuffer = 0;
}


////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_Keyboard::GetBufferedKey():
// ---------------------------------------------------------------------------------------------
// Purpose:
// ---------------------------------------------------------------------------------------------
// Returns: The virtual key code for the latest key in the buffer, or 0 if buffer is empty.
//==============================================================================================

int GS_Keyboard::GetBufferedKey(BOOL bRemoveKey)
{

    int nKeyCode = 0;

    // Is there a key code in the buffer?
    if (m_nItemsInBuffer > 0)
    {
        // Get the last added key code.
        nKeyCode = m_nKeyBuffer[m_nItemsInBuffer-1];
        // Should we remove the key code?
        if (bRemoveKey)
        {
            // Remove the code from the buffer.
            m_nKeyBuffer[m_nItemsInBuffer-1] = 0;
            m_nItemsInBuffer--;
        }
    }

    return nKeyCode;
}


////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_Keyboard::GetBufferedChar():
// ---------------------------------------------------------------------------------------------
// Purpose:
// ---------------------------------------------------------------------------------------------
// Returns: The ascii code for the latest key in the buffer, or 0 if buffer is empty.
//==============================================================================================

unsigned short GS_Keyboard::GetBufferedChar(BOOL bRemoveKey)
{

    unsigned short sChar;

    // Get a key code from the keyboard buffer.
    int nKeyCode = this->GetBufferedKey(bRemoveKey);

    // Convert the virtual key code to ascii.
    this->KeyCodeToAscii(nKeyCode, &sChar);

    return sChar;
}



////////////////////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////////////////////
// Conversion Methods //////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_Keyboard::KeyCodeToAscii():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Returns: The number of characters extracted. It will return 0 in the case that there was no
//          conversion, or 1 or 2, depending multibyte character sets, for a valid output.
//==============================================================================================

int GS_Keyboard::KeyCodeToAscii(unsigned int uVirtualKey, unsigned short* sAsciiKey)
{
#ifdef GS_PLATFORM_WINDOWS
    unsigned char cKeyState[256] = { 0 };
    HKL hKeyboardLayout = GetKeyboardLayout(0);
    if (GetKeyboardState(cKeyState)==FALSE)
    {
        return 0;
    }
    unsigned int uScanCode = MapVirtualKeyEx(uVirtualKey, 0, hKeyboardLayout);
    return ToAsciiEx(uVirtualKey, uScanCode, cKeyState, sAsciiKey, 0, hKeyboardLayout);
#else
    *sAsciiKey = (unsigned short)uVirtualKey;
    return 1;
#endif
}


////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_Keyboard::GetKeyDescription():
// ---------------------------------------------------------------------------------------------
// Purpose:
// ---------------------------------------------------------------------------------------------
// Returns: A character string describing the DirectX key code.
//==============================================================================================

const char* GS_Keyboard::GetKeyDescription(int nVirtualKeyCode)
{

    // Is the key code within bounds?
    if ((nVirtualKeyCode < 0) || (nVirtualKeyCode > 255))
    {
        nVirtualKeyCode = 0;
    }

    return KeyCodeDescription[nVirtualKeyCode];
}


////////////////////////////////////////////////////////////////////////////////////////////////


#ifndef GS_PLATFORM_WINDOWS
// SDL scancode to GSK virtual key mapping
int GS_Keyboard::MapSDLKey(SDL_Scancode scancode) {
    switch (scancode) {
        case SDL_SCANCODE_ESCAPE: return GSK_ESCAPE;
        case SDL_SCANCODE_RETURN: return GSK_ENTER;
        case SDL_SCANCODE_SPACE: return GSK_SPACE;
        case SDL_SCANCODE_BACKSPACE: return GSK_BACK;
        case SDL_SCANCODE_TAB: return GSK_TAB;
        
        case SDL_SCANCODE_LEFT: return GSK_LEFT;
        case SDL_SCANCODE_RIGHT: return GSK_RIGHT;
        case SDL_SCANCODE_UP: return GSK_UP;
        case SDL_SCANCODE_DOWN: return GSK_DOWN;
        
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
        
        case SDL_SCANCODE_F1: return GSK_F1;
        case SDL_SCANCODE_F2: return GSK_F2;
        case SDL_SCANCODE_F3: return GSK_F3;
        case SDL_SCANCODE_F4: return GSK_F4;
        case SDL_SCANCODE_F5: return GSK_F5;
        case SDL_SCANCODE_F6: return GSK_F6;
        case SDL_SCANCODE_F7: return GSK_F7;
        case SDL_SCANCODE_F8: return GSK_F8;
        case SDL_SCANCODE_F9: return GSK_F9;
        case SDL_SCANCODE_F10: return GSK_F10;
        case SDL_SCANCODE_F11: return GSK_F11;
        case SDL_SCANCODE_F12: return GSK_F12;
        
        case SDL_SCANCODE_LSHIFT:
        case SDL_SCANCODE_RSHIFT: return GSK_SHIFT;
        case SDL_SCANCODE_LCTRL:
        case SDL_SCANCODE_RCTRL: return GSK_CONTROL;
        case SDL_SCANCODE_LALT:
        case SDL_SCANCODE_RALT: return GSK_ALT;
        
        case SDL_SCANCODE_PERIOD: return GSK_DECIMAL;
        case SDL_SCANCODE_DELETE: return GSK_DELETE;
        case SDL_SCANCODE_COMMA: return GSK_LESS;
        case SDL_SCANCODE_MINUS: return GSK_MINUS;
        case SDL_SCANCODE_EQUALS: return GSK_PLUS;
        case SDL_SCANCODE_SEMICOLON: return GSK_COLON;
        case SDL_SCANCODE_SLASH: return GSK_QUESTION;
        case SDL_SCANCODE_GRAVE: return GSK_APOSTROPHE;
        case SDL_SCANCODE_LEFTBRACKET: return GSK_LBRACKET;
        case SDL_SCANCODE_RIGHTBRACKET: return GSK_RBRACKET;
        case SDL_SCANCODE_BACKSLASH: return GSK_SEPARATOR;
        case SDL_SCANCODE_APOSTROPHE: return GSK_QUOTE;
        
        default: return GSK_NONE;
    }
}
#endif

////////////////////////////////////////////////////////////////////////////////////////////////
