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

#ifndef GS_OGL_TEXTURE_H
#define GS_OGL_TEXTURE_H


//==============================================================================================
// Include platform abstraction header files.
// ---------------------------------------------------------------------------------------------
#include "gs_platform.h"
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


//==============================================================================================
// Include Game System (GS) header files.
// ---------------------------------------------------------------------------------------------
#include "gs_error.h"
#include "gs_object.h"
#include "gs_ogl_image.h"
//==============================================================================================


//==============================================================================================
// Texture type enumeration.
// ---------------------------------------------------------------------------------------------
typedef enum GS_TEXTURE_TYPE
{
    GS_IMAGE2D,
    GS_MIPMAP
} GS_TextureType;
//==============================================================================================


//==============================================================================================
// Texture information structure.
// ---------------------------------------------------------------------------------------------
typedef struct GS_TEXTURE_INFO
{
    GLuint glID;     // The OpenGL texture ID.
    GLenum glFormat; // The color format of the OpenGL texture (RGB, RGBA etc.).
    int nWidth;      // Texture width in pixels.
    int nHeight;     // Texture height in pixels.
    int nBpp;        // Texture color size in bytes.
    BOOL bHasAlpha;  // Whether the texture has an alpha component.
    BOOL bIsReady;   // Wether the texture has been created or not.
} GS_TextureInfo;
//==============================================================================================


////////////////////////////////////////////////////////////////////////////////////////////////
// Class Definition. ///////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////


class GS_OGLTexture : public GS_Object
{

private:

    GS_OGLImage m_gsImage; // An image object to load the texture image.

    GS_TextureInfo m_gsTextureInfo; // Contains all the relevant texture information.

protected:

    // No protected members.

public:

    GS_OGLTexture();
    ~GS_OGLTexture();

    BOOL Create(const char* pszFilename, GS_TextureType gsTextureType = GS_MIPMAP,
                GLint glMinFilter = GL_LINEAR, GLint glMagFilter = GL_LINEAR);
    // BOOL CreateFromBMP(const char* pszFilename, GS_TextureType gsTextureType = GS_MIPMAP,
    //                    GLint glMinFilter = GL_LINEAR, GLint glMagFilter = GL_LINEAR);

    void Destroy();

    GLuint GetID()
    {
        return m_gsTextureInfo.glID;
    }
    int    GetWidth()
    {
        return m_gsTextureInfo.nWidth;
    }
    int    GetHeight()
    {
        return m_gsTextureInfo.nHeight;
    }
    int    GetBpp()
    {
        return m_gsTextureInfo.nBpp;
    }
    BOOL   HasAlpha()
    {
        return m_gsTextureInfo.bHasAlpha;
    }
    BOOL   IsReady()
    {
        return m_gsTextureInfo.bIsReady;
    }
};


////////////////////////////////////////////////////////////////////////////////////////////////

#endif
