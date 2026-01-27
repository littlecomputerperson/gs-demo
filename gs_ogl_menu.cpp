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


//==============================================================================================
// Include Game System (GS) header files.
// ---------------------------------------------------------------------------------------------
#include "gs_ogl_menu.h"
//==============================================================================================


////////////////////////////////////////////////////////////////////////////////////////////////
// Constructor/Destructor Methods. /////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_OGLMenu::GS_OGLMenu():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Returns: Nothing.
//==============================================================================================

GS_OGLMenu::GS_OGLMenu()
{

    m_bIsReady = FALSE;

    m_lDestX = 0;
    m_lDestY = 0;

    m_nHighlightOption = -1;

    m_pszTitleString = NULL;
}


////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_OGLMenu::~GS_OGLMenu():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Returns: Nothing.
//==============================================================================================

GS_OGLMenu::~GS_OGLMenu()
{

    this->Destroy();
}


////////////////////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////////////////////
// Create/Destroy Methods. /////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_OGLMenu::Create():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Returns: TRUE if successfull, FALSE if failed.
//==============================================================================================

BOOL GS_OGLMenu::Create(const char* pszMenuPathname, const char* pszFontPathname, int nTilesPerLine,
                        int nTileWidth, int nTileHeight, BOOL bFiltered, BOOL bMipmap)
{

    BOOL bResult;

    // Create the tiled sprite that will be used to render the menu. Note that the images used
    // for the menu must always have the same number of tiles (MENU_TILES) and tiles per line
    // (TILES_PER_LINE), though the size of the tiles may differ.
    bResult = m_gsMenuSprite.Create(pszMenuPathname, MENU_TILES, MENU_TILES_PER_LINE,
                                    nTileWidth, nTileHeight, bFiltered, bMipmap);

    // Exit if failed.
    if (bResult != TRUE)
    {
        GS_Error::Report("GS_OGL_MENU.CPP", 93, "Failed to create menu sprite!");
        return FALSE;
    }

    // Create the font that will be used to render the menu title and options. Note that the
    // letter width and height for the font characters must be the same size as the tiles used
    // to create the menu itself.
    bResult = m_gsMenuFont.Create(pszFontPathname, nTilesPerLine, nTileWidth, nTileHeight,
                                  bFiltered, bMipmap);

    // Exit if failed.
    if (bResult != TRUE)
    {
        GS_Error::Report("GS_OGL_MENU.CPP", 105, "Failed to create menu font!");
        return FALSE;
    }

    m_bIsReady = TRUE;

    return TRUE;
}


////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_OGLMenu::Create():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Returns: TRUE if successfull, FALSE if failed.
//==============================================================================================

BOOL GS_OGLMenu::Create(const char* pszMenuPathname, GLuint glFontTexture, int nWidth, int nHeight,
                        int nTilesPerLine, int nTileWidth, int nTileHeight, BOOL bFiltered,
                        BOOL bMipmap)
{

    BOOL bResult;

    // Create the tiled sprite that will be used to render the menu. Note that the images used
    // for the menu must always have the same number of tiles (MENU_TILES) and tiles per line
    // (TILES_PER_LINE), though the size of the tiles may differ.
    bResult = m_gsMenuSprite.Create(pszMenuPathname, MENU_TILES, MENU_TILES_PER_LINE,
                                    nTileWidth, nTileHeight, bFiltered, bMipmap);

    // Exit if failed.
    if (bResult != TRUE)
    {
        GS_Error::Report("GS_OGL_MENU.CPP", 140, "Failed to create menu sprite!");
        return FALSE;
    }

    // Create the font that will be used to render the menu title and options. Note that the
    // letter width and height for the font characters must be the same size as the tiles used
    // to create the menu itself.
    bResult = m_gsMenuFont.Create(glFontTexture, nWidth,nHeight, nTilesPerLine, nTileWidth,
                                  nTileHeight);

    // Exit if failed.
    if (bResult != TRUE)
    {
        GS_Error::Report("GS_OGL_MENU.CPP", 152, "Failed to create menu font!");
        return FALSE;
    }

    m_bIsReady = TRUE;

    return TRUE;
}


////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_OGLMenu::Destroy():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Returns: Nothing.
//==============================================================================================

void GS_OGLMenu::Destroy()
{

    if (!m_bIsReady)
    {
        return;
    }

    m_gsMenuSprite.Destroy();
    m_gsMenuFont.Destroy();

    if (m_pszTitleString)
    {
        delete [] m_pszTitleString;
    }
    m_pszTitleString = NULL;

    for (int nLoop=0; nLoop<m_gsOptionList.GetNumItems(); nLoop++)
    {
        if (m_gsOptionList.GetItem(nLoop))
        {
            delete [] m_gsOptionList.GetItem(nLoop);
        }
    }
    m_gsOptionList.RemoveAllItems();

    m_lDestX = 0;
    m_lDestY = 0;

    m_nHighlightOption = -1;

    m_bIsReady = FALSE;
}


