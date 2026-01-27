/*============================================================================================+
 | Game System (GS) Library                                                                   |
 |--------------------------------------------------------------------------------------------|
 | FILES: gs_ogl_particle.cpp, gs_ogl_particle.h                                              |
 |--------------------------------------------------------------------------------------------|
 | CLASS: GS_OGLParticle                                                                      |
 |--------------------------------------------------------------------------------------------|
 | ABOUT: ...                                                                                 |
 |--------------------------------------------------------------------------------------------|
 |                                                                                    08/2003 |
 +============================================================================================*/


//==============================================================================================
// Include Game System (GS) header files.
// ---------------------------------------------------------------------------------------------
#include "gs_ogl_particle.h"
//==============================================================================================


////////////////////////////////////////////////////////////////////////////////////////////////
// Constructor/Destructor Methods //////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_OGLParticle::GS_OGLParticle():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Returns: Nothing.
//==============================================================================================

GS_OGLParticle::GS_OGLParticle()
{

    m_gluTexture = 0;

    m_nNumParticles = 0;

    m_bIsReady = FALSE;

    m_bLightingEnabled  = FALSE;
    m_bDepthTestEnabled = FALSE;

    m_gliWidth  = 0;
    m_gliHeight = 0;

    for (int nLoop = 0; nLoop < MAX_PARTICLES; nLoop++)
    {
        m_gsParticles[nLoop].bIsActive = FALSE;
        m_gsParticles[nLoop].gliDestX = 0;
        m_gsParticles[nLoop].gliDestY = 0;
        m_gsParticles[nLoop].glfRotateX = 0.0f;
        m_gsParticles[nLoop].glfRotateY = 0.0f;
        m_gsParticles[nLoop].glfRotateZ = 0.0f;
        m_gsParticles[nLoop].glfScaleX = 1.0f;
        m_gsParticles[nLoop].glfScaleY = 1.0f;
        m_gsParticles[nLoop].glfR = 1.0f;
        m_gsParticles[nLoop].glfG = 1.0f;
        m_gsParticles[nLoop].glfB = 1.0f;
        m_gsParticles[nLoop].glfA = 1.0f;
    }
}


////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_OGLParticle::~GS_OGLParticle():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Returns: Nothing.
//==============================================================================================

GS_OGLParticle::~GS_OGLParticle()
{

    this->Destroy();
}


////////////////////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////////////////////
// Create/Destroy Methods. /////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_OGLParticle::Create():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Returns: TRUE if successfull, FALSE if not.
//==============================================================================================

BOOL GS_OGLParticle::Create(const char* pszPathname,int nNumParticles,BOOL bFiltered,BOOL bMipmap)
{

    // Can't create particle if no valid pathname.
    if (!pszPathname)
    {
        return FALSE;
    }

    // Destroy particle if created previously.
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
            // Create a filtered mipmap particle texture from the specified file.
            bResult = m_gsTexture.Create(pszPathname, GS_MIPMAP, GL_LINEAR, GL_LINEAR);
        }
        else
        {
            // Create a filtered particle texture from the specified file.
            bResult = m_gsTexture.Create(pszPathname, GS_IMAGE2D, GL_LINEAR, GL_LINEAR);
        }
    }
    else
    {
        // Should texture be a mipmap?
        if (bMipmap)
        {
            // Create an unfiltered mipmap particle texture from the specified file.
            bResult = m_gsTexture.Create(pszPathname, GS_MIPMAP, GL_NEAREST, GL_NEAREST);
        }
        else
        {
            // Create an unfiltered particle texture from the specified file.
            bResult = m_gsTexture.Create(pszPathname, GS_IMAGE2D, GL_NEAREST, GL_NEAREST);
        }
    }

    // Were we able to create the particle texture?
    if (!bResult)
    {
        GS_Error::Report("GS_OGL_PARTICLE.CPP", 138, "Failed to create particle texture!");
        return FALSE;
    }

    // Remember the texture ID.
    m_gluTexture = m_gsTexture.GetID();

    // Determine the actual dimensions of the particle.
    m_gliWidth  = m_gsTexture.GetWidth();
    m_gliHeight = m_gsTexture.GetHeight();

    // Set how many particles are active initially.
    if (nNumParticles > MAX_PARTICLES)
    {
        nNumParticles = MAX_PARTICLES;
    }
    else if (nNumParticles < 0)
    {
        nNumParticles = 0;
    }

    m_nNumParticles = nNumParticles;

    // Activate the specified number of particles.
    for (int nLoop = 0; nLoop < m_nNumParticles; nLoop++)
    {
        m_gsParticles[nLoop].bIsActive = TRUE;
    }

    m_bIsReady = TRUE;

    return TRUE;
}


