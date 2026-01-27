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


//==============================================================================================
// Include Game System (GS) header files.
// ---------------------------------------------------------------------------------------------
#include "gs_ogl_image.h"
//==============================================================================================


////////////////////////////////////////////////////////////////////////////////////////////////
// Constructor/Destructor Methods //////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_OGLImage::GS_OGLImage():
// ---------------------------------------------------------------------------------------------
// Purpose: The constructor, initializes class data when class object is created.
// ---------------------------------------------------------------------------------------------
// Returns: Nothing.
//==============================================================================================

GS_OGLImage::GS_OGLImage()
{

    m_pImage = NULL;

    m_nImageWidth  = 0;
    m_nImageHeight = 0;
    m_nColorBytes  = 0;
    m_nImageSize   = 0;

    m_bHasAlpha = FALSE;
}


////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_OGLImage::~GS_OGLImage():
// ---------------------------------------------------------------------------------------------
// Purpose: The destructor, de-initializes class data when class object is destroyed.
// ---------------------------------------------------------------------------------------------
// Returns: Nothing.
//==============================================================================================

GS_OGLImage::~GS_OGLImage()
{

    this->Destroy();
}


////////////////////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////////////////////
// Helper Methods To Load TGA Files ////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_OGLImage::LoadUncompressedTGA():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Returns: TRUE if successful, FALSE if failed.
//==============================================================================================

BOOL GS_OGLImage::LoadUncompressedTGA(FILE* pFile)
{

    // Variable for holding the TGA header.
    BYTE TGAHeader[6] = { 0,0,0,0,0,0 };

    // Read TGA header.
    if (fread(TGAHeader, sizeof(TGAHeader), 1, pFile) == 0)
    {
        // Is file still open?
        if (pFile != NULL)
        {
            fclose(pFile);
        }
        return FALSE;
    }

    // Determine the image width (highbyte * 256 + lowbyte).
    m_nImageWidth  = TGAHeader[1] * 256 + TGAHeader[0];
    // Determine the image height (highbyte * 256 + lowbyte).
    m_nImageHeight = TGAHeader[3] * 256 + TGAHeader[2];
    // Determine the color size of the image in bytes.
    m_nColorBytes  = TGAHeader[4] / 8;
    // Determine if the image has an alpha component.
    m_bHasAlpha    = (m_nColorBytes==4);

    // Is the image information valid?
    if ((m_nImageWidth <= 0) || (m_nImageHeight <= 0) || ((m_nColorBytes != 3) &&
            (m_nColorBytes != 4)))
    {
        // Clear image data and attributes.
        this->Destroy();
        // Is file still open?
        if (pFile != NULL)
        {
            fclose(pFile);
        }
        return FALSE;
    }

    // Compute the total amount of memory needed to store the image data.
    m_nImageSize = (m_nColorBytes * m_nImageWidth * m_nImageHeight);

    // Does image exist already?
    if (m_pImage != NULL)
    {
        // De-allocate memory for image data.
        delete []m_pImage;
    }
    m_pImage = NULL;

    // Allocate enough memory for the image data.
    m_pImage = new BYTE [m_nImageSize];

    // Were we able to allocate memory?
    if (!m_pImage)
    {
        // Clear image data and attributes.
        this->Destroy();
        // Is file still open?
        if (pFile != NULL)
        {
            fclose(pFile);
        }
        return FALSE;
    }

    // Were we able to read the TGA image data?
    if (fread(m_pImage, 1, m_nImageSize, pFile) != (unsigned)m_nImageSize)
    {
        // Clear image data and attributes.
        this->Destroy();
        // Is file still open?
        if (pFile != NULL)
        {
            fclose(pFile);
        }
        return FALSE;
    }

    // Swap bytes to the correct color format (BGR -> RGB).
    for (int nSwap = 0; nSwap < m_nImageSize; nSwap += m_nColorBytes)
    {
        m_pImage[nSwap] ^= m_pImage[nSwap+2] ^= m_pImage[nSwap] ^= m_pImage[nSwap+2];
    }

    // Close the TGA file.
    fclose(pFile);

    return TRUE;
}


////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_OGLImage::LoadCompressedTGA():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Returns: TRUE if successful, FALSE if failed.
//==============================================================================================

