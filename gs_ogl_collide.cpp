/*============================================================================================+
 | Game System (GS) Library                                                                   |
 |--------------------------------------------------------------------------------------------|
 | FILES: gs_ogl_collide.cpp, gs_ogl_collide.h                                                |
 |--------------------------------------------------------------------------------------------|
 | CLASS: GS_OGLCollide                                                                       |
 |--------------------------------------------------------------------------------------------|
 | ABOUT: ...                                                                                 |
 |--------------------------------------------------------------------------------------------|
 |                                                                                    08/2003 |
 +============================================================================================*/


//==============================================================================================
// Include Game System (GS) header files.
// ---------------------------------------------------------------------------------------------
#include "gs_ogl_collide.h"
//==============================================================================================


////////////////////////////////////////////////////////////////////////////////////////////////
// Constructor/Destructor Methods. /////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_OGLCollide::GS_OGLCollide():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Returns: Nothing.
//==============================================================================================

GS_OGLCollide::GS_OGLCollide()
{

    // ...
}


////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_OGLCollide::~GS_OGLCollide():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Returns: Nothing.
//==============================================================================================

GS_OGLCollide::~GS_OGLCollide()
{

    // ...
}


////////////////////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////////////////////
// Adjustment Methods. /////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_OGLCollide::SetRectPercentX():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Returns: Nothing.
//==============================================================================================

void GS_OGLCollide::SetRectPercentX(RECT* prcSource, int nPercentX)
{

    // Is the percentage within the allowed range (0-100)?
    if ((nPercentX <= 0) || (nPercentX > 100))
    {
        return;
    }

    // Determine the new width as a percentage of the original width.
    int nNewWidth  = (int)((prcSource->right - prcSource->left) * ((float)nPercentX/100));

    // Determine the coordinates of the new rectangle centered in the old.
    prcSource->left += (((prcSource->right - prcSource->left) - nNewWidth)  / 2);
    prcSource->right = prcSource->left + nNewWidth;
}


////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_OGLCollide::SetRectPercentY():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Returns: Nothing.
//==============================================================================================

void GS_OGLCollide::SetRectPercentY(RECT* prcSource, int nPercentY)
{

    // Is the percentage within the allowed range (0-100)?
    if ((nPercentY <= 0) || (nPercentY > 100))
    {
        return;
    }

    // Determine the new height as a percentage of the original height.
    int nNewHeight = (int)((prcSource->top - prcSource->bottom) * ((float)nPercentY/100));

    // Determine the coordinates of the new rectangle centered in the old.
    prcSource->bottom += (((prcSource->top - prcSource->bottom) - nNewHeight) / 2);
    prcSource->top    = prcSource->bottom + nNewHeight;
}


////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_OGLCollide::SetRectPercentXY():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Returns: Nothing.
//==============================================================================================

void GS_OGLCollide::SetRectPercentXY(RECT* prcSource, int nPercent)
{

    // Is the percentage within the allowed range (0-100)?
    if ((nPercent <= 0) || (nPercent > 100))
    {
        return;
    }

    // Determine the new width and height as a percentage of the original width and height.
    int nNewWidth  = (int)((prcSource->right - prcSource->left) * ((float)nPercent/100));
    int nNewHeight = (int)((prcSource->top - prcSource->bottom) * ((float)nPercent/100));

    // Determine the coordinates of the new rectangle centered in the old.
    prcSource->left += (((prcSource->right - prcSource->left) - nNewWidth)  / 2);
    prcSource->right = prcSource->left + nNewWidth;
    prcSource->bottom += (((prcSource->top - prcSource->bottom) - nNewHeight) / 2);
    prcSource->top    = prcSource->bottom + nNewHeight;
}


////////////////////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////////////////////
// Coordinate Methods. /////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_OGLCollide::IsCoordInRect():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Returns: TRUE if the specified coordinates are within the specified rectangle, FALSE if not.
//==============================================================================================

BOOL GS_OGLCollide::IsCoordInRect(int nCoordX, int nCoordY, RECT rcDest, int nPercent)
{

    // Should we adjust the rectangle?
    if (nPercent > 0)
    {
        // Adjust the size of the rectangle by a percentage.
        this->SetRectPercentXY(&rcDest, nPercent);
    }

    // (x, y)
    // +--------+
    // |        |
    // | SOURCE |
    // |        |
    // +--------+
    // (x, y)

    if ((nCoordY < rcDest.bottom) || (nCoordY >= rcDest.top))
    {
        // Coords not within dest.
        return FALSE;
    }

    //        +--------+
    //        |        |
    // (x, y) | SOURCE | (x, y)
    //        |        |
    //        +--------+

    if ((nCoordX < rcDest.left) || (nCoordX >= rcDest.right))
    {
        // Coords not within dest.
        return FALSE;
    }

    // Coords within dest.
    return TRUE;
}


////////////////////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////////////////////
// Rectangle Methods. //////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_OGLCollide::IsRectInRect():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Returns: TRUE if the source rectangle is within the destination rectangle, FALSE if not.
//==============================================================================================

BOOL GS_OGLCollide::IsRectInRect(RECT rcSource, RECT rcDest, int nPercent)
{

    // Should we adjust the rectangles?
    if (nPercent > 0)
    {
        // Adjust the size of both rectangles by a percentage.
        this->SetRectPercentXY(&rcSource, nPercent);
        this->SetRectPercentXY(&rcDest,   nPercent);
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

    if ((rcSource.top > rcDest.top)  || (rcSource.bottom < rcDest.bottom))
    {
        // Source not within dest.
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

    if ((rcSource.right > rcDest.right) || (rcSource.left < rcDest.left))
    {
        // Source not within dest.
        return FALSE;
    }


    // Source within dest.
    return TRUE;
}


////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_OGLCollide::IsRectOnRect():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Returns: TRUE if the source rectangle overlaps the destination rectangle, FALSE if not.
//==============================================================================================

BOOL GS_OGLCollide::IsRectOnRect(RECT rcSource, RECT rcDest, int nPercent)
{

    // Should we adjust the rectangles?
    if (nPercent > 0)
    {
        // Adjust the size of both rectangles by a percentage.
        this->SetRectPercentXY(&rcSource, nPercent);
        this->SetRectPercentXY(&rcDest,   nPercent);
    }

    // +--------+
    // |        |
    // | SOURCE |
    // |        |
    // +--------+
    // +--------+
    // |        |
    // |  DEST  |
    // |        |
    // +--------+
    // +--------+
    // |        |
    // | SOURCE |
    // |        |
    // +--------+

    if ((rcSource.top <= rcDest.bottom) || (rcSource.bottom >= rcDest.top))
    {
        // No overlap possible.
        return FALSE;
    }

    // +--------+ +--------+ +--------+
    // |        | |        | |        |
    // | SOURCE | |  DEST  | | SOURCE |
    // |        | |        | |        |
    // +--------+ +--------+ +--------+

    if ((rcSource.right <= rcDest.left) || (rcSource.left >= rcDest.right))
    {
        // No overlap possible.
        return FALSE;
    }

    // An overlap has occured.
    return TRUE;
}


////////////////////////////////////////////////////////////////////////////////////////////////
