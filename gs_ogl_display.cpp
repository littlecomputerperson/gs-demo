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


//==============================================================================================
// Include Game System (GS) header files.
// ---------------------------------------------------------------------------------------------
#include "gs_ogl_display.h"
//==============================================================================================


////////////////////////////////////////////////////////////////////////////////////////////////
// Constructor/Destructor Methods //////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_OGLDisplay::GS_OGLDisplay():
// ---------------------------------------------------------------------------------------------
// Purpose: The constructor, initializes class data when class object is created.
// ---------------------------------------------------------------------------------------------
// Returns: Nothing.
//==============================================================================================

GS_OGLDisplay::GS_OGLDisplay()
{
    m_hRC = NULL;
    m_PixelFormat = 0;

    m_bIsAliasingEnabled = FALSE;
    m_bIsVSyncEnabled    = FALSE;

    m_bIsDepthTestEnabled = FALSE;
    m_bIsLightingEnabled  = FALSE;

    m_bIsReady = FALSE;

    g_fRenderModX = 0.0f;
    g_fRenderModY = 0.0f;
    g_fScaleFactorX = 1.0f;
    g_fScaleFactorY = 1.0f;
}


////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_OGLDisplay::~GS_OGLDisplay():
// ---------------------------------------------------------------------------------------------
// Purpose: The destructor, de-initializes class data when class object is destroyed.
// ---------------------------------------------------------------------------------------------
// Returns: Nothing.
//==============================================================================================

GS_OGLDisplay::~GS_OGLDisplay()
{

    this->Destroy();
}


////////////////////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////////////////////
// Create/Destroy Methods //////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_OGLDisplay::Create():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Returns: TRUE if successfull,FALSE if failed.
//==============================================================================================

