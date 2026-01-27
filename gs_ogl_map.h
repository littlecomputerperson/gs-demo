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


#ifndef GS_OGL_MAP_H
#define GS_OGL_MAP_H


//==============================================================================================
// Include platform abstraction header files.
// ---------------------------------------------------------------------------------------------
#include "gs_platform.h"
//==============================================================================================


//==============================================================================================
// Include standard C library header files.
// ---------------------------------------------------------------------------------------------
#include <math.h>
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
// Include Game System (GS) header files.
// ---------------------------------------------------------------------------------------------
#include "gs_error.h"
#include "gs_object.h"
#include "gs_file.h"
#include "gs_math.h"
#include "gs_ogl_color.h"
#include "gs_ogl_sprite_ex.h"
//==============================================================================================


//==============================================================================================
// Defines.
// ---------------------------------------------------------------------------------------------
#define MAX_MAP_COLS 128
#define MAX_MAP_ROWS 128
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


// TODO:

// Screen coordinates vs. map coordinates. The viewport is in screen coordinates while the
// map coordinates is the bottom left position of the map relative to the bottom left
// position of the viewport (i.e. a map coordinate of (-20, 20) means that the bottom left
// coordinate of the map is 20 pixels left and 20 pixels higher than the bottom left
// coordinate of the viewport).

////////////////////////////////////////////////////////////////////////////////////////////////
// Class Definition. ///////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////


class GS_OGLMap : public GS_Object
{

private:

    GS_OGLSpriteEx m_gsTileSprites; // Sprite object to hold tile images.

    int m_nMap[MAX_MAP_ROWS][MAX_MAP_COLS]; // Map array to hold tile IDs.
    int m_nNumCols;
    int m_nNumRows;

    RECT m_rcClipBox; // Rectangle structure containing the viewport coordinates.

    int m_nMapCoordX; // Horizontal map coordinate.
    int m_nMapCoordY; // Vertical map coordinate.

    int m_nTileWidth;  // Tile width in pixels
    int m_nTileHeight; // Tile height in pixels.

    int m_nClearTileID; // Which tile ID will not be drawn.

    BOOL m_bLimitX; // Wether scrolling should be limited horizontally.
    BOOL m_bLimitY; // Wether scrolling should be limited vertically.
    BOOL m_bWrapX;  // Wether scrolling should be wrapped horizontally.
    BOOL m_bWrapY;  // Wether scrolling should be wrapped vertically.

    BOOL m_bIsReady; // Wether we are ready to render or not.

protected:

    // ...

public:

    GS_OGLMap();
    ~GS_OGLMap();

    // Create/Destroy methods.

    BOOL Create(const char* pszPathname, int nTotalTiles, int nTilesPerLine, int nTileWidth,
                int nTileHeight, BOOL bFiltered = FALSE, BOOL bMipmap = TRUE);
    BOOL Create(GLuint glTexture, int nWidth, int nHeight, int nTotalTiles,
                int nTilesPerLine, int nTileWidth, int nTileHeight);

    void Destroy();

    // Render methods.

    BOOL Render(int nDestX, int nDestY, HWND = NULL);
    BOOL Render(HWND = NULL);

    // ClipBox methods.

    void SetClipBox(int nMinX = -1, int nMinY = -1, int nMaxX = -1, int nMaxY = -1);
    void SetClipBox(RECT rcClipBox);
    RECT GetClipBox();

    long GetClipBoxLeft();
    long GetClipBoxRight();
    long GetClipBoxBottom();
    long GetClipBoxTop();

    int  GetClipBoxWidth();
    int  GetClipBoxHeight();

    // Map methods.

    BOOL LoadMap(const char* pszPathname, int nNumRows, int nNumCols);
    BOOL SaveMap(const char* pszPathname = NULL);

    void SetMapX(int nDestX);
    void SetMapY(int nDestY);
    int  GetMapX();
    int  GetMapY();

    int  GetMapWidth();
    int  GetMapHeight();

    void SetLimitX(BOOL bEnableLimit);
    void SetLimitY(BOOL bEnableLimit);

    void SetWrapX(BOOL bEnableWrap);
    void SetWrapY(BOOL bEnableWrap);

    void ScrollX(int nDistance);
    void ScrollY(int nDistance);

    // Tile methods.

    void SetTileID(int nRow, int nCol, int nNewTileID);
    int  GetTileID(int nRow, int nCol);
    void ReplaceTileID(int nOldTileID, int nNewTileID);

    void SetClearTileID(int nTileID);

    GLfloat GetTileWidth();
    GLfloat GetTileHeight();

    void SetTotalTiles(int nTotalTiles);
    int  GetTotalTiles();

    void SetTilesPerLine(int nTilesPerLine);
    int  GetTilesPerLine();

    // Conversion methods.

    int MapToID(int nCoordX, int nCoordY);
    int MapToCol(int nCoordX);
    int MapToRow(int nCoordY);

    int ScreenToID(int nCoordX, int nCoordY);
    int ScreenToCol(int nCoordX);
    int ScreenToRow(int nCoordY);

    BOOL MapToScreen(int* pnCoordX, int* pnCoordY);
    BOOL MapToScreen(RECT* prcSource);

    BOOL ScreenToMap(int* pnCoordX, int* pnCoordY);
    BOOL ScreenToMap(RECT* prcSource);

    // Collision methods.

    BOOL IsOnTile(int nCoordX, int nCoordY, int nTileID);
    BOOL IsOnTile(RECT rcSource, int nTileID);
    BOOL IsInTile(RECT rcSource, int nTileID);

    BOOL IsOnTile(int nCoordX, int nCoordY, int nRow, int nCol);
    BOOL IsOnTile(RECT rcSource, int nRow, int nCol);
    BOOL IsInTile(RECT rcSource, int nRow, int nCol);

    BOOL IsOnArea(int nCoordX, int nCoordY, RECT rcMapArea);
    BOOL IsOnArea(RECT rcSource, RECT rcMapArea);
    BOOL IsInArea(RECT rcSource, RECT rcMapArea);

    // Color methods.

    void GetModulateColor(GS_OGLColor* pgsColor);
    void GetModulateColor(float* pRed, float* pGreen, float* pBlue, float* pAlpha);
    void SetModulateColor(GS_OGLColor   gsColor);
    void SetModulateColor(float fRed = -1.0f, float fGreen = -1.0f, float fBlue = -1.0f,
                          float fAlpha = -1.0f);
};


////////////////////////////////////////////////////////////////////////////////////////////////

#endif
