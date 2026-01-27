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


//==============================================================================================
// Include Game System (GS) header files.
// ---------------------------------------------------------------------------------------------
#include "gs_ogl_sprite.h"
//==============================================================================================


//==============================================================================================
// Global variables decaled in the gs_ogl_display.h file which effects upscaling or downscaling
// without affecting the game mechanics
// ---------------------------------------------------------------------------------------------
float g_fScaleFactorX;
float g_fScaleFactorY;
float g_fRenderModX;
float g_fRenderModY;
// ---------------------------------------------------------------------------------------------


////////////////////////////////////////////////////////////////////////////////////////////////
// Constructor/Destructor Methods. /////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_OGLSprite::GS_OGLSprite():
// ---------------------------------------------------------------------------------------------
// Purpose: The constructor, initializes class data to defaults when class object is created.
// ---------------------------------------------------------------------------------------------
// Returns: Nothing.
//==============================================================================================

GS_OGLSprite::GS_OGLSprite()
{

    ZeroMemory(&m_SourceRect, sizeof(RECT));

    m_gluTexture = 0;

    m_bIsReady = FALSE;

    m_bLightingEnabled  = FALSE;
    m_bDepthTestEnabled = FALSE;

    m_gliWidth  = 0.0f;
    m_gliHeight = 0.0f;

    m_gliDestX = 0;
    m_gliDestY = 0;

    m_glfScrollX = 0.0f;
    m_glfScrollY = 0.0f;

    m_glfRotateX = 0.0f;
    m_glfRotateY = 0.0f;
    m_glfRotateZ = 0.0f;

    m_gliRotationPointX = -1;
    m_gliRotationPointY = -1;

    m_glfScaleX = 1.0f;
    m_glfScaleY = 1.0f;

    m_gsModulateColor.fRed   = 1.0f;
    m_gsModulateColor.fGreen = 1.0f;
    m_gsModulateColor.fBlue  = 1.0f;
    m_gsModulateColor.fAlpha = 1.0f;
}


////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_OGLSprite::~GS_OGLSprite():
// ---------------------------------------------------------------------------------------------
// Purpose: The de-structor, de-initializes class data when class object is destroyed and frees
//          all memory used by it.
// ---------------------------------------------------------------------------------------------
// Returns: Nothing.
//==============================================================================================

GS_OGLSprite::~GS_OGLSprite()
{

    this->Destroy();
}


////////////////////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////////////////////
// Create/Destroy Methods. /////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_OGLSprite::Create():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Details: If we wanted to create many sprites using the same image (or texture to be more
//          accurate) it would be far better to create a texture externally and then use the
//          GS_OGLSprite::Load() function that takes a texture as an argument to create the
//          sprite(s). If we created a new texture for each sprite we'd soon run out of memory.
// ---------------------------------------------------------------------------------------------
// Returns: TRUE if successfull, FALSE if not.
//==============================================================================================

BOOL GS_OGLSprite::Create(const char* pszPathname, BOOL bFiltered, BOOL bMipmap)
{

    // Can't create sprite if no valid pathname.
    if (!pszPathname)
    {
        return FALSE;
    }

    // Destroy sprite if created previously.
    if (m_bIsReady)
    {
        this->Destroy();
    }

    BOOL bResult;

    // Should the texture we create be filtered or not?
    if (bFiltered)
    {
        // Should texture be a mipmap?
        if (bMipmap)
        {
            // Create a filtered mipmap sprite texture from the specified file.
            bResult = m_gsTexture.Create(pszPathname, GS_MIPMAP, GL_LINEAR, GL_LINEAR);
        }
        else
        {
            // Create a filtered sprite texture from the specified file.
            bResult = m_gsTexture.Create(pszPathname, GS_IMAGE2D, GL_LINEAR, GL_LINEAR);
        }
    }
    else
    {
        // Should texture be a mipmap?
        if (bMipmap)
        {
            // Create an unfiltered mipmap sprite texture from the specified file.
            bResult = m_gsTexture.Create(pszPathname, GS_MIPMAP, GL_NEAREST, GL_NEAREST);
        }
        else
        {
            // Create an unfiltered sprite texture from the specified file.
            bResult = m_gsTexture.Create(pszPathname, GS_IMAGE2D, GL_NEAREST, GL_NEAREST);
        }
    }

    // Were we able to create the sprite texture?
    if (!bResult)
    {
        GS_Error::Report("GS_OGL_SPRITE.CPP", 147, "Failed to create sprite texture!");
        return FALSE;
    }

    // Remember the texture ID.
    m_gluTexture = m_gsTexture.GetID();

    // Determine the actual dimensions of the sprite.
    m_gliWidth  = m_gsTexture.GetWidth();
    m_gliHeight = m_gsTexture.GetHeight();

    // Set source rectangle to texture size.
    m_SourceRect.left   = 0;
    m_SourceRect.bottom = 0;
    m_SourceRect.right  = m_gliWidth;
    m_SourceRect.top    = m_gliHeight;

    m_bIsReady = TRUE;

    return TRUE;
}


//==============================================================================================
// GS_OGLSprite::Create():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Returns: TRUE if successfull, FALSE if not.
//==============================================================================================

