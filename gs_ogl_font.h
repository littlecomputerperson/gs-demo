/*============================================================================================+
 | Game System (GS) Library                                                                   |
 |--------------------------------------------------------------------------------------------|
 | FILES: gs_ogl_font.cpp, gs_ogl_font.h                                                      |
 |--------------------------------------------------------------------------------------------|
 | CLASS: GS_OGLFont                                                                          |
 |--------------------------------------------------------------------------------------------|
 | ABOUT: ...                                                                                 |
 |--------------------------------------------------------------------------------------------|
 |                                                                                    08/2003 |
 +============================================================================================*/


#ifndef GS_OGL_FONT_H
#define GS_OGL_FONT_H


//==============================================================================================
// Include platform abstraction header files.
// ---------------------------------------------------------------------------------------------
#include "gs_platform.h"
//==============================================================================================


//==============================================================================================
// Include OpenGL header files.
// ---------------------------------------------------------------------------------------------
#ifdef __APPLE__
    #include <OpenGL/gl.h>
    #include <OpenGL/glu.h>
#else
    #include <GL/gl.h>
    #include <GL/glu.h>
#endif
//==============================================================================================


//==============================================================================================
// Include Game System (GS) header files.
// ---------------------------------------------------------------------------------------------
#include "gs_error.h"
#include "gs_ogl_sprite.h"
//==============================================================================================


////////////////////////////////////////////////////////////////////////////////////////////////
// Class Definition. ///////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////


class GS_OGLFont : public GS_OGLSprite
{

private:

    // GS_RenderTarget m_gsTargets[160]; // ...

    char* m_pszTextString; // Text string to output to screen.

    int   m_nCharsPerLine; // Number of characters per line of the font image.
    int   m_nLetterWidth;  // Width of each letter.
    int   m_nLetterHeight; // Height of each letter.

protected:

    // ...

public:

    GS_OGLFont();
    ~GS_OGLFont();

    BOOL Create(const char* pszPathname, int nCharsPerLine, int nLetterWidth, int nLetterHeight,
                BOOL bFiltered = FALSE, BOOL bMipmap = TRUE);
    BOOL Create(GLuint glTexture, int nWidth, int nHeight, int nCharsPerLine,
                int nLetterWidth, int nLetterHeight);

    void Destroy();

    // BOOL Render(HWND hWnd = NULL);
    BOOL RenderChar(char cTextChar, HWND hWnd = NULL);
    BOOL Render(HWND hWnd = NULL);

    int  GetCharsPerLine();
    void SetCharsPerLine(int  nCharsPerLine);

    float GetLetterWidth();
    void  SetLetterWidth(int  nLetterWidth);

    float GetLetterHeight();
    void  SetLetterHeight(int  nLetterHeight);

    void GetText(char* pszTextString);
    void SetText(const char* pszTextString, ...);

    float GetTextWidth();
    float GetTextHeight();

    void GetDestRect(long* pLeft, long* pTop, long* pRight, long* pBottom);
    void GetDestRect(RECT* pDestRect);
};

////////////////////////////////////////////////////////////////////////////////////////////////

#endif
