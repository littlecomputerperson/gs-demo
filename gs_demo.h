/*============================================================================================+
 | Game System (GS) Library                                                                   |
 |--------------------------------------------------------------------------------------------|
 | FILES: gs_demo.cpp, gs_demo.h                                                              |
 |--------------------------------------------------------------------------------------------|
 | CLASS: GS_Demo                                                                             |
 |--------------------------------------------------------------------------------------------|
 | ABOUT: ...                                                                                 |
 |--------------------------------------------------------------------------------------------|
 |                                                                                    08/2003 |
 +============================================================================================*/


#ifndef GS_DEMO_H
#define GS_DEMO_H


//================================================================================================
// Include Game System (GS) header files.
// -----------------------------------------------------------------------------------------------
#include "gs_main.h"
//================================================================================================


//================================================================================================
// Include standard C library header files.
// -----------------------------------------------------------------------------------------------
#include <time.h>
//================================================================================================


//==============================================================================================
// Demo defines.
// ---------------------------------------------------------------------------------------------
#define DEFAULT_RES_X 640
#define DEFAULT_RES_Y 480
#define INTERNAL_RES_X 640
#define INTERNAL_RES_Y 480
// ---------------------------------------------------------------------------------------------
#define MUSIC_BACKGROUND 0
// ---------------------------------------------------------------------------------------------
#define SAMPLE_IMPACT  0
#define SAMPLE_RESIZE  1
#define SAMPLE_COLLIDE 2
#define SAMPLE_OPTION  3
#define SAMPLE_SELECT  4
// ---------------------------------------------------------------------------------------------
#define MAXIMUM_SPRITES  5
#define MAXIMUM_VELOCITY 3
#define MAXIMUM_ROTATION 10
//==============================================================================================


////////////////////////////////////////////////////////////////////////////////////////////////


class GS_Demo : public GS_Application
{

private:

    GS_Keyboard m_gsKeyboard;  // Keyboard object.
    GS_Mouse    m_gsMouse;     // Mouse object.

    GS_OGLDisplay m_gsDisplay; // OpenGL display object.
    GS_FmodSound  m_gsSound;   // FMOD sound object.

    GS_OGLTexture  m_gsTexture;  // OpenGL texture object.
    GS_OGLSprite   m_gsBackgrnd; // OpenGL sprite object.
    GS_OGLSprite   m_gsSprite;   // OpenGL sprite object.

    GS_OGLFont     m_gsSmallFont; // OpenGL font object.
    GS_OGLFont     m_gsLargeFont; // OpenGL font object.
    GS_OGLSpriteEx m_gsSpriteEx;  // OpenGL sprite ex object.

    GS_OGLCollide   m_gsCollide;                   // Collision detection object.
    GS_OGLTexture   m_gsSpriteTexture;             // OpenGL texture object.
    GS_OGLSpriteEx  m_gsSprites[MAXIMUM_SPRITES];  // Array of sprite ex objects.

    GS_OGLMenu      m_gsMenu;           // OpenGL menu object.
    GS_OGLTexture   m_gsGroundTexture;  // OpenGL texture object.
    GS_OGLTexture   m_gsCloudsTexture;  // OpenGL texture object.

    GS_OGLMap      m_gsMap;          // OpenGL map object.
    GS_OGLSpriteEx m_gsPlayerSprite; // OpenGL sprite ex object.

    GS_OGLParticle m_gsParticle; // OpenGL particle object.

    BOOL    m_bMoveRight[MAXIMUM_SPRITES]; // Wether to move right or left.
    BOOL    m_bMoveDown[MAXIMUM_SPRITES];  // Wether to move Up or down.
    GLfloat m_fVelocity[MAXIMUM_SPRITES];  // Speed at which to move.
    GLfloat m_fRotation[MAXIMUM_SPRITES];  // Speed at which to rotate.

    RECT m_rcScreen;

    int m_nGameProgress;
    int m_nVolume;
    BOOL m_bIsFirstRun;

    BOOL m_bUseVSync;
    BOOL m_bUseAliasing;
    BOOL m_bUseLighting;
    BOOL m_bUseBlending;

protected:

    // Methods that override base class methods.
    BOOL GameInit();
    BOOL GameShutdown();
    BOOL GameRelease();
    BOOL GameRestore();
    BOOL GameLoop();
    void OnChangeMode();

public:

    // The constuctor and destructor.
    GS_Demo();
    ~GS_Demo();

    // Message procedure that override the base class message procedure.
    LRESULT MsgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

    // Game setup methods.
    BOOL GameSetup();

    // Demonstration methods.
    BOOL ClearDemo();
    BOOL PolyDemo();
    BOOL ColorDemo();
    BOOL RotateDemo();
    BOOL ShapesDemo();
    BOOL TextureDemo();
    BOOL SpriteDemo();
    BOOL FontDemo();
    BOOL CollisionDemo();
    BOOL MenuDemo();
    BOOL MapDemo();
    BOOL ParticleDemo();

    // Helper methods.
    void SetRenderScaling(int nWidth, int nHeight, bool bKeepAspect);
    float GetActionInterval(float fActionsPerSecond);
};

////////////////////////////////////////////////////////////////////////////////////////////////


#endif
