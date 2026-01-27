/*============================================================================================+
 | Game System (GS) Library                                                                   |
 |--------------------------------------------------------------------------------------------|
 | FILES: gs_ogl_sprite.cpp, gs_ogl_sprite.h                                                  |
 |--------------------------------------------------------------------------------------------|
 | CLASS: GS_OGLSprite                                                                        |
 |--------------------------------------------------------------------------------------------|
 | ABOUT: ...                                                                                 |
 |--------------------------------------------------------------------------------------------|
 |                                                                                    08/2003 |
 +============================================================================================*/


#ifndef GS_OGL_SPRITE_H
#define GS_OGL_SPRITE_H


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
#include "gs_ogl_texture.h"
#include "gs_ogl_color.h"
//==============================================================================================


//==============================================================================================
// Render target structure.
// ---------------------------------------------------------------------------------------------
typedef struct GS_RENDER_TARGET
{
    GLint nDestX;      // The screen destination coordinates of the bottom left of the image.
    GLint nDestY;
    GLint nSrcTop;     // Source coordinates of the texture image.
    GLint nSrcLeft;
    GLint nSrcBottom;
    GLint nSrcRight;
    GLfloat fScaleX;   // Horizontal image scale (1.0f is normal).
    GLfloat fScaleY;
    GLfloat fRotateX;  // How much the image should be rotated (in degrees).
    GLfloat fRotateY;
    GLfloat fRotateZ;
    GLfloat fRed;      // The modulate color of the rendered image.
    GLfloat fGreen;
    GLfloat fBlue;
    GLfloat fAlpha;
    GLfloat fScrollX;  // How far a image should be scrolled.
    GLfloat fScrollY;
    GS_RENDER_TARGET()
    {
        nDestX = 0;
        nDestY = 0;
        nSrcTop    = 0;
        nSrcLeft   = 0;
        nSrcBottom = 0;
        nSrcRight  = 0;
        fScaleX  = 1.0f;
        fScaleY  = 1.0f;
        fRotateX = 0.0f;
        fRotateY = 0.0f;
        fRotateZ = 0.0f;
        fRed   = 1.0f;
        fGreen = 1.0f;
        fBlue  = 1.0f;
        fAlpha = 1.0f;
        fScrollX = 0.0f;
        fScrollY = 0.0f;
    }
} GS_RenderTarget;
//==============================================================================================


////////////////////////////////////////////////////////////////////////////////////////////////
// Class Definition. ///////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////


class GS_OGLSprite : public GS_Object
{

private:

    GS_OGLTexture m_gsTexture;  // The texture surface holding the sprite image.
    GLuint m_gluTexture;        // The texture surface holding the sprite image.

    RECT m_SourceRect; // The source rectangle containing the sprite.

    BOOL m_bIsReady; // Wether sprite object has been created or not.

    BOOL m_bLightingEnabled;  // Wether OpenGL lighting is enabled.
    BOOL m_bDepthTestEnabled; // Wether OpenGL depth testing is enabled.

    GLint m_gliWidth;  // The actual width of the sprite.
    GLint m_gliHeight; // The actual height of the sprite.

    GLint m_gliDestX; // The destination X position of the bottom left corner of the sprite.
    GLint m_gliDestY; // The destination Y position of the bottom left corner of the sprite.

    GLfloat m_glfScrollX; // How far a sprite should be scrolled on the x-axis.
    GLfloat m_glfScrollY; // How far a sprite should be scrolled on the y-axis.

    GLfloat m_glfRotateX; // How much the sprite should be rotated on the X-axis (in degrees)
    GLfloat m_glfRotateY; // How much the sprite should be rotated on the Y-axis (in degrees)
    GLfloat m_glfRotateZ; // How much the sprite should be rotated on the Z-axis (in degrees)

    GLint m_gliRotationPointX; // A point on the horizontal axis of the sprite around which the sprite will rotate
    GLint m_gliRotationPointY; // A point on the vertical axis of the sprite around which the sprite will rotate

    // GLfloat m_glfScaleFactor; // The scale factor of the sprite (used for up- or downscaling)
    GLfloat m_glfScaleX;  // Horizontal sprite scale (1.0f is normal)
    GLfloat m_glfScaleY;  // Verical sprite scale (1.0f is normal)

