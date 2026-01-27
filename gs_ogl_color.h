/*============================================================================================+
 | Game System (GS) Library                                                                   |
 |--------------------------------------------------------------------------------------------|
 | FILES: gs_ogl_color.h                                                                      |
 |--------------------------------------------------------------------------------------------|
 | CLASS: none                                                                                |
 |--------------------------------------------------------------------------------------------|
 | ABOUT: ...                                                                                 |
 |--------------------------------------------------------------------------------------------|
 |                                                                                    08/2003 |
 +============================================================================================*/

#ifndef GS_OGL_COLOR_H
#define GS_OGL_COLOR_H


// Defines for converting a byte to a float (0.0-1.0) and vice versa.
#define ByteToFloat(val) (((float)(val))/256.0f)
#define FloatToByte(val) ((unsigned char)((val)*255.0f))


////////////////////////////////////////////////////////////////////////////////////////////////
// Color Structure. ////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////

typedef struct GS_OGLColor
{

    float fRed;
    float fGreen;
    float fBlue;
    float fAlpha;

    GS_OGLColor()
    {
        fRed = fGreen = fBlue = 0.0f;
        fAlpha = 1.0f;
    }

    GS_OGLColor(float r, float g, float b)
    {
        fRed   = r;
        fGreen = g;
        fBlue  = b;
        fAlpha = 1.0f;
    }

    GS_OGLColor(float r, float g, float b, float a)
    {
        fRed   = r;
        fGreen = g;
        fBlue  = b;
        fAlpha = a;
    }

    GS_OGLColor(const GS_OGLColor &c)
    {
        fRed   = c.fRed;
        fGreen = c.fGreen;
        fBlue  = c.fBlue;
        fAlpha = c.fAlpha;
    }

    GS_OGLColor &operator =(const GS_OGLColor &c)
    {
        fRed   = c.fRed;
        fGreen = c.fGreen;
        fBlue  = c.fBlue;
        fAlpha = c.fAlpha;
        return *this;
    }
} GS_OGLColor;

////////////////////////////////////////////////////////////////////////////////////////////////

#endif