//==============================================================================================
// GS_OGLParticle::Create():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Returns: TRUE if successfull, FALSE if not.
//==============================================================================================

BOOL GS_OGLParticle::Create(GLuint glTexture, int nWidth, int nHeight, int nNumParticles)
{

    // Can't create particle if no valid texture.
    if (!glTexture)
    {
        return FALSE;
    }

    // Destroy particle if created previously.
    if (m_bIsReady)
    {
        this->Destroy();
    }

    // Remember the texture ID.
    m_gluTexture = glTexture;

    // Determine the actual dimensions of the particle.
    m_gliWidth  = nWidth;
    m_gliHeight = nHeight;

    // Set how many particles are active initially.
    if (nNumParticles > MAX_PARTICLES)
    {
        nNumParticles = MAX_PARTICLES;
    }
    else if (nNumParticles < 0)
    {
        nNumParticles = 0;
    }

    m_nNumParticles = nNumParticles;

    // Activate the specified number of particles.
    for (int nLoop = 0; nLoop < m_nNumParticles; nLoop++)
    {
        m_gsParticles[nLoop].bIsActive = TRUE;
    }

    m_bIsReady = TRUE;

    return TRUE;
}


////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_OGLParticle::Destroy():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Returns: Nothing.
//==============================================================================================

void GS_OGLParticle::Destroy()
{

    if (!m_bIsReady)
    {
        return;
    }

    m_gsTexture.Destroy();

    m_gluTexture = 0;

    m_nNumParticles = 0;

    m_bIsReady = FALSE;

    m_bLightingEnabled  = FALSE;
    m_bDepthTestEnabled = FALSE;

    m_gliWidth  = 0;
    m_gliHeight = 0;

    for (int nLoop = 0; nLoop < MAX_PARTICLES; nLoop++)
    {
        m_gsParticles[nLoop].bIsActive = FALSE;
        m_gsParticles[nLoop].gliDestX = 0;
        m_gsParticles[nLoop].gliDestY = 0;
        m_gsParticles[nLoop].glfRotateX = 0.0f;
        m_gsParticles[nLoop].glfRotateY = 0.0f;
        m_gsParticles[nLoop].glfRotateZ = 0.0f;
        m_gsParticles[nLoop].glfScaleX = 1.0f;
        m_gsParticles[nLoop].glfScaleY = 1.0f;
        m_gsParticles[nLoop].glfR = 1.0f;
        m_gsParticles[nLoop].glfG = 1.0f;
        m_gsParticles[nLoop].glfB = 1.0f;
        m_gsParticles[nLoop].glfA = 1.0f;
    }
}


////////////////////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////////////////////
// Render Methods. /////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_OGLParticle::BeginRender():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Returns: TRUE if successfull, FALSE if not.
//==============================================================================================

