/*============================================================================================+
 | Game System (GS) Library                                                                   |
 |--------------------------------------------------------------------------------------------|
 | FILES: gs_ogl_map.cpp, gs_ogl_map.h                                                        |
 |--------------------------------------------------------------------------------------------|
 | CLASS: GS_OGLMap                                                                           |
 |--------------------------------------------------------------------------------------------|
 | ABOUT: ...                                                                                 |
 |--------------------------------------------------------------------------------------------|
 |                                                                                    08/2003 |
 +============================================================================================*/


//==============================================================================================
// Include Game System (GS) header files.
// ---------------------------------------------------------------------------------------------
#include "gs_ogl_map.h"
//==============================================================================================


////////////////////////////////////////////////////////////////////////////////////////////////
// Constructor/Destructor Methods. /////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_OGLMap::GS_OGLMap():
// ---------------------------------------------------------------------------------------------
// Purpose: The constructor, initializes class data to defaults when class object is created.
// ---------------------------------------------------------------------------------------------
// Returns: Nothing.
//==============================================================================================

GS_OGLMap::GS_OGLMap()
{

    for (int nRows = 0; nRows < MAX_MAP_ROWS; nRows++)
    {
        for (int nCols = 0; nCols < MAX_MAP_COLS; nCols++)
        {
            m_nMap[nRows][nCols] = -1;
        }
    }
    m_nNumCols = 0;
    m_nNumRows = 0;

    m_rcClipBox.left   = 0;
    m_rcClipBox.top    = 0;
    m_rcClipBox.right  = 0;
    m_rcClipBox.bottom = 0;

    m_nMapCoordX = 0;
    m_nMapCoordY = 0;

    m_nTileWidth  = 0;
    m_nTileHeight = 0;

    m_nClearTileID = -1;

    m_bLimitX = FALSE;
    m_bLimitY = FALSE;
    m_bWrapX  = FALSE;
    m_bWrapY  = FALSE;

    m_bIsReady = FALSE;
}


////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_OGLMap::~GS_OGLMap():
// ---------------------------------------------------------------------------------------------
// Purpose: The de-structor, de-initializes class data when class object is destroyed and frees
//          all memory used by it.
// ---------------------------------------------------------------------------------------------
// Returns: Nothing.
//==============================================================================================

GS_OGLMap::~GS_OGLMap()
{

    this->Destroy();
}


////////////////////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////////////////////
// Create/Destroy Methods. /////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_OGLMap::Create():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Returns: TRUE if successfull, FALSE if not.
//==============================================================================================

BOOL GS_OGLMap::Create(const char* pszPathname, int nTotalTiles, int nTilesPerLine, int nTileWidth,
                       int nTileHeight, BOOL bFiltered, BOOL bMipmap)
{

    // Create the sprite containing all the image tiles for the map.
    BOOL bResult = m_gsTileSprites.Create(pszPathname, nTotalTiles, nTilesPerLine, nTileWidth,
                                          nTileHeight, bFiltered, bMipmap);

    // Was creating tile sprites not successful?
    if (!bResult)
    {
        return FALSE;
    }

    // Assign tile width and height.
    m_nTileWidth  = nTileWidth;
    m_nTileHeight = nTileHeight;

    m_bIsReady = TRUE;

    return TRUE;
}


//==============================================================================================
// GS_OGLMap::Create():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Returns: TRUE if successfull, FALSE if not.
//==============================================================================================

BOOL GS_OGLMap::Create(GLuint glTexture, int nWidth, int nHeight, int nTotalTiles,
                       int nTilesPerLine, int nTileWidth, int nTileHeight)
{

    // Create the sprite containing all the image tiles for the map.
    BOOL bResult = m_gsTileSprites.Create(glTexture, nWidth, nHeight, nTotalTiles, nTilesPerLine,
                                          nTileWidth, nTileHeight);

    // Was creating tile sprites not successful?
    if (!bResult)
    {
        return FALSE;
    }

    // Assign tile width and height.
    m_nTileWidth  = nTileWidth;
    m_nTileHeight = nTileHeight;

    m_bIsReady = TRUE;

    return TRUE;
}


////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_OGLMap::Destroy():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Returns: Nothing.
//==============================================================================================

void GS_OGLMap::Destroy()
{

    if (!m_bIsReady)
    {
        return;
    }

    m_gsTileSprites.Destroy();

    for (int nRows = 0; nRows < MAX_MAP_ROWS; nRows++)
    {
        for (int nCols = 0; nCols < MAX_MAP_COLS; nCols++)
        {
            m_nMap[nRows][nCols] = -1;
        }
    }
    m_nNumCols = 0;
    m_nNumRows = 0;

    m_rcClipBox.left   = 0;
    m_rcClipBox.top    = 0;
    m_rcClipBox.right  = 0;
    m_rcClipBox.bottom = 0;

    m_nMapCoordX = 0;
    m_nMapCoordY = 0;

    m_nTileWidth  = 0;
    m_nTileHeight = 0;

    m_nClearTileID = -1;

    m_bLimitX = FALSE;
    m_bLimitY = FALSE;
    m_bWrapX  = FALSE;
    m_bWrapY  = FALSE;

    m_bIsReady = FALSE;
}


////////////////////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////////////////////
// Render Methods. /////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_OGLMap::Render():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Returns: TRUE if successfull, FALSE if not.
//==============================================================================================

