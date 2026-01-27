/*============================================================================================+
 | Game System (GS) Library                                                                   |
 |--------------------------------------------------------------------------------------------|
 | FILES: gs_ogl_menu.cpp, gs_ogl_menu.h                                                      |
 |--------------------------------------------------------------------------------------------|
 | CLASS: GS_OGLMenu                                                                          |
 |--------------------------------------------------------------------------------------------|
 | ABOUT: ...                                                                                 |
 |--------------------------------------------------------------------------------------------|
 |                                                                                    08/2003 |
 +============================================================================================*/


#ifndef GS_OGL_MENU_H
#define GS_OGL_MENU_H


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
#include "gs_list.h"
#include "gs_object.h"
#include "gs_ogl_color.h"
#include "gs_ogl_font.h"
#include "gs_ogl_sprite_ex.h"
//==============================================================================================


//==============================================================================================
// Menu defines.
// ---------------------------------------------------------------------------------------------
#define MENU_TILES          16
#define MENU_TILES_PER_LINE 16
// ---------------------------------------------------------------------------------------------
#define MENU_LEFT            0
#define MENU_RIGHT           1
#define MENU_TOP_LEFT        2
#define MENU_TOP             3
#define MENU_TOP_RIGHT       4
#define MENU_BOTTOM_LEFT     5
#define MENU_BOTTOM          6
#define MENU_BOTTOM_RIGHT    7
#define MENU_SEPERATOR_LEFT  8
#define MENU_SEPERATOR       9
#define MENU_SEPERATOR_RIGHT 10
#define MENU_BACKGROUND      11
#define MENU_HIGHLIGHT_LEFT  12
#define MENU_HIGHLIGHT       13
#define MENU_HIGHLIGHT_RIGHT 14
#define MENU_EXTRA           15
//==============================================================================================


////////////////////////////////////////////////////////////////////////////////////////////////
// Class Definition. ///////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////


class GS_OGLMenu : public GS_Object
{

private:

    GS_List<char*> m_gsOptionList;

    // GS_RenderTarget m_gsTargets[1200];
    GS_OGLSpriteEx  m_gsMenuSprite;
    GS_OGLFont      m_gsMenuFont;

    BOOL m_bIsReady;

    long m_lDestX, m_lDestY;

    char* m_pszTitleString;

    int m_nHighlightOption;

protected:

    // ...

public:

    GS_OGLMenu();
    ~GS_OGLMenu();

    BOOL Create(const char* pszMenuPathname, const char* pszFontPathname, int nTilesPerLine,
                int nTileWidth, int nTileHeight, BOOL bFiltered = FALSE, BOOL bMipmap = TRUE);
    BOOL Create(const char* pszMenuPathname, GLuint glFontTexture, int nWidth, int nHeight,
                int nTilesPerLine, int nTileWidth, int nTileHeight, BOOL bFiltered = FALSE,
                BOOL bMipmap = TRUE);
    void Destroy();

    BOOL Render(HWND hWnd = NULL);
    BOOL Render(GS_OGLColor gsNormalColor, GS_OGLColor gsHighlightColor, HWND hWnd = NULL);

    BOOL  AddOption(const char* pszOptionString);
    BOOL  RemoveOption(int nIndex);
    BOOL  SetOption(int nIndex, const char* pszOptionString);
    char* GetOption(int nIndex);
    void  GetOptionRect(int nIndex, RECT* pDestRect);
    int   GetNumOptions();
    void  ClearOptions();

    int   GetHighlight();
    void  SetHighlight(int nIndex);
    char* GetHighlightOption();
    void  GetHighlightRect(RECT* pDestRect);
    void  HighlightNext(BOOL bSkipEmpty = TRUE);
    void  HighlightPrev(BOOL bSkipEmpty = TRUE);

    void SetTitle(const char* pszTextString);

    int GetCols();
    int GetRows();

    void GetScaleXY(float* pScaleX, float* pScaleY);
    void SetScaleXY(float glfScaleX, float glfScaleY);

    float GetScaleX();
    void SetScaleX(float glfScaleX);

    float GetScaleY();
    void SetScaleY(float glfScaleY);

    long GetWidth();
    long GetHeight();

    void SetDestX(long lDestX);
    long GetDestX();

    void SetDestY(long lDestY);
    long GetDestY();

    void GetDestRect(long* pLeft, long* pTop, long* pRight, long* pBottom);
    void GetDestRect(RECT* pDestRect);

    void GetModulateColor(GS_OGLColor* pgsColor);
    void GetModulateColor(float* pRed, float* pGreen, float* pBlue, float* pAlpha);
    void SetModulateColor(GS_OGLColor   gsColor);
    void SetModulateColor(float fRed = -1.0f, float fGreen = -1.0f, float fBlue = -1.0f,
                          float fAlpha = -1.0f);
};


////////////////////////////////////////////////////////////////////////////////////////////////

#endif