BOOL GS_OGLParticle::BeginRender(HWND hWnd)
{

    // Can't render if no valid particle.
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
// GS_OGLParticle::EndRender():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Returns: TRUE if successfull, FALSE if not.
//==============================================================================================

void GS_OGLParticle::EndRender()
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
// GS_OGLParticle::Render():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Returns: TRUE if successfull, FALSE if not.
//==============================================================================================

BOOL GS_OGLParticle::Render(HWND hWnd)
{

    // Can't render if no valid particle.
    if (!m_bIsReady)
    {
        return FALSE;
    }

    // Setup display for 2D rendering.
    if (hWnd != NULL)
    {
        this->BeginRender(hWnd);
    }

    GLint gliSrcFactor, gliDstFactor;

    GLboolean glbIsBlendingEnabled = glIsEnabled(GL_BLEND);

    // Is blending enabled?
    if (glbIsBlendingEnabled)
    {
        // Save blending parameters.
        glGetIntegerv(GL_BLEND_SRC, &gliSrcFactor);
        glGetIntegerv(GL_BLEND_DST, &gliDstFactor);
        // Disable blending.
        glDisable(GL_BLEND);
    }

    // Setup blending for particles.
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);
    glEnable(GL_BLEND);

    // For each particle.
    for (int nLoop = 0; nLoop < m_nNumParticles; nLoop++)
    {

        // Should the specified particle be rendered?
        if (m_gsParticles[nLoop].bIsActive)
        {
            // Reset the modelview matrix.
            glLoadIdentity();

            // Select the particle texture.
            glBindTexture(GL_TEXTURE_2D, m_gluTexture);

            // Draw particle in a different hue or transparency depending on the alpha factor.
            glColor4f(m_gsParticles[nLoop].glfR, m_gsParticles[nLoop].glfG,
                      m_gsParticles[nLoop].glfB, m_gsParticles[nLoop].glfA);

            // Determine the scaled particle dimensions and divide by two to take into account
            // that the particle is drawn from the center outwards.
            float fHalfWidth  = ((float)m_gliWidth  / 2) * m_gsParticles[nLoop].glfScaleX * g_fScaleFactorX;
            float fHalfHeight = ((float)m_gliHeight / 2) * m_gsParticles[nLoop].glfScaleY * g_fScaleFactorY;

            // Position the particle (the coordinates (0,0) is the bottom left of the screen). Note
            // that because the particle is actually positioned from the center, some adjustments
            // have to be made for the particle to be postioned by the bottom left corner.
            glTranslated
            (
                (m_gsParticles[nLoop].gliDestX * g_fScaleFactorX) + fHalfWidth + g_fRenderModX,
                (m_gsParticles[nLoop].gliDestY * g_fScaleFactorX) + fHalfHeight + g_fRenderModY,
                0
            );

            // Rotate the particle on the X, Y and Z-axis.
            glRotatef(m_gsParticles[nLoop].glfRotateX, 1.0f, 0.0f, 0.0f);
            glRotatef(m_gsParticles[nLoop].glfRotateY, 0.0f, 1.0f, 0.0f);
            glRotatef(m_gsParticles[nLoop].glfRotateZ, 0.0f, 0.0f, 1.0f);

            // Start drawing a quad. In order for the particle to be rotated correctly, the
            // particle has to/ be drawn from a center point outwards. If the center of the
            // particle is (0, 0) then the bottom left corner of the particle is (0 - (particle
            // width / 2), 0 - (particle height/2)), the bottom right corner of the particle is
            // (0 + (particle width / 2), 0 - (particle height/2)), the top right corner of the
            // particle is (0 + (particle width / 2), 0 + (particle height/2)) and the top left
            // corner of the particle is (0 - (particle width / 2), 0 + (particle height/2)).
            glBegin(GL_QUADS);
            // Bottom left texture coordinate and vertex.
            glTexCoord2f(0.0f, 0.0f);
            glVertex2f(-fHalfWidth, -fHalfHeight);
            // Bottom right texture coordinate and vertex.
            glTexCoord2f(1.0f, 0.0f);
            glVertex2f( fHalfWidth, -fHalfHeight);
            // Top right texture coordinate and vertex.
            glTexCoord2f(1.0f, 1.0f);
            glVertex2f( fHalfWidth,  fHalfHeight);
            // Top left texture coordinate and vertex.
            glTexCoord2f(0.0f, 1.0f);
            glVertex2f(-fHalfWidth,  fHalfHeight);
            glEnd();

            // Unselect the particle texture.
            glBindTexture(GL_TEXTURE_2D, 0);  // Use 0 instead of NULL
        }
    }


    // Reset the color for other textures to white.
    glColor4f(1.0f, 1.0f, 1.0f, 1.0f);

    // Disable particle blending.
    glDisable(GL_BLEND);

    // Was blending enabled originally?
    if (glbIsBlendingEnabled)
    {
        // Restore and enable original blending;
        glBlendFunc(gliSrcFactor, gliDstFactor);
        glEnable(GL_BLEND);
    }

    // Reset display after 2D rendering.
    if (hWnd != NULL)
    {
        this->EndRender();
    }

    return TRUE;
}


////////////////////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////////////////////
// Status Methods. /////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_OGLParticle::IsReady():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Returns: TRUE if particles have been created, FALSE if not.
//==============================================================================================

BOOL GS_OGLParticle::IsReady()
{

    return m_bIsReady;
}


//==============================================================================================
// GS_OGLParticle::IsActive():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Returns: TRUE if particle is active, FALSE if not.
//==============================================================================================

BOOL GS_OGLParticle::IsActive(int nParticleIndex)
{

    if (nParticleIndex >= m_nNumParticles)
    {
        nParticleIndex = m_nNumParticles - 1;
    }
    else if (nParticleIndex < 0)
    {
        nParticleIndex = 0;
    }

    return m_gsParticles[nParticleIndex].bIsActive;
}


//==============================================================================================
// GS_OGLParticle::Activate():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Returns: Nothing.
//==============================================================================================

void GS_OGLParticle::Activate(int nParticleIndex, BOOL bActivate)
{

    if (nParticleIndex >= m_nNumParticles)
    {
        nParticleIndex = m_nNumParticles - 1;
    }
    else if (nParticleIndex < 0)
    {
        nParticleIndex = 0;
    }

    m_gsParticles[nParticleIndex].bIsActive = bActivate;
}


void GS_OGLParticle::Activate(BOOL bActivate)
{

    for (int nLoop = 0; nLoop < m_nNumParticles; nLoop++)
    {
        m_gsParticles[nLoop].bIsActive = bActivate;
    }
}