    GS_OGLColor m_gsModulateColor; // Color to modulate the sprite with.

protected:

    // ...

public:

    GS_OGLSprite();
    ~GS_OGLSprite();

    BOOL Create(const char* pszPathname, BOOL bFiltered = TRUE, BOOL bMipmap = TRUE);
    BOOL Create(GLuint glTexture, int nWidth, int nHeight);
    void Destroy();

    BOOL BeginRender(HWND hWnd);
    void EndRender();
    BOOL Render(HWND hWnd = NULL);
    BOOL RenderTiles(RECT rcDestRect, BOOL bClipRect = TRUE, HWND hWnd = NULL);
    BOOL RenderTargets(int nNumTargets, GS_RenderTarget* gsTargets = NULL, HWND hWnd = NULL);

    BOOL IsReady();

    void AddDestX(GLint gliAddX);
    void AddDestY(GLint gliAddY);

    void AddScrollX(GLfloat glfDistance);
    void AddScrollY(GLfloat glfDistance);

    void AddRotateX(GLfloat glfRotateX);
    void AddRotateY(GLfloat glfRotateY);
    void AddRotateZ(GLfloat glfRotateZ);

    void AddScaleX(GLfloat glfScaleX);
    void AddScaleY(GLfloat glfScaleY);

    void GetModulateColor(GS_OGLColor* pgsColor);
    void GetModulateColor(float* pRed, float* pGreen, float* pBlue, float* pAlpha);
    void SetModulateColor(GS_OGLColor  gsColor);
    void SetModulateColor(float fRed = -1.0f, float fGreen = -1.0f, float fBlue = -1.0f,
                          float fAlpha = -1.0f);

    void GetSourceRect(long* pLeft, long* pTop, long* pRight, long* pBottom);
    void GetSourceRect(RECT* pSourceRect);
    void SetSourceRect(long lLeft, long lTop, long lRight, long lBottom);
    void SetSourceRect(RECT SourceRect);

    GLint GetRotationPointX();
    void SetRotationPointX(GLint gliRotationPointX);

    GLint GetRotationPointY();
    void SetRotationPointY(GLint gliRotationPointY);

    void SetRotationPointXY(GLint gliRotationPointX, GLint gliRotationPointY);

    GLfloat GetRotateX();
    void SetRotateX(GLfloat glfRotateX);

    GLfloat GetRotateY();
    void SetRotateY(GLfloat glfRotateY);

    GLfloat GetRotateZ();
    void SetRotateZ(GLfloat glfRotateZ);

    void GetScaleXY(GLfloat* pScaleX, GLfloat* pScaleY);
    void SetScaleXY(GLfloat glfScaleX, GLfloat glfScaleY);

    GLfloat GetScaleX();
    void SetScaleX(GLfloat glfScaleX);

    GLfloat GetScaleY();
    void SetScaleY(GLfloat glfScaleY);

    long GetWidth();
    long GetHeight();

    GLint GetTextureWidth();
    GLint GetTextureHeight();

    GLfloat GetScaledWidth();
    GLfloat GetScaledHeight();

    void SetScaledWidth(GLint gliWidth);
    void SetScaledHeight(GLint gliHeight);

    void GetDestXY(GLint* pDestX, GLint* pDestY);
    void SetDestXY(GLint gliDestX, GLint gliDestY);

    GLint GetDestX();
    void  SetDestX(GLint gliDestX);

    GLint GetDestY();
    void  SetDestY(GLint gliDestY);

    void GetScrollXY(GLfloat* pScrollX, GLfloat* pScrollY);
    void SetScrollXY(GLfloat glfScrollX, GLfloat glfScrollY);

    GLfloat GetScrollX();
    void    SetScrollX(GLfloat glfScrollX);

    GLfloat GetScrollY();
    void    SetScrollY(GLfloat glfScrollY);

    void GetDestRect(long* pLeft, long* pTop, long* pRight, long* pBottom);
    void GetDestRect(RECT* pDestRect);
};


////////////////////////////////////////////////////////////////////////////////////////////////

#endif