#ifdef GS_PLATFORM_WINDOWS
BOOL GS_OGLDisplay::Create(HDC& hDC, int nWidth, int nHeight, int nDepth)
#else
BOOL GS_OGLDisplay::Create(SDL_Window* pWindow, int nWidth, int nHeight, int nDepth)
#endif
{
    (void)nDepth; // Depth handled by SDL/OpenGL automatically
    
#ifdef GS_PLATFORM_WINDOWS
    // Did we get a valid window handle?
    if (!hDC)
    {
        return FALSE;
    }
#else
    if (!pWindow)
    {
        return FALSE;
    }
#endif

    // Destroy display if already created.
    if (m_bIsReady)
    {
        this->Destroy();
    }

#ifdef GS_PLATFORM_WINDOWS
    // Declare and set the pixel format descriptor.
    PIXELFORMATDESCRIPTOR pfd =
    {
        sizeof(PIXELFORMATDESCRIPTOR), // Size of this pixel format descriptor.
        1,                             // Version number.
        PFD_DRAW_TO_WINDOW |           // Format must support window.
        PFD_SUPPORT_OPENGL |           // Format must support OpenGL.
        PFD_DOUBLEBUFFER,              // Must support double buffering.
        PFD_TYPE_RGBA,                 // Request an RGBA format.
        (BYTE) nDepth,                 // Select our color depth.
        0, 0, 0, 0, 0, 0,              // Color bits ignored.
        0,                             // No alpha buffer.
        0,                             // Shift bit ignored.
        0,                             // No accumulation buffer.
        0, 0, 0, 0,                    // Accumulation bits ignored.
        16,                            // 16-Bit z-buffer (depth buffer).
        0,                             // No stencil buffer.
        0,                             // No Auxiliary Buffer.
        PFD_MAIN_PLANE,                // Main drawing layer.
        0,                             // Reserved.
        0, 0, 0                        // Layer masks ignored.
    };

    // Get a matching OpenGL pixel format.
    m_PixelFormat = ChoosePixelFormat(hDC, &pfd);

    // Did windows find a matching pixel format?
    if (!m_PixelFormat)
    {
        GS_Error::Report("GS_OGL_DISPLAY.CPP", 122, "Failed to find suitable pixel format!");
        return FALSE;
    }

    // Are we able to set the pixel format?
    if (!SetPixelFormat(hDC, m_PixelFormat, &pfd))
    {
        GS_Error::Report("GS_OGL_DISPLAY.CPP", 128, "Failed to set the pixel format!");
        return FALSE;
    }

    // Get an OpenGL rendering context.
    m_hRC = wglCreateContext(hDC);

    // Were we able to get a rendering context?
    if (!m_hRC)
    {
        GS_Error::Report("GS_OGL_DISPLAY.CPP", 137, "Failed to create GL rendering context!");
        return FALSE;
    }

    // Try to activate the rendering context.
    if (!wglMakeCurrent(hDC, m_hRC))
    {
        this->Destroy();
        GS_Error::Report("GS_OGL_DISPLAY.CPP", 144, "Failed to activate GL rendering context!");
        return FALSE;
    }
#else
    // SDL OpenGL initialization
    m_hRC = SDL_GL_CreateContext(pWindow);
    if (!m_hRC)
    {
        GS_Error::Report("GS_OGL_DISPLAY.CPP", 144, "Failed to create GL context!");
        return FALSE;
    }
    
    if (SDL_GL_MakeCurrent(pWindow, m_hRC) < 0)
    {
        this->Destroy();
        GS_Error::Report("GS_OGL_DISPLAY.CPP", 150, "Failed to activate GL context!");
        return FALSE;
    }
#endif

    // Display has been successfully created.
    m_bIsReady = TRUE;

    // Set the perspective of the OpenGL screen.
    this->SetViewport(nWidth, nHeight, 45.0f);

    // Set OpenGL clear color to black.
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    // Depth buffer setup.
    glClearDepth(1.0f);

    // Enable texture mapping.
    glEnable(GL_TEXTURE_2D);

    // Draw front and back faces of polygons.
    glDisable(GL_CULL_FACE);
    // Use depth testing.
    glEnable(GL_DEPTH_TEST);
    // The type of depth testing to do.
    glDepthFunc(GL_LEQUAL);
    // Enable smooth shading.
    glShadeModel(GL_SMOOTH);

    // Really nice perspective calculations.
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

    return TRUE;
}


////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_OGLDisplay::Destroy():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Returns: Nothing.
//==============================================================================================

GLvoid GS_OGLDisplay::Destroy()
{
    if (!m_bIsReady)
    {
        return;
    }

#ifdef GS_PLATFORM_WINDOWS
    if (m_hRC)
    {
        if (!wglMakeCurrent(NULL, NULL))
        {
            GS_Error::Report("GS_OGL_DISPLAY.CPP", 197, "Failed to release DC and RC!");
        }
        if (!wglDeleteContext(m_hRC))
        {
            GS_Error::Report("GS_OGL_DISPLAY.CPP", 200, "Failed to delete rendering context!");
        }
    }
#else
    if (m_hRC)
    {
        SDL_GL_DeleteContext(m_hRC);
    }
#endif
    m_hRC = NULL;

    m_PixelFormat = 0;

    m_bIsAliasingEnabled = FALSE;
    m_bIsVSyncEnabled    = FALSE;

    m_bIsDepthTestEnabled = FALSE;
    m_bIsLightingEnabled  = FALSE;

    m_bIsReady = FALSE;
}


////////////////////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////////////////////
// Utility Methods /////////////////////////////////////////////////////////////////////////////
//---------------------------------------------------------------------------------------------
// Purpose: Resize and initialize the GL Window.
// ---------------------------------------------------------------------------------------------
// Returns: Nothing.
//==============================================================================================