BOOL GS_OGLImage::LoadCompressedTGA(FILE* pFile)
{

    // Variable for holding the TGA header.
    BYTE TGAHeader[6] = { 0,0,0,0,0,0 };

    // Read the TGA header.
    if (fread(TGAHeader, sizeof(TGAHeader), 1, pFile) == 0)
    {
        // Is file still open?
        if (pFile != NULL)
        {
            fclose(pFile);
        }
        return FALSE;
    }

    // Determine the image width (highbyte * 256 + lowbyte).
    m_nImageWidth  = TGAHeader[1] * 256 + TGAHeader[0];
    // Determine the image height (highbyte * 256 + lowbyte).
    m_nImageHeight = TGAHeader[3] * 256 + TGAHeader[2];
    // Determine the color size of the image in bytes.
    m_nColorBytes  = TGAHeader[4] / 8;
    // Determine the alpha component.
    m_bHasAlpha    = (m_nColorBytes==4);

    // Is the image information valid?
    if ((m_nImageWidth <= 0) || (m_nImageHeight <= 0) || ((m_nColorBytes != 3) &&
            (m_nColorBytes != 4)))
    {
        // Clear image data and attributes.
        this->Destroy();
        // Is file still open?
        if (pFile != NULL)
        {
            fclose(pFile);
        }
        return FALSE;
    }

    // Compute the total amout of memory needed to store the image data.
    m_nImageSize = (m_nColorBytes * m_nImageWidth * m_nImageHeight);

    // Does image exist already?
    if (m_pImage != NULL)
    {
        // De-allocate memory for image data.
        delete []m_pImage;
    }
    m_pImage = NULL;

    // Allocate enough memory for the image data.
    m_pImage = new BYTE [m_nImageSize];

    // Were we able to allocate memory?
    if (!m_pImage)
    {
        // Clear image data and attributes.
        this->Destroy();
        // Is file still open?
        if (pFile != NULL)
        {
            fclose(pFile);
        }
        return FALSE;
    }

    unsigned int nPixelCount   = m_nImageHeight * m_nImageWidth; // Nuber of pixels in the image.
    unsigned int nCurrentPixel = 0; // Current pixel being read.
    unsigned int nCurrentByte  = 0; // Current byte.

    // Allocate enough memory for the color of a single pixel.
    BYTE* pColorBuffer = new BYTE [m_nColorBytes];

    // Were we able to allocate memory?
    if (!pColorBuffer)
    {
        // Clear image data and attributes.
        this->Destroy();
        // Is file still open?
        if (pFile != NULL)
        {
            fclose(pFile);
        }
        return FALSE;
    }

    do
    {
        BYTE ucChunkHeader = 0; // Storage for "chunk" header.

        // Read in the chunk header.
        if (fread(&ucChunkHeader, sizeof(BYTE), 1, pFile) == 0)
        {
            // Clear image data and attributes.
            this->Destroy();
            // Is file still open?
            if (pFile != NULL)
            {
                fclose(pFile);
            }
            return FALSE;
        }

        // If the header is < 128, it means the that is the number of RAW color packets minus one.
        if (ucChunkHeader < 128)
        {
            // Add 1 to get number of following color values.
            ucChunkHeader++;
            // Read RAW color values.
            for (short sLoop = 0; sLoop < ucChunkHeader; sLoop++)
            {
                // Try to read 1 pixel.
                if (fread(pColorBuffer, 1, m_nColorBytes, pFile) != (unsigned)m_nColorBytes)
                {
                    // Clear image data and attributes.
                    this->Destroy();
                    // De-allocate memory for color data.
                    delete [] pColorBuffer;
                    // Is file still open?
                    if (pFile != NULL)
                    {
                        fclose(pFile);
                    }
                    return FALSE;
                }
                // Write to memory and flip the R and B color values around in the process.
                m_pImage[nCurrentByte]   = pColorBuffer[2];
                m_pImage[nCurrentByte+1] = pColorBuffer[1];
                m_pImage[nCurrentByte+2] = pColorBuffer[0];
                // If it's a 32-bit image.
                if (m_nColorBytes == 4)
                {
                    // Copy the 4th byte.
                    m_pImage[nCurrentByte+3] = pColorBuffer[3];
                }
                // Increase the current byte by the number of bytes per pixel.
                nCurrentByte += m_nColorBytes;
                // Increase current pixel by 1.
                nCurrentPixel++;
                // Make sure we haven't read too many pixels.
                if (nCurrentPixel > nPixelCount)
                {
                    // Clear image data and attributes.
                    this->Destroy();
                    // De-allocate memory for color data.
                    delete [] pColorBuffer;
                    // Is file still open?
                    if (pFile != NULL)
                    {
                        fclose(pFile);
                    }
                    return FALSE;
                }
            }
        }
        // ucChunkHeader > 128 RLE data, next color reapeated ucChunkHeader - 127 times.
        else
        {
            // Subteact 127 to get rid of the ID bit.
            ucChunkHeader -= (unsigned char)127;
            // Read a single RLE color value.
            if (fread(pColorBuffer, 1, m_nColorBytes, pFile) != (unsigned)m_nColorBytes)
            {
                // Clear image data and attributes.
                this->Destroy();
                // De-allocate memory for color data.
                delete [] pColorBuffer;
                // Is file still open?
                if (pFile != NULL)
                {
                    fclose(pFile);
                }
                return FALSE;
            }
            // Write RLE the color value to memory as many times as required.
            for (short sLoop = 0; sLoop < ucChunkHeader; sLoop++)
            {
                // Write to memory and flip the R and B color values around in the process.
                m_pImage[nCurrentByte]   = pColorBuffer[2];
                m_pImage[nCurrentByte+1] = pColorBuffer[1];
                m_pImage[nCurrentByte+2] = pColorBuffer[0];
                // If TGA images is 32 bpp.
                if (m_nColorBytes == 4)
                {
                    // Copy 4th byte
                    m_pImage[nCurrentByte+3] = pColorBuffer[3];
                }
                // Increase current byte by the number of bytes per pixel.
                nCurrentByte += m_nColorBytes;
                // Increase pixel count by 1
                nCurrentPixel++;
                // Make sure we havent written too many pixels.
                if (nCurrentPixel > nPixelCount)
                {
                    // Clear image data and attributes.
                    this->Destroy();
                    // De-allocate memory for color data.
                    delete [] pColorBuffer;
                    // Is file still open?
                    if (pFile != NULL)
                    {
                        fclose(pFile);
                    }
                    return FALSE;
                }
            }
        }
        // Loop while there are still pixels left.
    }
    while(nCurrentPixel < nPixelCount);

    // Close the TGA file.
    fclose(pFile);

    return TRUE;
}


