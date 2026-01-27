/*============================================================================================+
 | Game System (GS) Library                                                                   |
 |--------------------------------------------------------------------------------------------|
 | FILES: gs_ogl_particle.cpp, gs_ogl_particle.h                                              |
 |--------------------------------------------------------------------------------------------|
 | CLASS: GS_OGLParticle                                                                      |
 |--------------------------------------------------------------------------------------------|
 | ABOUT: ...                                                                                 |
 |--------------------------------------------------------------------------------------------|
 |                                                                                    08/2003 |
 +============================================================================================*/

#ifndef GS_OGL_PARTICLE_H
#define GS_OGL_PARTICLE_H


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
#include "gs_ogl_texture.h"
#include "gs_ogl_color.h"
//==============================================================================================


//==============================================================================================
// Particle defines.
// ---------------------------------------------------------------------------------------------
#define MAX_PARTICLES 1000
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


//==============================================================================================
// Particle structures.
// ---------------------------------------------------------------------------------------------
typedef struct GS_PARTICLE
{
    BOOL    bIsActive;  // Wether particle should be rendered or not.
    GLint   gliDestX;   // Destination X and Y of the bottom left corner of the particle.
    GLint   gliDestY;
    GLfloat glfRotateX; // Particle rotation on the X, Y and Z-axis (in degrees).
    GLfloat glfRotateY;
    GLfloat glfRotateZ;
    GLfloat glfScaleX;  // X and Y particle scale (1.0f is normal).
    GLfloat glfScaleY;
    GLfloat glfR, glfG, glfB, glfA; // Colors to modulate the particle with.
} GS_Particle;
//==============================================================================================


////////////////////////////////////////////////////////////////////////////////////////////////
// Class Definition. ///////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////


class GS_OGLParticle : public GS_Object
{

private:

    GS_OGLTexture m_gsTexture; // The texture surface holding the particle image.
    GLuint m_gluTexture;       // The texture surface holding the particle image.

    int m_nNumParticles; // How many particles are active.

    BOOL m_bIsReady; // Wether particle object has been created or not.

    BOOL m_bLightingEnabled;  // Wether OpenGL lighting is enabled.
    BOOL m_bDepthTestEnabled; // Wether OpenGL depth testing is enabled.

    GLint m_gliWidth;  // The actual width of the particle.
    GLint m_gliHeight; // The actual height of the particle.

    GS_Particle m_gsParticles[MAX_PARTICLES];

protected:

    // ...

public:

    GS_OGLParticle();
    ~GS_OGLParticle();

    BOOL Create(const char* pszPathname, int nNumParticles, BOOL bFiltered = TRUE,
                BOOL bMipmap = FALSE);
    BOOL Create(GLuint glTexture, int nWidth, int nHeight, int nNumParticles);
    void Destroy();

    BOOL BeginRender(HWND hWnd);
    void EndRender();
    BOOL Render(HWND hWnd = NULL);

    BOOL IsReady();

    BOOL IsActive(int nParticleIndex);
    void Activate(int nParticleIndex, BOOL bActivate = TRUE);
    void Activate(BOOL bActivate = TRUE);

    int  GetNumParticles();
    void SetNumParticles(int nNumParticles);

    GLint GetDestX(int nParticleIndex);
    void  SetDestX(int nParticleIndex, GLint gliDestX);
    void  SetDestX(GLint gliDestX);

    GLint GetDestY(int nParticleIndex);
    void  SetDestY(int nParticleIndex, GLint gliDestY);
    void  SetDestY(GLint gliDestY);

    GLfloat GetRotateX(int nParticleIndex);
    void SetRotateX(int nParticleIndex, GLfloat glfRotateX);
    void SetRotateX(GLfloat glfRotateX);

    GLfloat GetRotateY(int nParticleIndex);
    void SetRotateY(int nParticleIndex, GLfloat glfRotateY);
    void SetRotateY(GLfloat glfRotateY);

    GLfloat GetRotateZ(int nParticleIndex);
    void SetRotateZ(int nParticleIndex, GLfloat glfRotateZ);
    void SetRotateZ(GLfloat glfRotateZ);

    GLfloat GetScaleX(int nParticleIndex);
    void SetScaleX(int nParticleIndex, GLfloat glfScaleX);
    void SetScaleX(GLfloat glfScaleX);

    GLfloat GetScaleY(int nParticleIndex);
    void SetScaleY(int nParticleIndex, GLfloat glfScaleY);
    void SetScaleY(GLfloat glfScaleY);

    GLint GetWidth();
    GLint GetHeight();

    GLfloat GetScaledWidth(int nParticleIndex);
    void SetScaledWidth(int nParticleIndex, GLint gliWidth);
    void SetScaledWidth(GLint gliWidth);

    GLfloat GetScaledHeight(int nParticleIndex);
    void SetScaledHeight(int nParticleIndex, GLint gliHeight);
    void SetScaledHeight(GLint gliHeight);

    void GetDestRect(int nParticleIndex, long* pLeft,long* pTop,long* pRight,long* pBottom);
    void GetDestRect(int nParticleIndex, RECT* pDestRect);

    void GetModulateColor(int nParticleIndex, GS_OGLColor* pgsColor);
    void GetModulateColor(int nParticleIndex, float* pRed, float* pGreen, float* pBlue,
                          float* pAlpha);
    void SetModulateColor(int nParticleIndex, GS_OGLColor  gsColor);
    void SetModulateColor(GS_OGLColor  gsColor);
    void SetModulateColor(int nParticleIndex, float fRed = -1.0f, float fGreen = -1.0f,
                          float fBlue = -1.0f, float fAlpha = -1.0f);
    void SetModulateColor(float fRed = -1.0f, float fGreen = -1.0f, float fBlue = -1.0f,
                          float fAlpha = -1.0f);
    
    void AddDestX(int nParticleIndex, GLint gliAddX);
    void AddDestX(GLint gliAddX);
    void AddDestY(int nParticleIndex, GLint gliAddY);
    void AddDestY(GLint gliAddY);

    void AddRotateX(int nParticleIndex, GLfloat glfRotateX);
    void AddRotateX(GLfloat glfRotateX);
    void AddRotateY(int nParticleIndex, GLfloat glfRotateY);
    void AddRotateY(GLfloat glfRotateY);
    void AddRotateZ(int nParticleIndex, GLfloat glfRotateZ);
    void AddRotateZ(GLfloat glfRotateZ);

    void AddScaleX(int nParticleIndex, GLfloat glfScaleX);
    void AddScaleX(GLfloat glfScaleX);
    void AddScaleY(int nParticleIndex, GLfloat glfScaleY);
    void AddScaleY(GLfloat glfScaleY);
};


////////////////////////////////////////////////////////////////////////////////////////////////


#endif
