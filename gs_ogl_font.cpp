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


//==============================================================================================
// Include Game System (GS) header files.
// ---------------------------------------------------------------------------------------------
#include "gs_ogl_font.h"
//==============================================================================================


////////////////////////////////////////////////////////////////////////////////////////////////
// Constructor/Destructor Methods. /////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_OGLFont::GS_OGLFont():
// ---------------------------------------------------------------------------------------------
// Purpose: The constructor, initializes class data to defaults when class object is created.
// ---------------------------------------------------------------------------------------------
// Returns: Nothing.
//==============================================================================================

GS_OGLFont::GS_OGLFont() : GS_OGLSprite()
{

    m_pszTextString = NULL;

    m_nCharsPerLine = 0;
    m_nLetterWidth  = 0;
    m_nLetterHeight = 0;
}


////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_OGLFont::~GS_OGLFont():
// ---------------------------------------------------------------------------------------------
// Purpose: The de-structor, de-initializes class data when class object is destroyed and frees
//          all memory used by it.
// ---------------------------------------------------------------------------------------------
// Returns: Nothing.
//==============================================================================================

GS_OGLFont::~GS_OGLFont()
{

    this->Destroy();
}


////////////////////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////////////////////
// Create/Destroy Methods. /////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_OGLFont::Create():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Returns: TRUE if successfull, FALSE if not.
//==============================================================================================

BOOL GS_OGLFont::Create(const char* pszPathname, int nCharsPerLine, int nLetterWidth,
                        int nLetterHeight, BOOL bFiltered, BOOL bMipmap)
{

    // Destroy object if created previously.
    if (GS_OGLSprite::IsReady())
    {
        this->Destroy();
    }

    // Set the appropriate members.
    m_nCharsPerLine = nCharsPerLine;
    m_nLetterWidth  = nLetterWidth;
    m_nLetterHeight = nLetterHeight;

    // Create the sprite containing the font characters.
    return GS_OGLSprite::Create(pszPathname, bFiltered, bMipmap);
}


///==============================================================================================
// GS_OGLFont::Create():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Returns: TRUE if successfull, FALSE if not.
//==============================================================================================

BOOL GS_OGLFont::Create(GLuint glTexture, int nWidth, int nHeight, int nCharsPerLine,
                        int nLetterWidth, int nLetterHeight)
{

    // Destroy object if created previously.
    if (GS_OGLSprite::IsReady())
    {
        this->Destroy();
    }

    // Set the appropriate members.
    m_nCharsPerLine = nCharsPerLine;
    m_nLetterWidth  = nLetterWidth;
    m_nLetterHeight = nLetterHeight;

    // Create the sprite containing the font characters.
    return GS_OGLSprite::Create(glTexture, nWidth, nHeight);
}


////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_OGLFont::Destroy():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Returns: TRUE if successfull, FALSE if not.
//==============================================================================================

void GS_OGLFont::Destroy()
{

    if (!GS_OGLSprite::IsReady())
    {
        return;
    }

    GS_OGLSprite::Destroy();

    if (m_pszTextString)
    {
        delete [] m_pszTextString;
    }
    m_pszTextString = NULL;

    m_nCharsPerLine = 0;
    m_nLetterWidth  = 0;
    m_nLetterHeight = 0;
}


////////////////////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////////////////////
// Render Methods. /////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_OGLFont::Render():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Notes: For the calculation to be correct, this method assumes that the source image of the
//        font begins at the coordinate (0, 0) on the texture surface.
// ---------------------------------------------------------------------------------------------
// Returns: TRUE if successfull, FALSE if not.
//==============================================================================================

BOOL GS_OGLFont::Render(HWND hWnd)
{

    // Can't render if no valid sprite.
    if (!GS_OGLSprite::IsReady())
    {
        return FALSE;
    }

    BOOL bResult;

    // Store the original (x ,y) starting position.
    GLint gliStartX = GS_OGLSprite::GetDestX();
    GLint gliStartY = GS_OGLSprite::GetDestY();

    // For each character in the string to be rendered.
    for (int nLoop=0; nLoop<(int)(lstrlen(m_pszTextString)); nLoop++)
    {
        // Find and render the current character from the text string.
        bResult = RenderChar(m_pszTextString[nLoop], hWnd);
        // Was rendering character successfull?
        if (!bResult)
        {
            GS_Error::Report("GS_OGL_FONT.CPP", 190, "Failed to render character to surface!");
            // Restore the original (x ,y) starting position.
            GS_OGLSprite::SetDestX(gliStartX);
            GS_OGLSprite::SetDestY(gliStartY);
            return FALSE;
        }
        // Shift the target X location to the right by the pixel width of each letter. If we
        // don't do this, we'll be drawing each letter directly on top of the previous.
        GS_OGLSprite::AddDestX(int(this->GetLetterWidth()));
    }

    // Restore the original (x ,y) starting position.
    GS_OGLSprite::SetDestX(gliStartX);
    GS_OGLSprite::SetDestY(gliStartY);

    return TRUE;
}


