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


//==============================================================================================
// Include Game System (GS) header files.
// ---------------------------------------------------------------------------------------------
#include "gs_ogl_sprite_ex.h"
//==============================================================================================


////////////////////////////////////////////////////////////////////////////////////////////////
// Constructor/Destructor Methods. /////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_OGLSpriteEx::GS_OGLSpriteEx():
// ---------------------------------------------------------------------------------------------
// Purpose: The constructor, initializes class data to defaults when class object is created.
// ---------------------------------------------------------------------------------------------
// Returns: Nothing.
//==============================================================================================

GS_OGLSpriteEx::GS_OGLSpriteEx() : GS_OGLSprite()
{

    m_nTotalFrames   = 0;
    m_nCurrentFrame  = 0;
    m_nFramesPerLine = 0;
    m_nFrameWidth    = 0;
    m_nFrameHeight   = 0;

    m_pFrameRects = NULL;
}


////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_OGLSpriteEx::~GS_OGLSpriteEx():
// ---------------------------------------------------------------------------------------------
// Purpose: The de-structor, de-initializes class data when class object is destroyed and frees
//          all memory used by it.
// ---------------------------------------------------------------------------------------------
// Returns: Nothing.
//==============================================================================================

GS_OGLSpriteEx::~GS_OGLSpriteEx()
{

    this->Destroy();
}


////////////////////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////////////////////
// Create/Destroy Methods. /////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_OGLSpriteEx::Create():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Returns: TRUE if successfull, FALSE if not.
//==============================================================================================

BOOL GS_OGLSpriteEx::Create(const char* pszPathname, int nTotalFrames, int nFramesPerLine,
                            int nFrameWidth, int nFrameHeight, BOOL bFiltered, BOOL bMipmap)
{

    // Destroy object if created previously.
    if (GS_OGLSprite::IsReady())
    {
        this->Destroy();
    }

    // Create the sprite containing all the frames.
    BOOL bResult = GS_OGLSprite::Create(pszPathname, bFiltered, bMipmap);

    // Were we able to create the sprite?
    if (!bResult)
    {
        return FALSE;
    }

    // Set the appropriate members.
    m_nTotalFrames   = nTotalFrames;
    m_nFramesPerLine = nFramesPerLine;
    m_nFrameWidth    = nFrameWidth;
    m_nFrameHeight   = nFrameHeight;

    // Allocate memory to hold the coordinates of each of the sprite frames.
    m_pFrameRects = new RECT[m_nTotalFrames];

    int nXoffset, nYoffset; // The starting x and y offsets of the frame.
    int nQuotient;          // The quotient used to determine the offset.

    // Determine the coordinates of each frame.
    for (int nLoop = 0; nLoop < m_nTotalFrames; nLoop++)
    {
        // Use the remainder to determine the horizontal offset of the frame.
        nXoffset   = (nLoop%m_nFramesPerLine) * m_nFrameWidth;
        // Use the quotient to determine the vertical offset of the frame.
        nQuotient = (nLoop-(nLoop%m_nFramesPerLine)) / m_nFramesPerLine;
        nYoffset  = this->GetTextureHeight() - (nQuotient * m_nFrameHeight);
        // Set the source rectangle to the specified frame.
        m_pFrameRects[nLoop].left   = nXoffset;
        m_pFrameRects[nLoop].top    = nYoffset;
        m_pFrameRects[nLoop].right  = nXoffset + m_nFrameWidth;
        m_pFrameRects[nLoop].bottom = nYoffset - m_nFrameHeight;
    }

    // Set the source rectangle to the first frame.
    GS_OGLSprite::SetSourceRect(m_pFrameRects[0]);

    return bResult;
}


//==============================================================================================
// GS_OGLSpriteEx::Create():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Returns: TRUE if successfull, FALSE if not.
//==============================================================================================

BOOL GS_OGLSpriteEx::Create(GLuint glTexture, int nWidth, int nHeight, int nTotalFrames,
                            int nFramesPerLine, int nFrameWidth, int nFrameHeight)
{

    // Destroy object if created previously.
    if (GS_OGLSprite::IsReady())
    {
        this->Destroy();
    }

    // Create the sprite containing all the frames.
    BOOL bResult = GS_OGLSprite::Create(glTexture, nWidth, nHeight);

    // Were we able to create the sprite?
    if (!bResult)
    {
        return FALSE;
    }

    // Set the appropriate members.
    m_nTotalFrames   = nTotalFrames;
    m_nFramesPerLine = nFramesPerLine;
    m_nFrameWidth    = nFrameWidth;
    m_nFrameHeight   = nFrameHeight;

    // Allocate memory to hold the coordinates of each of the sprite frames.
    m_pFrameRects = new RECT[m_nTotalFrames];

    int nXoffset, nYoffset; // The starting x and y offsets of the frame.
    int nQuotient;          // The quotient used to determine the offset.

    // Determine the coordinates of each frame.
    for (int nLoop = 0; nLoop < m_nTotalFrames; nLoop++)
    {
        // Use the remainder to determine the horizontal offset of the frame.
        nXoffset   = (nLoop%m_nFramesPerLine) * m_nFrameWidth;
        // Use the quotient to determine the vertical offset of the frame.
        nQuotient = (nLoop-(nLoop%m_nFramesPerLine)) / m_nFramesPerLine;
        nYoffset  = nHeight - (nQuotient * m_nFrameHeight);
        // Set the source rectangle to the specified frame.
        m_pFrameRects[nLoop].left   = nXoffset;
        m_pFrameRects[nLoop].top    = nYoffset;
        m_pFrameRects[nLoop].right  = nXoffset + m_nFrameWidth;
        m_pFrameRects[nLoop].bottom = nYoffset - m_nFrameHeight;
    }

    // Set the source rectangle to the first frame.
    GS_OGLSprite::SetSourceRect(m_pFrameRects[0]);

    return bResult;
}


