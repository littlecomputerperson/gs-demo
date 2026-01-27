/*============================================================================================+
 | Game System (GS) Library                                                                   |
 |--------------------------------------------------------------------------------------------|
 | FILES: gs_ogl_display.cpp, gs_ogl_display.h                                                |
 |--------------------------------------------------------------------------------------------|
 | CLASS: GS_OGLDisplay                                                                       |
 |--------------------------------------------------------------------------------------------|
 | ABOUT: ...                                                                                 |
 |--------------------------------------------------------------------------------------------|
 |                                                                                    08/2003 |
 +============================================================================================*/

#ifndef GS_OGL_DISPLAY_H
#define GS_OGL_DISPLAY_H


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
#include "gs_object.h"
//==============================================================================================


//==============================================================================================
// Global variables for adjusting the scale factor of sprites allowing upscaling and downscaling
// without affecting the game mechanics in any way. These variable will be declared and used in
// the gs_ogl_sprite.cpp file
// ---------------------------------------------------------------------------------------------
extern float g_fRenderModX;
extern float g_fRenderModY;
extern float g_fScaleFactorX;
extern float g_fScaleFactorY;
// ---------------------------------------------------------------------------------------------


////////////////////////////////////////////////////////////////////////////////////////////////
// Class Definition. ///////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////


class GS_OGLDisplay : public GS_Object
{

private:

#ifdef GS_PLATFORM_WINDOWS
    HGLRC  m_hRC;         // Permanent rendering context.
#else
    SDL_GLContext m_hRC;  // SDL OpenGL context.
#endif
    GLuint m_PixelFormat; // OpenGL pixel format.

    BOOL m_bIsAliasingEnabled; // Wether vertical sync is enabled or not.
    BOOL m_bIsVSyncEnabled;    // Wether vertical sync is enabled or not.

    BOOL m_bIsDepthTestEnabled; // Wether depth testing is enabled (for 2D rendering).
    BOOL m_bIsLightingEnabled;  // Wether lighting is enabled (for 2D rendering).

    BOOL m_bIsReady;      // Wether OpenGL display is ready or not.

protected:

    // No protected members.

public:

    GS_OGLDisplay();
    ~GS_OGLDisplay();

#ifdef GS_PLATFORM_WINDOWS
    BOOL Create(HDC& hDC, int nWidth, int nHeight, int nDepth);
#else
    BOOL Create(SDL_Window* pWindow, int nWidth, int nHeight, int nDepth);
#endif
    GLvoid Destroy();

    GLvoid SetViewport(GLsizei glWidth, GLsizei glHeight, GLdouble glView);

    GLvoid EnableAliasing(BOOL bEnable = TRUE, GLenum glType = GL_NICEST);
    GLvoid EnableVSync(BOOL bEnable = TRUE);
    GLvoid EnableDepthTesting(BOOL bEnable = TRUE, GLenum glType = GL_LEQUAL);
    GLvoid EnableBlending(BOOL bEnable = TRUE, GLenum glSourceFactor = GL_SRC_ALPHA,
                          GLenum glDestFactor = GL_ONE_MINUS_SRC_ALPHA);
    GLvoid EnableLighting(BOOL bEnable = TRUE, GLenum glLightNum = GL_LIGHT1);

    GLvoid SetLightAmbient( GLfloat fRed, GLfloat fGreen, GLfloat fBlue, GLfloat fAlpha,
                            GLenum glLightNum = GL_LIGHT1);
    GLvoid SetLightDiffuse( GLfloat fRed, GLfloat fGreen, GLfloat fBlue, GLfloat fAlpha,
                            GLenum glLightNum = GL_LIGHT1);
    GLvoid SetLightSpecular(GLfloat fRed, GLfloat fGreen, GLfloat fBlue, GLfloat fAlpha,
                            GLenum glLightNum = GL_LIGHT1);
    GLvoid SetLightPosition(GLfloat fCoordX, GLfloat fCoordY, GLfloat fCoordZ,
                            GLfloat fSource = 1.0f, GLenum glLightNum = GL_LIGHT1);

    GLvoid BeginRender();
#ifdef GS_PLATFORM_WINDOWS
    GLvoid EndRender(HDC& hDC);
#else
    GLvoid EndRender(SDL_Window* pWindow);
#endif

    BOOL   BeginRender2D(HWND& hWnd);
#ifdef GS_PLATFORM_WINDOWS
    BOOL   EndRender2D();
#else
    BOOL   BeginRender2D(void* pWindow);
    BOOL   EndRender2D();
#endif

    BOOL IsReady()
    {
        return m_bIsReady;
    }

    BOOL IsAliasingEnabled()
    {
        return m_bIsAliasingEnabled;
    }
    BOOL IsVSyncEnabled()
    {
        return m_bIsVSyncEnabled;
    }

    BOOL IsBlendingEnabled()
    {
        return glIsEnabled(GL_BLEND);
    }
    BOOL IsLightingEnabled()
    {
        return glIsEnabled(GL_LIGHTING);
    }
    BOOL IsDepthTestingEnabled()
    {
        return glIsEnabled(GL_DEPTH_TEST);
    }

    // Static functions for ajusting the scale factor
    static float GetScaleFactorX() { return g_fScaleFactorX; }
    static void SetScaleFactorX(float fScaleFactorX)
    {
        g_fScaleFactorX = fScaleFactorX;
    }

    static float GetScaleFactorY() { return g_fScaleFactorY; }
    static void SetScaleFactorY(float fScaleFactorY)
    {
        g_fScaleFactorY = fScaleFactorY;
    }

    static void SetScaleFactorXY(float fScaleFactorX, float fScaleFactorY)
    {
        g_fScaleFactorX = fScaleFactorX;
        g_fScaleFactorY = fScaleFactorY;
    }

    static float GetRenderModX() { return g_fRenderModX; }
    static void SetRenderModX(float fRenderModX)
    {
        g_fRenderModX = fRenderModX;
    }

    static float GetRenderModY() { return g_fRenderModY; }
    static void SetRenderModY(float fRenderModY)
    {
        g_fRenderModY = fRenderModY;
    }

    static void SetRenderModXY(float fRenderModX, float fRenderModY)
    {
        g_fRenderModX = fRenderModX;
        g_fRenderModY = fRenderModY;
    }
};


////////////////////////////////////////////////////////////////////////////////////////////////

#endif