GLvoid GS_OGLDisplay::SetViewport(GLsizei glWidth, GLsizei glHeight, GLdouble glView)
{

    // Exit if display not ready.
    if (!m_bIsReady)
    {
        return;
    }

    // Prevent a divide by zero.
    if (glHeight==0)
    {
        glHeight=1;
    }

    // Set the OpenGL viewport.
    glViewport(0, 0, glWidth, glHeight);

    // Set projection matrix to the identity matrix.
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    // Calculate the aspect ratio of the window with the following parameters: field of view in
    // degrees (glView), the aspect ratio ((GLfloat)glWidth/(GLfloat)glHeight), front clipping
    // plane (0.1f), back clipping plane (100.0f).
    gluPerspective(glView, (GLfloat)glWidth / (GLfloat)glHeight, 0.1f, 100.0f);

    // Set the model view matrix to the identity matrix.
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}


////////////////////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////////////////////
// Display Setup Methods ///////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_OGLDisplay::EnableAliasing():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Returns: Nothing.
//==============================================================================================

GLvoid GS_OGLDisplay::EnableAliasing(BOOL bEnable, GLenum glType)
{

    // Should we enable anti-aliasing?
    if (bEnable)
    {
        // Set hints for line and polygon rasterization.
        glHint(GL_LINE_SMOOTH_HINT,    glType);
        glHint(GL_POLYGON_SMOOTH_HINT, glType);
    }
    else
    {
        // Set hints for line and polygon rasterization.
        glHint(GL_LINE_SMOOTH_HINT,    GL_FASTEST);
        glHint(GL_POLYGON_SMOOTH_HINT, GL_FASTEST);
    }

    m_bIsAliasingEnabled = bEnable;
}


////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_OGLDisplay::EnableVSync():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Returns: Nothing.
//==============================================================================================

GLvoid GS_OGLDisplay::EnableVSync(BOOL bEnable)
{
#ifdef GS_PLATFORM_WINDOWS
    typedef BOOL (APIENTRY* PFNWGLSWAPINTERVALEXTPROC)(int interval);
    PFNWGLSWAPINTERVALEXTPROC wglSwapIntervalEXT;

    wglSwapIntervalEXT = (PFNWGLSWAPINTERVALEXTPROC)wglGetProcAddress("wglSwapIntervalEXT");

    if (bEnable)
    {
        if (wglSwapIntervalEXT)
        {
            wglSwapIntervalEXT(1);
        }
    }
    else
    {
        if (wglSwapIntervalEXT)
        {
            wglSwapIntervalEXT(0);
        }
    }
#else
    SDL_GL_SetSwapInterval(bEnable ? 1 : 0);
#endif

    m_bIsVSyncEnabled = bEnable;
}


////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_OGLDisplay::EnableDepthTesting():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Returns: Nothing.
//==============================================================================================

GLvoid GS_OGLDisplay::EnableDepthTesting(BOOL bEnable, GLenum glType)
{

    // Should we enable depth testing?
    if (bEnable)
    {
        // Enable depth testing and set type.
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(glType);
    }
    else
    {
        // Disable depth testing.
        glDisable(GL_DEPTH_TEST);
    }
}


////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_OGLDisplay::EnableBlending():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Returns: Nothing.
//==============================================================================================

GLvoid GS_OGLDisplay::EnableBlending(BOOL bEnable, GLenum glSourceFactor, GLenum glDestFactor)
{

    // Should we enable blending?
    if (bEnable)
    {
        // Set type of blending and enable it.
        glBlendFunc(glSourceFactor, glDestFactor);
        glEnable(GL_BLEND);
    }
    else
    {
        // Disable blending.
        glDisable(GL_BLEND);
    }
}


////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_OGLDisplay::EnableLighting():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Returns: Nothing.
//==============================================================================================

GLvoid GS_OGLDisplay::EnableLighting(BOOL bEnable, GLenum glLightNum)
{

    // Should we enable lighting?
    if (bEnable)
    {
        // Enable lighting and set type.
        glEnable(GL_LIGHTING);
        glEnable(glLightNum);
    }
    else
    {
        // Disable lighting.
        glDisable(GL_LIGHTING);
    }
}