////////////////////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////////////////////
// Image Create/Load Methods. //////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_OGLImage::Create():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Returns: TRUE if successful, FALSE if failed.
//==============================================================================================

BOOL GS_OGLImage::Create(int nWidth, int nHeight, BOOL bHasAlpha)
{

    // Does image exist already?
    if (m_pImage != NULL)
    {
        // De-allocate memory for image data.
        delete []m_pImage;
    }
    m_pImage = NULL;

    // Copy over image information.
    m_nImageWidth  = nWidth;
    m_nImageHeight = nHeight;
    m_bHasAlpha    = bHasAlpha;

    // Calculate the color size.
    m_nColorBytes = bHasAlpha ? 4 : 3;

    // Determine space needed to store image data.
    m_nImageSize = m_nImageWidth * m_nImageHeight * m_nColorBytes;

    // Allocate space for the image data.
    m_pImage = new BYTE [m_nImageSize];

    // Were we able to allocate memory for the image?
    if (!m_pImage)
    {
        GS_Error::Report("GS_IMAGE.CPP", 405, "Failed to create image!");
        // Clear image data and attributes.
        this->Destroy();
        return FALSE;
    }

    return TRUE;
}


////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_OGLImage::Destroy():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Returns: TRUE if successful, FALSE if failed.
//==============================================================================================

void GS_OGLImage::Destroy()
{

    if (m_pImage != NULL)
    {
        delete []m_pImage;
    }
    m_pImage = NULL;

    m_nImageWidth  = 0;
    m_nImageHeight = 0;
    m_nColorBytes  = 0;
    m_nImageSize   = 0;

    m_bHasAlpha = FALSE;
}


////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_OGLImage::Load():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Returns: TRUE if successful, FALSE if failed.
//==============================================================================================