////////////////////////////////////////////////////////////////////////////////////////////////


/*
BOOL GS_OGLFont::Render(HWND hWnd) {

   // Can't render if no valid sprite.
   if (!GS_OGLSprite::IsReady()) {
      return FALSE;
      }

   char cTextChar;
   int nLoop;
   int nXoffset, nYoffset; // The x,y top left location of the character image.
   GS_OGLColor gsColor;
   this->GetModulateColor(&gsColor);

   // Store the original (x ,y) starting position.
   GLint gliStartX = GS_OGLSprite::GetDestX();
   GLint gliStartY = GS_OGLSprite::GetDestY();

   // For each character in the string to be rendered.
   for (nLoop=0; nLoop<(int)(lstrlen(m_pszTextString)); nLoop++) {
      // Find and render the current character from the text string.
      // bResult = RenderChar(m_pszTextString[nLoop], hWnd);
      // We are allowing ascii characters from space (32) to ~ (126). As the 1st char on our
      // image surface is a space, we can subtract 32 from the ascii value of the character we
      // want to render. This gives us a zero based index.
      cTextChar = char(m_pszTextString[nLoop] - 32);
      // Check wether character is within minimum bounds.
      if (cTextChar < 0) {
         cTextChar = 0;
         }
      // Use the remainder to determine the horizontal offset of the letter.
      nXoffset = (cTextChar%m_nCharsPerLine) * m_nLetterWidth;
      // Use the quotient to determine the vertical offset of the letter starting from the top
      // of the image and working towards the bottom.
      nYoffset = this->GetTextureHeight() - (((cTextChar - (cTextChar%m_nCharsPerLine)) /
                 m_nCharsPerLine) * m_nLetterHeight);
      // Set the target to the specified letter.
      m_gsTargets[nLoop].nDestX     = gliStartX;
      m_gsTargets[nLoop].nDestY     = gliStartY;
      m_gsTargets[nLoop].nSrcLeft   = nXoffset;
      m_gsTargets[nLoop].nSrcTop    = nYoffset;
      m_gsTargets[nLoop].nSrcRight  = nXoffset + m_nLetterWidth;
      m_gsTargets[nLoop].nSrcBottom = nYoffset - m_nLetterHeight;
      m_gsTargets[nLoop].fScaleX  = this->GetScaleX();
      m_gsTargets[nLoop].fScaleY  = this->GetScaleY();
      m_gsTargets[nLoop].fRotateX = this->GetRotateX();
      m_gsTargets[nLoop].fRotateY = this->GetRotateY();
      m_gsTargets[nLoop].fRotateZ = this->GetRotateZ();
      m_gsTargets[nLoop].fRed     = gsColor.fRed;
      m_gsTargets[nLoop].fGreen   = gsColor.fGreen;
      m_gsTargets[nLoop].fBlue    = gsColor.fBlue;
      m_gsTargets[nLoop].fAlpha   = gsColor.fAlpha;
      m_gsTargets[nLoop].fScrollX = this->GetScrollX();
      m_gsTargets[nLoop].fScrollY = this->GetScrollY();
      // Shift the target X location to the right by the pixel width of each letter. If we
      // don't do this, we'll be drawing each letter directly on top of the previous.
      gliStartX += this->GetLetterWidth();
      }

   // Render all the letters at once.
   this->RenderTargets(nLoop, m_gsTargets, hWnd);

   return TRUE;
}
*/


////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_OGLFont::RenderChar():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Notes: For the calculation to be correct, this method assumes that the source image of the
//        font begins at the coordinate (0, 0) on the texture surface.
// ---------------------------------------------------------------------------------------------
// Returns: TRUE if successfull, FALSE if not.
//==============================================================================================

BOOL GS_OGLFont::RenderChar(char cTextChar, HWND hWnd)
{

    // Can't render if no valid sprite.
    if (!GS_OGLSprite::IsReady())
    {
        return FALSE;
    }

    int nXoffset, nYoffset; // The x,y top left location of the character image.
    RECT SourceRect; // The source rectangle for the specified character.

    // We are allowing ascii characters from space (32) to ~ (126). As the 1st char on our
    // image surface is a space, we can subtract 32 from the ascii value of the character we
    // want to render. This gives us a zero based index.
    cTextChar -= (char) 32;

    // Check wether character is within minimum bounds.
    if (cTextChar < 0)
    {
        cTextChar = 0;
    }

    // Use the remainder to determine the horizontal offset of the letter.
    nXoffset = (cTextChar%m_nCharsPerLine) * m_nLetterWidth;

    // Use the quotient to determine the vertical offset of the letter starting from the top
    // of the image and working towards the bottom.
    nYoffset = this->GetTextureHeight() - (((cTextChar - (cTextChar%m_nCharsPerLine)) /
                                            m_nCharsPerLine) * m_nLetterHeight);

    // Set the source rectangle to the specified frame.
    SourceRect.left   = nXoffset;
    SourceRect.top    = nYoffset;
    SourceRect.right  = nXoffset + m_nLetterWidth;
    SourceRect.bottom = nYoffset - m_nLetterHeight;
    GS_OGLSprite::SetSourceRect(SourceRect);

    // Draw the selected character on the surface.
    return GS_OGLSprite::Render(hWnd);
}