////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_OGLDisplay::SetLightAmbient():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Returns: Nothing.
//==============================================================================================

GLvoid GS_OGLDisplay::SetLightAmbient(GLfloat fRed, GLfloat fGreen, GLfloat fBlue,
                                      GLfloat fAlpha, GLenum glLightNum)
{

    GLfloat fLightAmbient[] = { fRed, fGreen, fBlue, fAlpha };

    glLightfv(glLightNum, GL_AMBIENT, fLightAmbient);
}


//==============================================================================================
// GS_OGLDisplay::SetLightDifuse():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Returns: Nothing.
//==============================================================================================

GLvoid GS_OGLDisplay::SetLightDiffuse(GLfloat fRed, GLfloat fGreen, GLfloat fBlue,
                                      GLfloat fAlpha, GLenum glLightNum)
{

    GLfloat fLightDiffuse[] = { fRed, fGreen, fBlue, fAlpha };

    glLightfv(glLightNum, GL_DIFFUSE, fLightDiffuse);
}


//==============================================================================================
// GS_OGLDisplay::SetLightSpecular():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Returns: Nothing.
//==============================================================================================

GLvoid GS_OGLDisplay::SetLightSpecular(GLfloat fRed, GLfloat fGreen, GLfloat fBlue,
                                       GLfloat fAlpha, GLenum glLightNum)
{

    GLfloat fLightSpecular[] = { fRed, fGreen, fBlue, fAlpha };

    glLightfv(glLightNum, GL_SPECULAR, fLightSpecular);
}


//==============================================================================================
// GS_OGLDisplay::SetLightPosition():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Returns: Nothing.
//==============================================================================================

GLvoid GS_OGLDisplay::SetLightPosition(GLfloat fCoordX, GLfloat fCoordY, GLfloat fCoordZ,
                                       GLfloat fSource, GLenum glLightNum)
{

    GLfloat fLightPosition[] = { fCoordX, fCoordY, fCoordZ, fSource };

    glLightfv(glLightNum, GL_POSITION, fLightPosition);
}


////////////////////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////////////////////
// Rendering Methods ///////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_OGLDisplay::BeginRender():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Returns: Nothing.
//==============================================================================================

GLvoid GS_OGLDisplay::BeginRender()
{

    // Exit if display not ready.
    if (!m_bIsReady)
    {
        return;
    }

    // Clear screen and depth buffer.
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Reset the current modelview matrix.
    glLoadIdentity();

    // The gluLookAt function creates a viewing matrix derived from an eye point (i.e. where
    // we're looking from (0,0,5)), a reference point indicating the center of the scene (i.e.
    // where we're looking at (0,0,0)), and an up vector (0,1,0).
    gluLookAt(0,0,5,0,0,0,0,1,0);
}


////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_OGLDisplay::EndRender():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Returns: Nothing.
//==============================================================================================

#ifdef GS_PLATFORM_WINDOWS
GLvoid GS_OGLDisplay::EndRender(HDC& hDC)
{
    if (!m_bIsReady)
    {
        return;
    }
    SwapBuffers(hDC);
}
#else
GLvoid GS_OGLDisplay::EndRender(SDL_Window* pWindow)
{
    if (!m_bIsReady)
    {
        return;
    }
    SDL_GL_SwapWindow(pWindow);
}
#endif


////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_OGLDisplay::BeginRender2D():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Returns: TRUE if successfull, FALSE if not.
//==============================================================================================