////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_OGLSpriteEx::Destroy():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Returns: Nothing.
//==============================================================================================

void GS_OGLSpriteEx::Destroy()
{

    if (!GS_OGLSprite::IsReady())
    {
        return;
    }

    GS_OGLSprite::Destroy();

    m_nTotalFrames   = 0;
    m_nCurrentFrame  = 0;
    m_nFramesPerLine = 0;
    m_nFrameWidth    = 0;
    m_nFrameHeight   = 0;

    if (m_pFrameRects)
    {
        delete [] m_pFrameRects;
    }
    m_pFrameRects = NULL;
}


////////////////////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////////////////////
// Render Methods. /////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_OGLSpriteEx::Render():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Details: ...
// ---------------------------------------------------------------------------------------------
// Returns: TRUE if successfull, FALSE if not.
//==============================================================================================

BOOL GS_OGLSpriteEx::Render(HWND hWnd)
{

    // Can't render if no valid sprite.
    if (!GS_OGLSprite::IsReady())
    {
        return FALSE;
    }

    // Set the source rectangle to the specified frame.
    GS_OGLSprite::SetSourceRect(m_pFrameRects[m_nCurrentFrame]);

    // Draw the selected frame on the surface.
    return GS_OGLSprite::Render(hWnd);
}


////////////////////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////////////////////
// Add Methods. ////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_OGLSpriteEx::AddFrame():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Returns: Nothing.
//==============================================================================================

void GS_OGLSpriteEx::AddFrame(int nFramesToAdd)
{

    m_nCurrentFrame += nFramesToAdd;

    if (m_nCurrentFrame >= m_nTotalFrames)
    {
        m_nCurrentFrame = 0;
    }
    else if (m_nCurrentFrame < 0)
    {
        m_nCurrentFrame = m_nTotalFrames - 1;
    }
}


////////////////////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////////////////////
// Get/Set Methods. ////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_OGLSpriteEx::GetFrame():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Returns: An integer value indicating the current frame.
//==============================================================================================

int GS_OGLSpriteEx::GetFrame()
{

    return m_nCurrentFrame;
}


//==============================================================================================
// GS_OGLSpriteEx::SetFrame():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Returns: Nothing.
//==============================================================================================

void GS_OGLSpriteEx::SetFrame(int nFrame)
{

    m_nCurrentFrame = nFrame;

    if (m_nCurrentFrame >= m_nTotalFrames)
    {
        m_nCurrentFrame = m_nTotalFrames - 1;
    }
    else if (m_nCurrentFrame < 0)
    {
        m_nCurrentFrame = 0;
    }
}


////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_OGLSpriteEx::GetTotalFrames():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Returns: An integer value indicating the total number of frames.
//==============================================================================================

int GS_OGLSpriteEx::GetTotalFrames()
{

    return m_nTotalFrames;
}


//==============================================================================================
// GS_OGLSpriteEx::SetTotalFrames():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Returns: Nothing.
//==============================================================================================

void GS_OGLSpriteEx::SetTotalFrames(int nTotalFrames)
{

    m_nTotalFrames = nTotalFrames;
}


////////////////////////////////////////////////////////////////////////////////////////////////

//==============================================================================================
// GS_OGLSpriteEx::GetFramesPerLine():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Returns: An integer value indicating how many frames per line.
//==============================================================================================

int GS_OGLSpriteEx::GetFramesPerLine()
{

    return m_nFramesPerLine;
}


//==============================================================================================
// GS_OGLSpriteEx::SetFramesPerLine():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Returns: Nothing.
//==============================================================================================

void GS_OGLSpriteEx::SetFramesPerLine(int nFramesPerLine)
{

    m_nFramesPerLine = nFramesPerLine;
}


////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_OGLSpriteEx::GetFrameWidth():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Returns: A floating point value indicating the width of the frame.
//==============================================================================================

GLfloat GS_OGLSpriteEx::GetFrameWidth()
{

    return(m_nFrameWidth * GS_OGLSprite::GetScaleX());
}


//==============================================================================================
// GS_OGLSpriteEx::GetFrameHeight():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Returns: A floating point value indicating the height of the frame.
//==============================================================================================

GLfloat GS_OGLSpriteEx::GetFrameHeight()
{

    return(m_nFrameHeight * GS_OGLSprite::GetScaleY());
}


////////////////////////////////////////////////////////////////////////////////////////////////