////////////////////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////////////////////
// Get/Set Methods. ////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_OGLFont::GetCharsPerLine():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Returns: Nothing.
//==============================================================================================

int GS_OGLFont::GetCharsPerLine()
{

    return m_nCharsPerLine;
}


//==============================================================================================
// GS_OGLFont::SetCharsPerLine():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Returns: Nothing.
//==============================================================================================

void GS_OGLFont::SetCharsPerLine(int nCharsPerLine)
{

    m_nCharsPerLine = nCharsPerLine;
}


////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_OGLFont::GetLetterWidth():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Returns: Nothing.
//==============================================================================================

float GS_OGLFont::GetLetterWidth()
{

    return (m_nLetterWidth * GS_OGLSprite::GetScaleX());
}


//==============================================================================================
// GS_OGLFont::SetLetterWidth():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Returns: Nothing.
//==============================================================================================

void GS_OGLFont::SetLetterWidth(int nLetterWidth)
{

    m_nLetterWidth = nLetterWidth;
}


////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_OGLFont::GetLetterHeight():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Returns: Nothing.
//==============================================================================================

float GS_OGLFont::GetLetterHeight()
{

    return (m_nLetterHeight * GS_OGLSprite::GetScaleY());
}


//==============================================================================================
// GS_OGLFont::SetLetterHeight():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Returns: Nothing.
//==============================================================================================

void GS_OGLFont::SetLetterHeight(int nLetterHeight)
{

    m_nLetterHeight = nLetterHeight;
}


////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_OGLFont::GetText():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Returns: Nothing.
//==============================================================================================

void GS_OGLFont::GetText(char* pszTextString)
{

    strncpy(pszTextString, m_pszTextString, lstrlen(pszTextString)-1);
}


////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_OGLFont::SetText():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Returns: Nothing.
//==============================================================================================

void GS_OGLFont::SetText(const char* pszTextString, ...)
{

    // If no string, do nothing.
    if (!pszTextString)
    {
        return;
    }

    // Delete string if exists.
    if (m_pszTextString)
    {
        delete [] m_pszTextString;
    }
    m_pszTextString=NULL;

    char szTempString[256];

    // Pointer to a list of arguments.
    va_list pArg;

    // Parses the string for variables.
    va_start(pArg, pszTextString);
    // Converts symbols to actual numbers and save.
    vsprintf(szTempString, pszTextString, pArg);
    va_end(pArg);

    // Determine length of string and allocate memory.
    int nStringLength = lstrlen(szTempString) + 1;
    m_pszTextString = new char[nStringLength];

    // Save the parsed string.
    lstrcpy(m_pszTextString, szTempString);

    // Terminate the string.
    m_pszTextString[nStringLength-1] = '\0';
}


////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_OGLFont::GetTextWidth():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Returns: Nothing.
//==============================================================================================

float GS_OGLFont::GetTextWidth()
{

    return (this->GetLetterWidth() * lstrlen(m_pszTextString));
}


////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_OGLFont::GetTextHeight():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Returns: Nothing.
//==============================================================================================

float GS_OGLFont::GetTextHeight()
{

    return this->GetLetterHeight();
}


////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_OGLFont::GetDestRect():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Returns: Nothing.
//==============================================================================================

void GS_OGLFont::GetDestRect(long* pLeft, long* pTop, long* pRight, long* pBottom)
{

    *pLeft   = GS_OGLSprite::GetDestX();
    *pTop    = GS_OGLSprite::GetDestY() + (long)this->GetTextHeight();
    *pRight  = GS_OGLSprite::GetDestX() + (long)this->GetTextWidth();
    *pBottom = GS_OGLSprite::GetDestY();
}


void GS_OGLFont::GetDestRect(RECT* pDestRect)
{

    SetRect(pDestRect, GS_OGLSprite::GetDestX(),
            (GS_OGLSprite::GetDestY() + (long)this->GetTextHeight()),
            (GS_OGLSprite::GetDestX() + (long)this->GetTextWidth()),
            GS_OGLSprite::GetDestY());
}


////////////////////////////////////////////////////////////////////////////////////////////////