////////////////////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////////////////////
// Get/Set Methods. ////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_OGLParticle::GetNumParticles():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Returns: An integer value indicating the number of particles.
//==============================================================================================

int GS_OGLParticle::GetNumParticles()
{

    return m_nNumParticles;
}


//==============================================================================================
// GS_OGLParticle::SetNumParticles():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Returns: Nothing.
//==============================================================================================

void GS_OGLParticle::SetNumParticles(int nNumParticles)
{

    // Is number of particles within bounds?
    if (nNumParticles > MAX_PARTICLES)
    {
        nNumParticles = MAX_PARTICLES;
    }
    else if (nNumParticles < 0)
    {
        nNumParticles = 0;
    }

    // Set the number of particles.
    m_nNumParticles = nNumParticles;
}


//==============================================================================================
// GS_OGLParticle::GetDestX():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Returns: An integer value indicating the horizontal position of the particle.
//==============================================================================================

GLint GS_OGLParticle::GetDestX(int nParticleIndex)
{

    if (nParticleIndex >= m_nNumParticles)
    {
        nParticleIndex = m_nNumParticles - 1;
    }
    else if (nParticleIndex < 0)
    {
        nParticleIndex = 0;
    }

    return m_gsParticles[nParticleIndex].gliDestX;
}


//==============================================================================================
// GS_OGLParticle::SetDestX():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Returns: Nothing.
//==============================================================================================

void  GS_OGLParticle::SetDestX(int nParticleIndex, GLint gliDestX)
{

    if (nParticleIndex >= m_nNumParticles)
    {
        nParticleIndex = m_nNumParticles - 1;
    }
    else if (nParticleIndex < 0)
    {
        nParticleIndex = 0;
    }

    m_gsParticles[nParticleIndex].gliDestX = gliDestX;
}


void  GS_OGLParticle::SetDestX(GLint gliDestX)
{

    for (int nLoop = 0; nLoop < m_nNumParticles; nLoop++)
    {
        m_gsParticles[nLoop].gliDestX = gliDestX;
    }
}


//==============================================================================================
// GS_OGLParticle::GetDestY():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Returns: An integer value indicating the vertical position of the particle.
//==============================================================================================

GLint GS_OGLParticle::GetDestY(int nParticleIndex)
{

    if (nParticleIndex >= m_nNumParticles)
    {
        nParticleIndex = m_nNumParticles - 1;
    }
    else if (nParticleIndex < 0)
    {
        nParticleIndex = 0;
    }

    return m_gsParticles[nParticleIndex].gliDestY;
}


//==============================================================================================
// GS_OGLParticle::SetDestY():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Returns: Nothing.
//==============================================================================================

void  GS_OGLParticle::SetDestY(int nParticleIndex, GLint gliDestY)
{

    if (nParticleIndex >= m_nNumParticles)
    {
        nParticleIndex = m_nNumParticles - 1;
    }
    else if (nParticleIndex < 0)
    {
        nParticleIndex = 0;
    }

    m_gsParticles[nParticleIndex].gliDestY = gliDestY;
}

void  GS_OGLParticle::SetDestY(GLint gliDestY)
{

    for (int nLoop = 0; nLoop < m_nNumParticles; nLoop++)
    {
        m_gsParticles[nLoop].gliDestY = gliDestY;
    }
}


//==============================================================================================
// GS_OGLParticle::GetRotateX():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Returns: A floating point value indicating the rotation on the x-axis.
//==============================================================================================

GLfloat GS_OGLParticle::GetRotateX(int nParticleIndex)
{

    if (nParticleIndex >= m_nNumParticles)
    {
        nParticleIndex = m_nNumParticles - 1;
    }
    else if (nParticleIndex < 0)
    {
        nParticleIndex = 0;
    }

    return m_gsParticles[nParticleIndex].glfRotateX;
}


//==============================================================================================
// GS_OGLParticle::SetRotateX():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Returns: Nothing.
//==============================================================================================

void GS_OGLParticle::SetRotateX(int nParticleIndex, GLfloat glfRotateX)
{

    if (nParticleIndex >= m_nNumParticles)
    {
        nParticleIndex = m_nNumParticles - 1;
    }
    else if (nParticleIndex < 0)
    {
        nParticleIndex = 0;
    }

    m_gsParticles[nParticleIndex].glfRotateX = glfRotateX;
}

void GS_OGLParticle::SetRotateX(GLfloat glfRotateX)
{

    for (int nLoop = 0; nLoop < m_nNumParticles; nLoop++)
    {
        m_gsParticles[nLoop].glfRotateX = glfRotateX;
    }
}