////////////////////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////////////////////
// Render Methods. /////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_OGLMenu::Render():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Returns: TRUE if successfull, FALSE if failed.
//==============================================================================================

BOOL GS_OGLMenu::Render(HWND hWnd)
{

    // Exit if menu not created.
    if (!m_bIsReady)
    {
        return FALSE;
    }

    // Setup variables that will be used to render the menu.
    int nRows    = this->GetRows();
    int nColumns = this->GetCols();

    int nHighlightRow;

    float fWidth  =  this->GetCols() * m_gsMenuSprite.GetFrameWidth();
    float fHeight = this->GetRows() * m_gsMenuSprite.GetFrameHeight();

    float fTileWidth  = m_gsMenuSprite.GetFrameWidth();
    float fTileHeight = m_gsMenuSprite.GetFrameHeight();

    /////////////////////////////////////////////////////////////////////////////////////////////
    // Render the menu. /////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////

    // Render top left corner.
    m_gsMenuSprite.SetFrame(MENU_TOP_LEFT);
    m_gsMenuSprite.SetDestX(m_lDestX);
    m_gsMenuSprite.SetDestY(m_lDestY + fHeight - fTileHeight);
    m_gsMenuSprite.Render(hWnd);

    // Render top right corner.
    m_gsMenuSprite.SetFrame(MENU_TOP_RIGHT);
    m_gsMenuSprite.SetDestX(m_lDestX + fWidth - fTileWidth);
    m_gsMenuSprite.SetDestY(m_lDestY + fHeight - fTileHeight);
    m_gsMenuSprite.Render(hWnd);

    // Render bottom left corner.
    m_gsMenuSprite.SetFrame(MENU_BOTTOM_LEFT);
    m_gsMenuSprite.SetDestX(m_lDestX);
    m_gsMenuSprite.SetDestY(m_lDestY);
    m_gsMenuSprite.Render(hWnd);

    // Render bottom right corner.
    m_gsMenuSprite.SetFrame(MENU_BOTTOM_RIGHT);
    m_gsMenuSprite.SetDestX(m_lDestX + fWidth - fTileWidth);
    m_gsMenuSprite.SetDestY(m_lDestY);
    m_gsMenuSprite.Render(hWnd);

    int xLoop, yLoop;

    // Render the top and bottom borders.
    for (xLoop = 1; xLoop < (nColumns - 1); xLoop++)
    {
        // Render top border.
        m_gsMenuSprite.SetFrame(MENU_TOP);
        m_gsMenuSprite.SetDestX(m_lDestX + (xLoop * fTileWidth));
        m_gsMenuSprite.SetDestY(m_lDestY + fHeight - fTileHeight);
        m_gsMenuSprite.Render(hWnd);
        // Render bottom border.
        m_gsMenuSprite.SetFrame(MENU_BOTTOM);
        m_gsMenuSprite.SetDestX(m_lDestX + (xLoop * fTileWidth));
        m_gsMenuSprite.SetDestY(m_lDestY);
        m_gsMenuSprite.Render(hWnd);
    }

    // Render the left and right borders.
    for (yLoop = 1; yLoop < (nRows - 1); yLoop++)
    {
        // Determine wether to render the left seperator.
        if ((m_pszTitleString) && (yLoop==2))
        {
            m_gsMenuSprite.SetFrame(MENU_SEPERATOR_LEFT);
        }
        else
        {
            m_gsMenuSprite.SetFrame(MENU_LEFT);
        }
        // Render left border or seperator.
        m_gsMenuSprite.SetDestX(m_lDestX);
        m_gsMenuSprite.SetDestY(m_lDestY + fHeight - ((yLoop+1) * fTileHeight));
        m_gsMenuSprite.Render(hWnd);
        // Determine wether to render the right seperator.
        if ((m_pszTitleString) && (yLoop==2))
        {
            m_gsMenuSprite.SetFrame(MENU_SEPERATOR_RIGHT);
        }
        else
        {
            m_gsMenuSprite.SetFrame(MENU_RIGHT);
        }
        // Render right border or seperator.
        m_gsMenuSprite.SetDestX(m_lDestX + fWidth - fTileWidth);
        m_gsMenuSprite.SetDestY(m_lDestY + fHeight - ((yLoop+1) * fTileHeight));
        m_gsMenuSprite.Render(hWnd);
    }


    // Render the menu background and seperator.
    for (xLoop = 1; xLoop < (nColumns - 1); xLoop++)
    {
        for (yLoop = 1; yLoop < (nRows - 1); yLoop++)
        {
            // Determine wether to render menu background or seperator.
            if ((m_pszTitleString) && (yLoop==2))
            {
                m_gsMenuSprite.SetFrame(MENU_SEPERATOR);
            }
            else
            {
                m_gsMenuSprite.SetFrame(MENU_BACKGROUND);
            }
            // Render menu background or seperator.
            m_gsMenuSprite.SetDestX(m_lDestX + (xLoop * fTileWidth));
            m_gsMenuSprite.SetDestY(m_lDestY + fHeight - ((yLoop+1) * fTileHeight));
            m_gsMenuSprite.Render(hWnd);
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////
    // Render the menu highlight.////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////

    // Render highlight only if valid option is highlighted.
    if (m_nHighlightOption >= 0)
    {

        // Determine which row to highlight.
        if (m_pszTitleString)
        {
            nHighlightRow = (3 + m_nHighlightOption);
        }
        else
        {
            nHighlightRow = (1 + m_nHighlightOption);
        }

        // Render the left highlight.
        m_gsMenuSprite.SetFrame(MENU_HIGHLIGHT_LEFT);
        m_gsMenuSprite.SetDestX(m_lDestX);
        m_gsMenuSprite.SetDestY(m_lDestY + fHeight - ((nHighlightRow+1) * fTileHeight));
        m_gsMenuSprite.Render(hWnd);

        // Render the highlighted row.
        for (xLoop = 1; xLoop <= lstrlen(m_gsOptionList[m_nHighlightOption]); xLoop++)
        {
            m_gsMenuSprite.SetFrame(MENU_HIGHLIGHT);
            m_gsMenuSprite.SetDestX(m_lDestX + (xLoop * fTileWidth));
            m_gsMenuSprite.SetDestY(m_lDestY + fHeight - ((nHighlightRow+1) * fTileHeight));
            m_gsMenuSprite.Render(hWnd);
        }

        // Render the right highlight.
        m_gsMenuSprite.SetFrame(MENU_HIGHLIGHT_RIGHT);
        m_gsMenuSprite.SetDestX(m_lDestX + fWidth - fTileWidth);
        m_gsMenuSprite.SetDestY(m_lDestY + fHeight - ((nHighlightRow+1) * fTileHeight));
        m_gsMenuSprite.Render(hWnd);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////
    // Render the menu text. ////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////

    // Render the title if any.
    if (m_pszTitleString)
    {
        m_gsMenuFont.SetText(m_pszTitleString);
        m_gsMenuFont.SetDestX(m_lDestX + fTileWidth);
        m_gsMenuFont.SetDestY(m_lDestY + fHeight - (fTileHeight*2));
        m_gsMenuFont.Render(hWnd);
    }

    // Render the menu options.
    for (yLoop = 0; yLoop < m_gsOptionList.GetNumItems(); yLoop++)
    {
        // Get the option text to render.
        m_gsMenuFont.SetText(m_gsOptionList[yLoop]);
        // Determine the destination coordinates.
        m_gsMenuFont.SetDestX(m_lDestX + fTileWidth);
        if (m_pszTitleString)
        {
            m_gsMenuFont.SetDestY(m_lDestY + fHeight - (fTileHeight*(yLoop+4)));
        }
        else
        {
            m_gsMenuFont.SetDestY(m_lDestY + fHeight - (fTileHeight*(yLoop+2)));
        }
        // Render the option text.
        m_gsMenuFont.Render(hWnd);
    }

    return TRUE;
}


////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_OGLMenu::Render():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Returns: TRUE if successfull, FALSE if failed.
//==============================================================================================

BOOL GS_OGLMenu::Render(GS_OGLColor gsNormalColor, GS_OGLColor gsHighlightColor, HWND hWnd)
{

    // Exit if menu not created.
    if (!m_bIsReady)
    {
        return FALSE;
    }

    // Setup variables that will be used to render the menu.
    int nRows    = this->GetRows();
    int nColumns = this->GetCols();

    float fWidth  =  this->GetCols() * m_gsMenuSprite.GetFrameWidth();
    float fHeight = this->GetRows() * m_gsMenuSprite.GetFrameHeight();

    float fTileWidth  = m_gsMenuSprite.GetFrameWidth();
    float fTileHeight = m_gsMenuSprite.GetFrameHeight();

    int nHighlightRow;

    /////////////////////////////////////////////////////////////////////////////////////////////
    // Render the menu. /////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////

    // Render top left corner.
    m_gsMenuSprite.SetFrame(MENU_TOP_LEFT);
    m_gsMenuSprite.SetDestX(m_lDestX);
    m_gsMenuSprite.SetDestY(m_lDestY + fHeight - fTileHeight);
    m_gsMenuSprite.Render(hWnd);

    // Render top right corner.
    m_gsMenuSprite.SetFrame(MENU_TOP_RIGHT);
    m_gsMenuSprite.SetDestX(m_lDestX + fWidth - fTileWidth);
    m_gsMenuSprite.SetDestY(m_lDestY + fHeight - fTileHeight);
    m_gsMenuSprite.Render(hWnd);

    // Render bottom left corner.
    m_gsMenuSprite.SetFrame(MENU_BOTTOM_LEFT);
    m_gsMenuSprite.SetDestX(m_lDestX);
    m_gsMenuSprite.SetDestY(m_lDestY);
    m_gsMenuSprite.Render(hWnd);

    // Render bottom right corner.
    m_gsMenuSprite.SetFrame(MENU_BOTTOM_RIGHT);
    m_gsMenuSprite.SetDestX(m_lDestX + fWidth - fTileWidth);
    m_gsMenuSprite.SetDestY(m_lDestY);
    m_gsMenuSprite.Render(hWnd);

    int xLoop, yLoop;

    // Render the top and bottom borders.
    for (xLoop = 1; xLoop < (nColumns - 1); xLoop++)
    {
        // Render top border.
        m_gsMenuSprite.SetFrame(MENU_TOP);
        m_gsMenuSprite.SetDestX(m_lDestX + (xLoop * fTileWidth));
        m_gsMenuSprite.SetDestY(m_lDestY + fHeight - fTileHeight);
        m_gsMenuSprite.Render(hWnd);
        // Render bottom border.
        m_gsMenuSprite.SetFrame(MENU_BOTTOM);
        m_gsMenuSprite.SetDestX(m_lDestX + (xLoop * fTileWidth));
        m_gsMenuSprite.SetDestY(m_lDestY);
        m_gsMenuSprite.Render(hWnd);
    }

    // Render the left and right borders.
    for (yLoop = 1; yLoop < (nRows - 1); yLoop++)
    {
        // Determine wether to render the left seperator.
        if ((m_pszTitleString) && (yLoop==2))
        {
            m_gsMenuSprite.SetFrame(MENU_SEPERATOR_LEFT);
        }
        else
        {
            m_gsMenuSprite.SetFrame(MENU_LEFT);
        }
        // Render left border or seperator.
        m_gsMenuSprite.SetDestX(m_lDestX);
        m_gsMenuSprite.SetDestY(m_lDestY + fHeight - ((yLoop+1) * fTileHeight));
        m_gsMenuSprite.Render(hWnd);
        // Determine wether to render the right seperator.
        if ((m_pszTitleString) && (yLoop==2))
        {
            m_gsMenuSprite.SetFrame(MENU_SEPERATOR_RIGHT);
        }
        else
        {
            m_gsMenuSprite.SetFrame(MENU_RIGHT);
        }
        // Render right border or seperator.
        m_gsMenuSprite.SetDestX(m_lDestX + fWidth - fTileWidth);
        m_gsMenuSprite.SetDestY(m_lDestY + fHeight - ((yLoop+1) * fTileHeight));
        m_gsMenuSprite.Render(hWnd);
    }


    // Render the menu background and seperator.
    for (xLoop = 1; xLoop < (nColumns - 1); xLoop++)
    {
        for (yLoop = 1; yLoop < (nRows - 1); yLoop++)
        {
            // Determine wether to render menu background or seperator.
            if ((m_pszTitleString) && (yLoop==2))
            {
                m_gsMenuSprite.SetFrame(MENU_SEPERATOR);
            }
            else
            {
                m_gsMenuSprite.SetFrame(MENU_BACKGROUND);
            }
            // Render menu background or seperator.
            m_gsMenuSprite.SetDestX(m_lDestX + (xLoop * fTileWidth));
            m_gsMenuSprite.SetDestY(m_lDestY + fHeight - ((yLoop+1) * fTileHeight));
            m_gsMenuSprite.Render(hWnd);
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////
    // Render the menu highlight.////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////

    // Render highlight only if valid option is highlighted.
    if (m_nHighlightOption >= 0)
    {
        // Determine which row to highlight.
        if (m_pszTitleString)
        {
            nHighlightRow = (3 + m_nHighlightOption);
        }
        else
        {
            nHighlightRow = (1 + m_nHighlightOption);
        }
        // Render the left highlight.
        m_gsMenuSprite.SetFrame(MENU_HIGHLIGHT_LEFT);
        m_gsMenuSprite.SetDestX(m_lDestX);
        m_gsMenuSprite.SetDestY(m_lDestY + fHeight - ((nHighlightRow+1) * fTileHeight));
        m_gsMenuSprite.Render(hWnd);
        // Render the right highlight.
        m_gsMenuSprite.SetFrame(MENU_HIGHLIGHT_RIGHT);
        m_gsMenuSprite.SetDestX(m_lDestX + fWidth - fTileWidth);
        m_gsMenuSprite.SetDestY(m_lDestY + fHeight - ((nHighlightRow+1) * fTileHeight));
        m_gsMenuSprite.Render(hWnd);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////
    // Render the menu text. ////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////

    // Reset the modulate color for the title.
    m_gsMenuFont.SetModulateColor(1.0f, 1.0f, 1.0f, 1.0f);

    // Render the title if any.
    if (m_pszTitleString)
    {
        m_gsMenuFont.SetText(m_pszTitleString);
        m_gsMenuFont.SetDestX(m_lDestX + fTileWidth);
        m_gsMenuFont.SetDestY(m_lDestY + fHeight - (fTileHeight*2));
        m_gsMenuFont.Render(hWnd);
    }

    // Render the menu options.
    for (yLoop = 0; yLoop < m_gsOptionList.GetNumItems(); yLoop++)
    {
        // Render options using the normal color.
        if (m_nHighlightOption >= 0)
        {
            m_gsMenuFont.SetModulateColor(gsNormalColor);
        }
        // Render the higlighted option in the highlight color.
        if (yLoop == m_nHighlightOption)
        {
            m_gsMenuFont.SetModulateColor(gsHighlightColor);
        }
        // Get the option text to render.
        m_gsMenuFont.SetText(m_gsOptionList[yLoop]);
        // Determine the destination coordinates.
        m_gsMenuFont.SetDestX(m_lDestX + fTileWidth);
        if (m_pszTitleString)
        {
            m_gsMenuFont.SetDestY(m_lDestY + fHeight - (fTileHeight*(yLoop+4)));
        }
        else
        {
            m_gsMenuFont.SetDestY(m_lDestY + fHeight - (fTileHeight*(yLoop+2)));
        }
        // Render the option text.
        m_gsMenuFont.Render(hWnd);
    }

    return TRUE;
}


////////////////////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////////////////////
// List Methods. ///////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_OGLMenu::AddOption():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Returns: TRUE if successfull, FALSE if failed.
//==============================================================================================

BOOL GS_OGLMenu::AddOption(const char* pszOptionString)
{

    // Exit if no valid string.
    if (!pszOptionString)
    {
        return FALSE;
    }

    // Allocate memory for option string and copy it over.
    int nStringLength = strlen(pszOptionString) + 1;
    char* pszTempString = new char [nStringLength];
    strcpy(pszTempString, pszOptionString);
    pszTempString[nStringLength-1] = '\0';

    // Add the option to the list.
    m_gsOptionList.AddItem(pszTempString);

    return TRUE;
}


////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_OGLMenu::RemoveOption():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Returns: TRUE if successfull, FALSE if failed.
//==============================================================================================

BOOL GS_OGLMenu::RemoveOption(int nIndex)
{

    // Can't remove option if invalid index.
    if ((nIndex < 0) || (nIndex > m_gsOptionList.GetNumItems()))
    {
        return FALSE;
    }

    // Do we have a valid item?
    if (m_gsOptionList.GetItem(nIndex))
    {
        // De-allocate memory for the selected item.
        delete [] m_gsOptionList.GetItem(nIndex);
    }

    // Remove option indicated by index from list.
    m_gsOptionList.RemoveIndex(nIndex);

    return TRUE;
}


////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_OGLMenu::SetOption():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Returns: TRUE if successfull, FALSE if failed.
//==============================================================================================

BOOL GS_OGLMenu::SetOption(int nIndex, const char* pszOptionString)
{

    // Exit if no valid string or invalid index number.
    if ((!pszOptionString) || (nIndex < 0) || (nIndex > m_gsOptionList.GetNumItems()))
    {
        return FALSE;
    }

    // Do we have a valid item?
    if (m_gsOptionList.GetItem(nIndex))
    {
        // De-allocate memory for the selected item.
        delete [] m_gsOptionList.GetItem(nIndex);
    }

    // Allocate memory for option string and copy it over.
    int nStringLength = strlen(pszOptionString) + 1;
    char* pszTempString = new char [nStringLength];
    strcpy(pszTempString, pszOptionString);
    pszTempString[nStringLength-1] = '\0';

    // Set the item indicated to the new value.
    m_gsOptionList.SetItem(nIndex, pszTempString);

    return TRUE;
}


////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_OGLMenu::GetOption():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Returns: The option text if successfull, NULL if failed.
//==============================================================================================

char* GS_OGLMenu::GetOption(int nIndex)
{

    // Exit if no valid index number.
    if ((nIndex < 0) || (nIndex > m_gsOptionList.GetNumItems()))
    {
        return NULL;
    }

    // Retrieve a pointer to the item indicated.
    if (m_gsOptionList[nIndex])
    {
        return m_gsOptionList[nIndex];
    }

    return NULL;
}


////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_OGLMenu::GetOptionRect():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Returns: Nothing.
//==============================================================================================

void GS_OGLMenu::GetOptionRect(int nIndex, RECT* pDestRect)
{

    // Exit if invalid index number.
    if ((nIndex < 0) || (nIndex > m_gsOptionList.GetNumItems()) ||
            (m_gsOptionList.GetNumItems() <= 0))
    {
        return;
    }

    long lStartX = m_lDestX;
    long lStartY = m_lDestY;

    // Set starting x position to the option.

    // Add width of border.
    lStartX += m_gsMenuSprite.GetFrameWidth();

    // Set starting y position to the option.

    // Add a row for each option prior to the specified option.
    lStartY += (m_gsMenuSprite.GetFrameHeight() * (m_gsOptionList.GetNumItems() - nIndex));

    // Fill the rectangle with the screen coordinates of specified option.
    SetRect(pDestRect, lStartX, lStartY + m_gsMenuSprite.GetFrameHeight(),
            lStartX + (lstrlen(m_gsOptionList[nIndex]) * m_gsMenuSprite.GetFrameWidth()),
            lStartY);
}


////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_OGLMenu::GetNumOptions():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Returns: An integer value indicating the number of options in the menu.
//==============================================================================================

int GS_OGLMenu::GetNumOptions()
{

    // Return the number of options in the list.
    return (m_gsOptionList.GetNumItems());
}


////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_OGLMenu::ClearOptions():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Returns: Nothing.
//==============================================================================================

void GS_OGLMenu::ClearOptions()
{

    // For every option in the list.
    for (int nLoop=0; nLoop<m_gsOptionList.GetNumItems(); nLoop++)
    {
        // Do we have a valid item?
        if (m_gsOptionList.GetItem(nLoop))
        {
            // De-allocate memory for the selected item.
            delete [] m_gsOptionList.GetItem(nLoop);
        }
    }

    // Remove all the options from the list.
    m_gsOptionList.RemoveAllItems();
}


////////////////////////////////////////////////////////////////////////////////////////////////
// Highlight Methods. //////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_OGLMenu::GetHighlight():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Returns: The index number of the currently highlighted option.
//==============================================================================================

int GS_OGLMenu::GetHighlight()
{

    return m_nHighlightOption;
}


////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_OGLMenu::SetHighlight():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Returns: Nothing.
//==============================================================================================

void GS_OGLMenu::SetHighlight(int nIndex)
{

    // Highlight no option if invalid option is highlighted.
    if ((nIndex < 0) || (nIndex > m_gsOptionList.GetNumItems()))
    {
        m_nHighlightOption = -1;
    }

    m_nHighlightOption = nIndex;
}


////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_OGLMenu::GetHighlightOption():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Returns: The option text if successfull, NULL if failed.
//==============================================================================================

char* GS_OGLMenu::GetHighlightOption()
{

    // Exit if no option is highlighted
    if (m_nHighlightOption < 0)
    {
        return NULL;
    }

    // Retrieve a pointer to the item indicated.
    if (m_gsOptionList[m_nHighlightOption])
    {
        return m_gsOptionList[m_nHighlightOption];
    }

    return NULL;
}


////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_OGLMenu::HighlightNext():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Returns: Nothing.
//==============================================================================================

void GS_OGLMenu::HighlightNext(BOOL bSkipEmpty)
{

    if (m_nHighlightOption < m_gsOptionList.GetNumItems() - 1)
    {
        m_nHighlightOption++;
    }
    else
    {
        m_nHighlightOption = 0;
    }

    // Should we skip options with no text?
    if (bSkipEmpty)
    {
        int nOptionCounter = 0;
        // Loop while the next option has no text.
        while (lstrlen(m_gsOptionList[m_nHighlightOption]) < 1)
        {
            if (m_nHighlightOption < m_gsOptionList.GetNumItems() - 1)
            {
                m_nHighlightOption++;
            }
            else
            {
                m_nHighlightOption = 0;
            }
            // Prevent endless loop if all options are empty.
            nOptionCounter++;
            if (nOptionCounter >= m_gsOptionList.GetNumItems())
            {
                break;
            }
        }
    }
}


////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_OGLMenu::HighlightPrev():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Returns: Nothing.
//==============================================================================================

void GS_OGLMenu::HighlightPrev(BOOL bSkipEmpty)
{

    if (m_nHighlightOption > 0)
    {
        m_nHighlightOption--;
    }
    else
    {
        m_nHighlightOption = m_gsOptionList.GetNumItems() - 1;
    }

    // Should we skip options with no text?
    if (bSkipEmpty)
    {
        int nOptionCounter = 0;
        // Loop while the previous option has no text.
        while (lstrlen(m_gsOptionList[m_nHighlightOption]) < 1)
        {
            if (m_nHighlightOption > 0)
            {
                m_nHighlightOption--;
            }
            else
            {
                m_nHighlightOption = m_gsOptionList.GetNumItems() - 1;
            }
            // Prevent endless loop if all options are empty.
            nOptionCounter++;
            if (nOptionCounter >= m_gsOptionList.GetNumItems())
            {
                break;
            }
        }
    }
}


////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_OGLMenu::GetHighlightRect():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Returns: Nothing.
//==============================================================================================

void GS_OGLMenu::GetHighlightRect(RECT* pDestRect)
{

    // Exit if invalid index number.
    if ((m_nHighlightOption < 0) || (m_gsOptionList.GetNumItems() <= 0))
    {
        return;
    }

    long lStartX = m_lDestX;
    long lStartY = m_lDestY;

    float fTileWidth  = m_gsMenuSprite.GetFrameWidth();
    float fTileHeight = m_gsMenuSprite.GetFrameHeight();

    // Set starting x position to the option.

    // Add width of border.
    lStartX += fTileWidth;

    // Set starting y position to the option.

    // Add a row for each option prior to the highlighted option.
    lStartY += (fTileHeight * (m_gsOptionList.GetNumItems() - m_nHighlightOption));

    // Fill the rectangle with the screen coordinates of specified option.
    SetRect(pDestRect, lStartX, lStartY + fTileHeight,
            lStartX + (lstrlen(m_gsOptionList[m_nHighlightOption]) * fTileWidth), lStartY);
}


////////////////////////////////////////////////////////////////////////////////////////////////
// Get/Set Methods. ////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_OGLMenu::SetTitle():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Returns: Nothing.
//==============================================================================================

void GS_OGLMenu::SetTitle(const char* pszTextString)
{

    // Free the title string if any.
    if (m_pszTitleString)
    {
        delete [] m_pszTitleString;
    }
    m_pszTitleString = NULL;

    // Allocate memory and assign new title string.
    if (lstrlen(pszTextString) > 0)
    {
        int nStringLength = strlen(pszTextString) + 1; // Determine length of the title string.
        m_pszTitleString  = new char[nStringLength];   // Allocate memory for the title string.
        strcpy(m_pszTitleString, pszTextString);       // Assign the string to the class string.
        m_pszTitleString[nStringLength-1] = '\0';      // End the title string with a NULL;
    }
}


////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_OGLMenu::GetCols():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Returns: An integer number indicating the number of columns in the menu.
//==============================================================================================

int GS_OGLMenu::GetCols()
{

    int nNumColumns = lstrlen(m_pszTitleString);

    // Check all the options and set the number of columns to the longest one.
    for (int nLoop = 0; nLoop < m_gsOptionList.GetNumItems(); nLoop++)
    {
        if (lstrlen(m_gsOptionList[nLoop]) > nNumColumns)
        {
            nNumColumns = lstrlen(m_gsOptionList[nLoop]);
        }
    }

    // Add the left and right columns.
    nNumColumns += 2;

    return nNumColumns;
}


////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_OGLMenu::GetRows():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Returns: An integer number indicating the number of rows in the menu.
//==============================================================================================

int GS_OGLMenu::GetRows()
{

    int nNumRows = 0;

    // If there is a valid title, add two rows, one for the title string and one for the
    // seperator between the title and the rest of the menu.
    if (m_pszTitleString)
    {
        nNumRows += 2;
    }

    // Add a row for each menu option.
    nNumRows += m_gsOptionList.GetNumItems();

    // Add the top and bottom rows.
    nNumRows += 2;

    return nNumRows;
}


////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_OGLMenu::GetScaleXY():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Returns: Nothing.
//==============================================================================================

void GS_OGLMenu::GetScaleXY(float* pScaleX, float* pScaleY)
{

    m_gsMenuSprite.GetScaleXY(pScaleX, pScaleY);
}


////////////////////////////////////////////////////////////////////////////////////////////////
// GS_OGLMenu::SetScaleXY():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Returns: Nothing.
//==============================================================================================

void GS_OGLMenu::SetScaleXY(float glfScaleX, float glfScaleY)
{

    m_gsMenuSprite.SetScaleXY(glfScaleX, glfScaleY);
    m_gsMenuFont.SetScaleXY(glfScaleX, glfScaleY);
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_OGLMenu::GetScaleX():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Returns: The horizontal scale of the sprite as a factor of 1.
//==============================================================================================

float GS_OGLMenu::GetScaleX()
{

    return m_gsMenuSprite.GetScaleX();
}


////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_OGLMenu::SetScaleX():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Returns: Nothing.
//==============================================================================================

void GS_OGLMenu::SetScaleX(float glfScaleX)
{

    m_gsMenuSprite.SetScaleX(glfScaleX);
    m_gsMenuFont.SetScaleX(glfScaleX);
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_OGLMenu::GetScaleY():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Returns: The vertical scale of the sprite as a factor of 1.
//==============================================================================================

float GS_OGLMenu::GetScaleY()
{

    return m_gsMenuSprite.GetScaleY();
}


////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_OGLMenu::SetScaleY():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Returns: Nothing.
//==============================================================================================

void GS_OGLMenu::SetScaleY(float glfScaleY)
{

    m_gsMenuSprite.SetScaleY(glfScaleY);
    m_gsMenuFont.SetScaleY(glfScaleY);
}


////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_OGLMenu::GetWidth():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Returns: A long number indicating the width of the entire menu.
//==============================================================================================

long GS_OGLMenu::GetWidth()
{

    return (this->GetCols() * m_gsMenuSprite.GetFrameWidth());
}


////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_OGLMenu::GetHeight():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Returns: A long number indicating the height of the entire menu.
//==============================================================================================

long GS_OGLMenu::GetHeight()
{

    return (this->GetRows() * m_gsMenuSprite.GetFrameHeight());
}


////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_OGLMenu::SetDestX():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Returns: Nothing.
//==============================================================================================

void GS_OGLMenu::SetDestX(long lDestX)
{

    m_lDestX = lDestX;
}


////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_OGLMenu::GetDestX():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Returns: A long number indicating the horizontal screen coordinate of the menu.
//==============================================================================================

long GS_OGLMenu::GetDestX()
{

    return m_lDestX;
}


////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_OGLMenu::SetDestY():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Returns: Nothing.
//==============================================================================================

void GS_OGLMenu::SetDestY(long lDestY)
{

    m_lDestY = lDestY;
}


////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_OGLMenu::GetDestY():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Returns: A long number indicating the vertical screen coordinate of the menu.
//==============================================================================================

long GS_OGLMenu::GetDestY()
{

    return m_lDestY;
}


////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_OGLMenu::GetDestRect():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Returns: Nothing.
//==============================================================================================

void GS_OGLMenu::GetDestRect(long* pLeft, long* pTop, long* pRight, long* pBottom)
{

    *pLeft   = m_lDestX;
    *pTop    = m_lDestY + this->GetHeight();
    *pBottom = m_lDestY;
    *pRight  = m_lDestX + this->GetWidth();
}


void GS_OGLMenu::GetDestRect(RECT* pDestRect)
{

    SetRect(pDestRect, m_lDestX, m_lDestY + this->GetHeight(), m_lDestX + this->GetWidth(),
            m_lDestY);
}


////////////////////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////////////////////
// Color Methods. //////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_OGLMenu::GetModulateColor():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Returns: Nothing.
//==============================================================================================

void GS_OGLMenu::GetModulateColor(GS_OGLColor* pgsColor)
{

    m_gsMenuSprite.GetModulateColor(pgsColor);
}


void GS_OGLMenu::GetModulateColor(float* pRed, float* pGreen, float* pBlue, float* pAlpha)
{

    m_gsMenuSprite.GetModulateColor(pRed, pGreen, pBlue, pAlpha);
}


////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_OGLMenu::SetModulateColor():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Returns: Nothing.
//==============================================================================================

void GS_OGLMenu::SetModulateColor(GS_OGLColor gsColor)
{

    m_gsMenuSprite.SetModulateColor(gsColor);
    m_gsMenuFont.SetModulateColor(gsColor);
}


void GS_OGLMenu::SetModulateColor(float fRed, float fGreen, float fBlue, float fAlpha)
{

    m_gsMenuSprite.SetModulateColor(fRed, fGreen, fBlue, fAlpha);
    m_gsMenuFont.SetModulateColor(fRed, fGreen, fBlue, fAlpha);
}


////////////////////////////////////////////////////////////////////////////////////////////////