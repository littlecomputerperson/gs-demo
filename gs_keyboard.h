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

#ifndef GS_KEYBOARD_H
#define GS_KEYBOARD_H


//==============================================================================================
// Include Game System (GS) header files.
// ---------------------------------------------------------------------------------------------
#include "gs_object.h"
//==============================================================================================


//==============================================================================================
// Include platform abstraction header files.
// ---------------------------------------------------------------------------------------------
#include "gs_platform.h"
//==============================================================================================


//==============================================================================================
// Buffer defines.
// ---------------------------------------------------------------------------------------------
#define GS_MAX_BUFFER_SIZE 10
//==============================================================================================


//==============================================================================================
// Virual Key Code defines.
// ---------------------------------------------------------------------------------------------
#define GSK_NONE        0x00
#define GSK_LMOUSE      0x01
#define GSK_RMOUSE      0x02
#define GSK_CANCEL      0x03 /*  control+break */
#define GSK_MMOUSE      0x04
#define GSK_BACK        0x08 /*  backspace key */
#define GSK_TAB         0x09
#define GSK_CLEAR       0x0C
#define GSK_ENTER       0x0D
#define GSK_SHIFT       0x10
#define GSK_CONTROL     0x11
#define GSK_ALT         0x12
#define GSK_PAUSE       0x13
#define GSK_CAPSLOCK    0x14
#define GSK_ESCAPE      0x1B
#define GSK_SPACE       0x20
#define GSK_PAGEUP      0x21
#define GSK_PAGEDOWN    0x22
#define GSK_END         0x23
#define GSK_HOME        0x24
// ---------------------------------------------------------------------------------------------
#define GSK_LEFT        0x25
#define GSK_UP          0x26
#define GSK_RIGHT       0x27
#define GSK_DOWN        0x28
// ---------------------------------------------------------------------------------------------
#define GSK_SELECT      0x29
#define GSK_EXECUTE     0x2B
#define GSK_PRINTSCREEN 0x2C
#define GSK_INSERT      0x2D
#define GSK_DELETE      0x2E
#define GSK_HELP        0x2F
// ---------------------------------------------------------------------------------------------
#define GSK_0           0x30
#define GSK_1           0x31
#define GSK_2           0x32
#define GSK_3           0x33
#define GSK_4           0x34
#define GSK_5           0x35
#define GSK_6           0x36
#define GSK_7           0x37
#define GSK_8           0x38
#define GSK_9           0x39
// ---------------------------------------------------------------------------------------------
#define GSK_A           0x41
#define GSK_B           0x42
#define GSK_C           0x43
#define GSK_D           0x44
#define GSK_E           0x45
#define GSK_F           0x46
#define GSK_G           0x47
#define GSK_H           0x48
#define GSK_I           0x49
#define GSK_J           0x4A
#define GSK_K           0x4B
#define GSK_L           0x4C
#define GSK_M           0x4D
#define GSK_N           0x4E
#define GSK_O           0x4F
#define GSK_P           0x50
#define GSK_Q           0x51
#define GSK_R           0x52
#define GSK_S           0x53
#define GSK_T           0x54
#define GSK_U           0x55
#define GSK_V           0x56
#define GSK_W           0x57
#define GSK_X           0x58
#define GSK_Y           0x59
#define GSK_Z           0x5A
// ---------------------------------------------------------------------------------------------
#define GSK_LWIN        0x5B /*  left window key  */
#define GSK_RWIN        0x5C /*  right window key */
#define GSK_APPS        0x5D /*  AppMenu key      */
// ---------------------------------------------------------------------------------------------
#define GSK_NUMPAD0     0x60
#define GSK_NUMPAD1     0x61
#define GSK_NUMPAD2     0x62
#define GSK_NUMPAD3     0x63
#define GSK_NUMPAD4     0x64
#define GSK_NUMPAD5     0x65
#define GSK_NUMPAD6     0x66
#define GSK_NUMPAD7     0x67
#define GSK_NUMPAD8     0x68
#define GSK_NUMPAD9     0x69
// ---------------------------------------------------------------------------------------------
#define GSK_MULTIPLY    0x6A /*  * on numeric keypad */
#define GSK_ADD         0x6B /*  + on numeric keypad */
#define GSK_SUBTRACT    0x6D /*  - on numeric keypad */
#define GSK_DECIMAL     0x6E /*  . on numeric keypad */
#define GSK_DIVIDE      0x6F /*  / on numeric keypad */
// ---------------------------------------------------------------------------------------------
#define GSK_F1          0x70
#define GSK_F2          0x71
#define GSK_F3          0x72
#define GSK_F4          0x73
#define GSK_F5          0x74
#define GSK_F6          0x75
#define GSK_F7          0x76
#define GSK_F8          0x77
#define GSK_F9          0x78
#define GSK_F10         0x79
#define GSK_F11         0x7A
#define GSK_F12         0x7B
#define GSK_F13         0x7C
#define GSK_F14         0x7D
#define GSK_F15         0x7E
#define GSK_F16         0x7F
#define GSK_F17         0x80
#define GSK_F18         0x81
#define GSK_F19         0x82
#define GSK_F20         0x83
#define GSK_F21         0x84
#define GSK_F22         0x85
#define GSK_F23         0x86
#define GSK_F24         0x87
// ---------------------------------------------------------------------------------------------
#define GSK_NUMLOCK     0x90
#define GSK_SCROLL      0x91 /* scroll lock key */
// ---------------------------------------------------------------------------------------------
// Note that the defines in the following section may differ from system to system.
// ---------------------------------------------------------------------------------------------
#define GSK_COLON       0xBA /* : on main keyboard */
#define GSK_PLUS        0xBB /* + on main keyboard */
#define GSK_LESS        0xBC /* < on main keyboard */
#define GSK_MINUS       0xBD /* - on main keyboard */
#define GSK_GREATER     0xBE /* > on main keyboard */
#define GSK_QUESTION    0xBF /* ? on main keyboard */
#define GSK_APOSTROPHE  0xC0 /* ` on main keyboard */
#define GSK_LBRACKET    0xDB /* [ on main keyboard */
#define GSK_SEPARATOR   0xDC /* | on main keyboard */
#define GSK_RBRACKET    0xDD /* ] on main keyboard */
#define GSK_QUOTE       0xDE /* " on main keyboard */
// ---------------------------------------------------------------------------------------------
#define GSK_ATTN        0xF6 /*  Attn key      */
#define GSK_CRSEL       0xF7 /*  CrSel key     */
#define GSK_EXSEL       0xF8 /*  ExSel key     */
#define GSK_EREOF       0xF9 /*  Erase EOF key */
#define GSK_PLAY        0xFA /*  Play key      */
#define GSK_ZOOM        0xFB /*  Zoom key      */
#define GSK_PA1         0xFD /*  PA1  key      */
#define GSK_OEM_CLEAR   0xFE /*  Clear key     */
//==============================================================================================