BOOL GS_OGLImage::Load(const char* pszFilename)
{

    // Check if valid filename.
    if(!pszFilename)
    {
        return FALSE;
    }

    // Get the length of the filename.
    int nLength = strlen(pszFilename);

    // Check extension to see wether it's a TGA, PCX, BMP or other image format.
    if (toupper(pszFilename[nLength-3])=='T' && toupper(pszFilename[nLength-2])=='G' &&
            toupper(pszFilename[nLength-1])=='A')
    {
        // Load the tga picture file.
        return this->LoadTGA(pszFilename);
    }
    else if (toupper(pszFilename[nLength-3])=='P' && toupper(pszFilename[nLength-2])=='C' &&
             toupper(pszFilename[nLength-1])=='X')
    {
        // Load the pcx picture file.
        return this->LoadPCX(pszFilename);
    }
    else if (toupper(pszFilename[nLength-3])=='B' && toupper(pszFilename[nLength-2])=='M' &&
             toupper(pszFilename[nLength-1])=='P')
    {
        // Load the bmp picture file.
        return this->LoadBMP(pszFilename);
    }
    else
    {
        // Unknown file format try loading it anyway.
        if (this->LoadBMP(pszFilename))
        {
            return TRUE;
        }
        if (this->LoadPCX(pszFilename))
        {
            return TRUE;
        }
        if (this->LoadTGA(pszFilename))
        {
            return TRUE;
        }
        // Failed to load image.
        GS_Error::Report("GS_IMAGE.CPP", 491, "Failed to load image. Unknown format!");
        return FALSE;
    }
}


////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_OGLImage::LoadBMP():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Returns: TRUE if successful, FALSE if failed.
//==============================================================================================

BOOL GS_OGLImage::LoadBMP(const char* pszFilename)
{

    // Do we have a valid filename?
    if (!pszFilename)
    {
        return FALSE;
    }

    // Try to open the bitmap file.
    FILE* pFile = fopen(pszFilename, "rb");

    // Were we able to open the bitmap file?
    if (!pFile)
    {
        return FALSE;
    }

    BITMAPFILEHEADER BitmapHeader;

    // Read the header of the bitmap file.
    if (!fread(&BitmapHeader, sizeof(BITMAPFILEHEADER), 1, pFile))
    {
        // Is file still open?
        if (pFile != NULL)
        {
            fclose(pFile);
        }
        return FALSE;
    }

    // Check the type field to make sure we have a valid bitmap file.
    if (memcmp(&BitmapHeader.bfType, "BM", 2))
    {
        // Is file still open?
        if (pFile != NULL)
        {
            fclose(pFile);
        }
        return FALSE;
    }

    BITMAPINFOHEADER HeaderInfo;

    // Read the information header of the bitmap file.
    if (!fread(&HeaderInfo, sizeof(BITMAPINFOHEADER), 1, pFile))
    {
        // Is file still open?
        if (pFile != NULL)
        {
            fclose(pFile);
        }
        return FALSE;
    }

    // We only support 24-bit and 32-bit bitmaps so make sure that's what we have.
    if ((HeaderInfo.biBitCount!=24) && (HeaderInfo.biBitCount!=32))
    {
        // Is file still open?
        if (pFile != NULL)
        {
            fclose(pFile);
        }
        return FALSE;
    }

    // Save image dimensions.
    m_nImageWidth  = HeaderInfo.biWidth;
    m_nImageHeight = HeaderInfo.biHeight;

    // Save image color size in bytes.
    m_nColorBytes  = HeaderInfo.biBitCount / 8;

    // Image has no alpha component.
    m_bHasAlpha    = FALSE;

    // Set image size for allocating memory.
    m_nImageSize = m_nImageWidth * m_nImageHeight * m_nColorBytes;

    // Does image exist already?
    if (m_pImage != NULL)
    {
        // De-allocate memory for image data.
        delete []m_pImage;
    }
    m_pImage = NULL;

    // Allocate enough space for the image data.
    m_pImage = new BYTE [m_nImageSize];

    // Were we able to allocate memory?
    if (!m_pImage)
    {
        // Clear image data and attributes.
        this->Destroy();
        // Is file still open?
        if (pFile != NULL)
        {
            fclose(pFile);
        }
        return FALSE;
    }

    // Set stride - The stride is the TRUE number of bytes in a line of pixels. Windows makes
    // all the bitmaps DWORD aligned (divisible evenly by 4), so if you bitmap say was 103x103
    // pixels, Windows would add 1 "padding byte" to it so in memory it would be 104x103 pixels.
    // The "padding bytes" do not get blit (drawn) to the screen, they're just there so again
    // everything is DWORD aligned which makes blitting (drawing to the screen) easier.
    int nBitmapStride = HeaderInfo.biWidth * (HeaderInfo.biBitCount / 8);

    // Ensure nBitmapStride is DWORD aligned.
    while ((nBitmapStride % 4) != 0)
    {
        nBitmapStride++;
    }

    // Bytes per line (number of bytes in a scan line).
    unsigned int nBytesPerLine = HeaderInfo.biWidth * (HeaderInfo.biBitCount / 8);

    // Calculate how many "padding" bytes there are. We do NOT want to read in the padding bytes.
    // Remember that Windows adds padding bytes to ensure all bitmaps are DWORD aligned.
    unsigned int nPaddingBytes = nBitmapStride - nBytesPerLine;

    // Jump to the location where the pixel data is stored.
    if (fseek(pFile, BitmapHeader.bfOffBits, SEEK_SET))
    {
        // Clear image data and attributes.
        this->Destroy();
        // Is file still open?
        if (pFile != NULL)
        {
            fclose(pFile);
        }
        return FALSE;
    }

    // Loop over all the scan lines (all the rows of pixels in the image).
    for (int y = 0; y < HeaderInfo.biHeight; y++)
    {
        // Read the precise number of bytes that the scan line requires into the image.
        if (!fread(m_pImage + (nBitmapStride * y), nBytesPerLine, 1, pFile))
        {
            // Clear image data and attributes.
            this->Destroy();
            // Is file still open?
            if (pFile != NULL)
            {
                fclose(pFile);
            }
            return FALSE;
        }
        // Skip over any padding bytes.
        if (fseek(pFile, nPaddingBytes, SEEK_CUR))
        {
            // Clear image data and attributes.
            this->Destroy();
            // Is file still open?
            if (pFile != NULL)
            {
                fclose(pFile);
            }
            return FALSE;
        }
    }

    // Close the BMP file.
    fclose(pFile);

    BYTE temp;

    // Convert image data from BGR to RGB color format.
    for (long i = 0; i < m_nImageSize; i += m_nColorBytes)
    {
        temp = m_pImage[i+0];
        m_pImage[i+0] = m_pImage[i+2];
        m_pImage[i+2] = temp;
    }

    return TRUE;
}