//==============================================================================================
// GS_OGLParticle::GetRotateY():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Returns: A floating point value indicating the rotation on the y-axis.
//==============================================================================================

GLfloat GS_OGLParticle::GetRotateY(int nParticleIndex)
{

    if (nParticleIndex >= m_nNumParticles)
    {
        nParticleIndex = m_nNumParticles - 1;
    }
    else if (nParticleIndex < 0)
    {
        nParticleIndex = 0;
    }

    return m_gsParticles[nParticleIndex].glfRotateY;
}


//==============================================================================================
// GS_OGLParticle::SetRotateY():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Returns: Nothing.
//==============================================================================================

void GS_OGLParticle::SetRotateY(int nParticleIndex, GLfloat glfRotateY)
{

    if (nParticleIndex >= m_nNumParticles)
    {
        nParticleIndex = m_nNumParticles - 1;
    }
    else if (nParticleIndex < 0)
    {
        nParticleIndex = 0;
    }

    m_gsParticles[nParticleIndex].glfRotateY = glfRotateY;
}

void GS_OGLParticle::SetRotateY(GLfloat glfRotateY)
{

    for (int nLoop = 0; nLoop < m_nNumParticles; nLoop++)
    {
        m_gsParticles[nLoop].glfRotateY = glfRotateY;
    }
}


//==============================================================================================
// GS_OGLParticle::GetRotateZ():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Returns: A floating point value indicating the rotation on the z-axis.
//==============================================================================================

GLfloat GS_OGLParticle::GetRotateZ(int nParticleIndex)
{

    if (nParticleIndex >= m_nNumParticles)
    {
        nParticleIndex = m_nNumParticles - 1;
    }
    else if (nParticleIndex < 0)
    {
        nParticleIndex = 0;
    }

    return m_gsParticles[nParticleIndex].glfRotateZ;
}


//==============================================================================================
// GS_OGLParticle::SetRotateZ():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Returns: Nothing.
//==============================================================================================

void GS_OGLParticle::SetRotateZ(int nParticleIndex, GLfloat glfRotateZ)
{

    if (nParticleIndex >= m_nNumParticles)
    {
        nParticleIndex = m_nNumParticles - 1;
    }
    else if (nParticleIndex < 0)
    {
        nParticleIndex = 0;
    }

    m_gsParticles[nParticleIndex].glfRotateZ = glfRotateZ;
}

void GS_OGLParticle::SetRotateZ(GLfloat glfRotateZ)
{

    for (int nLoop = 0; nLoop < m_nNumParticles; nLoop++)
    {
        m_gsParticles[nLoop].glfRotateZ = glfRotateZ;
    }
}


//==============================================================================================
// GS_OGLParticle::GetScaleX():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Returns: A floating point value indicating the horizontal scale of the particle.
//==============================================================================================

GLfloat GS_OGLParticle::GetScaleX(int nParticleIndex)
{

    if (nParticleIndex >= m_nNumParticles)
    {
        nParticleIndex = m_nNumParticles - 1;
    }
    else if (nParticleIndex < 0)
    {
        nParticleIndex = 0;
    }

    return m_gsParticles[nParticleIndex].glfScaleX;
}


//==============================================================================================
// GS_OGLParticle::SetScaleX():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Returns: Nothing.
//==============================================================================================

void GS_OGLParticle::SetScaleX(int nParticleIndex, GLfloat glfScaleX)
{

    if (nParticleIndex >= m_nNumParticles)
    {
        nParticleIndex = m_nNumParticles - 1;
    }
    else if (nParticleIndex < 0)
    {
        nParticleIndex = 0;
    }

    m_gsParticles[nParticleIndex].glfScaleX = glfScaleX;
}

void GS_OGLParticle::SetScaleX(GLfloat glfScaleX)
{

    for (int nLoop = 0; nLoop < m_nNumParticles; nLoop++)
    {
        m_gsParticles[nLoop].glfScaleX = glfScaleX;
    }
}


//==============================================================================================
// GS_OGLParticle::GetScaleY():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Returns: A floating point value indicating the vertical scale of the particle.
//==============================================================================================

GLfloat GS_OGLParticle::GetScaleY(int nParticleIndex)
{

    if (nParticleIndex >= m_nNumParticles)
    {
        nParticleIndex = m_nNumParticles - 1;
    }
    else if (nParticleIndex < 0)
    {
        nParticleIndex = 0;
    }

    return m_gsParticles[nParticleIndex].glfScaleY;
}


//==============================================================================================
// GS_OGLParticle::SetScaleY():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Returns: Nothing.
//==============================================================================================