BOOL GS_OGLSprite::Create(GLuint glTexture, int nWidth, int nHeight)
{

    // Can't create sprite if no valid texture.
    if (!glTexture)
    {
        return FALSE;
    }

    // Destroy sprite if created previously.
    if (m_bIsReady)
    {
        this->Destroy();
    }

    // Remember the texture ID.
    m_gluTexture = glTexture;

    // Determine the actual dimensions of the sprite.
    m_gliWidth  = nWidth;
    m_gliHeight = nHeight;

    // Set source rectangle to texture size.
    m_SourceRect.left   = 0;
    m_SourceRect.bottom = 0;
    m_SourceRect.right  = nWidth;
    m_SourceRect.top    = nHeight;

    m_bIsReady = TRUE;

    return TRUE;
}


////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_OGLSprite::Destroy():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Returns: Nothing.
//==============================================================================================

void GS_OGLSprite::Destroy()
{

    if (!m_bIsReady)
    {
        return;
    }

    if (m_gsTexture.IsReady())
    {
        m_gsTexture.Destroy();
    }

    ZeroMemory(&m_SourceRect, sizeof(RECT));

    m_gluTexture = 0;

    m_bLightingEnabled  = FALSE;
    m_bDepthTestEnabled = FALSE;

    m_gliWidth  = 0.0f;
    m_gliHeight = 0.0f;

    m_gliDestX = 0;
    m_gliDestY = 0;

    m_glfScrollX = 0.0f;
    m_glfScrollY = 0.0f;

    m_glfRotateX = 0.0f;
    m_glfRotateY = 0.0f;
    m_glfRotateZ = 0.0f;

    m_glfScaleX = 1.0f;
    m_glfScaleY = 1.0f;

    m_gsModulateColor.fRed   = 1.0f;
    m_gsModulateColor.fGreen = 1.0f;
    m_gsModulateColor.fBlue  = 1.0f;
    m_gsModulateColor.fAlpha = 1.0f;

    m_bIsReady = FALSE;
}


////////////////////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////////////////////
// Render Methods. /////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_OGLSprite::BeginRender():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Returns: TRUE if successfull, FALSE if not.
//==============================================================================================