////////////////////////////////////////////////////////////////////////////////////////////////
// Class Definition. ///////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////


class GS_Keyboard : public GS_Object
{

private:

#ifdef GS_PLATFORM_WINDOWS
    BOOL m_bKeyPressed[256];
#else
    BOOL m_bKeyPressed[256];
#endif

    int m_nKeyBuffer[GS_MAX_BUFFER_SIZE];       // The keyboard buffer for buffered input.
    int m_nItemsInBuffer;                       // The number of items in the keybaord buffer.

    static const char* KeyCodeDescription[256]; // An array containing text descriptions of
    // each virtual keycode.

protected:

    // No protected members.

public:

    GS_Keyboard();
    ~GS_Keyboard();

#ifndef GS_PLATFORM_WINDOWS
    static int MapSDLKey(SDL_Scancode scancode);
#endif

    BOOL* GetKeys()
    {
        return m_bKeyPressed;
    }

    void Update(HWND hWnd);
    void Reset();

    void KeyUp(int nKeyCode)
    {
        m_bKeyPressed[nKeyCode] = FALSE;
    }
    void KeyDown(int nKeyCode)
    {
        m_bKeyPressed[nKeyCode] = TRUE;
    }

    BOOL IsKeyDown(int nKeyCode);
    BOOL IsCharDown(unsigned short sAsciiKey);
    BOOL AreKeysDown(int nNumKeys, int* pKeys);
    BOOL AreKeysUp(int nNumKeys, int* pKeys);

    int GetKeyPressed();
    int GetKeyPressed(int nExcludeKey);
    int GetKeyPressed(int nFromKey, int nToKey);

    int* GetBuffer()
    {
        return m_nKeyBuffer;
    }
    int  GetBufferSize()
    {
        return m_nItemsInBuffer;
    }
    BOOL AddKeyToBuffer(int nVirtualKeyCode);
    void ClearBuffer();
    int  GetBufferedKey(BOOL bRemoveKey = TRUE);
    unsigned short GetBufferedChar(BOOL bRemoveKey = TRUE);

    int KeyCodeToAscii(unsigned int uVirtualKey, unsigned short* sAsciiKey);
    const char* GetKeyDescription(int nVirtualKeyCode);
};


////////////////////////////////////////////////////////////////////////////////////////////////

#endif