BOOL GS_OGLMap::Render(int nMapCoordX, int nMapCoordY, HWND hWnd)
{

    // Can't render if no valid sprite or map.
    if ((!m_bIsReady) || (m_nNumCols <= 0) || (m_nNumRows <= 0))
    {
        return FALSE;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////
    // Create a scissor box. ////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////

    // Create a window (called a scissor box) that will automatically clip any rendering to fit.
    glScissor
    (
        m_rcClipBox.left * g_fScaleFactorX + g_fRenderModX,
        m_rcClipBox.bottom * g_fScaleFactorY + g_fRenderModY,
        (m_rcClipBox.right - m_rcClipBox.left) * g_fScaleFactorX + g_fRenderModX,
        (m_rcClipBox.top - m_rcClipBox.bottom) * g_fScaleFactorY + g_fRenderModY
    );

    // Enable the scissor box.
    glEnable(GL_SCISSOR_TEST);

    /////////////////////////////////////////////////////////////////////////////////////////////
    // Render the map. //////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////

    // For every element of the map.
    for (int nRow = 0; nRow < m_nNumRows; nRow++)
    {
        for (int nCol = 0; nCol < m_nNumCols; nCol++)
        {
            // Is the map element visible?
            if (m_nMap[nRow][nCol] != m_nClearTileID)
            {
                // Set the tile image depending on the map.
                m_gsTileSprites.SetFrame(m_nMap[nRow][nCol]);
                // Determine the x and y coordinates at which to render the tile image.
                int nDestX = m_rcClipBox.left   + nMapCoordX + (m_nTileWidth  * nCol);
                int nDestY = m_rcClipBox.bottom + nMapCoordY + (m_nTileHeight * nRow);
                // Does the tile overlap the clip box area?
                if ((nDestX > m_rcClipBox.left   -  m_nTileWidth) &&
                        (nDestX < m_rcClipBox.right) &&
                        (nDestY > m_rcClipBox.bottom - m_nTileHeight) &&
                        (nDestY < m_rcClipBox.top))
                {
                    // Set the tile coordinates.
                    m_gsTileSprites.SetDestX(nDestX);
                    m_gsTileSprites.SetDestY(nDestY);
                    // Render the tile.
                    m_gsTileSprites.Render(hWnd);
                }
            }
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////
    // Destroy the scissor box. /////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////

    // Disable the scissor box.
    glDisable(GL_SCISSOR_TEST);

    return TRUE;
}


////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_OGLMap::Render():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Returns: TRUE if successfull, FALSE if not.
//==============================================================================================

BOOL GS_OGLMap::Render(HWND hWnd)
{

    // Can't render if no valid sprite or map.
    if ((!m_bIsReady) || (m_nNumCols <= 0) || (m_nNumRows <= 0))
    {
        return FALSE;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////
    // Initialize variables. ////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////

    // Setup initial wrap coordinates.
    int nWrapCoordX = m_nMapCoordX;
    int nWrapCoordY = m_nMapCoordY;

    /////////////////////////////////////////////////////////////////////////////////////////////
    // Render the map at the default map coordinates. ///////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////

    // Was rendering the map at the map coordinates not successful?
    if (!this->Render(m_nMapCoordX, m_nMapCoordY, hWnd))
    {
        return FALSE;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////
    // Render the horizontal wrap of the map. ///////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////

    // Should map be wrapped horizontally?
    if (m_bWrapX)
    {
        // Is there an open space to the right of the clip box?
        if (m_nMapCoordX < (this->GetClipBoxWidth() - this->GetMapWidth()))
        {
            // Set the map coordinate all the way right to fill the space.
            nWrapCoordX = m_nMapCoordX + this->GetMapWidth();
        }
        // Is there an open space to the left of the clip box?
        else if (m_nMapCoordX > 0)
        {
            // Set the map coordinate all the way left to fill the space.
            nWrapCoordX = m_nMapCoordX - this->GetMapWidth();
        }
        // Was the x coordinate altered?
        if (nWrapCoordX != m_nMapCoordX)
        {
            // Was rendering the map at the wrap x coordinate not successful?
            if (!this->Render(nWrapCoordX, m_nMapCoordY, hWnd))
            {
                return FALSE;
            }
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////
    // Render the vertical wrap of the map. /////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////

    // Should map be wrapped vertically?
    if (m_bWrapY)
    {
        // Is there an open space at the top of the clip box?
        if (m_nMapCoordY < (this->GetClipBoxHeight() - this->GetMapHeight()))
        {
            // Set the map coordinate all the way to the top to fill the space.
            nWrapCoordY = m_nMapCoordY + this->GetMapHeight();
        }
        // Is there an open space at the bottom of the clip box?
        else if (m_nMapCoordY > 0)
        {
            // Set the map coordinate all the way to the bottom to fill the space.
            nWrapCoordY = m_nMapCoordY - this->GetMapHeight();
        }
        // Was the y coordinate altered?
        if (nWrapCoordY != m_nMapCoordY)
        {
            // Was rendering the map at the wrap y coordinate not successful?
            if (!this->Render(m_nMapCoordX, nWrapCoordY, hWnd))
            {
                return FALSE;
            }
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////
    // Render the combined horizontal and vertical wraps. ///////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////

    // Were both the x and y coordinates altered?
    if ((nWrapCoordX != m_nMapCoordX) && (nWrapCoordY != m_nMapCoordY))
    {
        // Was rendering the map at the wrap coordinates not successful?
        if (!this->Render(nWrapCoordX, nWrapCoordY, hWnd))
        {
            return FALSE;
        }
    }

    return TRUE;
}


////////////////////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////////////////////
// ClipBox Methods. ///////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_OGLMap::SetClipBox():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Returns: Nothing.
//==============================================================================================

void GS_OGLMap::SetClipBox(int nMinX, int nMinY, int nMaxX, int nMaxY)
{

    if (nMinX >= 0)
    {
        m_rcClipBox.left   = nMinX;
    }
    if (nMinY >= 0)
    {
        m_rcClipBox.bottom = nMinY;
    }
    if (nMaxX >= 0)
    {
        m_rcClipBox.right  = nMaxX;
    }
    if (nMaxY >= 0)
    {
        m_rcClipBox.top    = nMaxY;
    }
}


void GS_OGLMap::SetClipBox(RECT rcClipBox)
{

    m_rcClipBox = rcClipBox;
}


//==============================================================================================
// GS_OGLMap::GetClipBox():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Returns: A RECT structure containing the clip box coordinates.
//==============================================================================================

RECT GS_OGLMap::GetClipBox()
{

    return m_rcClipBox;
}


////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_OGLMap::GetClipBoxLeft():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Returns: The left coordinate of the clip box.
//==============================================================================================

long GS_OGLMap::GetClipBoxLeft()
{

    return m_rcClipBox.left;
}


//==============================================================================================
// GS_OGLMap::GetClipBoxRight():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Returns: The right coordinate of the clip box.
//==============================================================================================

long GS_OGLMap::GetClipBoxRight()
{

    return m_rcClipBox.right;
}


//==============================================================================================
// GS_OGLMap::GetClipBoxBottom():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Returns: The bottom coordinate of the clip box.
//==============================================================================================

long GS_OGLMap::GetClipBoxBottom()
{

    return m_rcClipBox.bottom;
}



//==============================================================================================
// GS_OGLMap::GetClipBoxTop():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Returns: The top coordinate of the clip box.
//==============================================================================================

long GS_OGLMap::GetClipBoxTop()
{

    return m_rcClipBox.top;
}


////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_OGLMap::GetClipBoxWidth():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Returns: The width of the clip box in pixels.
//==============================================================================================

int GS_OGLMap::GetClipBoxWidth()
{

    return (m_rcClipBox.right - m_rcClipBox.left);
}


//==============================================================================================
// GS_OGLMap::GetClipBoxHeight():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Returns: The height of the clip box in pixels.
//==============================================================================================

int GS_OGLMap::GetClipBoxHeight()
{

    return (m_rcClipBox.top - m_rcClipBox.bottom);
}


////////////////////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////////////////////
// Map Methods. ////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_OGLMap::LoadMap():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Details: Maps can have a total of 61 different elements. Characters '0' - '9' represents the
//          elements 0 - 9, 'A' - 'Z' represents the elements 10 - 35 and 'a' - 'z' represents
//          the elements 36 - 61.
// ---------------------------------------------------------------------------------------------
// Returns: TRUE if successful, FALSE if failed.
//==============================================================================================

BOOL GS_OGLMap::LoadMap(const char* pszPathname, int nNumRows, int nNumCols)
{

    // Are the filename and the row and column values valid?
    if ((NULL == pszPathname) || (nNumRows <= 0) || (nNumCols <= 0))
    {
        return FALSE;
    }

    GS_File gsMapFile;

    // Were we able to open the file for reading?
    if (FALSE == gsMapFile.Open(pszPathname, FILE_READ))
    {
        return FALSE;
    }

    // Make sure map is not too large.
    if (nNumRows > MAX_MAP_ROWS)
    {
        nNumRows =  MAX_MAP_ROWS;
    }
    if (nNumCols > MAX_MAP_COLS)
    {
        nNumCols =  MAX_MAP_COLS;
    }

    // Read all the values from the file into the map array.
    for (int nRows = nNumRows - 1; nRows >= 0; nRows--)
    {
        for (int nCols = 0; nCols < nNumCols; nCols++)
        {
            // Read the value from the file.
            int nTemp = gsMapFile.GetByte();
            // Have we encountered an end of line character?
            while ((nTemp == 0x0A) || (nTemp == 0x0D))
            {
                // Eat all end of line characters.
                nTemp = gsMapFile.GetByte();
            }
            // Determine the map equivalent of the file value.
            if ((nTemp >= 48) && (nTemp <= 57))
            {
                // 0 - 9 is 0 - 9.
                m_nMap[nRows][nCols] = nTemp - 48;
            }
            else if ((nTemp >= 65) && (nTemp <= 90))
            {
                // A - Z is 10 to 35.
                m_nMap[nRows][nCols] = nTemp - 55;
            }
            else if ((nTemp >= 97) && (nTemp <= 122))
            {
                // a - z is 36 to 61.
                m_nMap[nRows][nCols] = nTemp - 61;
            }
            else
            {
                // Unrecognised value.
                m_nMap[nRows][nCols] = -1;
            }
        }
    }

    // Save map dimensions.
    m_nNumCols = nNumCols;
    m_nNumRows = nNumRows;

    // Close the map file.
    gsMapFile.Close();

    return TRUE;
}


////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_OGLMap::SaveMap():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Returns: TRUE if successful, FALSE if failed.
//==============================================================================================

BOOL GS_OGLMap::SaveMap(const char* pszPathname)
{

    // Do we have a valid filename?
    if (NULL == pszPathname)
    {
        return FALSE;
    }

    GS_File gsMapFile;

    // Were we able to open the file for writing?
    if (FALSE == gsMapFile.Open(pszPathname, FILE_WRITE))
    {
        return FALSE;
    }

    // Write all the values from the map array to the file.
    for (int nRows = m_nNumRows - 1; nRows >= 0; nRows--)
    {
        for (int nCols = 0; nCols < m_nNumCols; nCols++)
        {
            // Write numbers 0 - 9 as characters '0' - '9'.
            if ((m_nMap[nRows][nCols] >= 0) && (m_nMap[nRows][nCols] <= 9))
            {
                gsMapFile.PutByte((unsigned short)(m_nMap[nRows][nCols] + 48));
            }
            // Write numbers 10 - 35 as characters 'A' - 'Z'.
            else if ((m_nMap[nRows][nCols] >= 10) && (m_nMap[nRows][nCols] <= 35))
            {
                gsMapFile.PutByte((unsigned short)(m_nMap[nRows][nCols] + 55));
            }
            // Write numbers 36 - 61 as characters 'a' - 'z'.
            else if ((m_nMap[nRows][nCols] >= 36) && (m_nMap[nRows][nCols] <= 61))
            {
                gsMapFile.PutByte((unsigned short)(m_nMap[nRows][nCols] + 61));
            }
            else
            {
                gsMapFile.PutByte(' ');
            }
            // Are we at the end of the line?
            if (nCols == m_nNumCols-1)
            {
                // Write end of line characters.
                gsMapFile.PutByte(0x0D);
                gsMapFile.PutByte(0x0A);
            }
        }
    }

    // Close the map file.
    gsMapFile.Close();

    return TRUE;
}


////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_OGLMap::SetMapX():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Returns: Nothing.
//==============================================================================================

void GS_OGLMap::SetMapX(int nDestX)
{

    m_nMapCoordX = nDestX;
}


//==============================================================================================
// GS_OGLMap::SetMapY():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Returns: Nothing.
//==============================================================================================

void GS_OGLMap::SetMapY(int nDestY)
{

    m_nMapCoordY = nDestY;
}


//==============================================================================================
// GS_OGLMap::GetMapX():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Returns: The current horizontal map coordinate.
//==============================================================================================

int GS_OGLMap::GetMapX()
{

    return m_nMapCoordX;
}


//==============================================================================================
// GS_OGLMap::GetMapY():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Returns: The current vertical map coordinate.
//==============================================================================================

int GS_OGLMap::GetMapY()
{

    return m_nMapCoordY;
}


////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_OGLMap::GetMapWidth():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Returns: The current width of the map in pixels.
//==============================================================================================

int GS_OGLMap::GetMapWidth()
{

    return(m_nNumCols *  m_gsTileSprites.GetFrameWidth());
}


//==============================================================================================
// GS_OGLMap::GetMapHeight():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Returns: The current height of the map in pixels.
//==============================================================================================

int GS_OGLMap::GetMapHeight()
{

    return(m_nNumRows * m_gsTileSprites.GetFrameHeight());
}


////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_OGLMap::SetLimitX():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Returns: Nothing.
//==============================================================================================

void GS_OGLMap::SetLimitX(BOOL bEnableLimit)
{

    m_bLimitX = bEnableLimit;

    if (m_bLimitX)
    {
        m_bWrapX = FALSE;
    }
}


//==============================================================================================
// GS_OGLMap::SetLimitY():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Returns: Nothing.
//==============================================================================================

void GS_OGLMap::SetLimitY(BOOL bEnableLimit)
{

    m_bLimitY = bEnableLimit;

    if (m_bLimitY)
    {
        m_bWrapY = FALSE;
    }
}


////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_OGLMap::SetWrapX():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Returns: Nothing.
//==============================================================================================

void GS_OGLMap::SetWrapX(BOOL bEnableWrap)
{

    m_bWrapX = bEnableWrap;

    if (m_bWrapX)
    {
        m_bLimitX = FALSE;
    }
}


//==============================================================================================
// GS_OGLMap::SetWrapY():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Returns: Nothing.
//==============================================================================================

void GS_OGLMap::SetWrapY(BOOL bEnableWrap)
{

    m_bWrapY = bEnableWrap;

    if (m_bWrapY)
    {
        m_bLimitY = FALSE;
    }
}


////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_OGLMap::ScrollX():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Returns: Nothing.
//==============================================================================================

void GS_OGLMap::ScrollX(int nDistance)
{

    // Is scroll limited in the horizontal aspect?
    if (m_bLimitX)
    {
        // Will the scroll distance leave a space to the right of the clip box?
        if ((m_nMapCoordX + nDistance) < (this->GetClipBoxWidth() - this->GetMapWidth()))
        {
            // Adjust the scroll distance to prevent open spaces.
            nDistance = this->GetClipBoxWidth() - this->GetMapWidth() -  m_nMapCoordX;
        }
        // Will the scroll distance leave a space to the left of the clip box?
        else if ((m_nMapCoordX + nDistance) > 0)
        {
            // Adjust the scroll distance to prevent open spaces.
            nDistance = 0 - m_nMapCoordX;
        }
    }
    // Should scroll be wrapped in the horizontal aspect?
    else if (m_bWrapX)
    {
        // Has the map scrolled it's entire width to the left?
        if ((m_nMapCoordX + nDistance) <= (-this->GetMapWidth()))
        {
            // Adjust the scroll distance by the distance over-scrolled.
            nDistance = (m_nMapCoordX + nDistance) + this->GetMapWidth();
            // Set the map coordinate to the far right.
            m_nMapCoordX = this->GetMapWidth();
        }
        // Has the map scrolled it's entire width to the right?
        else if ((m_nMapCoordX + nDistance) >= (this->GetMapWidth()))
        {
            // Adjust the scroll distance by the distance over-scrolled.
            nDistance = (m_nMapCoordX + nDistance) - this->GetMapWidth();
            // Set the map coordinate to the far left.
            m_nMapCoordX = -this->GetMapWidth();
        }
    }

    // Adjust the horizontal coordinate.
    m_nMapCoordX += nDistance;
}


//==============================================================================================
// GS_OGLMap::ScrollY():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Returns: Nothing.
//==============================================================================================

void GS_OGLMap::ScrollY(int nDistance)
{

    // Is scroll limited in the vertical aspect?
    if (m_bLimitY)
    {
        // Will the scroll distance leave a space at the top of the clip box?
        if ((m_nMapCoordY + nDistance) < (this->GetClipBoxHeight() - this->GetMapHeight()))
        {
            // Adjust the scroll distance to prevent open spaces.
            nDistance = this->GetClipBoxHeight() - this->GetMapHeight() -  m_nMapCoordY;
        }
        // Will the scroll distance leave a space at the bottom of the clip box?
        else if ((m_nMapCoordY + nDistance) > 0)
        {
            // Adjust the scroll distance to prevent open spaces.
            nDistance = 0 - m_nMapCoordY;
        }
    }
    // Should scroll be wrapped in the vertical aspect?
    else if (m_bWrapY)
    {
        // Has the map scrolled it's entire height to the bottom?
        if ((m_nMapCoordY + nDistance) <= (-this->GetMapHeight()))
        {
            // Adjust the scroll distance by the distance over-scrolled.
            nDistance = (m_nMapCoordY + nDistance) + this->GetMapHeight();
            // Set the map coordinate to the far top.
            m_nMapCoordY = this->GetMapHeight();
        }
        // Has the map scrolled it's entire height to the top?
        else if ((m_nMapCoordY + nDistance) >= (this->GetMapHeight()))
        {
            // Adjust the scroll distance by the distance over-scrolled.
            nDistance = (m_nMapCoordY + nDistance) - this->GetMapHeight();
            // Set the map coordinate to the far bottom.
            m_nMapCoordY = -this->GetMapHeight();
        }
    }

    // Adjust the vertical coordinate.
    m_nMapCoordY += nDistance;
}


////////////////////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////////////////////
// Tile Methods. ///////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_OGLMap::SetTileID():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Returns: Nothing.
//==============================================================================================

void GS_OGLMap::SetTileID(int nRow, int nCol, int nNewTileID)
{

    if ((nRow >= 0) && (nRow < m_nNumRows) && (nCol >= 0) && (nCol < m_nNumCols))
    {
        m_nMap[nRow][nCol] = nNewTileID;
    }
}


//==============================================================================================
// GS_OGLMap::GetTileID():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Returns: An integer value indicating the selected tile ID or -1.
//==============================================================================================

int GS_OGLMap::GetTileID(int nRow, int nCol)
{

    if ((nRow >= 0) && (nRow < m_nNumRows) && (nCol >= 0) && (nCol < m_nNumCols))
    {
        return m_nMap[nRow][nCol];
    }

    return -1;
}


//==============================================================================================
// GS_OGLMap::ReplaceTileID():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Returns: Nothing.
//==============================================================================================

void GS_OGLMap::ReplaceTileID(int nOldTileID, int nNewTileID)
{

    for (int nRows = 0; nRows < m_nNumRows; nRows++)
    {
        for (int nCols = 0; nCols < m_nNumCols; nCols++)
        {
            if (m_nMap[nRows][nCols] == nOldTileID)
            {
                m_nMap[nRows][nCols] = nNewTileID;
            }
        }
    }
}


////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_OGLMap::SetClearTileID():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Returns: Nothing.
//==============================================================================================

void GS_OGLMap::SetClearTileID(int nTileID)
{

    m_nClearTileID = nTileID;
}


////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_OGLMap::GetTileWidth():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Returns: The width of each individual map tile in pixels.
//==============================================================================================

GLfloat GS_OGLMap::GetTileWidth()
{

    return m_nTileWidth;
}


//==============================================================================================
// GS_OGLMap::GetTileHeight():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Returns: The height of each individual map tile in pixels.
//==============================================================================================

GLfloat GS_OGLMap::GetTileHeight()
{

    return m_nTileHeight;
}


////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_OGLMap::SetTotalTiles():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Returns: Nothing.
//==============================================================================================

void GS_OGLMap::SetTotalTiles(int nTotalTiles)
{

    m_gsTileSprites.SetTotalFrames(nTotalTiles);
}


//==============================================================================================
// GS_OGLMap::GetTotalTiles():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Returns: The total number of tiles used by the map.
//==============================================================================================

int GS_OGLMap::GetTotalTiles()
{

    return m_gsTileSprites.GetTotalFrames();
}


////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_OGLMap::SetTilesPerLine():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Returns: Nothing.
//==============================================================================================

void GS_OGLMap::SetTilesPerLine(int nTilesPerLine)
{

    m_gsTileSprites.SetFramesPerLine(nTilesPerLine);
}


//==============================================================================================
// GS_OGLMap::GetTilesPerLine():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Returns: The number of tiles per line of the tile image.
//==============================================================================================

int GS_OGLMap::GetTilesPerLine()
{

    return m_gsTileSprites.GetFramesPerLine();
}


////////////////////////////////////////////////////////////////////////////////////////////////
// Conversion Methods. /////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_OGLMap::MapToID():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Returns: The ID of the tile located at the specified map coordinate, or -1.
//==============================================================================================

int GS_OGLMap::MapToID(int nCoordX, int nCoordY)
{

    // Determine the column and row of the map at the specified x and y coordinates.
    int nCol = (nCoordX - (nCoordX%m_nTileWidth))  /  m_nTileWidth;
    int nRow = (nCoordY - (nCoordY%m_nTileHeight)) / m_nTileHeight;

    // Do we have a valid map row and column?
    if ((nRow < 0) || (nRow >= m_nNumRows) || (nCol < 0) || (nCol >= m_nNumCols))
    {
        return -1;
    }

    // Return the tile ID at the specified coordinates.
    return(m_nMap[nRow][nCol]);
}


//==============================================================================================
// GS_OGLMap::MapToCol():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Returns: The column of the tile located at the specified map coordinate, or -1.
//==============================================================================================

int GS_OGLMap::MapToCol(int nCoordX)
{

    // Determine the column of the map at the specified x-coordinate.
    int nCol = (nCoordX - (nCoordX%m_nTileWidth))  /  m_nTileWidth;

    // Do we have a valid map column?
    if ((nCol < 0) || (nCol >= m_nNumCols))
    {
        return -1;
    }

    return nCol;
}


//==============================================================================================
// GS_OGLMap::MapToRow():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Returns: The row of the tile located at the specified map coordinate, or -1.
//==============================================================================================

int GS_OGLMap::MapToRow(int nCoordY)
{

    // Determine the row of the map at the specified y-coordinate.
    int nRow = (nCoordY - (nCoordY%m_nTileHeight)) / m_nTileHeight;

    // Do we have a valid map row and column?
    if ((nRow < 0) || (nRow >= m_nNumRows))
    {
        return -1;
    }

    return nRow;
}


////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_OGLMap::ScreenToID():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Returns: The ID of the tile located at the specified screen coordinate, or -1.
//==============================================================================================

int GS_OGLMap::ScreenToID(int nCoordX, int nCoordY)
{

    // Were we able to convert the screen- to a map coordinate.
    if (!this->ScreenToMap(&nCoordX, &nCoordY))
    {
        return -1;
    }

    // Return the ID of the tile at the specified coordinate.
    return(this->MapToID(nCoordX, nCoordY));
}


//==============================================================================================
// GS_OGLMap::ScreenToCol():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Returns: The column of the tile located at the specified screen coordinate, or -1.
//==============================================================================================

int GS_OGLMap::ScreenToCol(int nCoordX)
{

    // Is the x-coordinate not within the clip box?
    if ((nCoordX < m_rcClipBox.left) || (nCoordX >= m_rcClipBox.right))
    {
        return -1;
    }

    // Convert the x-coordinate to a map coordinate.
    nCoordX -= (m_nMapCoordX + m_rcClipBox.left);

    // Is the x-coordinate not within the map area?
    if ((nCoordX < 0) || (nCoordX >= (m_nNumCols * m_nTileWidth)))
    {
        // Is the map wrapped horizontally?
        if (m_bWrapX)
        {
            // Has the map been scrolled to the left?
            if (m_nMapCoordX <= 0)
            {
                // Adjust the x-coordinate to the left.
                nCoordX -= (m_nNumCols * m_nTileWidth);
            }
            else
            {
                // Adjust the x-coordinate to the right.
                nCoordX += (m_nNumCols * m_nTileWidth);
            }
            // Is the x-coordinate not within the map area?
            if ((nCoordX < 0) || (nCoordX >= (m_nNumCols * m_nTileWidth)))
            {
                // Conversion failed.
                return -1;
            }
        }
        else
        {
            // Conversion failed.
            return -1;
        }
    }

    // Return the column of the map at the specified x-coordinate.
    return(this->MapToCol(nCoordX));
}


//==============================================================================================
// GS_OGLMap::ScreenToRow():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Returns: The row of the tile located at the specified screen coordinate, or -1.
//==============================================================================================

int GS_OGLMap::ScreenToRow(int nCoordY)
{

    // Is the y-coordinate not within the clip box?
    if ((nCoordY < m_rcClipBox.bottom) || (nCoordY >= m_rcClipBox.top))
    {
        return -1;
    }

    // Convert the y-coordinate to a map coordinate.
    nCoordY -= (m_nMapCoordY + m_rcClipBox.bottom);

    // Is the y-coordinate not within the map area?
    if ((nCoordY < 0) || (nCoordY >= (m_nNumRows * m_nTileHeight)))
    {
        // Is the map wrapped vertically?
        if (m_bWrapY)
        {
            // Has the map been scrolled to the bottom?
            if (m_nMapCoordY <= 0)
            {
                // Adjust the y-coordinate to the bottom.
                nCoordY -= (m_nNumRows * m_nTileHeight);
            }
            else
            {
                // Adjust the y-coordinate to the top.
                nCoordY += (m_nNumRows * m_nTileHeight);
            }
            // Is the y-coordinate not within the map area?
            if ((nCoordY < 0) || (nCoordY >= (m_nNumRows * m_nTileHeight)))
            {
                // Conversion failed.
                return -1;
            }
        }
        else
        {
            // Conversion failed.
            return -1;
        }
    }

    // Return the column of the map at the specified y-coordinate.
    return(this->MapToRow(nCoordY));
}


////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_OGLMap::MapToScreen():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Returns: TRUE coordinate is within the clip box, FALSE if not.
//==============================================================================================

BOOL GS_OGLMap::MapToScreen(int* pnCoordX, int* pnCoordY)
{

    int nCoordX = *pnCoordX;
    int nCoordY = *pnCoordY;

    // Is the x-coordinate not within the map area?
    if ((nCoordX < 0) || (nCoordX >= (m_nNumCols * m_nTileWidth)))
    {
        // Conversion failed.
        return FALSE;
    }

    // Is the y-coordinate not within the map area?
    if ((nCoordY < 0) || (nCoordY >= (m_nNumRows * m_nTileHeight)))
    {
        // Conversion failed.
        return FALSE;
    }

    // Convert the x-coordinate from map to screen coordinates.
    nCoordX  += (m_nMapCoordX + m_rcClipBox.left);

    // Is the x-coordinate not within the clip box?
    if ((nCoordX < m_rcClipBox.left) || (nCoordX >= m_rcClipBox.right))
    {
        // Is the map wrapped horizontally?
        if (m_bWrapX)
        {
            // Has the map been scrolled to the left?
            if (m_nMapCoordX <= 0)
            {
                // Adjust the x-coordinate to the right.
                nCoordX  += (m_nNumCols * m_nTileWidth);
            }
            else
            {
                // Adjust the x-coordinate to the left.
                nCoordX  -= (m_nNumCols * m_nTileWidth);
            }
            // Is the x-coordinate not within the clip box?
            if ((nCoordX < m_rcClipBox.left) || (nCoordX >= m_rcClipBox.right))
            {
                // Conversion failed.
                return FALSE;
            }
        }
        else
        {
            // Conversion failed.
            return FALSE;
        }
    }

    // Convert the y-coordinate from map to screen coordinates.
    nCoordY +=  (m_nMapCoordY + m_rcClipBox.bottom);

    // Is the y-coordinate not within the clip box?
    if ((nCoordY < m_rcClipBox.bottom) || (nCoordY >= m_rcClipBox.top))
    {
        // Is the map wrapped vertically?
        if (m_bWrapY)
        {
            // Has the map been scrolled to the bottom?
            if (m_nMapCoordY <= 0)
            {
                // Adjust the y-coordinate to the top.
                nCoordY += (m_nNumRows * m_nTileHeight);
            }
            else
            {
                // Adjust the y-coordinate to the bottom.
                nCoordY -= (m_nNumRows * m_nTileHeight);
            }
            // Is the y-coordinate not within the clip box?
            if ((nCoordY < m_rcClipBox.bottom) || (nCoordY >= m_rcClipBox.top))
            {
                // Conversion failed.
                return FALSE;
            }
        }
        else
        {
            // Conversion failed.
            return FALSE;
        }
    }

    // Assign converted coordinate.
    *pnCoordX = nCoordX;
    *pnCoordY = nCoordY;

    // Conversion successful.
    return TRUE;
}


//==============================================================================================
// GS_OGLMap::MapToScreen():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Returns: TRUE source rectangle overlaps the clip box, FALSE if not.
//==============================================================================================

BOOL GS_OGLMap::MapToScreen(RECT* prcSource)
{

    RECT rcSource = *prcSource;

    // Are the x-coordinates of the source not within the map area?
    if ((rcSource.right <= 0) || (rcSource.left >= (m_nNumCols * m_nTileWidth)))
    {
        // Conversion failed.
        return FALSE;
    }

    // Are the y-coordinates of the source not within the map area?
    if ((rcSource.top <= 0) || (rcSource.bottom >= (m_nNumRows * m_nTileHeight)))
    {
        // Conversion failed.
        return FALSE;
    }

    int nSourceWidth  = rcSource.right - rcSource.left;
    int nSourceHeight = rcSource.top - rcSource.bottom;

    // Convert the x-coordinates of the map area to screen coordinates.
    rcSource.left  += (m_nMapCoordX + m_rcClipBox.left);
    rcSource.right = rcSource.left + nSourceWidth;

    // Are the x-coordinates of the area not within the clip box?
    if ((rcSource.left >= m_rcClipBox.right) || (rcSource.right <= m_rcClipBox.left))
    {
        // Is the map wrapped horizontally?
        if (m_bWrapX)
        {
            // Has the map been scrolled to the left?
            if (m_nMapCoordX <= 0)
            {
                // Adjust the area coordinates to the right.
                rcSource.left  += (m_nNumCols * m_nTileWidth);
                rcSource.right = rcSource.left + nSourceWidth;
            }
            else
            {
                // Adjust the area coordinates to the left.
                rcSource.left  -= (m_nNumCols * m_nTileWidth);
                rcSource.right = rcSource.left + nSourceWidth;
            }
            // Are the x-coordinates of the area not within the clip box?
            if ((rcSource.left>=m_rcClipBox.right) || (rcSource.right<=m_rcClipBox.left))
            {
                // Conversion failed.
                return FALSE;
            }
        }
        else
        {
            // Conversion failed.
            return FALSE;
        }
    }

    // Convert the y-coordinates of the map area to screen coordinates.
    rcSource.bottom +=  (m_nMapCoordY + m_rcClipBox.bottom);
    rcSource.top    = rcSource.bottom + nSourceHeight;

    // Are the y-coordinates of the area not within the clip box?
    if ((rcSource.bottom >= m_rcClipBox.top) || (rcSource.top <= m_rcClipBox.bottom))
    {
        // Is the map wrapped vertically?
        if (m_bWrapY)
        {
            // Has the map been scrolled to the bottom?
            if (m_nMapCoordY <= 0)
            {
                // Adjust the area coordinates to the top.
                rcSource.bottom += (m_nNumRows * m_nTileHeight);
                rcSource.top    = rcSource.bottom + nSourceHeight;
            }
            else
            {
                // Adjust the area coordinates to the bottom.
                rcSource.bottom -= (m_nNumRows * m_nTileHeight);
                rcSource.top    = rcSource.bottom + nSourceHeight;
            }
            // Are the y-coordinates of the area not within the clip box?
            if ((rcSource.bottom>=m_rcClipBox.top) || (rcSource.top<=m_rcClipBox.bottom))
            {
                // Conversion failed.
                return FALSE;
            }
        }
        else
        {
            // Conversion failed.
            return FALSE;
        }
    }

    // Assign converted rectangle.
    *prcSource = rcSource;

    // Conversion successful.
    return TRUE;
}


////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_OGLMap::ScreenToMap():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Returns: TRUE if the converted coordinate is within the map area, FALSE if not.
//==============================================================================================

BOOL GS_OGLMap::ScreenToMap(int* pnCoordX, int* pnCoordY)
{

    int nCoordX = *pnCoordX;
    int nCoordY = *pnCoordY;

    // Is the x-coordinate not within the clip box?
    if ((nCoordX < m_rcClipBox.left) || (nCoordX >= m_rcClipBox.right))
    {
        return FALSE;
    }

    // Is the y-coordinate not within the clip box?
    if ((nCoordY < m_rcClipBox.bottom) || (nCoordY >= m_rcClipBox.top))
    {
        return FALSE;
    }

    // Convert the x-coordinate to a map coordinate.
    nCoordX -= (m_nMapCoordX + m_rcClipBox.left);

    // Is the x-coordinate not within the map area?
    if ((nCoordX < 0) || (nCoordX >= (m_nNumCols * m_nTileWidth)))
    {
        // Is the map wrapped horizontally?
        if (m_bWrapX)
        {
            // Has the map been scrolled to the left?
            if (m_nMapCoordX <= 0)
            {
                // Adjust the x-coordinate to the left.
                nCoordX -= (m_nNumCols * m_nTileWidth);
            }
            else
            {
                // Adjust the x-coordinate to the right.
                nCoordX += (m_nNumCols * m_nTileWidth);
            }
            // Is the x-coordinate not within the map area?
            if ((nCoordX < 0) || (nCoordX >= (m_nNumCols * m_nTileWidth)))
            {
                // Conversion failed.
                return FALSE;
            }
        }
        else
        {
            // Conversion failed.
            return FALSE;
        }
    }

    // Convert the y-coordinate to a map coordinate.
    nCoordY -= (m_nMapCoordY + m_rcClipBox.bottom);

    // Is the y-coordinate not within the map area?
    if ((nCoordY < 0) || (nCoordY >= (m_nNumRows * m_nTileHeight)))
    {
        // Is the map wrapped vertically?
        if (m_bWrapY)
        {
            // Has the map been scrolled to the bottom?
            if (m_nMapCoordY <= 0)
            {
                // Adjust the y-coordinate to the bottom.
                nCoordY -= (m_nNumRows * m_nTileHeight);
            }
            else
            {
                // Adjust the y-coordinate to the top.
                nCoordY += (m_nNumRows * m_nTileHeight);
            }
            // Is the y-coordinate not within the map area?
            if ((nCoordY < 0) || (nCoordY >= (m_nNumRows * m_nTileHeight)))
            {
                // Conversion failed.
                return FALSE;
            }
        }
        else
        {
            // Conversion failed.
            return FALSE;
        }
    }

    // Assign converted coordinate.
    *pnCoordX = nCoordX;
    *pnCoordY = nCoordY;

    // Conversion successful.
    return TRUE;
}


//==============================================================================================
// GS_OGLMap::ScreenToMap():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Returns: TRUE source rectangle overlaps the map area, FALSE if not.
//==============================================================================================

BOOL GS_OGLMap::ScreenToMap(RECT* prcSource)
{

    RECT rcSource = *prcSource;

    // Are the x-coordinates of the source not within the clip box?
    if ((rcSource.right <= m_rcClipBox.left) || (rcSource.left >= m_rcClipBox.right))
    {
        return FALSE;
    }

    // Are the y-coordinates of the source not within the clip box?
    if ((rcSource.top <= m_rcClipBox.bottom) || (rcSource.bottom >= m_rcClipBox.top))
    {
        return FALSE;
    }

    int nSourceWidth  = rcSource.right - rcSource.left;
    int nSourceHeight = rcSource.top - rcSource.bottom;

    // Convert the source x-coordinates to map coordinates.
    rcSource.left -= (m_nMapCoordX + m_rcClipBox.left);
    rcSource.right = rcSource.left + nSourceWidth;

    // Are the x-coordinates of the source not within the map area?
    if ((rcSource.right <= 0) || (rcSource.left >= (m_nNumCols * m_nTileWidth)))
    {
        // Is the map wrapped horizontally?
        if (m_bWrapX)
        {
            // Has the map been scrolled to the left?
            if (m_nMapCoordX <= 0)
            {
                // Adjust the source coordinates to the left.
                rcSource.left  -= (m_nNumCols * m_nTileWidth);
                rcSource.right = rcSource.left + nSourceWidth;
            }
            else
            {
                // Adjust the source coordinates to the right.
                rcSource.left  += (m_nNumCols * m_nTileWidth);
                rcSource.right = rcSource.left + nSourceWidth;
            }
            // Are the x-coordinates of the source not within the map area?
            if ((rcSource.right <= 0) || (rcSource.left >= (m_nNumCols * m_nTileWidth)))
            {
                // Conversion failed.
                return FALSE;
            }
        }
        else
        {
            // Conversion failed.
            return FALSE;
        }
    }

    // Convert the source y-coordinates to map coordinates.
    rcSource.bottom -= (m_nMapCoordY + m_rcClipBox.bottom);
    rcSource.top     = rcSource.bottom + nSourceHeight;

    // Are the y-coordinates of the source not within the map area?
    if ((rcSource.top <= 0) || (rcSource.bottom >= (m_nNumRows * m_nTileHeight)))
    {
        // Is the map wrapped vertically?
        if (m_bWrapY)
        {
            // Has the map been scrolled to the bottom?
            if (m_nMapCoordY <= 0)
            {
                // Adjust the source coordinates to the bottom.
                rcSource.bottom -= (m_nNumRows * m_nTileHeight);
                rcSource.top = rcSource.bottom + nSourceHeight;
            }
            else
            {
                // Adjust the source coordinates to the top.
                rcSource.bottom += (m_nNumRows * m_nTileHeight);
                rcSource.top = rcSource.bottom + nSourceHeight;
            }
            // Are the y-coordinates of the source not within the map area?
            if ((rcSource.top <= 0) || (rcSource.bottom >= (m_nNumRows * m_nTileHeight)))
            {
                // Conversion failed.
                return FALSE;
            }
        }
        else
        {
            // Conversion failed.
            return FALSE;
        }
    }

    // Assign converted rectangle.
    *prcSource = rcSource;

    // Conversion successful.
    return TRUE;
}


////////////////////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////////////////////
// Collision Methods. //////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_OGLMap::IsOnTile():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Returns: TRUE if a certain point overlaps a certain kind of tile, FALSE if not.
//==============================================================================================

BOOL GS_OGLMap::IsOnTile(int nCoordX, int nCoordY, int nTileID)
{

    // Do we not have a valid tile ID?
    if ((nTileID < 0) || (nTileID >= m_gsTileSprites.GetTotalFrames()))
    {
        return FALSE;
    }

    // Were we unable to convert the screen to map coordinate?
    if (!this->ScreenToMap(&nCoordX, &nCoordY))
    {
        // Coordinate not on tile.
        return FALSE;
    }

    // Is the tile at the specified coordinate not the right one?
    if (this->MapToID(nCoordX, nCoordY) != nTileID)
    {
        // Coordinate not on tile.
        return FALSE;
    }

    // Coordinate on tile.
    return TRUE;
}


//==============================================================================================
// GS_OGLMap::IsOnTile():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Returns: TRUE if a certain source area overlaps a certain kind of tile, FALSE if not.
//==============================================================================================

BOOL GS_OGLMap::IsOnTile(RECT rcSource, int nTileID)
{

    // Do we not have a valid tile ID?
    if ((nTileID < 0) || (nTileID >= m_gsTileSprites.GetTotalFrames()))
    {
        return FALSE;
    }

    int nMaxCoordY = rcSource.top   + m_nTileHeight;
    int nMaxCoordX = rcSource.right + m_nTileWidth;

    int nTempX, nTempY;

    // Check every tile within the source rectangle's area.
    for (int nCoordY = rcSource.bottom; nCoordY < nMaxCoordY; nCoordY += m_nTileHeight)
    {
        for (int nCoordX = rcSource.left; nCoordX < nMaxCoordX; nCoordX += m_nTileWidth)
        {
            // Have we reached the maximum x-coordinate?
            if (nCoordX >= rcSource.right)
            {
                // Set x-coordinate to maximum.
                nTempX = rcSource.right-1;
            }
            else
            {
                nTempX = nCoordX;
            }
            // Have we reached the maximum y-coordinate?
            if (nCoordY >= rcSource.top)
            {
                // Set y-coordinate to maximum.
                nTempY = rcSource.top-1;
            }
            else
            {
                nTempY = nCoordY;
            }
            // Were we able to convert the screen coordinate to a map coordinate?
            if (this->ScreenToMap(&nTempX, &nTempY))
            {
                // Is the tile at the specified coordinate the right kind?
                if (this->MapToID(nTempX, nTempY) == nTileID)
                {
                    // Overlap occured.
                    return TRUE;
                }
            }
        }
    }

    // No overlap occured.
    return FALSE;
}


//==============================================================================================
// GS_OGLMap::IsInTile():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Returns: TRUE if a certain source area is within a certain kind of tile(s), FALSE if not.
//==============================================================================================

BOOL GS_OGLMap::IsInTile(RECT rcSource, int nTileID)
{

    // Do we not have a valid tile ID?
    if ((nTileID < 0) || (nTileID >= m_gsTileSprites.GetTotalFrames()))
    {
        return FALSE;
    }

    int nMaxCoordY = rcSource.top   + m_nTileHeight;
    int nMaxCoordX = rcSource.right + m_nTileWidth;

    int nTempX, nTempY;

    // Check every tile within the source rectangle's area.
    for (int nCoordY = rcSource.bottom; nCoordY < nMaxCoordY; nCoordY += m_nTileHeight)
    {
        for (int nCoordX = rcSource.left; nCoordX < nMaxCoordX; nCoordX += m_nTileWidth)
        {
            // Have we reached the maximum x-coordinate?
            if (nCoordX >= rcSource.right)
            {
                // Set x-coordinate to maximum.
                nTempX = rcSource.right-1;
            }
            else
            {
                nTempX = nCoordX;
            }
            // Are the x-coordinates not entirely within the clip box?
            if ((nTempX < m_rcClipBox.left) || (nTempX >= m_rcClipBox.right))
            {
                return FALSE;
            }
            // Have we reached the maximum y-coordinate?
            if (nCoordY >= rcSource.top)
            {
                // Set y-coordinate to maximum.
                nTempY = rcSource.top-1;
            }
            else
            {
                nTempY = nCoordY;
            }
            // Are the y-coordinates not entirely within the clip box?
            if ((nTempY < m_rcClipBox.bottom) || (nTempY >= m_rcClipBox.top))
            {
                return FALSE;
            }
            // Were we able to convert the screen coordinate to a map coordinate?
            if (this->ScreenToMap(&nTempX, &nTempY))
            {
                // Is the tile at the specified coordinate the wrong kind?
                if (this->MapToID(nTempX, nTempY) != nTileID)
                {
                    // Source not within tile(s).
                    return FALSE;
                }
            }
        }
    }

    // Source is within tile(s).
    return TRUE;
}


////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_OGLMap::IsOnTile():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Returns: TRUE if a certain point overlaps a certain tile, FALSE if not.
//==============================================================================================

BOOL GS_OGLMap::IsOnTile(int nCoordX, int nCoordY, int nRow, int nCol)
{

    // Do we not have a valid map row and column?
    if ((nRow < 0) || (nRow >= m_nNumRows) || (nCol < 0) || (nCol >= m_nNumCols))
    {
        return FALSE;
    }

    // Were we unable to convert the screen to map coordinate?
    if (!this->ScreenToMap(&nCoordX, &nCoordY))
    {
        // Coordinate not on tile.
        return FALSE;
    }

    // Is the x-coordinate not in the correct map column?
    if (nCol != (nCoordX-(nCoordX%m_nTileWidth)) / m_nTileWidth)
    {
        // Coordinate not on tile.
        return FALSE;
    }

    // Is the y-coordinate not in the correct map row?
    if (nRow != (nCoordY-(nCoordY%m_nTileHeight)) / m_nTileHeight)
    {
        // Coordinate not on tile.
        return FALSE;
    }

    // Coordinate on tile.
    return TRUE;
}


//==============================================================================================
// GS_OGLMap::IsOnTile():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Returns: TRUE if a certain source area overlaps a certain tile area, FALSE if not.
//==============================================================================================

BOOL GS_OGLMap::IsOnTile(RECT rcSource, int nRow, int nCol)
{

    // Do we not have a valid map row and column?
    if ((nRow < 0) || (nRow >= m_nNumRows) || (nCol < 0) || (nCol >= m_nNumCols))
    {
        return FALSE;
    }

    // Are the x-coordinates of the source not within the clip box?
    if ((rcSource.right <= m_rcClipBox.left) || (rcSource.left >= m_rcClipBox.right))
    {
        return FALSE;
    }

    // Are the y-coordinates of the source not within the clip box?
    if ((rcSource.top <= m_rcClipBox.bottom) || (rcSource.bottom >= m_rcClipBox.top))
    {
        return FALSE;
    }

    RECT rcTile;

    // Determine the map coordinates of the specified tile.
    rcTile.left   = nCol * m_nTileWidth;
    rcTile.right  = rcTile.left + m_nTileWidth;
    rcTile.bottom = nRow * m_nTileHeight;
    rcTile.top    = rcTile.bottom + m_nTileHeight;

    // Were we unable to convert the map to screen coordinates?
    if (!this->MapToScreen(&rcTile))
    {
        // No overlap possible.
        return FALSE;
    }

    // +--------+
    // |        |
    // |  TILE  |
    // |        |
    // +--------+
    // +--------+
    // |        |
    // | SOURCE |
    // |        |
    // +--------+

    if (rcSource.top <= rcTile.bottom)
    {
        // No overlap possible.
        return FALSE;
    }

    // +--------+
    // |        |
    // | SOURCE |
    // |        |
    // +--------+
    // +--------+
    // |        |
    // |  TILE  |
    // |        |
    // +--------+

    if (rcSource.bottom >= rcTile.top)
    {
        // No overlap possible.
        return FALSE;
    }

    // +--------+ +--------+
    // |        | |        |
    // | SOURCE | |  TILE  |
    // |        | |        |
    // +--------+ +--------+

    if (rcSource.right <= rcTile.left)
    {
        // No overlap possible.
        return FALSE;
    }

    // +--------+ +--------+
    // |        | |        |
    // |  TILE  | | SOURCE |
    // |        | |        |
    // +--------+ +--------+

    if (rcSource.left >= rcTile.right)
    {
        // No overlap possible.
        return FALSE;
    }

    // Overlap has occured.
    return TRUE;
}


//==============================================================================================
// GS_OGLMap::IsInTile():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Returns: TRUE if a certain source area is within a certain tile, FALSE if not.
//==============================================================================================

BOOL GS_OGLMap::IsInTile(RECT rcSource, int nRow, int nCol)
{

    // Do we not have a valid map row and column?
    if ((nRow < 0) || (nRow >= m_nNumRows) || (nCol < 0) || (nCol >= m_nNumCols))
    {
        return FALSE;
    }

    // Are the x-coordinates of the source not entirely within the clip box?
    if ((rcSource.left < m_rcClipBox.left) || (rcSource.right > m_rcClipBox.right))
    {
        return FALSE;
    }

    // Are the y-coordinates of the source not entirely within the clip box?
    if ((rcSource.bottom < m_rcClipBox.bottom) || (rcSource.top > m_rcClipBox.top))
    {
        return FALSE;
    }

    // Is the source wider than the tile?
    if (m_nTileWidth < (rcSource.right - rcSource.left))
    {
        return FALSE;
    }

    // Is the source higher than the tile?
    if (m_nTileHeight < (rcSource.top - rcSource.bottom))
    {
        return FALSE;
    }

    RECT rcTile;

    // Determine the map coordinates of the specified tile.
    rcTile.left   = nCol * m_nTileWidth;
    rcTile.right  = rcTile.left + m_nTileWidth;
    rcTile.bottom = nRow * m_nTileHeight;
    rcTile.top    = rcTile.bottom + m_nTileHeight;

    // Were we unable to convert the map to screen coordinates?
    if (!this->MapToScreen(&rcTile))
    {
        // Source not within tile.
        return FALSE;
    }

    //   +------+
    //   |      |
    // +-|      |-+
    // | |      | |
    // | |      | |
    // | |      | |
    // +-|      |-+
    //   |      |
    //   +------+

    if ((rcSource.top > rcTile.top)  || (rcSource.bottom < rcTile.bottom))
    {
        // Source not within tile.
        return FALSE;
    }

    //   +------+
    //   |      |
    // +----------+
    // |          |
    // |          |
    // |          |
    // +----------+
    //   |      |
    //   +------+

    if ((rcSource.right > rcTile.right) || (rcSource.left < rcTile.left))
    {
        // Source not within tile.
        return FALSE;
    }

    // Source within tile.
    return TRUE;
}


////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_OGLMap::IsOnArea():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Returns: TRUE if a certain point overlaps a certain map area, FALSE if not.
//==============================================================================================

BOOL GS_OGLMap::IsOnArea(int nCoordX, int nCoordY, RECT rcMapArea)
{

    // Were we unable to convert the screen to map coordinate?
    if (!this->ScreenToMap(&nCoordX, &nCoordY))
    {
        // Coordinate not in area.
        return FALSE;
    }

    // (x, y)
    // +--------+
    // |        |
    // |  TILE  |
    // |        |
    // +--------+
    // (x, y)

    if ((nCoordY < rcMapArea.bottom) || (nCoordY >= rcMapArea.top))
    {
        // Coordinate not in area.
        return FALSE;
    }

    //        +--------+
    //        |        |
    // (x, y) |  TILE  | (x, y)
    //        |        |
    //        +--------+

    if ((nCoordX < rcMapArea.left) || (nCoordX >= rcMapArea.right))
    {
        // Coordinate not in area.
        return FALSE;
    }

    // Coordinate in area.
    return TRUE;
}


//==============================================================================================
// GS_OGLMap::IsOnArea():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Returns: TRUE if a certain source area overlaps a certain map area, FALSE if not.
//==============================================================================================

BOOL GS_OGLMap::IsOnArea(RECT rcSource, RECT rcMapArea)
{

    // Are the x-coordinates of the source not within the clip box?
    if ((rcSource.right <= m_rcClipBox.left) || (rcSource.left >= m_rcClipBox.right))
    {
        return FALSE;
    }

    // Are the y-coordinates of the source not within the clip box?
    if ((rcSource.top <= m_rcClipBox.bottom) || (rcSource.bottom >= m_rcClipBox.top))
    {
        return FALSE;
    }

    // Were we unable to convert the map to screen coordinates?
    if (!this->MapToScreen(&rcMapArea))
    {
        // No overlap possible.
        return FALSE;
    }

    // +--------+
    // |        |
    // |  TILE  |
    // |        |
    // +--------+
    // +--------+
    // |        |
    // | SOURCE |
    // |        |
    // +--------+

    if (rcSource.top <= rcMapArea.bottom)
    {
        // No overlap possible.
        return FALSE;
    }

    // +--------+
    // |        |
    // | SOURCE |
    // |        |
    // +--------+
    // +--------+
    // |        |
    // |  TILE  |
    // |        |
    // +--------+

    if (rcSource.bottom >= rcMapArea.top)
    {
        // No overlap possible.
        return FALSE;
    }

    // +--------+ +--------+
    // |        | |        |
    // | SOURCE | |  TILE  |
    // |        | |        |
    // +--------+ +--------+

    if (rcSource.right <= rcMapArea.left)
    {
        // No overlap possible.
        return FALSE;
    }

    // +--------+ +--------+
    // |        | |        |
    // |  TILE  | | SOURCE |
    // |        | |        |
    // +--------+ +--------+

    if (rcSource.left >= rcMapArea.right)
    {
        // No overlap possible.
        return FALSE;
    }

    // Overlap has occured.
    return TRUE;
}


//==============================================================================================
// GS_OGLMap::IsInArea():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Returns: TRUE if a certain source area is within a certain map area, FALSE if not.
//==============================================================================================

BOOL GS_OGLMap::IsInArea(RECT rcSource, RECT rcMapArea)
{

    // Are the x-coordinates of the source not entirely within the clip box?
    if ((rcSource.left < m_rcClipBox.left) || (rcSource.right > m_rcClipBox.right))
    {
        return FALSE;
    }

    // Are the y-coordinates of the source not entirely within the clip box?
    if ((rcSource.bottom < m_rcClipBox.bottom) || (rcSource.top > m_rcClipBox.top))
    {
        return FALSE;
    }

    // Is the source wider than the map area?
    if ((rcMapArea.right - rcMapArea.left) < (rcSource.right - rcSource.left))
    {
        return FALSE;
    }

    // Is the source higher than the map area?
    if ((rcMapArea.top - rcMapArea.bottom) < (rcSource.top - rcSource.bottom))
    {
        return FALSE;
    }

    // Were we unable to convert the map to screen coordinates?
    if (!this->MapToScreen(&rcMapArea))
    {
        // No overlap possible.
        return FALSE;
    }

    //   +------+
    //   |      |
    // +-|      |-+
    // | |      | |
    // | |      | |
    // | |      | |
    // +-|      |-+
    //   |      |
    //   +------+

    if ((rcSource.top > rcMapArea.top)  || (rcSource.bottom < rcMapArea.bottom))
    {
        // Source not within area vertically.
        return FALSE;
    }

    //   +------+
    //   |      |
    // +----------+
    // |          |
    // |          |
    // |          |
    // +----------+
    //   |      |
    //   +------+

    if ((rcSource.right > rcMapArea.right) || (rcSource.left < rcMapArea.left))
    {
        // Source not within area horizontally.
        return FALSE;
    }

    // Source within area.
    return TRUE;
}


////////////////////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////////////////////
// Color Methods. //////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_OGLMap::GetModulateColor():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Returns: Nothing.
//==============================================================================================

void GS_OGLMap::GetModulateColor(GS_OGLColor* pgsColor)
{

    m_gsTileSprites.GetModulateColor(pgsColor);
}


void GS_OGLMap::GetModulateColor(float* pRed, float* pGreen, float* pBlue, float* pAlpha)
{

    m_gsTileSprites.GetModulateColor(pRed, pGreen, pBlue, pAlpha);
}


//==============================================================================================
// GS_OGLMap::SetModulateColor():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Returns: Nothing.
//==============================================================================================

void GS_OGLMap::SetModulateColor(GS_OGLColor gsColor)
{

    m_gsTileSprites.SetModulateColor(gsColor);
}


void GS_OGLMap::SetModulateColor(float fRed, float fGreen, float fBlue, float fAlpha)
{

    m_gsTileSprites.SetModulateColor(fRed, fGreen, fBlue, fAlpha);
}


////////////////////////////////////////////////////////////////////////////////////////////////