////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_OGLImage::LoadPCX():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Returns: TRUE if successful, FALSE if failed.
//==============================================================================================

BOOL GS_OGLImage::LoadPCX(const char* pszFilename)
{

    // Open the PCX file for reading.
    FILE* pFile = fopen(pszFilename,"rb");

    // Were we able to open the file?
    if (!pFile)
    {
        return FALSE;
    }

    // Creata an array to hold palette.
    BYTE Palette[768];

    // Go to the end of the file (where the palette is located).
    fseek(pFile, -769, SEEK_END);

    // Read value to see wether it's the palette.
    int nCurrentValue = fgetc(pFile);

    // Are we at the palette?
    if (nCurrentValue != 12)
    {
        // Is file still open?
        if (pFile != NULL)
        {
            fclose(pFile);
        }
        return FALSE;
    }

    // Were we able to read the entire palette?
    if (!fread(Palette, 1, 768, pFile))
    {
        // Is file still open?
        if (pFile != NULL)
        {
            fclose(pFile);
        }
        return FALSE;
    }

    // Return to the beginning of the file.
    fseek(pFile, 0, SEEK_SET);

    // Structure to hold PCX header.
    GS_PCXHeader gsPCXHeader;

    // Were we able to read the entire header?
    if (!fread(&gsPCXHeader, sizeof(GS_PCXHeader), 1, pFile))
    {
        // Is file still open?
        if (pFile != NULL)
        {
            fclose(pFile);
        }
        return FALSE;
    }

    // Make sure the file we have is a PCX file.
    if ((gsPCXHeader.cManufacturer != 10) || (gsPCXHeader.cVersion != 5) ||
            (gsPCXHeader.cEncoding != 1))
    {
        // Is file still open?
        if (pFile != NULL)
        {
            fclose(pFile);
        }
        return FALSE;
    }

    // Save image dimensions.
    m_nImageWidth  = gsPCXHeader.wMaxX - gsPCXHeader.wMinX + 1;
    m_nImageHeight = gsPCXHeader.wMaxY - gsPCXHeader.wMinY + 1;

    // Image has no alpha component.
    m_bHasAlpha   = FALSE;

    // Set image color size.
    m_nColorBytes = 3;

    // Set image size for allocating memory.
    m_nImageSize = m_nImageWidth * m_nImageHeight * m_nColorBytes;

    // Does image exist already?
    if (m_pImage != NULL)
    {
        // De-allocate memory for image data.
        delete []m_pImage;
    }
    m_pImage = NULL;

    // Allocate enough space for the image data.
    m_pImage = new BYTE [m_nImageSize];

    // Were we able to allocate memory?
    if (!m_pImage)
    {
        // Clear image data and attributes.
        this->Destroy();
        // Is file still open?
        if (pFile != NULL)
        {
            fclose(pFile);
        }
        return FALSE;
    }

    /* Varialble to hold postion in file.
    // int nPosition = 0;

    // Run through the image.
    while (nPosition < m_nImageSize) {
       // Get the current character.
       nCurrentValue = fgetc(pFile);
       // Is it a status byte (i.e. RLE encoded)?
       if (nCurrentValue >= 0xc0) {
          // Get the repeat length.
          int nRepeatLength = 0x3f & nCurrentValue;
          // Get the next character in the file.
          nCurrentValue = fgetc(pFile);
          // Repeat it the number of times.
          for (int nLoop = 0; nLoop < nRepeatLength; nLoop++) {
             // Write the pixel to the image data.
             m_pImage[nPosition]   = Palette[nCurrentValue*3];
             m_pImage[nPosition+1] = Palette[nCurrentValue*3+1];
             m_pImage[nPosition+2] = Palette[nCurrentValue*3+2];
             // Move forward to the next pixel.
             nPosition += m_nColorBytes;
            }
          }
       else {
          // Write the pixel to the image data.
          m_pImage[nPosition]   = Palette[nCurrentValue*3];
          m_pImage[nPosition+1] = Palette[nCurrentValue*3+1];
          m_pImage[nPosition+2] = Palette[nCurrentValue*3+2];
          // Move forward to the next pixel.
          nPosition += m_nColorBytes;
          }
       } */

    // Varialble to hold postion in file.
    int nPosition;

    // Run through the entire image from bottom to top and left to right thereby flipping
    // the image vertically (otherwise the image will be the wrong side up).
    for (long y = m_nImageHeight-1; y >= 0; y--)
    {
        for (long x = 0; x < m_nImageWidth; x++)
        {
            // Get the current character.
            nCurrentValue = fgetc(pFile);
            // Is it a status byte (i.e. RLE encoded)?
            if (nCurrentValue >= 0xc0)
            {
                // Get the repeat length.
                int nRepeatLength = 0x3f & nCurrentValue;
                // Get the next character in the file.
                nCurrentValue = fgetc(pFile);
                // Repeat it the number of times.
                for (int nLoop = 0; nLoop < nRepeatLength; nLoop++)
                {
                    // Determine the position of the current pixel.
                    nPosition = (((m_nImageWidth * y) + x) * m_nColorBytes);
                    // Write the pixel to the image data.
                    m_pImage[nPosition]   = Palette[nCurrentValue*3];
                    m_pImage[nPosition+1] = Palette[nCurrentValue*3+1];
                    m_pImage[nPosition+2] = Palette[nCurrentValue*3+2];
                    // Is horizontal value equal to image width?
                    if (x >= m_nImageWidth)
                    {
                        // Reset to start of row.
                        x = 0;
                        // Start a new row.
                        y--;
                    }
                    // Next value in row.
                    x++;
                }
                // Adjust value.
                x--;
            }
            else
            {
                // Determine the position of the current pixel.
                nPosition = (((m_nImageWidth * y) + x) * m_nColorBytes);
                // Write the pixel to the image data.
                m_pImage[nPosition]   = Palette[nCurrentValue*3];
                m_pImage[nPosition+1] = Palette[nCurrentValue*3+1];
                m_pImage[nPosition+2] = Palette[nCurrentValue*3+2];
            }
        }
    }

    // Close the PCX file.
    fclose(pFile);

    return TRUE;
}


