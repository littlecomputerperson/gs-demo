/*============================================================================================+
 | Game System (GS) Library                                                                   |
 |--------------------------------------------------------------------------------------------|
 | FILES: gs_ogl_sprite_ex.cpp, gs_ogl_sprite_ex.h                                            |
 |--------------------------------------------------------------------------------------------|
 | CLASS: GS_OGLSpriteEx                                                                      |
 |--------------------------------------------------------------------------------------------|
 | ABOUT: ...                                                                                 |
 |--------------------------------------------------------------------------------------------|
 |                                                                                    08/2003 |
 +============================================================================================*/


#ifndef GS_OGL_SPRITE_EX_H
#define GS_OGL_SPRITE_EX_H


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
#include "gs_ogl_texture.h"
//==============================================================================================


////////////////////////////////////////////////////////////////////////////////////////////////
// Class Definition. ///////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////


class GS_OGLSpriteEx : public GS_OGLSprite
{

private:

    int m_nTotalFrames;   // Total number of frames for the sprite.
    int m_nCurrentFrame;  // The currently selected sprite frame.
    int m_nFramesPerLine; // Number of frames per line of the sprite image.
    int m_nFrameWidth;    // Width of each frame.
    int m_nFrameHeight;   // Height of each frame.

    RECT* m_pFrameRects; // An array of rectangle structures used to hold the coordinates of
    // each frame of the sprite to avoid the overhead of having to
    // compute the coordinates each time a frame is rendered.

protected:

    // ...

public:

    GS_OGLSpriteEx();
    ~GS_OGLSpriteEx();

    BOOL Create(const char* pszPathname, int nTotalFrames, int nFramesPerLine, int nFrameWidth,
                int nFrameHeight, BOOL bFiltered = TRUE, BOOL bMipmap = TRUE);
    BOOL Create(GLuint glTexture, int nWidth, int nHeight, int nTotalFrames,
                int nFramesPerLine, int nFrameWidth, int nFrameHeight);

    void Destroy();

    BOOL Render(HWND hWnd = NULL);

    void AddFrame(int nFramesToAdd);

    int  GetFrame();
    void SetFrame(int nFrame);

    int  GetTotalFrames();
    void SetTotalFrames(int nTotalFrames);

    int  GetFramesPerLine();
    void SetFramesPerLine(int nFramesPerLine);

    GLfloat GetFrameWidth();
    GLfloat GetFrameHeight();
};


////////////////////////////////////////////////////////////////////////////////////////////////

#endif
