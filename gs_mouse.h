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

#ifndef GS_MOUSE_H
#define GS_MOUSE_H


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


////////////////////////////////////////////////////////////////////////////////////////////////
// Class Definition. ///////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////


class GS_Mouse : public GS_Object
{

private:

    int  m_nMouseX;
    int  m_nMouseY;

    BOOL m_bIsLeftPressed;
    BOOL m_bIsMiddlePressed;
    BOOL m_bIsRightPressed;

    BOOL m_bIsCursorHidden;

protected:

    // No protected members.

public:

    GS_Mouse();
    ~GS_Mouse();

    void Update(HWND hWnd);
    void Reset();

    void SetX(int nMouseX)
    {
        m_nMouseX = nMouseX;
    }
    void SetY(int nMouseY)
    {
        m_nMouseY = nMouseY;
    }

    int GetX()
    {
        return m_nMouseX;
    }
    int GetY()
    {
        return m_nMouseY;
    }

    void LeftPressed(BOOL bIsPressed)
    {
        m_bIsLeftPressed   = bIsPressed;
    }
    void MiddlePressed(BOOL bIsPressed)
    {
        m_bIsMiddlePressed = bIsPressed;
    }
    void RightPressed(BOOL bIsPressed)
    {
        m_bIsRightPressed  = bIsPressed;
    }

    BOOL IsLeftPressed()
    {
        return m_bIsLeftPressed;
    }
    BOOL IsMiddlePressed()
    {
        return m_bIsMiddlePressed;
    }
    BOOL IsRightPressed()
    {
        return m_bIsRightPressed;
    }

    BOOL IsInRect(RECT rcTemp);
    BOOL IsInWindow(HWND hWnd);
    BOOL IsInClient(HWND hWnd);

    void HideCursor(BOOL bHideCursor);
    BOOL IsCursorHidden()
    {
        return m_bIsCursorHidden;
    }
};


////////////////////////////////////////////////////////////////////////////////////////////////

#endif