////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_OGLImage::LoadTGA():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Returns: TRUE if successful, FALSE if failed.
//==============================================================================================

BOOL GS_OGLImage::LoadTGA(const char* pszFilename)
{

    // Pointer to TGA image file.
    FILE* pFile;

    // Open the TGA file for reading.
    pFile = fopen(pszFilename, "rb");

    // Were we able to open the file?
    if (pFile == NULL)
    {
        return FALSE;
    }

    // Variable to store TGA header.
    BYTE TGAHeader[12] = { 0,0,0,0,0,0,0,0,0,0,0,0 };

    // Attempt to read 12 byte header from file.
    if (fread(TGAHeader, sizeof(TGAHeader), 1, pFile) == 0)
    {
        // Is file still open?
        if (pFile != NULL)
        {
            fclose(pFile);
        }
        return FALSE;
    }

    BYTE UncompressedTGA[12] = { 0,0,2, 0,0,0,0,0,0,0,0,0 }; // Uncompressed TGA header.
    BYTE CompressedTGA[12]   = { 0,0,10,0,0,0,0,0,0,0,0,0 }; // Compressed TGA header.

    // See if header matches the predefined header of an uncompressed TGA image.
    if (memcmp(UncompressedTGA, TGAHeader, sizeof(TGAHeader)) == 0)
    {
        // If so, jump to uncompressed TGA loading code.
        LoadUncompressedTGA(pFile);
    }
    // See if header matches the predefined header of an RLE compressed TGA image.
    else if(memcmp(CompressedTGA, TGAHeader, sizeof(TGAHeader)) == 0)
    {
        // If so, jump to compressed TGA loading code.
        LoadCompressedTGA(pFile);
    }
    // If header matches neither type.
    else
    {
        fclose(pFile);
        return FALSE;
    }

    return TRUE;
}