void GS_OGLParticle::SetScaleY(int nParticleIndex, GLfloat glfScaleY)
{

    if (nParticleIndex >= m_nNumParticles)
    {
        nParticleIndex = m_nNumParticles - 1;
    }
    else if (nParticleIndex < 0)
    {
        nParticleIndex = 0;
    }

    m_gsParticles[nParticleIndex].glfScaleY = glfScaleY;
}

void GS_OGLParticle::SetScaleY(GLfloat glfScaleY)
{

    for (int nLoop = 0; nLoop < m_nNumParticles; nLoop++)
    {
        m_gsParticles[nLoop].glfScaleY = glfScaleY;
    }
}


//==============================================================================================
// GS_OGLParticle::GetWidth():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Returns: An integer value indicating the actual width of the particle.
//==============================================================================================

GLint GS_OGLParticle::GetWidth()
{

    return m_gliWidth;
}


//==============================================================================================
// GS_OGLParticle::GetHeight():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Returns: An integer value indicating the actual height of the particle.
//==============================================================================================

GLint GS_OGLParticle::GetHeight()
{

    return m_gliHeight;
}


//==============================================================================================
// GS_OGLParticle::GetScaledWidth():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Returns: A floating point value indicating the scaled width of the particle.
//==============================================================================================

GLfloat GS_OGLParticle::GetScaledWidth(int nParticleIndex)
{

    if (nParticleIndex >= m_nNumParticles)
    {
        nParticleIndex = m_nNumParticles - 1;
    }
    else if (nParticleIndex < 0)
    {
        nParticleIndex = 0;
    }

    return (m_gliWidth * m_gsParticles[nParticleIndex].glfScaleX);
}


////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_OGLParticle::SetScaledWidth():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Returns: Nothing.
//==============================================================================================

void GS_OGLParticle::SetScaledWidth(int nParticleIndex, GLint gliWidth)
{

    if (nParticleIndex >= m_nNumParticles)
    {
        nParticleIndex = m_nNumParticles - 1;
    }
    else if (nParticleIndex < 0)
    {
        nParticleIndex = 0;
    }

    if (m_gliWidth > 0)
    {
        m_gsParticles[nParticleIndex].glfScaleX = float(gliWidth) / m_gliWidth;
    }
    else
    {
        m_gsParticles[nParticleIndex].glfScaleX = 0.0f;
    }
}

void GS_OGLParticle::SetScaledWidth(GLint gliWidth)
{

    float fScaleX;

    if (m_gliWidth > 0)
    {
        fScaleX = float(gliWidth) / m_gliWidth;
    }
    else
    {
        fScaleX = 0.0f;
    }

    for (int nLoop = 0; nLoop < m_nNumParticles; nLoop++)
    {
        m_gsParticles[nLoop].glfScaleX = fScaleX;
    }
}


////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_OGLParticle::GetScaledHeight():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Returns: A floating point value indicating the scaled height of the particle.
//==============================================================================================

GLfloat GS_OGLParticle::GetScaledHeight(int nParticleIndex)
{

    if (nParticleIndex >= m_nNumParticles)
    {
        nParticleIndex = m_nNumParticles - 1;
    }
    else if (nParticleIndex < 0)
    {
        nParticleIndex = 0;
    }

    return (m_gliHeight * m_gsParticles[nParticleIndex].glfScaleY);
}


////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_OGLParticle::SetScaledHeight():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Returns: Nothing.
//==============================================================================================

void GS_OGLParticle::SetScaledHeight(int nParticleIndex, GLint gliHeight)
{

    if (nParticleIndex >= m_nNumParticles)
    {
        nParticleIndex = m_nNumParticles - 1;
    }
    else if (nParticleIndex < 0)
    {
        nParticleIndex = 0;
    }

    if (m_gliHeight > 0)
    {
        m_gsParticles[nParticleIndex].glfScaleY = float(gliHeight) / m_gliHeight;
    }
    else
    {
        m_gsParticles[nParticleIndex].glfScaleY = 0.0f;
    }
}

void GS_OGLParticle::SetScaledHeight(GLint gliHeight)
{

    float fScaleY;

    if (m_gliHeight > 0)
    {
        fScaleY = float(gliHeight) / m_gliHeight;
    }
    else
    {
        fScaleY = 0.0f;
    }

    for (int nLoop = 0; nLoop < m_nNumParticles; nLoop++)
    {
        m_gsParticles[nLoop].glfScaleY = fScaleY;
    }
}


////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_OGLParticle::GetDestRect():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Returns: Nothing.
//==============================================================================================

