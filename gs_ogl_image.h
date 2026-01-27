/*============================================================================================+
 | Game System (GS) Library                                                                   |
 |--------------------------------------------------------------------------------------------|
 | FILES: gs_ogl_image.cpp, gs_ogl_image.h                                                    |
 |--------------------------------------------------------------------------------------------|
 | CLASS: GS_OGLImage                                                                         |
 |--------------------------------------------------------------------------------------------|
 | ABOUT: ...                                                                                 |
 |--------------------------------------------------------------------------------------------|
 |                                                                                    08/2003 |
 +============================================================================================*/

#ifndef GS_OGL_IMAGE_H
#define GS_OGL_IMAGE_H


//==============================================================================================
// Include platform abstraction header files.
// ---------------------------------------------------------------------------------------------
#include "gs_platform.h"
//==============================================================================================


//==============================================================================================
// Include standard C library header files.
// ---------------------------------------------------------------------------------------------
#include <stdio.h>
#include <math.h>
#include <ctype.h>
//==============================================================================================


//==============================================================================================
// Include Game System (GS) header files.
// ---------------------------------------------------------------------------------------------
#include "gs_error.h"
#include "gs_object.h"
#include "gs_ogl_color.h"
//==============================================================================================


//==============================================================================================
// TGA image structures.
// ---------------------------------------------------------------------------------------------
typedef struct TGAHEADER
{
    BYTE Header[12];
} GS_TGAHeader;
// ---------------------------------------------------------------------------------------------
typedef struct TGA
{
    BYTE          Header[6];      // First 6 useful bytes from the header.
    unsigned int  nBytesPerPixel; // Holds number of bytes per pixel.
    unsigned int  nImageSize;     // Holds the image size when setting aside ram.
    unsigned int  nTemp;          // Temporary variable.
    unsigned int  nType;
    unsigned int  nHeight;        // Height of Image.
    unsigned int  nWidth;         // Width ofImage.
    unsigned int  nBitsPerPixel;  // Holds number of bits per pixel.
} GS_TGAInfo;
//==============================================================================================


//==============================================================================================
// PCX image structures.
// ---------------------------------------------------------------------------------------------
typedef struct PCXHEADER
{
    char cManufacturer; // Must be 10 (ZSoft).
    char cVersion;      // These days almost always five.
    char cEncoding;     // 1 = RLE.
    char cBitsPerPixel; // Bits per pixel (per plane).
    WORD wMinX, wMinY;  // Top left coordinates.
    WORD wMaxX, wMaxY;  // Bottom right coordinates.
    WORD wDpiH, wDpiV;  // Dots per inch in each axis.
    char cColorMap[48]; // Old fashioned palette.
    char cReserved;     // Should be 0.
    char cPlanes;       // Number of planes.
    WORD wBytesPerLine; // Bytes to allocate per line - rounded up to an even number.
    WORD wPaletteInfo;  // 1 - Colour, 2 - Grayscale.
    WORD wScreenH, wScreenV; // Screen dimensions.
    char cFiller[54];   // Fill out header to 128 bytes.
} GS_PCXHeader;
//==============================================================================================


////////////////////////////////////////////////////////////////////////////////////////////////
// Class Definition. ///////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////


class GS_OGLImage : public GS_Object
{

private:

    BYTE* m_pImage; // A pointer to the image data.

    int  m_nImageWidth;  // Image width in pixels.
    int  m_nImageHeight; // Image height in pixels.
    int  m_nColorBits;   // Image color size in bits.
    int  m_nColorBytes;  // Image color size in bytes.
    int  m_nImageSize;   // Image size in BYTES for allocation purposes.

    BOOL m_bHasAlpha;    // Whether the image has an alpha component.

    // Determine wether certain coordinates fall within the image area.
    BOOL InImage(int x, int y) const
    {
        return (((x >= 0) && (x < m_nImageWidth)) && ((y >= 0) && (y < m_nImageHeight)));
    };

    // Calculate the specific position of coordinates within the image data.
    int CalcPos(int x, int y) const
    {
        return (((y * m_nImageWidth) + x) * m_nColorBytes);
    };

    BOOL LoadUncompressedTGA(FILE *pFile);
    BOOL LoadCompressedTGA(FILE *pFile);

protected:

    // No protected members.

public:

    GS_OGLImage();
    ~GS_OGLImage();

    BOOL Create(int nWidth, int nHeight, BOOL bHasAlpha);
    void Destroy();

    BOOL Load(const char* pszFilename);
    BOOL LoadBMP(const char* pszFilename);
    BOOL LoadPCX(const char* pszFilename);
    BOOL LoadTGA(const char* pszFilename);

    void        SetPixel(int x, int y, GS_OGLColor gsColor);
    GS_OGLColor GetPixel(int x, int y);

    int GetWidth()  const
    {
        return m_nImageWidth;
    };
    int GetHeight() const
    {
        return m_nImageHeight;
    };

    int GetColorBytes() const
    {
        return m_nColorBytes;
    };
    int GetColorBits()  const
    {
        return m_nColorBytes * 8;
    };

    BOOL HasAlpha() const
    {
        return m_bHasAlpha;
    };

    BYTE* GetData() const
    {
        return m_pImage;
    };
    void  SetData(BYTE* pData);
};


////////////////////////////////////////////////////////////////////////////////////////////////

#endif