////////////////////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////////////////////
// Set Methods. ////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_OGLImage::SetPixel():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Returns: Nothing.
//==============================================================================================

void GS_OGLImage::SetPixel(int x, int y, GS_OGLColor gsColor)
{

    // Does an image exist?
    if (!m_pImage)
    {
        return;
    }

    // Are the Coordinates within the image area?
    if (!this->InImage(x, y))
    {
        return;
    }

    // Get the position of the pixel within the image.
    BYTE* pByte = m_pImage + this->CalcPos(x,y);

    // Set the parts of the color.
    if (m_bHasAlpha)
    {
        *(pByte++) = FloatToByte(gsColor.fRed);
        *(pByte++) = FloatToByte(gsColor.fGreen);
        *(pByte++) = FloatToByte(gsColor.fBlue);
        *(pByte)   = 0xff;
    }
    else
    {
        *(pByte++) = FloatToByte(gsColor.fRed);
        *(pByte++) = FloatToByte(gsColor.fGreen);
        *(pByte)   = FloatToByte(gsColor.fBlue);
    }
}


////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_OGLImage::SetData():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Returns: Nothing.
//==============================================================================================

void GS_OGLImage::SetData(BYTE* pData)
{

    if ((!pData) || (!m_pImage))
    {
        return;
    }

    CopyMemory(m_pImage, pData, (m_nImageWidth * m_nImageHeight * m_nColorBytes));
}


////////////////////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////////////////////
// Get Methods. ////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_OGLImage::GetPixel:
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Returns: The color of the selected pixel.
//==============================================================================================

GS_OGLColor GS_OGLImage::GetPixel(int x, int y)
{

    // Does an image exist?
    if (!m_pImage)
    {
        return GS_OGLColor(0.0f, 0.0f, 0.0f);
    }

    // Are the Coordinates within the image area?
    if (!this->InImage(x, y))
    {
        return GS_OGLColor(0.0f, 0.0f, 0.0f);
    }

    // For 32-bit RGBA
    if (m_nColorBytes == 4)
    {
        BYTE* pByte = &m_pImage[CalcPos(x, y)];
        BYTE r = *pByte++;
        BYTE g = *pByte++;
        BYTE b = *pByte++;
        BYTE a = *pByte;
        return(GS_OGLColor(ByteToFloat(r), ByteToFloat(g), 
                           ByteToFloat(b), ByteToFloat(a)));
    }
    
    // For 24-bit RGB
    BYTE* pByte = &m_pImage[CalcPos(x, y)];
    BYTE r = *pByte++;
    BYTE g = *pByte++;
    BYTE b = *pByte;
    return(GS_OGLColor(ByteToFloat(r), ByteToFloat(g), ByteToFloat(b)));
}


////////////////////////////////////////////////////////////////////////////////////////////////

// GS_Error::Report("GS_OGL_IMAGE.CPP", 10, "Break Point!");