void GS_OGLParticle::GetDestRect(int nParticleIndex, long* pLeft, long* pTop, long* pRight,
                                 long* pBottom)
{

    if (nParticleIndex >= m_nNumParticles)
    {
        nParticleIndex = m_nNumParticles - 1;
    }
    else if (nParticleIndex < 0)
    {
        nParticleIndex = 0;
    }

    *(pLeft)   = m_gsParticles[nParticleIndex].gliDestX;
    *(pTop)    = m_gsParticles[nParticleIndex].gliDestY +
                 long(this->GetScaledHeight(nParticleIndex));
    *(pRight)  = m_gsParticles[nParticleIndex].gliDestX +
                 long(this->GetScaledWidth(nParticleIndex));
    *(pBottom) = m_gsParticles[nParticleIndex].gliDestY;
}

void GS_OGLParticle::GetDestRect(int nParticleIndex, RECT* pDestRect)
{

    if (nParticleIndex >= m_nNumParticles)
    {
        nParticleIndex = m_nNumParticles - 1;
    }
    else if (nParticleIndex < 0)
    {
        nParticleIndex = 0;
    }

    SetRect(pDestRect, m_gsParticles[nParticleIndex].gliDestX,
            m_gsParticles[nParticleIndex].gliDestY + long(this->GetScaledHeight(nParticleIndex)),
            m_gsParticles[nParticleIndex].gliDestX + long(this->GetScaledWidth(nParticleIndex)),
            m_gsParticles[nParticleIndex].gliDestY);
}


////////////////////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////////////////////
// Add Methods. ////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_OGLParticle::AddDestX():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Returns: Nothing.
//==============================================================================================

void GS_OGLParticle::AddDestX(int nParticleIndex, GLint gliAddX)
{

    if (nParticleIndex >= m_nNumParticles)
    {
        nParticleIndex = m_nNumParticles - 1;
    }
    else if (nParticleIndex < 0)
    {
        nParticleIndex = 0;
    }

    m_gsParticles[nParticleIndex].gliDestX += gliAddX;
}

void GS_OGLParticle::AddDestX(GLint gliAddX)
{

    for (int nLoop = 0; nLoop < m_nNumParticles; nLoop++)
    {
        m_gsParticles[nLoop].gliDestX += gliAddX;
    }
}


////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_OGLParticle::AddDestY():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Returns: Nothing.
//==============================================================================================

void GS_OGLParticle::AddDestY(int nParticleIndex, GLint gliAddY)
{

    if (nParticleIndex >= m_nNumParticles)
    {
        nParticleIndex = m_nNumParticles - 1;
    }
    else if (nParticleIndex < 0)
    {
        nParticleIndex = 0;
    }

    m_gsParticles[nParticleIndex].gliDestY += gliAddY;
}

void GS_OGLParticle::AddDestY(GLint gliAddY)
{

    for (int nLoop = 0; nLoop < m_nNumParticles; nLoop++)
    {
        m_gsParticles[nLoop].gliDestY += gliAddY;
    }
}


////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_OGLParticle::AddRotateX():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Returns: Nothing.
//==============================================================================================

void GS_OGLParticle::AddRotateX(int nParticleIndex, GLfloat glfRotateX)
{

    if (nParticleIndex >= m_nNumParticles)
    {
        nParticleIndex = m_nNumParticles - 1;
    }
    else if (nParticleIndex < 0)
    {
        nParticleIndex = 0;
    }

    m_gsParticles[nParticleIndex].glfRotateX += glfRotateX;
}

void GS_OGLParticle::AddRotateX(GLfloat glfRotateX)
{

    for (int nLoop = 0; nLoop < m_nNumParticles; nLoop++)
    {
        m_gsParticles[nLoop].glfRotateX += glfRotateX;
    }
}


////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_OGLParticle::AddRotateY():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Returns: Nothing.
//==============================================================================================

void GS_OGLParticle::AddRotateY(int nParticleIndex, GLfloat glfRotateY)
{

    if (nParticleIndex >= m_nNumParticles)
    {
        nParticleIndex = m_nNumParticles - 1;
    }
    else if (nParticleIndex < 0)
    {
        nParticleIndex = 0;
    }

    m_gsParticles[nParticleIndex].glfRotateY += glfRotateY;
}

void GS_OGLParticle::AddRotateY(GLfloat glfRotateY)
{

    for (int nLoop = 0; nLoop < m_nNumParticles; nLoop++)
    {
        m_gsParticles[nLoop].glfRotateY += glfRotateY;
    }
}


////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_OGLParticle::AddRotateZ():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Returns: Nothing.
//==============================================================================================

void GS_OGLParticle::AddRotateZ(int nParticleIndex, GLfloat glfRotateZ)
{

    if (nParticleIndex >= m_nNumParticles)
    {
        nParticleIndex = m_nNumParticles - 1;
    }
    else if (nParticleIndex < 0)
    {
        nParticleIndex = 0;
    }

    m_gsParticles[nParticleIndex].glfRotateZ += glfRotateZ;
}

