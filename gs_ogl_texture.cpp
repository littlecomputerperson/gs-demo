/*============================================================================================+
 | Game System (GS) Library                                                                   |
 |--------------------------------------------------------------------------------------------|
 | FILES: gs_ogl_texture.cpp, gs_ogl_texture.h                                                |
 |--------------------------------------------------------------------------------------------|
 | CLASS: GS_OGLTexture                                                                       |
 |--------------------------------------------------------------------------------------------|
 | ABOUT: ...                                                                                 |
 |--------------------------------------------------------------------------------------------|
 |                                                                                    08/2003 |
 +============================================================================================*/


//==============================================================================================
// Include Game System (GS) header files.
// ---------------------------------------------------------------------------------------------
#include "gs_ogl_texture.h"
//==============================================================================================


////////////////////////////////////////////////////////////////////////////////////////////////
// Constructor/Destructor Methods //////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_OGLTexture::GS_OGLTexture():
// ---------------------------------------------------------------------------------------------
// Purpose: The constructor, initializes class data when class object is created.
// ---------------------------------------------------------------------------------------------
// Returns: Nothing.
//==============================================================================================

GS_OGLTexture::GS_OGLTexture()
{

    ZeroMemory(&m_gsTextureInfo, sizeof(GS_TextureInfo));
}


////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_OGLTexture::~GS_OGLTexture():
// ---------------------------------------------------------------------------------------------
// Purpose: The destructor, de-initializes class data when class object is destroyed.
// ---------------------------------------------------------------------------------------------
// Returns: Nothing.
//==============================================================================================

GS_OGLTexture::~GS_OGLTexture()
{

    this->Destroy();
}


////////////////////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////////////////////
// Create/Destroy Methods //////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_OGLTexture::Create():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Returns: TRUE if successful, FALSE if failed.
//==============================================================================================

BOOL GS_OGLTexture::Create(const char* pszFilename, GS_TextureType gsTextureType, GLint glMinFilter,
                           GLint glMagFilter)
{

    // Do we have a valid filename?
    if (!pszFilename)
    {
        return FALSE;
    }

    // Destroy the old texture before creating a new one.
    if (m_gsTextureInfo.bIsReady)
    {
        this->Destroy();
    }

    // Load the texture image from file (BMP, PCX or TGA).
    if (!m_gsImage.Load(pszFilename))
    {
        GS_Error::Report("GS_OGL_TEXTURE.CPP", 88, "Failed to load texture image!");
        return FALSE;
    }

    // Save the texture image attributes.
    m_gsTextureInfo.nWidth    = m_gsImage.GetWidth();
    m_gsTextureInfo.nHeight   = m_gsImage.GetHeight();
    m_gsTextureInfo.nBpp      = m_gsImage.GetColorBytes();
    m_gsTextureInfo.bHasAlpha = m_gsImage.HasAlpha();

    // Generate an OpenGL texture ID for one texture.
    glGenTextures(1, &m_gsTextureInfo.glID);

    // Bind the texture ID to the texture.
    glBindTexture(GL_TEXTURE_2D, m_gsTextureInfo.glID);

    // Determine the color format of the image. GL_RGB tells OpenGL the image data is made up of
    // red, green and blue data in that order, GL_RGBA includes an alpha channel.
    m_gsTextureInfo.glFormat = GL_RGB;

    if (m_gsTextureInfo.bHasAlpha)
    {
        m_gsTextureInfo.glFormat = GL_RGBA;
    }

    // The next two lines tell OpenGL what type of filtering to use when the texture is
    // stretched larger (GL_TEXTURE_MAG_FILTER) or smaller (GL_TEXTURE_MIN_FILTER) than the
    // actual texture dimensions. A texture that's filtered with GL_NEAREST will appear blocky
    // when it's stretched but doesn't use a lot of processing power. A texture that's filtered
    // with GL_LINEAR makes the texture look smooth when it's stretched, but requires alot of
    // work from the processor or video card.
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, glMinFilter);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, glMagFilter);

    // Do we wish to create a normal or a mipmap texture?
    if (gsTextureType == GS_IMAGE2D)
    {
        // Create the actual texture. The following line tells OpenGL the texture will be a
        // 2D texture (GL_TEXTURE_2D). Zero represents the image's level of detail, this is
        // usually left at zero. Three is the number of data components (i.e. the number of colors
        // that make up each pixel). Next is the width and height of the texture. If you know the
        // them, you can put them here, but it's easier to let the computer figure it out for you.
        // Zero is the border. It's usually left at zero. The next paramter specifies the data
        // format. GL_UNSIGNED_BYTE means the data that makes up the image is made up of unsigned
        // bytes, and finally... a pointer that tells OpenGL where to get the actual texture data.
        glTexImage2D(GL_TEXTURE_2D, 0, m_gsTextureInfo.nBpp, m_gsTextureInfo.nWidth,
                     m_gsTextureInfo.nHeight, 0, m_gsTextureInfo.glFormat, GL_UNSIGNED_BYTE,
                     m_gsImage.GetData());
    }
    else
    {
        // The following line builds a mipmapped texture. When you tell OpenGL to build a
        // mipmapped texture OpenGL tries to build different sized high quality textures. When you
        // draw a mipmapped texture to the screen OpenGL will select the BEST looking texture from
        // the ones it built (texture with the most detail) and draw it to the screen instead of
        // resizing the original image (which causes detail loss).

        // Unlike with the glTexImage2D() function where textures are limited to a width and
        // height of 64*64,128*128 or 256*256 the gluBuild2DMipmaps() function has no such limits
        // and you can use any size image you want since OpenGL will automatically size it to the
        // proper width and height.
        gluBuild2DMipmaps(GL_TEXTURE_2D, m_gsTextureInfo.nBpp, m_gsTextureInfo.nWidth,
                          m_gsTextureInfo.nHeight, m_gsTextureInfo.glFormat, GL_UNSIGNED_BYTE,
                          m_gsImage.GetData());
    }

    // Free all memory allocated to the texture image.
    m_gsImage.Destroy();

    // Texture was successfully created.
    m_gsTextureInfo.bIsReady = TRUE;

    return TRUE;
}