BOOL GS_OGLDisplay::BeginRender2D(HWND& hWnd)
{
    if ((!m_bIsReady) || (NULL == hWnd))
    {
        return FALSE;
    }

    RECT ClientRect;

#ifdef GS_PLATFORM_WINDOWS
    GetClientRect(hWnd, &ClientRect);
#else
    GS_Platform::GetClientRect(hWnd, &ClientRect);
#endif

    // Disable depth testing if enabled.
    if (glIsEnabled(GL_DEPTH_TEST) == GL_TRUE)
    {
        glDisable(GL_DEPTH_TEST);
        m_bIsDepthTestEnabled = TRUE;
    }
    else
    {
        m_bIsDepthTestEnabled = FALSE;
    }

    // Disable lighting if enabled.
    if (glIsEnabled(GL_LIGHTING) == GL_TRUE)
    {
        glDisable(GL_LIGHTING);
        m_bIsLightingEnabled = TRUE;
    }
    else
    {
        m_bIsLightingEnabled = FALSE;
    }

    glMatrixMode(GL_PROJECTION); // Select the projection matrix.
    glPushMatrix();              // Store the projection matrix.
    glLoadIdentity();            // Reset the projection matrix.

    // Set up an ortho screen with the same dimensions as the client area of the current
    // window and with a depth large enough to rotate the entire screen around either
    // the x or y axis.
    if (ClientRect.right >= ClientRect.top)
    {
        glOrtho(0,ClientRect.right,0,ClientRect.bottom,-((ClientRect.right-ClientRect.left)/2),
                ((ClientRect.right-ClientRect.left)/2));
    }
    else
    {
        glOrtho(0,ClientRect.right,0,ClientRect.bottom,-((ClientRect.top-ClientRect.bottom)/2),
                ((ClientRect.top-ClientRect.bottom)/2));
    }

    glMatrixMode(GL_MODELVIEW); // Select the modelview matrix.
    glPushMatrix();             // Store the modelview matrix.

    return TRUE;
}


////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_OGLDisplay::EndRender2D():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Returns: TRUE if successfull, FALSE if not.
//==============================================================================================

BOOL GS_OGLDisplay::EndRender2D()
{
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
    
    return TRUE;
}


////////////////////////////////////////////////////////////////////////////////////////////////


#ifndef GS_PLATFORM_WINDOWS
BOOL GS_OGLDisplay::BeginRender2D(void* pWindow)
{
    if (!m_bIsReady) return FALSE;

    RECT ClientRect;

    GS_Platform::GetClientRect(pWindow, &ClientRect);

    // Disable depth testing if enabled.
    if (glIsEnabled(GL_DEPTH_TEST) == GL_TRUE)
    {
        glDisable(GL_DEPTH_TEST);
        m_bIsDepthTestEnabled = TRUE;
    }
    else
    {
        m_bIsDepthTestEnabled = FALSE;
    }

    // Disable lighting if enabled.
    if (glIsEnabled(GL_LIGHTING) == GL_TRUE)
    {
        glDisable(GL_LIGHTING);
        m_bIsLightingEnabled = TRUE;
    }
    else
    {
        m_bIsLightingEnabled = FALSE;
    }

    glMatrixMode(GL_PROJECTION); // Select the projection matrix.
    glPushMatrix();              // Store the projection matrix.
    glLoadIdentity();            // Reset the projection matrix.

    // Set up an ortho screen with the same dimensions as the client area of the current
    // window and with a depth large enough to rotate the entire screen around either
    // the x or y axis.
    if (ClientRect.right >= ClientRect.top)
    {
        glOrtho(0,ClientRect.right,0,ClientRect.bottom,-((ClientRect.right-ClientRect.left)/2),
                ((ClientRect.right-ClientRect.left)/2));
    }
    else
    {
        glOrtho(0,ClientRect.right,0,ClientRect.bottom,-((ClientRect.top-ClientRect.bottom)/2),
                ((ClientRect.top-ClientRect.bottom)/2));
    }

    glMatrixMode(GL_MODELVIEW); // Select the modelview matrix.
    glPushMatrix();             // Store the modelview matrix.
    
    return TRUE;
}
#endif