void GS_OGLParticle::AddRotateZ(GLfloat glfRotateZ)
{

    for (int nLoop = 0; nLoop < m_nNumParticles; nLoop++)
    {
        m_gsParticles[nLoop].glfRotateZ += glfRotateZ;
    }
}


////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_OGLParticle::AddScaleX():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Returns: Nothing.
//==============================================================================================

void GS_OGLParticle::AddScaleX(int nParticleIndex, GLfloat glfScaleX)
{

    if (nParticleIndex >= m_nNumParticles)
    {
        nParticleIndex = m_nNumParticles - 1;
    }
    else if (nParticleIndex < 0)
    {
        nParticleIndex = 0;
    }

    m_gsParticles[nParticleIndex].glfScaleX += glfScaleX;
}

void GS_OGLParticle::AddScaleX(GLfloat glfScaleX)
{

    for (int nLoop = 0; nLoop < m_nNumParticles; nLoop++)
    {
        m_gsParticles[nLoop].glfScaleX += glfScaleX;
    }
}


////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_OGLParticle::AddScaleY():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Returns: Nothing.
//==============================================================================================

void GS_OGLParticle::AddScaleY(int nParticleIndex, GLfloat glfScaleY)
{

    if (nParticleIndex >= m_nNumParticles)
    {
        nParticleIndex = m_nNumParticles - 1;
    }
    else if (nParticleIndex < 0)
    {
        nParticleIndex = 0;
    }

    m_gsParticles[nParticleIndex].glfScaleY += glfScaleY;
}

void GS_OGLParticle::AddScaleY(GLfloat glfScaleY)
{

    for (int nLoop = 0; nLoop < m_nNumParticles; nLoop++)
    {
        m_gsParticles[nLoop].glfScaleY += glfScaleY;
    }
}


////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_OGLParticle::SetModulateColor():
// ---------------------------------------------------------------------------------------------
// Purpose: Sets the modulate color for a specific particle.
// ---------------------------------------------------------------------------------------------
// Returns: Nothing.
//==============================================================================================

void GS_OGLParticle::SetModulateColor(int nParticleIndex, GS_OGLColor gsColor)
{

    if (nParticleIndex >= m_nNumParticles)
    {
        nParticleIndex = m_nNumParticles - 1;
    }
    else if (nParticleIndex < 0)
    {
        nParticleIndex = 0;
    }

    m_gsParticles[nParticleIndex].glfR = gsColor.fRed;
    m_gsParticles[nParticleIndex].glfG = gsColor.fGreen;
    m_gsParticles[nParticleIndex].glfB = gsColor.fBlue;
    m_gsParticles[nParticleIndex].glfA = gsColor.fAlpha;
}

void GS_OGLParticle::SetModulateColor(GS_OGLColor gsColor)
{

    for (int nLoop = 0; nLoop < m_nNumParticles; nLoop++)
    {
        m_gsParticles[nLoop].glfR = gsColor.fRed;
        m_gsParticles[nLoop].glfG = gsColor.fGreen;
        m_gsParticles[nLoop].glfB = gsColor.fBlue;
        m_gsParticles[nLoop].glfA = gsColor.fAlpha;
    }
}

void GS_OGLParticle::SetModulateColor(int nParticleIndex, float fRed, float fGreen,
                                      float fBlue, float fAlpha)
{

    if (nParticleIndex >= m_nNumParticles)
    {
        nParticleIndex = m_nNumParticles - 1;
    }
    else if (nParticleIndex < 0)
    {
        nParticleIndex = 0;
    }

    if (fRed   > -1.0f)
    {
        m_gsParticles[nParticleIndex].glfR = fRed;
    }
    if (fGreen > -1.0f)
    {
        m_gsParticles[nParticleIndex].glfG = fGreen;
    }
    if (fBlue  > -1.0f)
    {
        m_gsParticles[nParticleIndex].glfB = fBlue;
    }
    if (fAlpha > -1.0f)
    {
        m_gsParticles[nParticleIndex].glfA = fAlpha;
    }
}

void GS_OGLParticle::SetModulateColor(float fRed, float fGreen, float fBlue, float fAlpha)
{

    for (int nLoop = 0; nLoop < m_nNumParticles; nLoop++)
    {
        if (fRed   > -1.0f)
        {
            m_gsParticles[nLoop].glfR = fRed;
        }
        if (fGreen > -1.0f)
        {
            m_gsParticles[nLoop].glfG = fGreen;
        }
        if (fBlue  > -1.0f)
        {
            m_gsParticles[nLoop].glfB = fBlue;
        }
        if (fAlpha > -1.0f)
        {
            m_gsParticles[nLoop].glfA = fAlpha;
        }
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////