////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_OGLTexture::CreateFromBMP():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Details: This function is an alternative to the GS_OGLTexture::Create() method and can only
//          be used to load bitmap files. The GS_OGLTexture::Create() method makes use of the
//          GS_OGLImage class which only has the ability to load 24-bit or 32-bit bitmap files.
//          This method uses the OpenGL auxiliary library (glaux.h) and enables one to also load
//          8-bit or 16-bit bitmap files.
// ---------------------------------------------------------------------------------------------
// Returns: TRUE if successful, FALSE if failed.
//==============================================================================================

/* BOOL GS_OGLTexture::CreateFromBMP(char* pszFilename, GS_TextureType gsTextureType,
                                  GLint glMinFilter, GLint glMagFilter) {

   // Do we have a valid filename?
   if (!pszFilename) {
      return FALSE;
      }

   FILE* pBitmapFile;

   // Try to open bitmap file for reading.
   pBitmapFile = fopen(pszFilename, "r");

   // Was opening bitmap file successful?
   if (!pBitmapFile) {
      GS_Error::Report("GS_OGL_TEXTURE.CPP", 193, "Failed to open bitmap file!");
      return FALSE;
      }

   // Close the bitmap file.
   fclose(pBitmapFile);

   // Allocate enough space for the texture image.
   AUX_RGBImageRec* auxTextureImage[1];
   memset(auxTextureImage, 0, sizeof(void *)*1);

   // Load the texture image from the bitmap file.
   auxTextureImage[0] = auxDIBImageLoad(pszFilename);

   // Was loading the bitmap image successful?
   if (!auxTextureImage) {
      GS_Error::Report("GS_OGL_TEXTURE.CPP", 209, "Failed to load texture image!");
      return FALSE;
      }

   // Destroy the old texture before creating a new one.
   if (m_gsTextureInfo.bIsReady) {
      this->Destroy();
      }

   // Save the texture image attributes.
   m_gsTextureInfo.nWidth    = auxTextureImage[0]->sizeX;
   m_gsTextureInfo.nHeight   = auxTextureImage[0]->sizeY;
   m_gsTextureInfo.nBpp      = 3;
   m_gsTextureInfo.bHasAlpha = FALSE;

   // Generate an OpenGL texture ID for one texture.
   glGenTextures(1, &m_gsTextureInfo.glID);

   // Bind the texture ID to the texture.
   glBindTexture(GL_TEXTURE_2D, m_gsTextureInfo.glID);

   // Determine the color format of the image. GL_RGB tells OpenGL the image data is made up of
   // red, green and blue data in that order, GL_RGBA includes an alpha channel.
   m_gsTextureInfo.glFormat = GL_RGB;

   if (m_gsTextureInfo.bHasAlpha) {
      m_gsTextureInfo.glFormat = GL_RGBA;
      }

   // The next two lines tell OpenGL what type of filtering to use when the texture is
   // stretched larger (GL_TEXTURE_MAG_FILTER) or smaller (GL_TEXTURE_MIN_FILTER) than the
   // actual texture dimensions. A texture that's filtered with GL_NEAREST will appear blocky
   // when it's stretched but doesn't use a lot of processing power. A texture that's filtered
   // with GL_LINEAR makes the texture look smooth when it's stretched, but requires alot of
   // work from the processor or video card.
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, glMinFilter);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, glMagFilter);

   // Do we wish to create a normal or a mipmap texture?
   if (gsTextureType == GS_IMAGE2D) {
      // Create the actual texture. The following line tells OpenGL the texture will be a
      // 2D texture (GL_TEXTURE_2D). Zero represents the image's level of detail, this is
      // usually left at zero. Three is the number of data components (i.e. the number of colors
      // that make up each pixel). Next is the width and height of the texture. If you know the
      // them, you can put them here, but it's easier to let the computer figure it out for you.
      // Zero is the border. It's usually left at zero. The next paramter specifies the data
      // format. GL_UNSIGNED_BYTE means the data that makes up the image is made up of unsigned
      // bytes, and finally... a pointer that tells OpenGL where to get the actual texture data.
      glTexImage2D(GL_TEXTURE_2D, 0, m_gsTextureInfo.nBpp, m_gsTextureInfo.nWidth,
                   m_gsTextureInfo.nHeight, 0, m_gsTextureInfo.glFormat, GL_UNSIGNED_BYTE,
                   auxTextureImage[0]->data);
      }
   else {
      // The following line builds a mipmapped texture. When you tell OpenGL to build a
      // mipmapped texture OpenGL tries to build different sized high quality textures. When you
      // draw a mipmapped texture to the screen OpenGL will select the BEST looking texture from
      // the ones it built (texture with the most detail) and draw it to the screen instead of
      // resizing the original image (which causes detail loss).

      // Unlike with the glTexImage2D() function where textures are limited to a width and
      // height of 64*64,128*128 or 256*256 the gluBuild2DMipmaps() function has no such limits
      // and you can use any size image you want since OpenGL will automatically size it to the
      // proper width and height.
      gluBuild2DMipmaps(GL_TEXTURE_2D, m_gsTextureInfo.nBpp, m_gsTextureInfo.nWidth,
                        m_gsTextureInfo.nHeight, m_gsTextureInfo.glFormat, GL_UNSIGNED_BYTE,
                        auxTextureImage[0]->data);
      }

   // Free all memory allocated to the texture image.
   if (auxTextureImage[0]) {
      if (auxTextureImage[0]->data) {
         free(auxTextureImage[0]->data);
         }
      free(auxTextureImage[0]);
      }

   // Texture was successfully created.
   m_gsTextureInfo.bIsReady = TRUE;

   return TRUE;
} */


////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_OGLTexture::Destroy():
// ---------------------------------------------------------------------------------------------
// Purpose: The destructor, de-initializes class data when class object is destroyed.
// ---------------------------------------------------------------------------------------------
// Returns: Nothing.
//==============================================================================================

void GS_OGLTexture::Destroy()
{

    if (!m_gsTextureInfo.bIsReady)
    {
        return;
    }

    glDeleteTextures(1, &m_gsTextureInfo.glID);

    ZeroMemory(&m_gsTextureInfo, sizeof(GS_TextureInfo));

    m_gsTextureInfo.bIsReady = FALSE;
}


////////////////////////////////////////////////////////////////////////////////////////////////