BOOL GS_OGLSprite::BeginRender(HWND hWnd)
{

    // Can't render if no valid sprite.
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
        m_bDepthTestEnabled = TRUE;
    }
    else
    {
        m_bDepthTestEnabled = FALSE;
    }

    // Disable lighting if enabled.
    if (glIsEnabled(GL_LIGHTING) == GL_TRUE)
    {
        glDisable(GL_LIGHTING);
        m_bLightingEnabled = TRUE;
    }
    else
    {
        m_bLightingEnabled = FALSE;
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
// GS_OGLSprite::EndRender():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Returns: TRUE if successfull, FALSE if not.
//==============================================================================================

void GS_OGLSprite::EndRender()
{

    glMatrixMode(GL_PROJECTION); // Select the projection matrix.
    glPopMatrix();               // Restore the old projection matrix.
    glMatrixMode(GL_MODELVIEW);  // Select the modelview matrix.
    glPopMatrix();               // Restore the old modelview matrix.

    // Was depth testing enabled?
    if (m_bDepthTestEnabled == TRUE)
    {
        glEnable(GL_DEPTH_TEST);
    }

    // Was lighting enabled?
    if (m_bLightingEnabled == TRUE)
    {
        glEnable(GL_LIGHTING);
    }
}


////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_OGLSprite::Render():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Returns: TRUE if successfull, FALSE if not.
//==============================================================================================

BOOL GS_OGLSprite::Render(HWND hWnd)
{
    // Can't render if no valid sprite.
    if (!m_bIsReady)
    {
        return FALSE;
    }

    // Setup display for 2D rendering.
    if (hWnd != NULL)
    {
        this->BeginRender(hWnd);
    }

    // Adjust the default scale and coordinates by the scale factor (this causes upscaling or
    // downscaling without affecting the actual game mechanics)
    GLfloat glfScaleX = m_glfScaleX * g_fScaleFactorX;
    GLfloat glfScaleY = m_glfScaleY * g_fScaleFactorY;
    GLfloat glfDestX = m_gliDestX * g_fScaleFactorX;
    GLfloat glfDestY = m_gliDestY * g_fScaleFactorY;

    // Reset the modelview matrix.
    glLoadIdentity();

    // Draw sprite in a different hue or even with transparency depending on the alpha factor.
    glColor4f
    (
        m_gsModulateColor.fRed,
        m_gsModulateColor.fGreen,
        m_gsModulateColor.fBlue,
        m_gsModulateColor.fAlpha
    );

    // Select the sprite texture.
    glBindTexture(GL_TEXTURE_2D, m_gluTexture);

    // Set default rotation point in the center of the source rectangle
    int nRotationPointX = (float((m_SourceRect.right-m_SourceRect.left))/2);
    int nRotationPointY = (float((m_SourceRect.top-m_SourceRect.bottom))/2);

    // Use given rortation x point, if any
    if( m_gliRotationPointX >= 0)
    {
        nRotationPointX = m_gliRotationPointX;
    }

    // Use given rortation y point, if any
    if( m_gliRotationPointY >= 0)
    {
        nRotationPointY = m_gliRotationPointY;
    }

    // Set the sections used to draw the sprite as determined by the rotation point
    float fVertexWidth1 = float((nRotationPointX) * glfScaleX);
    float fVertexWidth2 = float((m_SourceRect.right - m_SourceRect.left - nRotationPointX) * glfScaleX);
    float fVertexHeight1 = float((nRotationPointY) * glfScaleY);
    float fVertexHeight2 = float((m_SourceRect.top - m_SourceRect.bottom - nRotationPointY) * glfScaleY);

    // Position the sprite (note that the sprite will be drawn around the rotation point).
    glTranslated(glfDestX + g_fRenderModX + fVertexWidth1, glfDestY + g_fRenderModY + fVertexHeight1, 0);

    // Rotate the sprite on the X, Y and Z-axis.
    glRotatef(m_glfRotateX, 1.0f, 0.0f, 0.0f);
    glRotatef(m_glfRotateY, 0.0f, 1.0f, 0.0f);
    glRotatef(m_glfRotateZ, 0.0f, 0.0f, 1.0f);

    // Determine the sprite dimensions as a factor of one, taking into account the coordinates
    // of the source rectangle as well as how far the sprite is scrolled on the x- or y-axis.
    float fLeft   = (1.0f / (float)m_gliWidth)  * (m_SourceRect.left   + m_glfScrollX);
    float fBottom = (1.0f / (float)m_gliHeight) * (m_SourceRect.bottom + m_glfScrollY);
    float fRight  = (1.0f / (float)m_gliWidth)  * (m_SourceRect.right  + m_glfScrollX);
    float fTop    = (1.0f / (float)m_gliHeight) * (m_SourceRect.top    + m_glfScrollY);

    // Start drawing a quad. In order for the sprite to be rotated correctly, the sprite has to
    // be drawn from the rotation point outwards.
    glBegin(GL_QUADS);

    // Bottom left texture coordinate and vertex.
    glTexCoord2f(fLeft, fBottom);
    glVertex2f(-fVertexWidth1, -fVertexHeight1);

    // Bottom right texture coordinate and vertex.
    glTexCoord2f(fRight, fBottom);
    glVertex2f( fVertexWidth2, -fVertexHeight1);

    // Top right texture coordinate and vertex.
    glTexCoord2f(fRight, fTop);
    glVertex2f( fVertexWidth2,  fVertexHeight2);

    // Top left texture coordinate and vertex.
    glTexCoord2f(fLeft, fTop);
    glVertex2f(-fVertexWidth1,  fVertexHeight2);

    glEnd();

    // Unselect the sprite texture.
    glBindTexture(GL_TEXTURE_2D, 0);  // Use 0 instead of NULL

    // Reset the color for other textures to white.
    glColor4f(1.0f, 1.0f, 1.0f, 1.0f);

    // Reset display after 2D rendering.
    if (hWnd != NULL)
    {
        this->EndRender();
    }

    return TRUE;
}


////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_OGLSprite::RenderTiles():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Returns: TRUE if successfull, FALSE if not.
//==============================================================================================


BOOL GS_OGLSprite::RenderTiles(RECT rcTileArea, BOOL bWrapAround, HWND hWnd)
{

    // Can't render if no valid sprite.
    if (!m_bIsReady)
    {
        return FALSE;
    }

    // Setup display for 2D rendering.
    if (hWnd != NULL)
    {
        this->BeginRender(hWnd);
    }

    // Adjust the default scale and coordinates by the scale factor (this causes upscaling or
    // downscaling without affecting the actual game mechanics)
    GLfloat glfScaleX = m_glfScaleX * g_fScaleFactorX;
    GLfloat glfScaleY = m_glfScaleY * g_fScaleFactorY;

    rcTileArea.left   = rcTileArea.left * g_fScaleFactorX; //  + g_fRenderModX;
    rcTileArea.top    = rcTileArea.top * g_fScaleFactorY;  // + g_fRenderModY;
    rcTileArea.right  = rcTileArea.right * g_fScaleFactorX; //  + g_fRenderModX;
    rcTileArea.bottom = rcTileArea.bottom * g_fScaleFactorY; // + g_fRenderModY;

    // Determine the width and height of the tile sprite.
    int nTileWidth  = (m_SourceRect.right-m_SourceRect.left) * glfScaleX;
    int nTileHeight = (m_SourceRect.top-m_SourceRect.bottom) * glfScaleY;

    // Should we clip the tiles to the dest area?
    if (bWrapAround)
    {
        // Create a scissor box that will automatically clip any rendering to fit.
        glScissor
        (
            rcTileArea.left,
            rcTileArea.bottom,
            rcTileArea.right - rcTileArea.left,
            rcTileArea.top - rcTileArea.bottom
        );

        // Enable the scissor box.
        glEnable(GL_SCISSOR_TEST);
    }

    // Draw sprite in a different hue or even with transparency depending on the alpha factor.
    glColor4f
    (
        m_gsModulateColor.fRed,
        m_gsModulateColor.fGreen,
        m_gsModulateColor.fBlue,
        m_gsModulateColor.fAlpha
    );

    // Select the sprite texture.
    glBindTexture(GL_TEXTURE_2D, m_gluTexture);

    // Determine the scaled sprite dimensions and divide by two to take into account that the
    // sprite is drawn from the center outwards.
    float fHalfWidth  = (float((m_SourceRect.right-m_SourceRect.left))/2) * glfScaleX;
    float fHalfHeight = (float((m_SourceRect.top-m_SourceRect.bottom))/2) * glfScaleY;

    // Tile the tile sprite over the entire display area.
    for (int nDestY = rcTileArea.bottom; nDestY < (int)rcTileArea.top; nDestY+= nTileHeight)
    {
        for (int nDestX = rcTileArea.left; nDestX < (int)rcTileArea.right; nDestX+= nTileWidth)
        {
            // Reset the modelview matrix.
            glLoadIdentity();

            // Position the sprite (the coordinates (0,0) is the bottom left corner of the screen).
            // Because the sprite is actually positioned from the center, some adjustments have to
            // be made for the sprite to be postioned by the bottom left corner.
            glTranslated(nDestX + g_fRenderModX + fHalfWidth, nDestY + g_fRenderModY + fHalfHeight, 0);

            // Rotate the sprite on the X, Y and Z-axis.
            glRotatef(m_glfRotateX, 1.0f, 0.0f, 0.0f);
            glRotatef(m_glfRotateY, 0.0f, 1.0f, 0.0f);
            glRotatef(m_glfRotateZ, 0.0f, 0.0f, 1.0f);

            // Determine the sprite dimensions as a factor of one, taking into account the coords
            // of the source rectangle as well as how far the sprite is scrolled.
            float fLeft   = (1.0f / (float)m_gliWidth)  * (m_SourceRect.left   + m_glfScrollX);
            float fBottom = (1.0f / (float)m_gliHeight) * (m_SourceRect.bottom + m_glfScrollY);
            float fRight  = (1.0f / (float)m_gliWidth)  * (m_SourceRect.right  + m_glfScrollX);
            float fTop    = (1.0f / (float)m_gliHeight) * (m_SourceRect.top    + m_glfScrollY);

            // Start drawing a quad. In order for it to be rotated correctly, the sprite has to
            // be drawn from a center point outwards. If the center of the sprite is (0,0) then the
            // bottom left corner of the sprite is (0-(sprite width/2),0-(sprite height/2)), the
            // bottom right corner of the sprite is (0+(sprite width/2),0-(sprite height/2)), the
            // top right corner of the sprite is (0+(sprite width/2),0+(sprite height/2)) and the
            // top left corner of the sprite is (0-(sprite width/2),0+(sprite height/2)).
            glBegin(GL_QUADS);

            // Bottom left texture coordinate and vertex.
            glTexCoord2f(fLeft,  fBottom);
            glVertex2f(-fHalfWidth, -fHalfHeight);

            // Bottom right texture coordinate and vertex.
            glTexCoord2f(fRight, fBottom);
            glVertex2f( fHalfWidth, -fHalfHeight);

            // Top right texture coordinate and vertex.
            glTexCoord2f(fRight, fTop   );
            glVertex2f( fHalfWidth,  fHalfHeight);

            // Top left texture coordinate and vertex.
            glTexCoord2f(fLeft,  fTop   );
            glVertex2f(-fHalfWidth,  fHalfHeight);

            glEnd();
        }
    }

    // Unselect the sprite texture.
    glBindTexture(GL_TEXTURE_2D, 0);  // Use 0 instead of NULL

    // Reset the color for other textures to white.
    glColor4f(1.0f, 1.0f, 1.0f, 1.0f);

    // Was clipping enabled?
    if (bWrapAround)
    {
        // Disable the scissor box.
        glDisable(GL_SCISSOR_TEST);
    }

    // Reset display after 2D rendering.
    if (hWnd != NULL)
    {
        this->EndRender();
    }

    return TRUE;
}


////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_OGLSprite::RenderTargets():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Returns: TRUE if successfull, FALSE if not.
//==============================================================================================

BOOL GS_OGLSprite::RenderTargets(int nNumTargets, GS_RenderTarget* pgsTargets, HWND hWnd)
{
    GLfloat glfScaleX;
    GLfloat glfScaleY;
    GLfloat glfDestX;
    GLfloat glfDestY;

    // Can't render if no valid sprite.
    if ((!m_bIsReady) || (nNumTargets <= 0) || (NULL == pgsTargets))
    {
        return FALSE;
    }

    // Setup display for 2D rendering.
    if (hWnd != NULL)
    {
        this->BeginRender(hWnd);
    }

    float fHalfWidth, fHalfHeight, fLeft, fBottom, fRight, fTop;

    // Select the sprite texture.
    glBindTexture(GL_TEXTURE_2D, m_gluTexture);

    // Start rendering multipile objects.
    for (int nLoop = 0; nLoop < nNumTargets; nLoop++)
    {
        // Adjust the default scale and coordinates by the scale factor (this causes
        // upscaling or downscaling without affecting the actual game mechanics)
        glfScaleX = pgsTargets[nLoop].fScaleX * g_fScaleFactorX;
        glfScaleY = pgsTargets[nLoop].fScaleY * g_fScaleFactorY;
        glfDestX = pgsTargets[nLoop].nDestX * g_fScaleFactorX;
        glfDestY = pgsTargets[nLoop].nDestY * g_fScaleFactorY;

        RECT rcDestRect;
        rcDestRect.left   = pgsTargets[nLoop].nSrcLeft * g_fScaleFactorX;
        rcDestRect.top    = pgsTargets[nLoop].nSrcTop * g_fScaleFactorY;
        rcDestRect.right  = pgsTargets[nLoop].nSrcRight * g_fScaleFactorX;
        rcDestRect.bottom = pgsTargets[nLoop].nSrcBottom * g_fScaleFactorY;

        // Reset the modelview matrix.
        glLoadIdentity();

        // Draw sprite in a different hue or even with transparency depending on the alpha factor.
        glColor4f(pgsTargets[nLoop].fRed, pgsTargets[nLoop].fGreen, pgsTargets[nLoop].fBlue,
                  pgsTargets[nLoop].fAlpha);

        // Determine the scaled sprite dimensions and divide by two to take into account that the
        // sprite is drawn from the center outwards.
        fHalfWidth  = (float((rcDestRect.right-rcDestRect.left))/2) * glfScaleX;
        fHalfHeight = (float((rcDestRect.top-rcDestRect.bottom))/2) * glfScaleY;

        // Position the sprite (the coordinates (0,0) is the bottom left corner of the screen).
        // Because the sprite is actually positioned from the center, some adjustments have to
        // be made for the sprite to be postioned by the bottom left corner.
        glTranslated(glfDestX + g_fRenderModX + fHalfWidth, glfDestY + g_fRenderModY + fHalfHeight, 0);

        // Rotate the sprite on the X, Y and Z-axis.
        glRotatef(pgsTargets[nLoop].fRotateX, 1.0f, 0.0f, 0.0f);
        glRotatef(pgsTargets[nLoop].fRotateY, 0.0f, 1.0f, 0.0f);
        glRotatef(pgsTargets[nLoop].fRotateZ, 0.0f, 0.0f, 1.0f);

        // Determine the sprite dimensions as a factor of one, taking into account the coordinates
        // of the source rectangle as well as how far the sprite is scrolled on the x- or y-axis.
        fLeft   = (1.0f / (float)m_gliWidth) * (rcDestRect.left + pgsTargets[nLoop].fScrollX);
        fBottom = (1.0f / (float)m_gliHeight) * (rcDestRect.bottom + pgsTargets[nLoop].fScrollY);
        fRight  = (1.0f / (float)m_gliWidth) * (rcDestRect.right + pgsTargets[nLoop].fScrollX);
        fTop    = (1.0f / (float)m_gliHeight) * (rcDestRect.top + pgsTargets[nLoop].fScrollY);

        // Start drawing a quad. In order for the sprite to be rotated correctly, the sprite has to
        // be drawn from a center point outwards. If the center of the sprite is (0, 0) then the
        // bottom left corner of the sprite is (0 - (sprite width / 2), 0 - (sprite height/2)), the
        // bottom right corner of the sprite is (0 + (sprite width / 2), 0 - (sprite height/2)), the
        // top right corner of the sprite is (0 + (sprite width / 2), 0 + (sprite height/2)) and the
        // top left corner of the sprite is (0 - (sprite width / 2), 0 + (sprite height/2)).
        glBegin(GL_QUADS);

        // Bottom left texture coordinate and vertex.
        glTexCoord2f(fLeft,  fBottom);
        glVertex2f(-fHalfWidth, -fHalfHeight);

        // Bottom right texture coordinate and vertex.
        glTexCoord2f(fRight, fBottom);
        glVertex2f( fHalfWidth, -fHalfHeight);

        // Top right texture coordinate and vertex.
        glTexCoord2f(fRight, fTop   );
        glVertex2f( fHalfWidth,  fHalfHeight);

        // Top left texture coordinate and vertex.
        glTexCoord2f(fLeft,  fTop   );
        glVertex2f(-fHalfWidth,  fHalfHeight);
        glEnd();
    }

    // Unselect the sprite texture.
    glBindTexture(GL_TEXTURE_2D, 0);  // Use 0 instead of NULL

    // Reset the color for other textures to white.
    glColor4f(1.0f, 1.0f, 1.0f, 1.0f);

    // Reset display after 2D rendering.
    if (hWnd != NULL)
    {
        this->EndRender();
    }

    return TRUE;
}


////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_OGLSprite::IsReady():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Returns: TRUE if created, FALSE if not.
//==============================================================================================

BOOL GS_OGLSprite::IsReady()
{

    return m_bIsReady;
}


////////////////////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////////////////////
// Add Methods. ////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_OGLSprite::AddDestX():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Returns: Nothing.
//==============================================================================================

void GS_OGLSprite::AddDestX(GLint gliAddX)
{

    m_gliDestX += gliAddX;
}


////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_OGLSprite::AddDestY():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Returns: Nothing.
//==============================================================================================

void GS_OGLSprite::AddDestY(GLint gliAddY)
{

    m_gliDestY += gliAddY;
}


////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_OGLSprite::AddScrollX():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Returns: Nothing.
//==============================================================================================

void GS_OGLSprite::AddScrollX(GLfloat glfDistance)
{

    m_glfScrollX -= glfDistance;

    // Has the sprite been scrolled the entire width?
    if (m_glfScrollX >= (float)m_gliWidth)
    {
        // Begin scrolling again from the start.
        m_glfScrollX = (m_glfScrollX - (float)m_gliWidth);
    }
    else if (m_glfScrollX <= -(float)m_gliWidth)
    {
        // Begin scrolling again from the start.
        m_glfScrollX = (m_glfScrollX + (float)m_gliWidth);
    }
}


////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_OGLSprite::AddScrollY():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Returns: Nothing.
//==============================================================================================

void GS_OGLSprite::AddScrollY(GLfloat glfDistance)
{

    m_glfScrollY -= glfDistance;

    // Has the sprite been scrolled the entire height?
    if (m_glfScrollY >= (float)m_gliHeight)
    {
        // Begin scrolling again from the start.
        m_glfScrollY = (m_glfScrollY - (float)m_gliHeight);
    }
    else if (m_glfScrollY <= -(float)m_gliHeight)
    {
        // Begin scrolling again from the start.
        m_glfScrollY = (m_glfScrollY + (float)m_gliHeight);
    }
}


////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_OGLSprite::AddRotateX():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Returns: Nothing.
//==============================================================================================

void GS_OGLSprite::AddRotateX(GLfloat glfRotateX)
{

    m_glfRotateX += glfRotateX;
}


////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_OGLSprite::AddRotateY():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Returns: Nothing.
//==============================================================================================

void GS_OGLSprite::AddRotateY(GLfloat glfRotateY)
{

    m_glfRotateY += glfRotateY;
}


////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_OGLSprite::AddRotateZ():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Returns: Nothing.
//==============================================================================================

void GS_OGLSprite::AddRotateZ(GLfloat glfRotateZ)
{

    m_glfRotateZ += glfRotateZ;
}


////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_OGLSprite::AddScaleX():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Returns: Nothing.
//==============================================================================================

void GS_OGLSprite::AddScaleX(GLfloat glfScaleX)
{

    m_glfScaleX += glfScaleX;

    if (m_glfScaleX < 0.0f)
    {
        m_glfScaleX = 0.0f;
    }
}


////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_OGLSprite::AddScaleY():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Returns: Nothing.
//==============================================================================================

void GS_OGLSprite::AddScaleY(GLfloat glfScaleY)
{

    m_glfScaleY += glfScaleY;

    if (m_glfScaleY < 0.0f)
    {
        m_glfScaleY = 0.0f;
    }
}


////////////////////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////////////////////
// Get/Set Methods. ////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_OGLSprite::GetModulateColor():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Returns: Nothing.
//==============================================================================================

void GS_OGLSprite::GetModulateColor(GS_OGLColor* pgsColor)
{

    *pgsColor = m_gsModulateColor;
}

void GS_OGLSprite::GetModulateColor(float* pRed, float* pGreen, float* pBlue, float* pAlpha)
{

    *pRed   = m_gsModulateColor.fRed;
    *pGreen = m_gsModulateColor.fGreen;
    *pBlue  = m_gsModulateColor.fBlue;
    *pAlpha = m_gsModulateColor.fAlpha;
}


//==============================================================================================
// GS_OGLSprite::SetModulateColor():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Returns: Nothing.
//==============================================================================================

void GS_OGLSprite::SetModulateColor(GS_OGLColor gsColor)
{

    m_gsModulateColor = gsColor;
}

void GS_OGLSprite::SetModulateColor(float fRed, float fGreen, float fBlue, float fAlpha)
{

    if (fRed   > -1.0f)
    {
        m_gsModulateColor.fRed   = fRed;
    }
    if (fGreen > -1.0f)
    {
        m_gsModulateColor.fGreen = fGreen;
    }
    if (fBlue  > -1.0f)
    {
        m_gsModulateColor.fBlue  = fBlue;
    }
    if (fAlpha > -1.0f)
    {
        m_gsModulateColor.fAlpha = fAlpha;
    }
}


////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_OGLSprite::GetSourceRect():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Returns: Nothing.
//==============================================================================================

void GS_OGLSprite::GetSourceRect(long* pLeft, long* pTop, long* pRight, long* pBottom)
{

    *pLeft   = m_SourceRect.left;
    *pTop    = m_SourceRect.top;
    *pRight  = m_SourceRect.right;
    *pBottom = m_SourceRect.bottom;
}


void GS_OGLSprite::GetSourceRect(RECT* pSourceRect)
{

    *pSourceRect = m_SourceRect;
}


//==============================================================================================
// GS_D3DBitmap::SetSourceRect():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Returns: Nothing.
//==============================================================================================

void GS_OGLSprite::SetSourceRect(long lLeft, long lTop, long lRight, long lBottom)
{

    m_SourceRect.left   = lLeft;
    m_SourceRect.top    = lTop;
    m_SourceRect.right  = lRight;
    m_SourceRect.bottom = lBottom;
}


void GS_OGLSprite::SetSourceRect(RECT SourceRect)
{

    m_SourceRect = SourceRect;
}


////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_OGLSprite::GetRotationPointX():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Returns: The point around which the sprite should rotate horizontally
//==============================================================================================

GLint GS_OGLSprite::GetRotationPointX()
{
    return m_gliRotationPointX;
}


////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_OGLSprite::SetRotationPointX():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Returns: Nothing
//==============================================================================================

void GS_OGLSprite::SetRotationPointX(GLint gliRotationPointX)
{
    if( gliRotationPointX < 0 )
    {
        m_gliRotationPointX = -1;
    }
    else if( gliRotationPointX > m_SourceRect.right - m_SourceRect.left )
    {
        m_gliRotationPointX = m_SourceRect.right - m_SourceRect.left;
    }
    else
    {
        m_gliRotationPointX = gliRotationPointX;
    }
}


////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_OGLSprite::GetRotationPointY():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Returns: The point around which the sprite should rotate vertically
//==============================================================================================

GLint GS_OGLSprite::GetRotationPointY()
{
    return m_gliRotationPointY;
}


////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_OGLSprite::SetRotationPointY():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Returns: Nothing
//==============================================================================================

void GS_OGLSprite::SetRotationPointY(GLint gliRotationPointY)
{
    if( gliRotationPointY < 0 )
    {
        m_gliRotationPointY = -1;
    }
    else if( gliRotationPointY > m_SourceRect.top - m_SourceRect.bottom )
    {
        m_gliRotationPointY = m_SourceRect.top - m_SourceRect.bottom;
    }
    else
    {
        m_gliRotationPointY = gliRotationPointY;
    }
}


////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_OGLSprite::SetRotationPointY():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Returns: Nothing
//==============================================================================================

void GS_OGLSprite::SetRotationPointXY(GLint gliRotationPointX, GLint gliRotationPointY)
{
    // Set the horizontal rotation point
    if( gliRotationPointX < 0 )
    {
        m_gliRotationPointX = -1;
    }
    else if( gliRotationPointX > m_SourceRect.right - m_SourceRect.left )
    {
        m_gliRotationPointX = m_SourceRect.right - m_SourceRect.left;
    }
    else
    {
        m_gliRotationPointX = gliRotationPointX;
    }

    // Set the vertical rotation point
    if( gliRotationPointY < 0 )
    {
        m_gliRotationPointY = -1;
    }
    else if( gliRotationPointY > m_SourceRect.top - m_SourceRect.bottom )
    {
        m_gliRotationPointY = m_SourceRect.top - m_SourceRect.bottom;
    }
    else
    {
        m_gliRotationPointY = gliRotationPointY;
    }
}


////////////////////////////////////////////////////////////////////////////////////////////////



//==============================================================================================
// GS_OGLSprite::GetRotateX():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Returns: Rotation around the X-axis in degrees.
//==============================================================================================

GLfloat GS_OGLSprite::GetRotateX()
{

    return m_glfRotateX;
}


////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_OGLSprite::SetRotateX():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Returns: Nothing.
//==============================================================================================

void GS_OGLSprite::SetRotateX(GLfloat glfRotateX)
{

    m_glfRotateX = glfRotateX;
}


////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_OGLSprite::GetRotateY():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Returns: Rotation around the Y-axis in degrees.
//==============================================================================================

GLfloat GS_OGLSprite::GetRotateY()
{

    return m_glfRotateY;
}


////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_OGLSprite::SetRotateY():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Returns: Nothing.
//==============================================================================================

void GS_OGLSprite::SetRotateY(GLfloat glfRotateY)
{

    m_glfRotateY = glfRotateY;
}


////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_OGLSprite::GetRotateZ():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Returns: Rotation around the Z-axis in degrees.
//==============================================================================================

GLfloat GS_OGLSprite::GetRotateZ()
{

    return m_glfRotateZ;
}


////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_OGLSprite::SetRotateZ():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Returns: Nothing.
//==============================================================================================

void GS_OGLSprite::SetRotateZ(GLfloat glfRotateZ)
{

    m_glfRotateZ = glfRotateZ;
}


////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_OGLSprite::GetScaleXY():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Returns: Nothing.
//==============================================================================================

void GS_OGLSprite::GetScaleXY(GLfloat* pScaleX, GLfloat* pScaleY)
{

    *pScaleX = m_glfScaleX;
    *pScaleY = m_glfScaleY;
}


////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_OGLSprite::SetScaleXY():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Returns: Nothing.
//==============================================================================================

void GS_OGLSprite::SetScaleXY(GLfloat glfScaleX, GLfloat glfScaleY)
{

    m_glfScaleX = glfScaleX;
    m_glfScaleY = glfScaleY;
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_OGLSprite::GetScaleX():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Returns: The horizontal scale of the sprite as a factor of 1.
//==============================================================================================

GLfloat GS_OGLSprite::GetScaleX()
{

    return m_glfScaleX;
}


////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_OGLSprite::SetScaleX():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Returns: Nothing.
//==============================================================================================

void GS_OGLSprite::SetScaleX(GLfloat glfScaleX)
{

    m_glfScaleX = glfScaleX;
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_OGLSprite::GetScaleY():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Returns: The vertical scale of the sprite as a factor of 1.
//==============================================================================================

GLfloat GS_OGLSprite::GetScaleY()
{

    return m_glfScaleY;
}


////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_OGLSprite::SetScaleY():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Returns: Nothing.
//==============================================================================================

void GS_OGLSprite::SetScaleY(GLfloat glfScaleY)
{

    m_glfScaleY = glfScaleY;
}


////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_OGLSprite::GetWidth():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Returns: A long value indicating the width of the sprite.
//==============================================================================================

long GS_OGLSprite::GetWidth()
{

    return (m_SourceRect.right - m_SourceRect.left);
}


////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_OGLSprite::GetHeight():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Returns: A long value indicating the height of the sprite.
//==============================================================================================

long GS_OGLSprite::GetHeight()
{

    return (m_SourceRect.top - m_SourceRect.bottom);
}


////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_OGLSprite::GetTextureWidth():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Returns: An integer value indicating the width of the sprite texture.
//==============================================================================================

GLint GS_OGLSprite::GetTextureWidth()
{

    return m_gliWidth;
}


////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_OGLSprite::GetTextureHeight():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Returns: An integer value indicating the height of the sprite texture.
//==============================================================================================

GLint GS_OGLSprite::GetTextureHeight()
{

    return m_gliHeight;
}


////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_OGLSprite::GetScaledWidth():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Returns: A float value indicating the actual width of the sprite.
//==============================================================================================

GLfloat GS_OGLSprite::GetScaledWidth()
{

    return (((m_SourceRect.right - m_SourceRect.left) * m_glfScaleX));
}


////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_OGLSprite::GetScaledHeight():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Returns: A float value indicating the actual height of the sprite.
//==============================================================================================

GLfloat GS_OGLSprite::GetScaledHeight()
{

    return (((m_SourceRect.top - m_SourceRect.bottom) * m_glfScaleY));
}


////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_OGLSprite::SetScaledWidth():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Returns: Nothing.
//==============================================================================================

void GS_OGLSprite::SetScaledWidth(GLint gliWidth)
{

    int nWidth = m_SourceRect.right - m_SourceRect.left;

    if ((nWidth > 0) && (gliWidth >= 0))
    {
        m_glfScaleX = (float)gliWidth / nWidth;
    }
}


////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_OGLSprite::SetScaledHeight():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Returns: Nothing.
//==============================================================================================

void GS_OGLSprite::SetScaledHeight(GLint gliHeight)
{

    int nHeight = m_SourceRect.top - m_SourceRect.bottom;

    if ((nHeight > 0) && (gliHeight >= 0))
    {
        m_glfScaleY = (float)gliHeight / nHeight;
    }
}


////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_OGLSprite::GetDestXY():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Returns: Nothing.
//==============================================================================================

void GS_OGLSprite::GetDestXY(GLint* pDestX, GLint* pDestY)
{

    *pDestX = m_gliDestX;
    *pDestY = m_gliDestY;
}


////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_OGLSprite::SetDestXY():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Returns: Nothing.
//==============================================================================================

void GS_OGLSprite::SetDestXY(GLint gliDestX, GLint gliDestY)
{

    m_gliDestX = gliDestX;
    m_gliDestY = gliDestY;
}


////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_OGLSprite::GetDestX():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Returns: An integer value indicating the X position of the sprite.
//==============================================================================================

GLint GS_OGLSprite::GetDestX()
{

    return m_gliDestX;
}


////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_OGLSprite::SetDestX():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Returns: Nothing.
//==============================================================================================

void GS_OGLSprite::SetDestX(GLint gliDestX)
{

    m_gliDestX = gliDestX;
}


////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_OGLSprite::GetDestY():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Returns: An integer value indicating the Y position of the sprite.
//==============================================================================================

GLint GS_OGLSprite::GetDestY()
{

    return m_gliDestY;
}


////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_OGLSprite::SetDestY():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Returns: Nothing.
//==============================================================================================

void GS_OGLSprite::SetDestY(GLint gliDestY)
{

    m_gliDestY = gliDestY;
}


////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_OGLSprite::GetScrollXY():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Returns: Nothing.
//==============================================================================================

void GS_OGLSprite::GetScrollXY(GLfloat* pScrollX, GLfloat* pScrollY)
{

    *pScrollX = m_glfScrollX;
    *pScrollY = m_glfScrollY;
}


////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_OGLSprite::SetScrollXY():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Returns: Nothing.
//==============================================================================================

void GS_OGLSprite::SetScrollXY(GLfloat glfScrollX, GLfloat glfScrollY)
{

    m_glfScrollX = glfScrollX;
    m_glfScrollY = glfScrollY;
}


////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_OGLSprite::GetScrollX():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Returns: A floating point value indicating how far the sprite is scrolled on the x-axis.
//==============================================================================================

GLfloat GS_OGLSprite::GetScrollX()
{

    return m_glfScrollX;
}


////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_OGLSprite::SetScrollX():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Returns: Nothing.
//==============================================================================================

void GS_OGLSprite::SetScrollX(GLfloat glfScrollX)
{

    m_glfScrollX = glfScrollX;
}


////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_OGLSprite::GetScrollY():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Returns: A floating point value indicating how far the sprite is scrolled on the y-axis.
//==============================================================================================

GLfloat GS_OGLSprite::GetScrollY()
{

    return m_glfScrollY;
}


////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_OGLSprite::SetScrollY():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Returns: Nothing.
//==============================================================================================

void GS_OGLSprite::SetScrollY(GLfloat glfScrollY)
{

    m_glfScrollY = glfScrollY;
}


////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_OGLSprite::GetDestRect():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Returns: Nothing.
//==============================================================================================

void GS_OGLSprite::GetDestRect(long* pLeft, long* pTop, long* pRight, long* pBottom)
{

    *pLeft   = m_gliDestX;
    *pBottom = m_gliDestY;
    *pRight  = m_gliDestX +  (long)this->GetScaledWidth();
    *pTop    = m_gliDestY + (long)this->GetScaledHeight();
}


void GS_OGLSprite::GetDestRect(RECT* pDestRect)
{
    SetRect(pDestRect, m_gliDestX, (m_gliDestY + (long)this->GetScaledHeight()),
            (m_gliDestX + (long)this->GetScaledWidth()), m_gliDestY);
}


////////////////////////////////////////////////////////////////////////////////////////////////
