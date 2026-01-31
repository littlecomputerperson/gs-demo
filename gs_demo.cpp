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


//==============================================================================================
// Include header files.
// ---------------------------------------------------------------------------------------------
#include "gs_demo.h"
//==============================================================================================


//==============================================================================================
// Global variables.
// ---------------------------------------------------------------------------------------------
GS_Demo g_gsDemo;
//==============================================================================================


////////////////////////////////////////////////////////////////////////////////////////////////
// Constructor/Destructor Methods //////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_Demo::GS_Demo():
// ---------------------------------------------------------------------------------------------
// Purpose: The constructor, initializes class data to defaults when class object is created.
// ---------------------------------------------------------------------------------------------
// Returns: Nothing.
//==============================================================================================

GS_Demo::GS_Demo() : GS_Application()
{

    m_nGameProgress = 11;

    m_nVolume = 255;

    m_bIsFirstRun = TRUE;

    m_bUseVSync    = FALSE;
    m_bUseAliasing = FALSE;
    m_bUseLighting = FALSE;
    m_bUseBlending = TRUE;

    this->GameSetup();
}


////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_Demo::~GS_Demo():
// ---------------------------------------------------------------------------------------------
// Purpose: The de-structor, de-initializes class data when class object is destroyed.
// ---------------------------------------------------------------------------------------------
// Returns: Nothing.
//==============================================================================================

GS_Demo::~GS_Demo()
{


    // Does nothing at the moment.
}


////////////////////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////////////////////
// Setup Methods ///////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_Demo::GameSetup():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Returns: TRUE if successful, FALSE if not.
//==============================================================================================

BOOL GS_Demo::GameSetup()
{

    char szTempString[_MAX_PATH] = { 0 };
    GS_IniFile gsIniFile;

    // Determine the full pathname of the INI file.
    GetCurrentDirectory(_MAX_PATH, szTempString);
    lstrcat(szTempString, "\\");
    lstrcat(szTempString, "settings.ini");

    // Open the INI file.
    gsIniFile.Open(szTempString);

    // Read all the game display settings from the ini file (-1 if failed).
    int  nWidth    = gsIniFile.ReadInt("Display", "DisplayWidth",  -1);
    int  nHeight   = gsIniFile.ReadInt("Display", "DisplayHeight", -1);
    int  nDepth    = gsIniFile.ReadInt("Display", "ColorDepth",    -1);
    BOOL bWindowed = gsIniFile.ReadInt("Display", "WindowMode",     1);

    // Were all the settings read valid?
    if ((nWidth>0) && (nHeight>0) && (nDepth>0))
    {
        // Use the ini display values.
        this->SetMode(nWidth, nHeight, nDepth, bWindowed);
    }
    else
    {
        // Display a message allowing the user to choose full screen or windowed mode.
        if (MessageBox(NULL, "Would you like to run in fullscreen mode?", "PLEASE SELECT",
                       MB_YESNO | MB_ICONQUESTION) == IDYES)
        {
            // Use default display values in fullscreen mode.
            this->SetMode(DEFAULT_RES_X, DEFAULT_RES_Y, this->GetColorDepth(), FALSE);
        }
        else
        {
            // Use default display values in windowed mode.
            this->SetMode(DEFAULT_RES_X, DEFAULT_RES_Y, 32, TRUE);
        }
    }

    // Set render scaling values
    this->SetRenderScaling(this->GetWidth(), this->GetHeight(), true);

    // Close the ini file.
    gsIniFile.Close();

    return TRUE;
}


////////////////////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////////////////////
// Initialize/Shutdown Methods /////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_Demo::GameInit():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Returns: TRUE if successful, FALSE if not.
//==============================================================================================

BOOL GS_Demo::GameInit()
{

    // Seed the timer for random numbers.
    time_t t;
    srand((unsigned) time(&t));

    // Set the text in the window title bar.
    this->SetTitle("GameSystem OpenGL v1.48b Demo");

    // Create the OpenGL display.
    if (!m_gsDisplay.Create(this->GetDevice(), this->GetWidth(), this->GetHeight(), this->GetColorDepth()))
    {
        GS_Error::Report("GS_DEMO.CPP", 162, "Failed to create OpenGL display!");
        return FALSE;
    }

    // Are we in fullscreen mode?
    // if (!this->IsWindowed())
    // {
        // Hide the mouse cursor.
        m_gsMouse.HideCursor(TRUE);
    // }

    // Set up a rectangle the size of the screen for rendering tiles.
    SetRect(&m_rcScreen, 0, INTERNAL_RES_Y, INTERNAL_RES_X, 0);

    /////////////////////////////////////////////////////////////////////////////////////////////
    // Initialize Controller Input //////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////

    // Initialize controller input.
    // (Controller auto-initializes in constructor, just log if connected)
    if (m_gsController.GetConnectedCount() > 0)
    {
        char szDebugMsg[256];
        sprintf(szDebugMsg, "Controller connected: %s\n", m_gsController.GetControllerName(0));
        GS_Platform::OutputDebugString(szDebugMsg);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////
    // OpenGL Initialization Code ///////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////

    // Setup anti-aliasing, vertical sync and blending for transaprency.
    m_gsDisplay.EnableAliasing(TRUE);
    m_gsDisplay.EnableVSync(FALSE);
    m_gsDisplay.EnableBlending(TRUE);

    // Setup a light for lighting effects.
    m_gsDisplay.SetLightAmbient( 0.5f, 0.5f, 0.5f, 1.0f);
    m_gsDisplay.SetLightDiffuse( 1.0f, 1.0f, 1.0f, 1.0f);
    m_gsDisplay.SetLightPosition(0.0f, 0.0f, 2.0f, 1.0f);

    /////////////////////////////////////////////////////////////////////////////////////////////

    // Create an OpenGL texture.
    m_gsTexture.Create("data/crate.tga");

    // Create an unfiltered OpenGL background sprite.
    m_gsBackgrnd.Create("data/backgrnd.tga", FALSE);
    // Create an unfiltered OpenGL sprite.
    m_gsSprite.Create("data/portrait.tga", FALSE);

    // Create an unfiltered sprite with 25 frames, 5 frames per line and 96x96 size.
    m_gsSpriteEx.Create("data/asteroid_l.tga", 25, 5, 96, 96, FALSE);
    // Create an OpenGL font with 16 characters per line and 16x16 size.
    m_gsSmallFont.Create("data/font_s.tga", 16, 16, 16);
    // Create an OpenGL font with 10 characters per line and 32x32 size.
    m_gsLargeFont.Create("data/font_l.tga", 10, 32, 32);

    // Create an unfiltered OpenGL mipmap texture for the asteroid sprites.
    m_gsSpriteTexture.Create("data/asteroid_s.tga", GS_MIPMAP, GL_NEAREST, GL_NEAREST);
    // Create an arry of sprites using the OpenGL texture.
    for (int nLoop=0; nLoop<MAXIMUM_SPRITES; nLoop++)
    {
        // Create a sprite with 24 frames, 8 frames per line and a 64x64 frame size from an
        // OpenGL texture with a width 0f 512 and height of 256.
        m_gsSprites[nLoop].Create(m_gsSpriteTexture.GetID(), 512, 256, 24, 8, 64, 64);
        // Set random starting coordinates within the screen area.
        m_gsSprites[nLoop].SetDestX(rand()%(int)(INTERNAL_RES_X - m_gsSprites[nLoop].GetFrameWidth()));
        m_gsSprites[nLoop].SetDestY(rand()%(int)(INTERNAL_RES_Y - m_gsSprites[nLoop].GetFrameHeight()));
        // Determine random directions to move in.
        m_bMoveRight[nLoop] = rand()%1;
        m_bMoveDown[nLoop]  = rand()%1;
        // Set a random velocity for the sprite to move at.
        m_fVelocity[nLoop]  = (rand()%MAXIMUM_VELOCITY) + 1.0f;
        // Determine a random speed for the sprite to rotate at.
        m_fRotation[nLoop]  = (rand()%MAXIMUM_ROTATION) + 1.0f;
    }

    // Create textures for the ground and clouds.
    m_gsGroundTexture.Create("data/ground.tga");
    m_gsCloudsTexture.Create("data/clouds.tga");
    // Create a menu using the "menu.tga" and "menu_font.tga" images that has 16 characters per
    // line with dimensions of 16x16 per letter/frame.
    m_gsMenu.Create("data/menu.tga","data/menu_font.tga", 16, 16, 16);

    // Create a map using the "map.tga" image that has 64 frames with 8 frames per line and
    // a frame size of 32x32 pixels.
    m_gsMap.Create("data/map.tga", 64, 8, 32, 32);
    // Load the map with 15 rows and 20 columns.
    m_gsMap.LoadMap("data/map.txt", 15, 20);
    // Set the clip box for the map.
    m_gsMap.SetClipBox(INTERNAL_RES_X / 4, INTERNAL_RES_Y / 4,
                       INTERNAL_RES_X - (INTERNAL_RES_X / 4),
                       INTERNAL_RES_Y - (INTERNAL_RES_Y / 4));
    // Set initial map position.
    m_gsMap.SetMapX(0);
    m_gsMap.SetMapY(0);
    // Wrap map scrolling horizontally and vertically.
    m_gsMap.SetWrapX(TRUE);
    m_gsMap.SetWrapY(TRUE);
    // Do not render tile eight.
    m_gsMap.SetClearTileID(8);
    // Create sprite with 4 frames, 4 frames per line and 32x32 size.
    m_gsPlayerSprite.Create("data/player.tga", 4, 4, 32, 32);
    // Set the sprite position one tile up and right from the bottom left corner of the clip box.
    m_gsPlayerSprite.SetDestX(m_gsMap.GetClipBoxLeft()   +  m_gsMap.GetTileWidth());
    m_gsPlayerSprite.SetDestY(m_gsMap.GetClipBoxBottom() + m_gsMap.GetTileHeight());

    // Create 100 particles using the "particle.tga" image.
    m_gsParticle.Create("data/particle.tga", 100);

    // Setup initial starting coordinates for all the particles.
    m_gsParticle.SetDestX(INTERNAL_RES_X / 2);
    m_gsParticle.SetDestY(INTERNAL_RES_Y / 2);

    // Set menu title.
    m_gsMenu.SetTitle("  MAIN MENU  ");

    // Add menu options.
    m_gsMenu.AddOption("Disable Title");
    m_gsMenu.AddOption("Disable Sound");
    m_gsMenu.AddOption("Quit         ");

    // Highlight the first option.
    m_gsMenu.SetHighlight(0);

    // Were we able to initialize the sound system?
    if (!m_gsSound.Initialize(GetWindow()))
    {
        GS_Error::Report("GS_DEMO.CPP", 275, "Failed to initialize FMOD sound!");
        m_gsDisplay.Destroy();
        return FALSE;
    }

    // Add music to list.
    m_gsSound.AddMusic("data/music.s3m");

    // Add samples to list.
    m_gsSound.AddSample("data/sample1.wav");
    m_gsSound.AddSample("data/sample2.wav");
    m_gsSound.AddSample("data/sample3.wav");
    m_gsSound.AddSample("data/sample4.wav");
    m_gsSound.AddSample("data/sample5.wav");

    // Start playing music at half the volume, half the speed and looping infinitely.
    m_gsSound.PlayMusic(MUSIC_BACKGROUND, 127, 0.5f, TRUE);

    return TRUE;
}

////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_Demo::GameShutdown():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Returns: TRUE if successful, FALSE if not.
//==============================================================================================

BOOL GS_Demo::GameShutdown()
{

    // Reset keyboard data.
    m_gsKeyboard.Reset();

    // Reset mouse data.
    m_gsMouse.Reset();

    // Reset controller data.
    m_gsController.Reset();

    // Clear all menu items.
    m_gsMenu.ClearOptions();

    // Destroy the OpenGL display.
    m_gsDisplay.Destroy();

    // Shutdown the sound system.
    m_gsSound.Shutdown();

    return TRUE;
}


////////////////////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////////////////////
// Release/Restore Methods /////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_Demo::GameRelease():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Returns: TRUE if successful, FALSE if not.
//==============================================================================================

BOOL GS_Demo::GameRelease()
{

    if (!GS_Application::IsActive())
    {
        return TRUE;
    }

    // Set OpenGL clear color to black.
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    // Clear screen and depth buffer.
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    // Reset the current modelview matrix.
    glLoadIdentity();
    // Swap buffers (double buffering) to display results.
    SwapBuffers(this->GetDevice());

    // Pause all sound.
    m_gsSound.Pause(TRUE);

    return TRUE;
}


////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_Demo::GameRestore():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Returns: TRUE if successful, FALSE if not.
//==============================================================================================

BOOL GS_Demo::GameRestore()
{

    if (GS_Application::IsActive())
    {
        return TRUE;
    }

    // Unpause all sound.
    m_gsSound.Pause(FALSE);

    return TRUE;
}


////////////////////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////////////////////
// Main Loop ///////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_Demo::GameLoop():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Returns: TRUE if successful, FALSE if not.
//==============================================================================================

BOOL GS_Demo::GameLoop()
{
    // Handle buffered keyboard input
    static BOOL bWasKeyReleased = TRUE;

    int KeyList[3] = { GSK_SPACE, GSK_L, GSK_B };

    // Were all the keys in the key list released?
    if (TRUE == m_gsKeyboard.AreKeysUp(3, KeyList))
    {
        // Set flag to indicate that all the keys were released.
        bWasKeyReleased = TRUE;
    }

    // Get key from buffer (if any).
    int nKey = m_gsKeyboard.GetBufferedKey();

    // Act depending on key pressed.
    switch (nKey)
    {
    // Was the SPACE key pressed?
    case GSK_SPACE:
        // Was this key released?
        if (bWasKeyReleased)
        {
            // Run the next demonstration.
            m_nGameProgress++;
            // Restart if the last demo.
            if (m_nGameProgress > 11)
            {
                m_nGameProgress = 0;
            }
            // Key is pressed.
            bWasKeyReleased = FALSE;
        }
        break;
    // Was the '1' key pressed?
    case GSK_1:
        m_gsSound.PlaySample(SAMPLE_IMPACT);
        break;
    // Was the '2' key pressed?
    case GSK_2:
        m_gsSound.PlaySample(SAMPLE_RESIZE);
        break;
    // Was the '3' key pressed?
    case GSK_3:
        m_gsSound.PlaySample(SAMPLE_COLLIDE);
        break;
    // Was the '4' key pressed?
    case GSK_4:
        m_gsSound.PlaySample(SAMPLE_OPTION);
        break;
    // Was the '5' key pressed?
    case GSK_5:
        m_gsSound.PlaySample(SAMPLE_SELECT);
        break;
    // Was the 'F1' key pressed?
    case GSK_F1:
        SetMode(320, 240, GetColorDepth(), IsWindowed());
        this->SetRenderScaling(this->GetWidth(), this->GetHeight(), true);
        break;
    // Was the 'F2' key pressed?
    case GSK_F2:
        SetMode(400, 300, GetColorDepth(), IsWindowed());
        this->SetRenderScaling(this->GetWidth(), this->GetHeight(), true);
        break;
    // Was the 'F3' key pressed?
    case GSK_F3:
        SetMode(640, 480, GetColorDepth(), IsWindowed());
        this->SetRenderScaling(this->GetWidth(), this->GetHeight(), true);
        break;
    // Was the 'F4' key pressed?
    case GSK_F4:
        SetMode(800, 600, GetColorDepth(), IsWindowed());
        this->SetRenderScaling(this->GetWidth(), this->GetHeight(), true);
        break;
    // Was the 'F5' key pressed?
    case GSK_F5:
        SetMode(1024, 768, GetColorDepth(), IsWindowed());
        this->SetRenderScaling(this->GetWidth(), this->GetHeight(), true);
        break;
    // Was the '+' key pressed?
    case GSK_ADD:
    case GSC_AXIS_TRIGGERRIGHT:
        // Increase sound master volume.
        if (m_nVolume<255)
        {
            m_nVolume+=5;
        }
        m_gsSound.SetMasterVolume(m_nVolume);
        break;
    // Was the '-' key pressed?
    case GSK_SUBTRACT:
    case GSC_AXIS_TRIGGERLEFT:
        // Decrease sound master volume.
        if (m_nVolume>0)
        {
            m_nVolume-=5;
        }
        m_gsSound.SetMasterVolume(m_nVolume);
        break;
    } // end switch(nButton)

    // Handle buffered controller input
    static BOOL bWasButtonReleased = TRUE;

    int ButtonList[3] = { GSC_BUTTON_GUIDE, GSC_BUTTON_X, GSC_BUTTON_Y };

    // Were all the buttons in the button list released?
    if (TRUE == m_gsController.AreButtonsUp(3, ButtonList))
    {
        // Set flag to indicate that all the keys were released.
        bWasButtonReleased = TRUE;
    }

    // Get button from buffer (if any).
    int nButton = m_gsController.GetBufferedButton();

    // Act depending on key pressed.
    switch (nButton)
    {
    // Was the SPACE key pressed?
    case GSC_BUTTON_BACK:
        // Was this key released?
        if (bWasButtonReleased)
        {
            // Run the next demonstration.
            m_nGameProgress++;
            // Restart if the last demo.
            if (m_nGameProgress > 11)
            {
                m_nGameProgress = 0;
            }
            // Key is pressed.
            bWasButtonReleased = FALSE;
        }
        break;
    } // end switch(nButton)

    // Setup particles for particle demo.
    if (m_nGameProgress != 11)
    {
        m_bIsFirstRun = TRUE;
    }

    // Run demo depending on progress.
    switch (m_nGameProgress)
    {
    case 0:
        ClearDemo();
        break;
    case 1:
        PolyDemo();
        break;
    case 2:
        ColorDemo();
        break;
    case 3:
        RotateDemo();
        break;
    case 4:
        ShapesDemo();
        break;
    case 5:
        TextureDemo();
        break;
    case 6:
        SpriteDemo();
        break;
    case 7:
        FontDemo();
        break;
    case 8:
        CollisionDemo();
        break;
    case 9:
        MenuDemo();
        break;
    case 10:
        MapDemo();
        break;
    case 11:
        ParticleDemo();
        break;
    default:
        // ...
        break;

    } // end switch

    return TRUE;
}


////////////////////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////////////////////
// Message Handling ////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_Demo::OnChangeMode():
// ---------------------------------------------------------------------------------------------
// Purpose: Overrides the OnChangeMode() function of the base class
// ---------------------------------------------------------------------------------------------
// Returns: Nothing
//==============================================================================================

void GS_Demo::OnChangeMode()
{
    // Change to windowed or fullscreen mode as selected by the user
    this->SetMode(this->GetWidth(), this->GetHeight(), this->GetColorDepth(), !this->IsWindowed());

    // Set values for scaling
    this->SetRenderScaling(this->GetWidth(), this->GetHeight(), true);
}


////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_Demo::MsgProc():
// ---------------------------------------------------------------------------------------------
// Purpose: Overrrides the main WndProc() and MsgProc() of the base class, so the application
//          can do custom message handling (e.g. processing mouse, keyboard, or menu commands).
// ---------------------------------------------------------------------------------------------
// Returns: LRESULT depending on the message processed.
//==============================================================================================

LRESULT GS_Demo::MsgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{

    switch (uMsg)
    {

    // Received when the user selects a command item from a menu, when a control sends a
    // notification message to its parent window, or when an accelerator keystroke is
    // translated.
    case WM_COMMAND:
        // Check for message sent by accelerator.
        switch(LOWORD(wParam))
        {
        // Demo options.
        case IDM_NEXTDEMO:
            // Run the next demonstration.
            m_nGameProgress++;
            // Restart if the last demo.
            if (m_nGameProgress > 11)
            {
                m_nGameProgress = 0;
            }
            return 0L;
        case IDM_TURBO:
            // Is the frame rate greater than 0?
            if (this->GetFrameRate() > 0.0f)
            {
                SetFrameRate(0.0f);
            }
            else
            {
                this->SetFrameRate(60.0f);
            }
            return 0L;
        // Display options.
        case IDM_BLENDING:
            // Are we using alpha blending?
            if (m_gsDisplay.IsBlendingEnabled())
            {
                // Disable blending.
                m_gsDisplay.EnableBlending(FALSE);
            }
            else
            {
                // Enable blending.
                m_gsDisplay.EnableBlending(TRUE);
            }
            return 0L;
        case IDM_LIGHTING :
            // Are we using lighting?
            if (m_gsDisplay.IsLightingEnabled())
            {
                // Disable lighting.
                m_gsDisplay.EnableLighting(FALSE);
            }
            else
            {
                // Enable lighting.
                m_gsDisplay.EnableLighting(TRUE);
            }
            return 0L;
        case IDM_VSYNC:
            // Are we using vertical sync?
            if (m_gsDisplay.IsVSyncEnabled())
            {
                // Disable vertical sync.
                m_gsDisplay.EnableVSync(FALSE);
            }
            else
            {
                // Enable vertical sync.
                m_gsDisplay.EnableVSync(TRUE);
            }
            return 0L;
        case IDM_320X240:
            SetMode(320, 240,  this->GetColorDepth(), this->IsWindowed());
            this->SetRenderScaling(this->GetWidth(), this->GetHeight(), true);
            return 0L;
        case IDM_400X300:
            SetMode(400, 300,  this->GetColorDepth(), this->IsWindowed());
            this->SetRenderScaling(this->GetWidth(), this->GetHeight(), true);
            return 0L;
        case IDM_640X480:
            SetMode(640, 480,  this->GetColorDepth(), this->IsWindowed());
            this->SetRenderScaling(this->GetWidth(), this->GetHeight(), true);
            return 0L;
        case IDM_800X600:
            SetMode(800, 600,  this->GetColorDepth(), this->IsWindowed());
            this->SetRenderScaling(this->GetWidth(), this->GetHeight(), true);
            return 0L;
        case IDM_1024X768:
            SetMode(1024, 768, this->GetColorDepth(), this->IsWindowed());
            this->SetRenderScaling(this->GetWidth(), this->GetHeight(), true);
            return 0L;
        // Sound options.
        case IDM_VOL_PLUS:
            // Increase sound master volume.
            m_nVolume += 50;
            if (m_nVolume > 255)
            {
                m_nVolume = 255;
            }
            m_gsSound.SetMasterVolume(m_nVolume);
            return 0L;
        case IDM_VOL_MINUS:
            // Decrease sound master volume.
            m_nVolume -= 50;
            if (m_nVolume < 0)
            {
                m_nVolume = 0;
            }
            m_gsSound.SetMasterVolume(m_nVolume);
            return 0L;
        case IDM_SAMPLE_1:
            m_gsSound.PlaySample(SAMPLE_IMPACT);
            break;
        case IDM_SAMPLE_2:
            m_gsSound.PlaySample(SAMPLE_RESIZE);
            break;
        case IDM_SAMPLE_3:
            m_gsSound.PlaySample(SAMPLE_COLLIDE);
            break;
        case IDM_SAMPLE_4:
            m_gsSound.PlaySample(SAMPLE_OPTION);
            break;
        case IDM_SAMPLE_5:
            m_gsSound.PlaySample(SAMPLE_SELECT);
            break;
        }
        break;

    // Received when a WM_KEYDOWN message is translated by the TranslateMessage() function.
    // WM_CHAR contains the character code of the key that was pressed.
    case WM_CHAR:
        // wParam contains information on which key was pressed.
        switch (wParam)
        {
        // User pressed 'p' or 'P'.
        case 'P':
        case 'p':
            // Pause application if unpause, unpause if paused.
            if (this->IsPaused()) this->Pause(FALSE);
            else this->Pause(TRUE);
            break;
        // User pressed 'b' or 'B'.
        case 'B':
        case 'b':
            // Are we using alpha blending?
            if (m_gsDisplay.IsBlendingEnabled())
            {
                // Disable blending.
                m_gsDisplay.EnableBlending(FALSE);
            }
            else
            {
                // Enable blending.
                m_gsDisplay.EnableBlending(TRUE);
            }
            break;
        // User pressed 'l' or 'L'.
        case 'L':
        case 'l':
            // Are we using lighting?
            if (m_gsDisplay.IsLightingEnabled())
            {
                // Disable lighting.
                m_gsDisplay.EnableLighting(FALSE);
            }
            else
            {
                // Enable lighting.
                m_gsDisplay.EnableLighting(TRUE);
            }
            break;
        // User pressed 't' or 'T'.
        case 'T':
        case 't':
            // Is the frame rate greater than 0?
            if (this->GetFrameRate() > 0.0f)
            {
                SetFrameRate(0.0f);
            }
            else
            {
                this->SetFrameRate(60.0f);
            }
            break;
        // User pressed 'v' or 'V'.
        case 'V':
        case 'v':
            // Are we using vertical sync?
            if (m_gsDisplay.IsVSyncEnabled())
            {
                // Disable vertical sync.
                m_gsDisplay.EnableVSync(FALSE);
            }
            else
            {
                // Enable vertical sync.
                m_gsDisplay.EnableVSync(TRUE);
            }
            break;
        // User pressed ESC.
        case 27:
            // Post a quit message in the message queue.
            PostQuitMessage(0);
            break;
        // Default processing.
        default:
            break;
        } // end switch (wParam...
        break;

    // Received when a nonsystem key is pressed. A nonsystem key is a key that is pressed
    // when the ALT key is not pressed.
    case WM_KEYDOWN:
        // Mark key being held down.
        m_gsKeyboard.KeyDown(wParam);
        // Add key to keyboard buffer.
        m_gsKeyboard.AddKeyToBuffer(wParam);

        // Also add to controller buffer if it's a controller button
        if (wParam >= GSC_BUTTON_A && wParam <= GSC_BUTTON_DPAD_RIGHT)
        {
            m_gsController.SetButtonDown(wParam);
            m_gsController.AddButtonToBuffer(wParam);
        }
        break;

    // Received when a nonsystem key is released. A nonsystem key is a key that is pressed
    // when the ALT key is not pressed.
    case WM_KEYUP:
        // Mark key that was released.
        m_gsKeyboard.KeyUp(wParam);

        // Also add to controller buffer if it's a controller button
        if (wParam >= GSC_BUTTON_A && wParam <= GSC_BUTTON_DPAD_RIGHT)
        {
            m_gsController.SetButtonUp(wParam);
        }
        break;

    // Received when the user holds down the ALT key and then presses another key.
    case WM_SYSKEYDOWN:
        // Mark key being held down.
        m_gsKeyboard.KeyDown(wParam);
        // Add key to keyboard buffer.
        m_gsKeyboard.AddKeyToBuffer(wParam);
        return 0L;

    // Received when the user releases a key that was pressed while the ALT key held down.
    case WM_SYSKEYUP:
        // Mark key that was released.
        m_gsKeyboard.KeyUp(wParam);
        return 0L;

    // Received when the cursor moves within a window and mouse input is not captured.
    case WM_SETCURSOR:
        // Are we in windowed mode?
        if (this->IsWindowed())
        {
            // Is the mouse cursor in the window client?
            if (m_gsMouse.IsInClient(hWnd))
            {
                // Hide the cursor.
                SetCursor(NULL);
                return TRUE;
            }
        }
        break;

    // The WM_MOUSEMOVE message is posted to a window when the cursor moves.
    case WM_MOUSEMOVE:
        // Save mouse coordinates (coordinates are relative to the top left of the client area
        // of the window). Note that the Y coordinates are inverted for OpenGL.
        m_gsMouse.SetX(LOWORD(lParam));
        m_gsMouse.SetY(this->GetHeight() - HIWORD(lParam) - 1);
        return 0L;

    // The WM_LBUTTONDOWN message is posted when the user presses the left mouse button while
    // the cursor is in the client area of a window.
    case WM_LBUTTONDOWN:
        // Set left mouse button down.
        m_gsMouse.LeftPressed(TRUE);
        return 0L;

    // The WM_MBUTTONDOWN message is posted when the user presses the middle mouse button
    // while the cursor is in the client area of a window.
    case WM_MBUTTONDOWN:
        // Set middle mouse button down.
        m_gsMouse.MiddlePressed(TRUE);
        return 0L;

    // The WM_RBUTTONDOWN message is posted when the user presses the right mouse button while
    // the cursor is in the client area of a window.
    case WM_RBUTTONDOWN:
        // Set right mouse button down.
        m_gsMouse.RightPressed(TRUE);
        return 0L;

    // The WM_LBUTTONUP message is posted when the user releases the left mouse button while
    // the cursor is in the client area of a window.
    case WM_LBUTTONUP:
        // Set left mouse button up.
        m_gsMouse.LeftPressed(FALSE);
        return 0L;

    // The WM_MBUTTONUP message is posted when the user releases the middle mouse button while
    // the cursor is in the client area of a window.
    case WM_MBUTTONUP:
        // Set middle mouse button up.
        m_gsMouse.MiddlePressed(FALSE);
        return 0L;

    // The WM_RBUTTONUP message is posted when the user releases the right mouse button while
    // the cursor is in the client area of a window.
    case WM_RBUTTONUP:
        // Set right mouse button up.
        m_gsMouse.RightPressed(FALSE);
        return 0L;
    } // end switch (uMsg...

    // Provide default processing by the base class.
    return GS_Application::MsgProc(hWnd, uMsg, wParam, lParam);
}


////////////////////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////////////////////
// Demonstartion Methods ///////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_Demo::ClearDemo():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Returns: TRUE if successfull and FALSE if not.
//==============================================================================================

BOOL GS_Demo::ClearDemo()
{

    // Make sure we have a valid display before drawing.
    if (!m_gsDisplay.IsReady())
    {
        return FALSE;
    }

    // Is the game paused?
    if (IsPaused())
    {
        // Set OpenGL clear color to red.
        glClearColor(1.0f, 0.0f, 0.0f, 0.0f);
        // Clear screen and depth buffer.
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        // Reset the current modelview matrix.
        glLoadIdentity();
        // Swap buffers (double buffering) to display results.
        SwapBuffers(this->GetDevice());
        // Exit the function.
        return TRUE;
    }

    // The following line sets the color of the screen when it clears to blue. If you don't know
    // how colors work, I'll quickly explain. The color values range from 0.0f to 1.0f. 0.0f
    // being the darkest and 1.0f being the brightest. The first parameter after glClearColor is
    // the Red Intensity, the second parameter is for Green and the third is for Blue. The higher
    // the number is to 1.0f, the brighter that specific color will be. The last number is an
    // Alpha value. When it comes to clearing the screen, we wont worry about the 4th number. For
    // now leave it at 0.0f. I will explain its use in another tutorial.

    // You create different colors by mixing the three primary colors for light (red, green,
    // blue). Hope you learned primaries in school. So, if you had glClearColor(0.0f,0.0f,1.0f,
    // 0.0f) you would be clearing the screen to a bright blue. If you had glClearColor(0.5f,
    // 0.0f,0.0f,0.0f) you would be clearing the screen to a medium red. Not bright (1.0f) and
    // not dark (0.0f). To make a white background, you would set all the colors as high as
    // possible (1.0f). To make a black background you would set all the colors to as low as
    // possible (0.0f).
    glClearColor(0.0f, 0.0f, 1.0f, 0.0f);

    // Clear screen and depth buffer.
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Reset the current modelview matrix. When you do a glLoadIdentity() what you are doing is
    // moving back to the center of the screen with the X axis running left to right, the Y axis
    // moving up and down, and the Z axis moving into, and out of the screen.

    // The center of an OpenGL screen is 0.0f on the X and Y axis. To the left of center would be
    // a negative number. To the right would be a positive number. Moving towards the top of the
    // screen would be a positive number, moving to the bottom of the screen would be a negative
    // number. Moving deeper into the screen is a negative number, moving towards the viewer
    // would be a positive number.
    glLoadIdentity();

    // Disable texturing for this demo (we're drawing colored shapes)
    glDisable(GL_TEXTURE_2D);
    
    // Swap buffers (double buffering) to display results.
    SwapBuffers(this->GetDevice());

    return TRUE;
}


////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_Demo::PolyDemo():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Returns: TRUE if successfull and FALSE if not.
//==============================================================================================

BOOL GS_Demo::PolyDemo()
{

    // Make sure we have a valid display before drawing.
    if (!m_gsDisplay.IsReady())
    {
        return FALSE;
    }

    // Is the game paused?
    if (IsPaused())
    {
        // Set OpenGL clear color to red.
        glClearColor(1.0f, 0.0f, 0.0f, 1.0f);
        // Clear screen and depth buffer.
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        // Reset the current modelview matrix.
        glLoadIdentity();
        // Swap buffers (double buffering) to display results.
        SwapBuffers(this->GetDevice());
        // Exit the function.
        return TRUE;
    }

    // Set OpenGL clear color to black.
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    // Clear screen and depth buffer.
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Reset the current modelview matrix.
    glLoadIdentity();

    // Disable texturing for this demo (we're drawing colored shapes)
    glDisable(GL_TEXTURE_2D);
    
    /////////////////////////////////////////////////////////////////////////////////////////////
    // New Code /////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////

    // The next line of code will be the first time we use the command glColor3f(r,g,b). The
    // three parameters in the brackets are red, green and blue intensity values. The values can
    // be from 0.0f to 1.0f. It works the same way as the color values we use to clear the
    // background of the screen.
    glColor3f(1.0f,1.0f,1.0f);

    // glTranslatef(x, y, z) moves along the X, Y and Z axis, in that order. The line of code
    // below moves left on the X axis 1.5 units. It does not move on the Y axis at all (0.0),
    // and it moves into the screen 6.0 units. When you translate, you are not moving a set
    // amount from the center of the screen, you are moving a set amount from wherever you
    // currently were on the screen.
    glTranslatef(-1.5f,0.0f,-6.0f);

    // Now that we have moved to the left half of the screen, and we've set the view deep enough
    // into the screen (-6.0) that we can see our entire scene we will create the Triangle.
    // glBegin(GL_TRIANGLES) means we want to start drawing a triangle, and glEnd() tells OpenGL
    // we are done creating the triangle. Typically if you want 3 points, use GL_TRIANGLES.
    // Drawing triangles is fairly fast on most video cards. If you want 4 points use GL_QUADS
    // to make life easier. From what I've heard, most video cards render objects as triangles
    // anyways. Finally if you want more than 4 points, use GL_POLYGON.

    // In our simple program, we draw just one triangle. If we wanted to draw a second triangle,
    // we could include another 3 lines of code (3 points) right after the first three. All six
    // lines of code would be between glBegin(GL_TRIANGLES) and glEnd(). There's no point in
    // putting a glBegin(GL_TRIANGLES) and a glEnd() around every group of 3 points. This applies
    // to quads as well. If you know you're drawing all quads, you can include the second group
    // of four lines of code right after the first four lines. A polygon on the other hand
    // (GL_POLYGON) can be made up of any amount of point so it doesn't matter how many lines you
    // have between glBegin(GL_POLYGON) and glEnd().

    // The first line after glBegin, sets the first point of our polygon. The first number of
    // glVertex is for the X axis, the second number is for the Y axis, and the third number is
    // for the Z axis. So in the first line, we don't move on the X axis. We move up one unit on
    // the Y axis, and we don't move on the Z axis. This gives us the top point of the triangle.
    // The second glVertex moves left one unit on the X axis and down one unit on the Y axis.
    // This gives us the bottom left point of the triangle. The third glVertex moves right one
    // unit, and down one unit. This gives us the bottom right point of the triangle. glEnd()
    // tells OpenGL there are no more points. The filled triangle will be displayed.
    glBegin(GL_TRIANGLES);
    glVertex3f( 0.0f, 1.0f, 0.0f); // Top of triangle.
    glVertex3f(-1.0f,-1.0f, 0.0f); // Bottom left of triangle.
    glVertex3f( 1.0f,-1.0f, 0.0f); // Bottom right of triangle.
    // Finished drawing the triangle.
    glEnd();

    // Move right 3 units from current position.
    glTranslatef(3.0f,0.0f,0.0f);

    // Now we create the square. We'll do this using GL_QUADS. A quad is basically a 4 sided
    // polygon. Perfect for making a square. The code for creating a square is very similar to
    // the code we used to create a triangle. The only difference is the use of GL_QUADS instead
    // of GL_TRIANGLES, and an extra glVertex3f for the 4th point of the square. We'll draw the
    // square top left, top right, bottom right, bottom left (clockwise). By drawing in a
    // clockwise order, the square will be drawn as a back face. Meaning the side of the quad we
    // see is actually the back. Objects drawn in a counter clockwise order will be facing us.
    // Not important at the moment, but later on you will need to know this.
    glBegin(GL_QUADS);
    glVertex3f(-1.0f, 1.0f, 0.0f); // Top left of quad.
    glVertex3f( 1.0f, 1.0f, 0.0f); // Top right of quad.
    glVertex3f( 1.0f,-1.0f, 0.0f); // Bottom right of quad.
    glVertex3f(-1.0f,-1.0f, 0.0f); // Bottom left of quad.
    // Finished drawing the quad.
    glEnd();

    /////////////////////////////////////////////////////////////////////////////////////////////

    // Swap buffers (double buffering) to display results.
    SwapBuffers(this->GetDevice());

    return TRUE;
}


////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_Demo::ColorDemo():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Returns: TRUE if successfull and FALSE if not.
//==============================================================================================

BOOL GS_Demo::ColorDemo()
{

    // Make sure we have a valid display before drawing.
    if (!m_gsDisplay.IsReady())
    {
        return FALSE;
    }

    // Is the game paused?
    if (IsPaused())
    {
        // Set OpenGL clear color to red.
        glClearColor(1.0f, 0.0f, 0.0f, 1.0f);
        // Clear screen and depth buffer.
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        // Reset the current modelview matrix.
        glLoadIdentity();
        // Swap buffers (double buffering) to display results.
        SwapBuffers(this->GetDevice());
        // Exit the function.
        return TRUE;
    }

    // Set OpenGL clear color to black.
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    // Clear screen and depth buffer.
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Reset the current modelview matrix.
    glLoadIdentity();

    // Disable texturing for this demo (we're drawing colored shapes)
    glDisable(GL_TEXTURE_2D);
        
    // Move left 1.5 units and into the screen 6.0.
    glTranslatef(-1.5f,0.0f,-6.0f);

    /////////////////////////////////////////////////////////////////////////////////////////////
    // New Code /////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////

    // Beigin drawing using triangles.
    glBegin(GL_TRIANGLES);
    // We are setting the color to red (full red intensity, no green, no blue). The line of
    // code right after that is the first vertex (the top of the triangle), and will be drawn
    // using the current color which is red. Anything we draw from now on will be red until
    // we change the color to something other than red.
    glColor3f(1.0f,0.0f,0.0f);
    glVertex3f( 0.0f, 1.0f, 0.0f);
    // We've placed the first vertex on the screen, setting it's color to red. Now before we
    // set the second vertex we'll change the color to green. That way the second vertex which
    // is the left corner of the triangle will be set to green.
    glColor3f(0.0f,1.0f,0.0f);
    glVertex3f(-1.0f,-1.0f, 0.0f);
    // Now we're on the third and final vertex. Just before we draw it, we set the color to
    // blue. This will be the right corner of the triangle. As soon as the glEnd() command is
    // issued, the polygon will be filled in. But because it has a different color at each
    // vertex, rather than one solid color throughout, the color will spread out from each
    // corner, eventually meeting in the middle, where the colors will blend together.
    // This is smooth coloring.
    glColor3f(0.0f,0.0f,1.0f);
    glVertex3f( 1.0f,-1.0f, 0.0f);
    // Finished drawing the triangle.
    glEnd();

    /////////////////////////////////////////////////////////////////////////////////////////////

    // Move right 3 units from current position.
    glTranslatef(3.0f,0.0f,0.0f);

    /////////////////////////////////////////////////////////////////////////////////////////////
    // New Code /////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////

    // To draw our square all one color, all we have to do is set the color once to a color we
    // like (blue in this example), then draw the square. The color blue will be used for each
    // vertex because we're not telling OpenGL to change the color at each vertex. The final
    // result... a solid blue square.
    glColor3f(0.5f,0.5f,1.0f);

    /////////////////////////////////////////////////////////////////////////////////////////////

    // Begin drawing using quads.
    glBegin(GL_QUADS);
    glVertex3f(-1.0f, 1.0f, 0.0f); // Top left of quad.
    glVertex3f( 1.0f, 1.0f, 0.0f); // Top right of quad.
    glVertex3f( 1.0f,-1.0f, 0.0f); // Bottom right of quad.
    glVertex3f(-1.0f,-1.0f, 0.0f); // Bottom left of quad.
    // Finished drawing the quad.
    glEnd();

    // Swap buffers (double buffering) to display results.
    SwapBuffers(this->GetDevice());

    return TRUE;
}


////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_Demo::RotateDemo():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Returns: TRUE if successfull and FALSE if not.
//==============================================================================================

BOOL GS_Demo::RotateDemo()
{

    // Make sure we have a valid display before drawing.
    if (!m_gsDisplay.IsReady())
    {
        return FALSE;
    }

    static GLfloat glfRotTrianle = 0.0f; // Rotation angle for the triangle. (NEW)
    static GLfloat glfRotQuad    = 0.0f; // Rotation angle for the quad. (NEW)

    // Is the game paused?
    if (IsPaused())
    {
        // Exit the function.
        return TRUE;
    }

    // Set OpenGL clear color to black.
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    // Clear screen and depth buffer.
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Reset the current modelview matrix.
    glLoadIdentity();

    // Disable texturing for this demo
    glDisable(GL_TEXTURE_2D);

    // Enable depth testing so faces are drawn in correct order
    glEnable(GL_DEPTH_TEST);
    
    // Move left 1.5 units and into the screen 6.0.
    glTranslatef(-1.5f,0.0f,-6.0f);

    /////////////////////////////////////////////////////////////////////////////////////////////
    // New Code /////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////

    // The next line of code is new. glRotatef(Angle,Xvector,Yvector,Zvector) is responsible for
    // rotating the object around an axis. You will get alot of use out of this command. Angle is
    // some number (usually stored in a variable) that represents how much you would like to spin
    // the object. Xvector, Yvector and Zvector parameters together represent the vector about
    // which the rotation will occur. If you use values (1,0,0), you are describing a vector
    // which travels in a direction of 1 unit along the x axis towards the right. Values (-1,0,0)
    // describes a vector that travels in a direction of 1 unit along the x axis, but this time
    // towards the left.

    // D. Michael Traub: has supplied the above explanation of the Xvector, Yvector and Zvector
    // parameters:

    // To better understand X, Y and Z rotation I'll explain using examples...

    // X Axis - You're working on a table saw. The bar going through the center of the blade runs
    // left to right (just like the x axis in OpenGL). The sharp teeth spin around the x axis
    // (bar running through the center of the blade), and appear to be cutting towards or away
    // from you depending on which way the blade is being spun. When we spin something on the x
    // axis in OpenGL it will spin the same way.

    // Y Axis - Imagine that you are standing in the middle of a field. There is a huge tornado
    // coming straight at you. The center of a tornado runs from the sky to the ground (up and
    // down, just like the y axis in OpenGL). The dirt and debris in the tornado spins around the
    // y axis (center of the tornado) from left to right or right to left. When you spin
    // something on the y axis in OpenGL it will spin the same way.

    // Z Axis - You are looking at the front of a fan. The center of the fan points towards you
    // and away from you (just like the z axis in OpenGL). The blades of the fan spin around the
    // z axis (center of the fan) in a clockwise or counterclockwise direction. When You spin
    // something on the z axis in OpenGL it will spin the same way.

    // So in the following line of code, if glfRotTrianle was equal to 7, we would spin 7 on the
    // Y axis (left to right). You can try experimenting with the code. Change the 0.0f's to
    // 1.0f's, and the 1.0f to a 0.0f to spin the triangle on the X and Y axes at the same time.

    // It's important to note that rotations are done in degrees. If glfRotTrianle had a value of
    // 10, we would be rotating 10 degrees on the y-axis.

    glRotatef(glfRotTrianle,0.0f,1.0f,0.0f); // Rotate the triangle on the Y axis.

    /////////////////////////////////////////////////////////////////////////////////////////////

    // Beigin drawing using triangles.
    glBegin(GL_TRIANGLES);
    glColor3f(1.0f,0.0f,0.0f);     // Set color to red.
    glVertex3f( 0.0f, 1.0f, 0.0f); // Top of triangle.
    glColor3f(0.0f,1.0f,0.0f);     // Set color to green.
    glVertex3f(-1.0f,-1.0f, 0.0f); // Bottom left of triangle.
    glColor3f(0.0f,0.0f,1.0f);     // Set color to blue.
    glVertex3f( 1.0f,-1.0f, 0.0f); // Bottom right of triangle.
    // Finished drawing the triangle.
    glEnd();

    /////////////////////////////////////////////////////////////////////////////////////////////
    // New Code /////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////

    // You'll notice in the code below, that we've added another glLoadIdentity(). We do this to
    // reset the view. If we didn't reset the view. If we translated after the object had been
    // rotated, you would get very unexpected results. Because the axis has been rotated, it may
    // not be pointing in the direction you think. So if we translate left on the X axis, we may
    // end up moving up or down instead, depending on how much we've rotated on each axis. Try
    // taking the glLoadIdentity() line out to see what I mean.
    glLoadIdentity();

    // Once the scene has been reset, so X is running left to right, Y up and down, and Z in and
    // out, we translate. You'll notice we're only moving 1.5 to the right instead of 3.0 like we
    // did in the last lesson. When we reset the screen, our focus moves to the center of the
    // screen. meaning we're no longer 1.5 units to the left, we're back at 0.0. So to get to 1.5
    // on the right side of zero we dont have to move 1.5 from left to center then 1.5 to the
    // right (total of 3.0) we only have to move from center to the right which is just 1.5
    // units.
    glTranslatef(1.5f,0.0f,-6.0f);

    // After we have moved to our new location on the right side of the screen, we rotate the
    // quad, on the X axis. This will cause the square to spin up and down.

    glRotatef(glfRotQuad,1.0f,0.0f,0.0f); // Rotate the quad on the X axis.

    /////////////////////////////////////////////////////////////////////////////////////////////

    // Set color to blue before drawing square.
    glColor3f(0.5f,0.5f,1.0f);

    // Begin drawing using quads.
    glBegin(GL_QUADS);
    glVertex3f(-1.0f, 1.0f, 0.0f); // Top left of quad.
    glVertex3f( 1.0f, 1.0f, 0.0f); // Top right of quad.
    glVertex3f( 1.0f,-1.0f, 0.0f); // Bottom right of quad.
    glVertex3f(-1.0f,-1.0f, 0.0f); // Bottom left of quad.
    // Finished drawing the quad.
    glEnd();

    /////////////////////////////////////////////////////////////////////////////////////////////
    // New Code /////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////

    // The next two lines are new. Think of glfRotTrianle, and glfRotQuad as containers. At the
    // top of our program we made the containers. When we built the containers they had nothing
    // in them. The first line below adds 0.2 to that container. So each time we check the value
    // in the glfRotTrianle container after this section of code, it will have gone up by 0.2.
    // The glfRotQuad container decreases by 0.15. So every time we check the glfRotQuad
    // container, it will have gone down by 0.15. Going down will cause the object to spin the
    // opposite direction it would spin if you were going up.

    // Try chaning the + to a - in the line below see how the object spins the other direction.
    // Try changing the values from 0.2 to 1.0. The higher the number, the faster the object will
    // spin. The lower the number, the slower it will spin.

    glfRotTrianle += 0.8f; // Increase the rotation variable for the triangle.
    glfRotQuad -= 0.75f;   // Decrease the rotation variable for the quad.

    /////////////////////////////////////////////////////////////////////////////////////////////

    // Swap buffers (double buffering) to display results.
    SwapBuffers(this->GetDevice());

    return TRUE;
}


////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_Demo::ShapesDemo():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Returns: TRUE if successfull and FALSE if not.
//==============================================================================================

BOOL GS_Demo::ShapesDemo()
{

    // Make sure we have a valid display before drawing.
    if (!m_gsDisplay.IsReady())
    {
        return FALSE;
    }

    static GLfloat glfRotTrianle = 0.0f; // Rotation angle for the triangle.
    static GLfloat glfRotQuad    = 0.0f; // Rotation angle for the quad.

    // Is the game paused?
    if (IsPaused())
    {
        // Exit the function.
        return TRUE;
    }

    // Set OpenGL clear color to black.
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    // Clear screen and depth buffer.
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Reset the current modelview matrix.
    glLoadIdentity();

    // Disable texturing for this demo
    glDisable(GL_TEXTURE_2D);

    // Enable depth testing so faces are drawn in correct order
    glEnable(GL_DEPTH_TEST);

    // Move left 1.5 units and into the screen 6.0.
    glTranslatef(-1.5f,0.0f,-6.0f);
    // Rotate the triangle on the Y axis.
    glRotatef(glfRotTrianle,0.0f,1.0f,0.0f);

    /////////////////////////////////////////////////////////////////////////////////////////////
    // New Code /////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////

    // The following code will create the pyramid around a central axis. The top of the pyramid
    // is one high from the center, the bottom of the pyramid is one down from the center. The
    // top point is right in the middle (zero), and the bottom points are one left from center,
    // and one right from center.

    // Note that all triangles are drawn in a counterclockwise rotation. This is important, and
    // will be explained in a future tutorial, for now, just know that it's good practice to make
    // objects either clockwise or counterclockwise, but you shouldn't mix the two unless you
    // have a reason to.

    // Beigin drawing using triangles.
    glBegin(GL_TRIANGLES);

    // We start off by drawing the Front Face. Because all of the faces share the top point,
    // we will make this point red on all of the triangles. The color on the bottom two points
    // of the triangles will alternate. The front face will have a green left point and a blue
    // right point. Then the triangle on the right side will have a blue left point and a
    // green right point. By alternating the bottom two colors on each face, we make a common
    // colored point at the bottom of each face.

    glColor4f(1.0f,0.0f,0.0f,0.5);  // Set color to red.
    glVertex3f( 0.0f, 1.0f, 0.0f);  // Top of triangle (front).
    glColor4f(0.0f,1.0f,0.0f,0.5f); // Set color to green.
    glVertex3f(-1.0f,-1.0f, 1.0f);  // Left of triangle (front).
    glColor4f(0.0f,0.0f,1.0f,0.5f); // Set color to blue.
    glVertex3f( 1.0f,-1.0f, 1.0f);  // Right of triangle (front).

    // Now we draw the right face. Notice then the two bottom point are drawn one to the right
    // of center, and the top point is drawn one up on the y axis, and right in the middle of
    // the x axis. causing the face to slope from center point at the top out to the right
    // side of the screen at the bottom.

    // Notice the left point is drawn blue this time. By drawing it blue, it will be the same
    // color as the right bottom corner of the front face. Blending blue outwards from that
    // one corner across both the front and right face of the pyramid.

    // Notice how the remaining three faces are included inside the same glBegin(GL_TRIANGLES)
    // and glEnd() as the first face. Because we're making this entire object out of
    // triangles, OpenGL will know that every three points we plot are the three points of a
    // triangle. Once it's drawn three points, if there are three more points, it will assume
    // another triangle needs to be drawn. If you were to put four points instead of three,
    // OpenGL would draw the first three and assume the fourth point is the start of a new
    // triangle. It would not draw a Quad. So make sure you don't add any extra points by
    // accident.

    glColor4f(1.0f,0.0f,0.0f,0.5f); // Set color to red.
    glVertex3f( 0.0f, 1.0f, 0.0f);  // Top of triangle (right).
    glColor4f(0.0f,0.0f,1.0f,0.5f); // Set color to blue.
    glVertex3f( 1.0f,-1.0f, 1.0f);  // Left of triangle (right).
    glColor4f(0.0f,1.0f,0.0f,0.5f); // Set color to green.
    glVertex3f( 1.0f,-1.0f, -1.0f); // Right of triangle (right).

    // Now for the back face. Again the colors switch. The left point is now green again,
    // because the corner it shares with the right face is green.

    glColor4f(1.0f,0.0f,0.0f,0.5f); // Set color to red.
    glVertex3f( 0.0f, 1.0f, 0.0f);  // Top of triangle (back).
    glColor4f(0.0f,1.0f,0.0f,0.5f); // Set color to green.
    glVertex3f( 1.0f,-1.0f, -1.0f); // Left of triangle (back).
    glColor4f(0.0f,0.0f,1.0f,0.5f); // Set color to blue.
    glVertex3f(-1.0f,-1.0f, -1.0f); // Right of triangle (back).

    // Finally we draw the left face. The colors switch one last time. The left point is blue,
    // and blends with the right point of the back face. The right point is green, and blends
    // with the left point of the front face.

    glColor4f(1.0f,0.0f,0.0f,0.5f); // Set color to red.
    glVertex3f( 0.0f, 1.0f, 0.0f);  // Top of triangle (left).
    glColor4f(0.0f,0.0f,1.0f,0.5f); // Set color to blue.
    glVertex3f(-1.0f,-1.0f,-1.0f);  // Left of triangle (left).
    glColor4f(0.0f,1.0f,0.0f,0.5f); // Set color to green.
    glVertex3f(-1.0f,-1.0f, 1.0f);  // Right of triangle (left).

    // We're done drawing the pyramid. Because the pyramid only spins on the Y axis, we will
    // never see the bottom, so there is no need to put a bottom on the pyramid. If you feel
    // like experimenting, try adding a bottom using a quad, then rotate on the X axis to see
    // if you've done it correctly. Make sure the color used on each corner of the quad
    // matches up with the colors being used at the four corners of the pyramid.

    // Finished drawing the triangle.
    glEnd();

    /////////////////////////////////////////////////////////////////////////////////////////////

    // Reset the current modelview matrix.
    glLoadIdentity();

    /////////////////////////////////////////////////////////////////////////////////////////////
    // New Code /////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////

    // Notice we move the cube a little further into the screen in this lesson. By doing this,
    // the size of the cube appears closer to the size of the pyramid. If you were to move it
    // only 6 units into the screen, the cube would appear much larger than the pyramid, and
    // parts of it might get cut off by the sides of the screen. You can play around with this
    // setting, and see how moving the cube further into the screen makes it appear smaller, and
    // moving it closer makes it appear larger. The reason this happens is perspective. Objects
    // in the distance should appear smaller :)

    // Move right 1.5 units and into the screen 7.0.
    glTranslatef(1.5f,0.0f,-7.0f);
    // Rotate the quad on the X, Y and Z axis.
    glRotatef(glfRotQuad,1.0f,1.0f,1.0f);

    // Now we'll draw the cube. It's made up of six quads. All of the quads are drawn in a
    // counter clockwise order. Meaning the first point is the top right, the second point is the
    // top left, third point is bottom left, and finally bottom right. When we draw the back
    // face, it may seem as though we are drawing clockwise, but you have to keep in mind that if
    // we were behind the cube looking at the front of it, the left side of the screen is
    // actually the right side of the quad, and the right side of the screen would actually be
    // the left side of the quad.

    // Begin drawing using quads.
    glBegin(GL_QUADS);

    // We'll start off by drawing the top of the cube. We move up one unit from the center of
    // the cube. Notice that the Y axis is always one. We then draw a quad on the Z plane.
    // Meaning into the screen. We start off by drawing the top right point of the top of the
    // cube. The top right point would be one unit right, and one unit into the screen. The
    // second point would be one unit to the left, and unit into the screen. Now we have to
    // draw the bottom of the quad towards the viewer. so to do this, instead of going into
    // the screen, we move one unit towards the screen. Make sense?

    glColor4f(0.0f,1.0f,0.0f,0.5f); // Set color to green.
    glVertex3f( 1.0f, 1.0f,-1.0f);  // Top right of the quad (top).
    glVertex3f(-1.0f, 1.0f,-1.0f);  // Top left of the quad (top).
    glVertex3f(-1.0f, 1.0f, 1.0f);  // Bottom left of the quad (top).
    glVertex3f( 1.0f, 1.0f, 1.0f);  // Bottom right of the quad (top).

    // The bottom is drawn the exact same way as the top, but because it's the bottom, it's
    // drawn down one unit from the center of the cube. Notice the Y axis is always minus one.
    // If we were under the cube, looking at the quad that makes the bottom, you would notice
    // the top right corner is the corner closest to the viewer, so instead of drawing in the
    // distance first, we draw closest to the viewer first, then on the left side closest to
    // the viewer, and then we go into the screen to draw the bottom two points.

    // If you didn't really care about the order the polygons were drawn in (clockwise or
    // not), you could just copy the same code for the top quad, move it down on the Y axis
    // to -1, and it would work, but ignoring the order the quad is drawn in can cause weird
    // results once you get into fancy things such as texture mapping.

    glColor4f(1.0f,0.5f,0.0f,0.5f); // Set color to orange.
    glVertex3f( 1.0f,-1.0f, 1.0f);  // Top right of the quad (bottom).
    glVertex3f(-1.0f,-1.0f, 1.0f);  // Top left of the quad (bottom).
    glVertex3f(-1.0f,-1.0f,-1.0f);  // Bottom left of the quad (bottom).
    glVertex3f( 1.0f,-1.0f,-1.0f);  // Bottom right of the quad (bottom).

    // Now we draw the front of the Quad. We move one unit towards the screen, and away from
    // the center to draw the front face. Notice the Z axis is always one. In the pyramid the
    // Z axis was not always one. At the top, the Z axis was zero. If you tried changing the
    // Z axis to zero in the following code, you'd notice that the corner you changed it on
    // would slope into the screen. That's not something we want to do right now :)

    glColor4f(1.0f,0.0f,0.0f,0.5f); // Set color to red.
    glVertex3f( 1.0f, 1.0f, 1.0f);  // Top right of the quad (front).
    glVertex3f(-1.0f, 1.0f, 1.0f);  // Top left of the quad (front).
    glVertex3f(-1.0f,-1.0f, 1.0f);  // Bottom left of the quad (front).
    glVertex3f( 1.0f,-1.0f, 1.0f);  // Bottom right of the quad (front).

    // The back face is a quad the same as the front face, but it's set deeper into the
    // screen. Notice the Z axis is now minus one for all of the points.

    glColor4f(1.0f,1.0f,0.0f,0.5f); // Set color to yellow.
    glVertex3f( 1.0f,-1.0f,-1.0f);  // Top right of the quad (back).
    glVertex3f(-1.0f,-1.0f,-1.0f);  // Top left of the quad (back).
    glVertex3f(-1.0f, 1.0f,-1.0f);  // Bottom left of the quad (back).
    glVertex3f( 1.0f, 1.0f,-1.0f);  // Bottom right of the quad (back).

    // Now we only have two more quads to draw and we're done. As usual, you'll notice one
    // axis is always the same for all the points. In this case the X axis is always minus
    // one. That's because we're always drawing to the left of center because this is the left
    // face.

    glColor4f(0.0f,0.0f,1.0f,0.5f); // Set color to blue.
    glVertex3f(-1.0f, 1.0f, 1.0f);  // Top right of the quad (left).
    glVertex3f(-1.0f, 1.0f,-1.0f);  // Top left of the quad (left).
    glVertex3f(-1.0f,-1.0f,-1.0f);  // Bottom left of the quad (left).
    glVertex3f(-1.0f,-1.0f, 1.0f);  // Bottom right of the quad (left).

    // This is the last face to complete the cube. The X axis is always one. Drawing is
    // counter clockwise. If you wanted to, you could leave this face out, and make a box :)

    glColor4f(1.0f,0.0f,1.0f,0.5f); // Set color to violet.
    glVertex3f( 1.0f, 1.0f,-1.0f);  // Top right of the quad (right).
    glVertex3f( 1.0f, 1.0f, 1.0f);  // Top left of the quad (right).
    glVertex3f( 1.0f,-1.0f, 1.0f);  // Bottom left of the quad (right).
    glVertex3f( 1.0f,-1.0f,-1.0f);  // Bottom right of the quad (right).

    // Finished drawing the quad.
    glEnd();

    /////////////////////////////////////////////////////////////////////////////////////////////

    glfRotTrianle += 0.8f; // Increase the rotation variable for the triangle.
    glfRotQuad -= 0.75f;   // Decrease the rotation variable for the quad.

    // Swap buffers (double buffering) to display results.
    SwapBuffers(this->GetDevice());

    return TRUE;
}


////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_Demo::TextureDemo():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Returns: TRUE if successfull and FALSE if not.
//==============================================================================================

BOOL GS_Demo::TextureDemo()
{

    // Make sure we have a valid display before drawing.
    if (!m_gsDisplay.IsReady())
    {
        return FALSE;
    }

    // Is the game paused?
    if (IsPaused())
    {
        // Exit the function.
        return TRUE;
    }

    // Set OpenGL clear color to black.
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    // Clear screen and depth buffer.
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Reset the current modelview matrix.
    glLoadIdentity();

    // Enable texturing for this demo
    glEnable(GL_TEXTURE_2D);

    // Enable depth testing so faces are drawn in correct order
    glEnable(GL_DEPTH_TEST);

    /////////////////////////////////////////////////////////////////////////////////////////////
    // New Code /////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////

    static GLfloat glfRotateX = 0.0f; // X rotation.
    static GLfloat glfRotateY = 0.0f; // Y rotation.
    static GLfloat glfRotateZ = 0.0f; // Z rotation.
    static GLfloat glfRotSpeedX = 0;  // X rotation speed.
    static GLfloat glfRotSpeedY = 0;  // Y rotation speed.
    static GLfloat glfDepthZ = -5.0f; // How deep the cube is on the Z axis.

    // Check to see wether a key or button was pressed.
    int nKey = m_gsKeyboard.GetKeyPressed();
    int nButton = m_gsController.GetButtonPressed();

    if( nButton != -1 )
    {
        nKey = nButton;
    }

    // Act depending on key pressed.
    switch (nKey)
    {
    // Was the left cursor key pressed?
    case GSK_LEFT:
    case GSC_BUTTON_DPAD_LEFT:
        if (glfRotSpeedY > -10.0f)
        {
            glfRotSpeedY -= 0.05f;
        }
        break;
    // Was the right cursor key pressed?
    case GSK_RIGHT:
    case GSC_BUTTON_DPAD_RIGHT:
        if (glfRotSpeedY < 10.0f)
        {
            glfRotSpeedY += 0.05f;
        }
        break;
    // Was the up cursor key pressed?
    case GSK_UP:
    case GSC_BUTTON_DPAD_UP:
        if (glfRotSpeedX > -10.0f)
        {
            glfRotSpeedX -= 0.05f;
        }
        break;
    // Was the down cursor key pressed?
    case GSK_DOWN:
    case GSC_BUTTON_DPAD_DOWN:
        if (glfRotSpeedX < 10.0f)
        {
            glfRotSpeedX += 0.05f;
        }
        break;
    // Was the page down key pressed?
    case GSK_PAGEDOWN:
    case GSC_BUTTON_LEFTSHOULDER:
        if (glfDepthZ > -100.0f)
        {
            glfDepthZ -= 0.05f;
        }
        break;
    // Was the page up key pressed?
    case GSK_PAGEUP:
    case GSC_BUTTON_RIGHTSHOULDER:
        if (glfDepthZ < 0.0f)
        {
            glfDepthZ += 0.05f;
        }
        break;
    // Was the home key pressed?
    case GSK_HOME:
    case GSC_BUTTON_START:
        // Reset all variables.
        glfRotateX   = 0.0f;
        glfRotateY   = 0.0f;
        glfRotateZ   = 0.0f;
        glfRotSpeedY = 0;
        glfRotSpeedX = 0;
        glfDepthZ    = -5.0f;
        break;
    }

    // Change the postion of the camera.
    glTranslatef(0.0f,0.0f,glfDepthZ);

    // The following three lines of code will rotate the cube on the x axis, then the y axis,
    // and finally the z axis. How much it rotates on each axis will depend on the value stored
    // in glfRotateX, glfRotateY and glfRotateZ.
    glRotatef(glfRotateX, 1.0f, 0.0f, 0.0f);
    glRotatef(glfRotateY, 0.0f, 1.0f, 0.0f);
    glRotatef(glfRotateZ, 0.0f, 0.0f, 1.0f);

    // This makes sure we draw the textures on a white background to make sure the texture is
    // displayed with the correct colors. Changing the color of the color of the background the
    // texture will be drawn upon will give the texture a different hue depending on the color.
    glColor4f(1.0f,1.0f,1.0f,1.0f);

    // The next line of code selects which texture we want to use. If there was more than one
    // texture you wanted to use in your scene, you would select the texture using
    // glBindTexture(GL_TEXTURE_2D, [number of texture to use]). If you wanted to change
    // textures, you would bind to the new texture. One thing to note is that you can NOT bind
    // a texture inside glBegin() and glEnd(), you have to do it before or after glBegin().
    // Notice how we use glBindTextures to specify which texture to create and to select a
    // specific texture.
    glBindTexture(GL_TEXTURE_2D, m_gsTexture.GetID());

    // To properly map a texture onto a quad, you have to make sure the top right of the texture
    // is mapped to the top right of the quad. The top left of the texture is mapped to the top
    // left of the quad, the bottom right of the texture is mapped to the bottom right of the
    // quad, and finally, the bottom left of the texture is mapped to the bottom left of the
    // quad. If the corners of the texture do not match the same corners of the quad, the image
    // may appear upside down, sideways, or not at all.

    // The first value of glTexCoord2f is the X coordinate. 0.0f is the left side of the texture.
    // 0.5f is the middle of the texture, and 1.0f is the right side of the texture. The second
    // value of glTexCoord2f is the Y coordinate. 0.0f is the bottom of the texture. 0.5f is the
    // middle of the texture, and 1.0f is the top of the texture.

    // So now we know the top left coordinate of a texture is 0.0f on X and 1.0f on Y, and the
    // top left vertex of a quad is -1.0f on X, and 1.0f on Y. Now all you have to do is match
    // the other three texture coordinates up with the remaining three corners of the quad.

    // Try playing around with the x and y values of glTexCoord2f. Changing 1.0f to 0.5f will
    // only draw the left half of a texture from 0.0f (left) to 0.5f (middle of the texture).
    // Changing 0.0f to 0.5f will only draw the right half of a texture from 0.5f (middle) to
    // 1.0f (right).

    glBegin(GL_QUADS);

    // Notice the glNormal3f() method below. A normal is a line pointing straight out of the
    // middle of a polygon at a 90 degree angle. When you use lighting, you need to specify a
    // normal. The normal tells OpenGL which direction the polygon is facing... which way is
    // up. If you don't specify normals, all kinds of weird things happen. Faces that
    // shouldn't light up will light up, the wrong side of a polygon will light up, etc. The
    // normal should point outwards from the polygon.

    // Looking at the front face you'll notice that the normal is positive on the z axis.
    // This means the normal is pointing at the viewer. Exactly the direction we want it
    // pointing. On the back face, the normal is pointing away from the viewer, into the
    // screen. Again exactly what we want. If the cube is spun 180 degrees on either the x
    // or y axis, the front will be facing into the screen and the back will be facing towards
    // the viewer. No matter what face is facing the viewer, the normal of that face will also
    // be pointing towards the viewer. Because the light is close to the viewer, any time the
    // normal is pointing towards the viewer it's also pointing towards the light. When it
    // does, the face will light up. The more a normal points towards the light, the brighter
    // that face is. If you move into the center of the cube you'll notice it's dark. The
    // normals are point out, not in, so there's no light inside the box, exactly as it should
    // be.

    // Front face.
    glNormal3f(0.0f, 0.0f, 1.0f);
    glTexCoord2f(0.0f, 0.0f);
    glVertex3f(-1.0f, -1.0f,  1.0f);
    glTexCoord2f(1.0f, 0.0f);
    glVertex3f( 1.0f, -1.0f,  1.0f);
    glTexCoord2f(1.0f, 1.0f);
    glVertex3f( 1.0f,  1.0f,  1.0f);
    glTexCoord2f(0.0f, 1.0f);
    glVertex3f(-1.0f,  1.0f,  1.0f);

    // Back face.
    glNormal3f(0.0f, 0.0f, -1.0f);
    glTexCoord2f(1.0f, 0.0f);
    glVertex3f(-1.0f, -1.0f, -1.0f);
    glTexCoord2f(1.0f, 1.0f);
    glVertex3f(-1.0f,  1.0f, -1.0f);
    glTexCoord2f(0.0f, 1.0f);
    glVertex3f( 1.0f,  1.0f, -1.0f);
    glTexCoord2f(0.0f, 0.0f);
    glVertex3f( 1.0f, -1.0f, -1.0f);

    // Top face.
    glNormal3f(0.0f, 1.0f, 0.0f);
    glTexCoord2f(0.0f, 1.0f);
    glVertex3f(-1.0f,  1.0f, -1.0f);
    glTexCoord2f(0.0f, 0.0f);
    glVertex3f(-1.0f,  1.0f,  1.0f);
    glTexCoord2f(1.0f, 0.0f);
    glVertex3f( 1.0f,  1.0f,  1.0f);
    glTexCoord2f(1.0f, 1.0f);
    glVertex3f( 1.0f,  1.0f, -1.0f);

    // Bottom face.
    glNormal3f(0.0f, -1.0f, 0.0f);
    glTexCoord2f(1.0f, 1.0f);
    glVertex3f(-1.0f, -1.0f, -1.0f);
    glTexCoord2f(0.0f, 1.0f);
    glVertex3f( 1.0f, -1.0f, -1.0f);
    glTexCoord2f(0.0f, 0.0f);
    glVertex3f( 1.0f, -1.0f,  1.0f);
    glTexCoord2f(1.0f, 0.0f);
    glVertex3f(-1.0f, -1.0f,  1.0f);

    // Right face
    glNormal3f(1.0f, 0.0f, 0.0f);
    glTexCoord2f(1.0f, 0.0f);
    glVertex3f( 1.0f, -1.0f, -1.0f);
    glTexCoord2f(1.0f, 1.0f);
    glVertex3f( 1.0f,  1.0f, -1.0f);
    glTexCoord2f(0.0f, 1.0f);
    glVertex3f( 1.0f,  1.0f,  1.0f);
    glTexCoord2f(0.0f, 0.0f);
    glVertex3f( 1.0f, -1.0f,  1.0f);

    // Left face.
    glNormal3f(-1.0f, 0.0f, 1.0f);
    glTexCoord2f(0.0f, 0.0f);
    glVertex3f(-1.0f, -1.0f, -1.0f);
    glTexCoord2f(1.0f, 0.0f);
    glVertex3f(-1.0f, -1.0f,  1.0f);
    glTexCoord2f(1.0f, 1.0f);
    glVertex3f(-1.0f,  1.0f,  1.0f);
    glTexCoord2f(0.0f, 1.0f);
    glVertex3f(-1.0f,  1.0f, -1.0f);

    glEnd();

    // Now we increase the value of glfRotateX, glfRotateY and glfRotateZ. Try changing the
    // number each variable increases by to make the cube spin faster or slower, or try changing
    // a + to a - to make the cube spin the other direction.
    glfRotateX += glfRotSpeedX;
    glfRotateY += glfRotSpeedY;
    // glfRotateZ += 0.8f;

    // Undo the texture bind to prevent the texture from being used in other demonstartions.
    glBindTexture(GL_TEXTURE_2D, 0);

    /////////////////////////////////////////////////////////////////////////////////////////////

    // Swap buffers (double buffering) to display results.
    SwapBuffers(this->GetDevice());

    return TRUE;
}


////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_Demo::SpriteDemo():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Returns: TRUE if successfull and FALSE if not.
//==============================================================================================

BOOL GS_Demo::SpriteDemo()
{

    // Make sure we have a valid display before drawing.
    if (!m_gsDisplay.IsReady())
    {
        return FALSE;
    }

    // Is the game paused?
    if (IsPaused())
    {
        glClearColor(1.0f, 0.0f, 0.0f, 0.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glLoadIdentity();
        m_gsDisplay.BeginRender2D(this->GetWindow());
        m_gsBackgrnd.SetDestX((INTERNAL_RES_X - (long)m_gsBackgrnd.GetScaledWidth())  / 2);
        m_gsBackgrnd.SetDestY((INTERNAL_RES_Y - (long)m_gsBackgrnd.GetScaledHeight()) / 2);
        m_gsBackgrnd.RenderTiles(m_rcScreen);
        m_gsSprite.SetDestX((INTERNAL_RES_X - (long)m_gsSprite.GetScaledWidth())  / 2);
        m_gsSprite.SetDestY((INTERNAL_RES_Y - (long)m_gsSprite.GetScaledHeight()) / 2);
        m_gsSprite.Render();
        m_gsDisplay.EndRender2D();
        SwapBuffers(this->GetDevice());
        return TRUE;
    }

    // Set OpenGL clear color to blue.
    glClearColor(0.0f, 0.0f, 1.0f, 0.0f);

    // Clear screen and depth buffer.
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Reset the current modelview matrix.
    glLoadIdentity();

    /////////////////////////////////////////////////////////////////////////////////////////////
    // New Code /////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////

    static GLfloat glfRotSpeed = 0.0f; // Rotation speed.
    static GLfloat glfTransparency = 1.0f; // Sprite transparency.

    // Check to see wether a key or button was pressed.
    int nKey = m_gsKeyboard.GetKeyPressed();
    int nButton = m_gsController.GetButtonPressed();

    if( nButton != -1 )
    {
        nKey = nButton;
    }

    // Act depending on key pressed.
    switch (nKey)
    {
    // Was the left cursor key pressed?
    case GSK_LEFT:
    case GSC_BUTTON_DPAD_LEFT:
        if (glfRotSpeed < 10.0f)
        {
            glfRotSpeed += 0.1f;
        }
        break;
    // Was the right cursor key pressed?
    case GSK_RIGHT:
    case GSC_BUTTON_DPAD_RIGHT:
        if (glfRotSpeed > -10.0f)
        {
            glfRotSpeed -= 0.1f;
        }
        break;
    // Was the up cursor key pressed?
    case GSK_UP:
    case GSC_BUTTON_DPAD_UP:
        m_gsSprite.AddScaleX(0.02f);
        m_gsSprite.AddScaleY(0.02f);
        m_gsBackgrnd.AddScaleX(0.02f);
        m_gsBackgrnd.AddScaleY(0.02f);
        break;
    // Was the down cursor key pressed?
    case GSK_DOWN:
    case GSC_BUTTON_DPAD_DOWN:
        m_gsSprite.AddScaleX(-0.02f);
        m_gsSprite.AddScaleY(-0.02f);
        if (m_gsBackgrnd.GetScaleX() > 0.2f)
        {
            m_gsBackgrnd.AddScaleX(-0.02f);
            m_gsBackgrnd.AddScaleY(-0.02f);
        }
        break;
    // Was the page up key pressed?
    case GSK_PAGEUP:
    case GSC_BUTTON_LEFTSHOULDER:
        if (glfTransparency < 1.0f)
        {
            glfTransparency += 0.05f;
        }
        break;
    // Was the page down key pressed?
    case GSK_PAGEDOWN:
    case GSC_BUTTON_RIGHTSHOULDER:
        if (glfTransparency > 0.0f)
        {
            glfTransparency -= 0.05f;
        }
        break;
    // Was the home key pressed?
    case GSK_HOME:
    case GSC_BUTTON_START:
        // Reset all variables.
        m_gsSprite.SetScaleX(1.0f);
        m_gsSprite.SetScaleY(1.0f);
        m_gsSprite.SetRotateZ(0.0f);
        m_gsBackgrnd.SetScaleX(1.0f);
        m_gsBackgrnd.SetScaleY(1.0f);
        glfRotSpeed = 0.0f;
        glfTransparency = 1.0f;
        break;
    }

    // Setup the display for rendering in 2D.
    m_gsDisplay.BeginRender2D(this->GetWindow());

    // Set the modulate color of the background.
    m_gsBackgrnd.SetModulateColor(-1.0f, -1.0f, -1.0f, glfTransparency);

    // Scroll the background left and upwards.
    m_gsBackgrnd.AddScrollX(-1.5f);
    m_gsBackgrnd.AddScrollY(+0.5f);

    // Draw the tile background image.
    m_gsBackgrnd.RenderTiles(m_rcScreen);

    // Set the modulate color of the sprite.
    m_gsSprite.SetModulateColor(-1.0f, -1.0f, -1.0f, glfTransparency);

    // Rotate the sprite on the Z-axis at the specified speed.
    m_gsSprite.AddRotateZ(glfRotSpeed);

    // Position the sprite centered on the screen.
    m_gsSprite.SetDestX((INTERNAL_RES_X - (long)m_gsSprite.GetScaledWidth())  / 2);
    m_gsSprite.SetDestY((INTERNAL_RES_Y - (long)m_gsSprite.GetScaledHeight()) / 2);

    // Draw the sprite.
    m_gsSprite.Render();

    // Reset display after rendering in 2D.
    m_gsDisplay.EndRender2D();

    /////////////////////////////////////////////////////////////////////////////////////////////

    // Swap buffers (double buffering) to display results.
    SwapBuffers(this->GetDevice());

    return TRUE;
}


////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_Demo::FontDemo():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Returns: TRUE if successfull and FALSE if not.
//==============================================================================================

BOOL GS_Demo::FontDemo()
{

    // Make sure we have a valid display before drawing.
    if (!m_gsDisplay.IsReady())
    {
        return FALSE;
    }

    // Is the game paused?
    if (IsPaused())
    {
        glClearColor(1.0f, 0.0f, 0.0f, 0.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glLoadIdentity();
        m_gsDisplay.BeginRender2D(this->GetWindow());
        m_gsBackgrnd.SetScrollXY(0.0f, 0.0f);
        m_gsBackgrnd.SetDestX((INTERNAL_RES_X - (long)m_gsBackgrnd.GetScaledWidth())  / 2);
        m_gsBackgrnd.SetDestY((INTERNAL_RES_Y - (long)m_gsBackgrnd.GetScaledHeight()) / 2);
        m_gsBackgrnd.RenderTiles(m_rcScreen);
        m_gsSpriteEx.Render();
        m_gsSmallFont.SetModulateColor(1.0f, 1.0f, 1.0f, 1.0f);
        m_gsSmallFont.SetText("PAUSED");
        m_gsSmallFont.SetDestXY((INTERNAL_RES_X - m_gsSmallFont.GetTextWidth())  / 2,
                                (INTERNAL_RES_Y - m_gsSmallFont.GetTextHeight()) / 2);
        m_gsSmallFont.Render();
        m_gsDisplay.EndRender2D();
        SwapBuffers(this->GetDevice());
        return TRUE;
    }

    // Set OpenGL clear color to blue.
    glClearColor(0.0f, 0.0f, 1.0f, 0.0f);

    // Clear screen and depth buffer.
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Reset the current modelview matrix.
    glLoadIdentity();

    // Setup the display for rendering in 2D.
    m_gsDisplay.BeginRender2D(this->GetWindow());

    /////////////////////////////////////////////////////////////////////////////////////////////
    // New Code /////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////

    static GLfloat glfRotSpeed = 0.0f;     // Rotation speed.
    static GLfloat glfTransparency = 1.0f; // Sprite transparency.

    static BOOL bMoveRight = TRUE;
    static BOOL bMoveUp    = TRUE;

    // Check to see wether a key or button was pressed.
    int nKey = m_gsKeyboard.GetKeyPressed();
    int nButton = m_gsController.GetButtonPressed();

    if( nButton != -1 )
    {
        nKey = nButton;
    }

    // Act depending on key pressed.
    switch (nKey)
    {
    // Was the left cursor key pressed?
    case GSK_LEFT:
    case GSC_BUTTON_DPAD_LEFT:
        if (glfRotSpeed < 10.0f)
        {
            glfRotSpeed += 0.1f;
        }
        break;
    // Was the right cursor key pressed?
    case GSK_RIGHT:
    case GSC_BUTTON_DPAD_RIGHT:
        if (glfRotSpeed > -10.0f)
        {
            glfRotSpeed -= 0.1f;
        }
        break;
    // Was the up cursor key pressed?
    case GSK_UP:
    case GSC_BUTTON_DPAD_UP:
        m_gsSpriteEx.AddScaleX(0.05f);
        m_gsSpriteEx.AddScaleY(0.05f);
        break;
    // Was the down cursor key pressed?
    case GSK_DOWN:
    case GSC_BUTTON_DPAD_DOWN:
        m_gsSpriteEx.AddScaleX(-0.05f);
        m_gsSpriteEx.AddScaleY(-0.05f);
        break;
    // Was the page up key pressed?
    case GSK_PAGEUP:
    case GSC_BUTTON_LEFTSHOULDER:
        if (glfTransparency < 1.0f)
        {
            glfTransparency += 0.05f;
        }
        break;
    // Was the page down key pressed?
    case GSK_PAGEDOWN:
    case GSC_BUTTON_RIGHTSHOULDER:
        if (glfTransparency > 0.0f)
        {
            glfTransparency -= 0.05f;
        }
        break;
    // Was the home key pressed?
    case GSK_HOME:
    case GSC_BUTTON_START:
        // Reset all variables.
        m_gsSpriteEx.SetScaleX(1.0f);
        m_gsSpriteEx.SetScaleY(1.0f);
        m_gsSpriteEx.SetRotateZ(0.0f);
        glfRotSpeed = 0.0f;
        glfTransparency = 1.0f;
        break;
    }

    // Reset the background scrolling and scaling.
    m_gsBackgrnd.SetScrollXY(0.0f, 0.0f);
    m_gsBackgrnd.SetScaleXY(1.0f, 1.0f);

    // Set the modulate color of the background.
    m_gsBackgrnd.SetModulateColor(-1.0f, -1.0f, -1.0f, glfTransparency);

    // Draw the tile background image.
    m_gsBackgrnd.RenderTiles(m_rcScreen);

    /////////////////////////////////////////////////////////////////////////////////////////////

    // Set the modulate color of the sprite.
    m_gsSpriteEx.SetModulateColor(-1.0f, -1.0f, -1.0f, glfTransparency);

    // Rotate the sprite on the Z-axis at the specified speed.
    m_gsSpriteEx.AddRotateZ(glfRotSpeed);

    // Determine the left/right movement.
    if (bMoveRight)
    {
        m_gsSpriteEx.AddDestX(2);
    }
    if (!bMoveRight)
    {
        m_gsSpriteEx.AddDestX(-2);
    }

    // Determine the up/down movement.
    if (bMoveUp)
    {
        m_gsSpriteEx.AddDestY(2);
    }
    if (!bMoveUp)
    {
        m_gsSpriteEx.AddDestY(-2);
    }

    RECT rcDest;
    m_gsSpriteEx.GetDestRect(&rcDest);

    // Has sprite reached left edge of screen?
    if (rcDest.left <= 0)
    {
        bMoveRight = TRUE;
        m_gsSound.PlaySample(SAMPLE_IMPACT);
    }

    // Has sprite reached right edge of screen?
    if (rcDest.right >= INTERNAL_RES_X)
    {
        bMoveRight = FALSE;
        m_gsSound.PlaySample(SAMPLE_IMPACT);
    }

    // Has sprite reached bottom edge of screen?
    if (rcDest.bottom <= 0)
    {
        bMoveUp = TRUE;
        m_gsSound.PlaySample(SAMPLE_IMPACT);
    }

    // Has sprite reached top edge of screen?
    if (rcDest.top >= INTERNAL_RES_Y)
    {
        bMoveUp = FALSE;
        m_gsSound.PlaySample(SAMPLE_IMPACT);
    }

    // Render the sprite.
    m_gsSpriteEx.Render();

    // Go to the next frame.
    m_gsSpriteEx.AddFrame(1);

    /////////////////////////////////////////////////////////////////////////////////////////////

    // Display the frames per second at the top left corner.
    m_gsSmallFont.SetText("%0.2f", this->GetCurrentFrameRate());
    m_gsSmallFont.SetDestXY(16, INTERNAL_RES_Y - m_gsSmallFont.GetTextHeight() -16);
    m_gsSmallFont.Render();


    // Display the title of the demo centered at the bottom.
    m_gsLargeFont.SetModulateColor(-1.0f, -1.0f, -1.0f, glfTransparency);
    m_gsLargeFont.SetText("FONTS");
    m_gsLargeFont.SetDestXY((INTERNAL_RES_X - m_gsLargeFont.GetTextWidth()) / 2, 16);
    m_gsLargeFont.Render();

    /////////////////////////////////////////////////////////////////////////////////////////////

    // Reset display after rendering in 2D.
    m_gsDisplay.EndRender2D();

    // Swap buffers (double buffering) to display results.
    SwapBuffers(this->GetDevice());

    return TRUE;
}


////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_Demo::CollisionDemo():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Returns: TRUE if successfull and FALSE if not.
//==============================================================================================

BOOL GS_Demo::CollisionDemo()
{

    // Make sure we have a valid display before drawing.
    if (!m_gsDisplay.IsReady())
    {
        return FALSE;
    }

    // Is the game paused?
    if (IsPaused())
    {
        glClearColor(1.0f, 0.0f, 0.0f, 0.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glLoadIdentity();
        m_gsDisplay.BeginRender2D(this->GetWindow());
        m_gsBackgrnd.SetScrollXY(0.0f, 0.0f);
        m_gsBackgrnd.SetDestX((INTERNAL_RES_X - (long)m_gsBackgrnd.GetScaledWidth())  / 2);
        m_gsBackgrnd.SetDestY((INTERNAL_RES_Y - (long)m_gsBackgrnd.GetScaledHeight()) / 2);
        m_gsBackgrnd.RenderTiles(m_rcScreen);
        for (int nLoop = 0; nLoop < MAXIMUM_SPRITES; nLoop++)
        {
            m_gsSprites[nLoop].Render();
        }
        m_gsSmallFont.SetModulateColor(1.0f, 1.0f, 1.0f, 1.0f);
        m_gsSmallFont.SetText("PAUSED");
        m_gsSmallFont.SetDestXY((INTERNAL_RES_X - m_gsSmallFont.GetTextWidth())  / 2,
                                (INTERNAL_RES_Y - m_gsSmallFont.GetTextHeight()) / 2);
        m_gsSmallFont.Render();
        m_gsDisplay.EndRender2D();
        SwapBuffers(this->GetDevice());
        return TRUE;
    }

    // Set OpenGL clear color to blue.
    glClearColor(0.0f, 0.0f, 1.0f, 0.0f);

    // Clear screen and depth buffer.
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Reset the current modelview matrix.
    glLoadIdentity();

    // Setup the display for rendering in 2D.
    m_gsDisplay.BeginRender2D(this->GetWindow());

    /////////////////////////////////////////////////////////////////////////////////////////////

    static GLfloat glfTransparency = 1.0f; // Sprite transparency.

    // Check to see wether a key or button was pressed.
    int nKey = m_gsKeyboard.GetKeyPressed();
    int nButton = m_gsController.GetButtonPressed();

    if( nButton != -1 )
    {
        nKey = nButton;
    }

    // Act depending on key pressed.
    switch (nKey)
    {
    // Was the page up key pressed?
    case GSK_PAGEUP:
        if (glfTransparency < 1.0f)
        {
            glfTransparency += 0.05f;
        }
        break;
    // Was the page down key pressed?
    case GSK_PAGEDOWN:
        if (glfTransparency > 0.0f)
        {
            glfTransparency -= 0.05f;
        }
        break;
    // Was the home key pressed?
    case GSK_HOME:
        // Reset all variables.
        glfTransparency = 1.0f;
        break;
    }

    // Set the modulate color of the background.
    m_gsBackgrnd.SetModulateColor(-1.0f, -1.0f, -1.0f, glfTransparency);

    // Draw the tile background image.
    m_gsBackgrnd.RenderTiles(m_rcScreen);

    /////////////////////////////////////////////////////////////////////////////////////////////

    /////////////////////////////////////////////////////////////////////////////////////////////
    // New Code. ////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////

    // For all the sprites.
    for (int nLoop = 0; nLoop < MAXIMUM_SPRITES; nLoop++)
    {

        static int  nFrameCount[MAXIMUM_SPRITES] = { 0 };
        static BOOL bHasCollided = FALSE;

        nFrameCount[nLoop]++;

        // Reset the modulate color of the sprite after 15 frames.
        if (nFrameCount[nLoop] >= 15)
        {
            m_gsSprites[nLoop].SetModulateColor(1.0f, 1.0f, 1.0f, glfTransparency);
            nFrameCount[nLoop] = 0;
            bHasCollided = FALSE;
        }

        // Increase the rotation factor to rotate the sprite.
        m_gsSprites[nLoop].AddRotateZ(m_fRotation[nLoop]);

        // Reset the rotation factor once the sprite has been rotated all the way around.
        if (m_gsSprites[nLoop].GetRotateZ() >= 360.0f)
        {
            m_gsSprites[nLoop].SetRotateZ(m_fRotation[nLoop]);
        }

        // Check for collisions.
        RECT rcSource, rcDest;

        // Determine the screen coordinates of the source sprite.
        m_gsSprites[nLoop].GetDestRect(&rcSource);

        // Test sprite against all other sprites.
        for (int cLoop = 0; cLoop < MAXIMUM_SPRITES; cLoop++)
        {
            // If the sprite is any sprite except the current one.
            if (cLoop!=nLoop)
            {
                // Determine the screen coordinates of the destination sprite.
                m_gsSprites[cLoop].GetDestRect(&rcDest);
                // If a collision has occured with one of the other sprites.
                if ((m_gsCollide.IsRectOnRect(rcSource, rcDest, 60)) && (!bHasCollided))
                {
                    // Set flag to indicate a collision has taken place.
                    bHasCollided = TRUE;
                    // Change colliding sprites directions to opposites.
                    m_bMoveRight[nLoop] = !m_bMoveRight[nLoop];
                    m_bMoveDown[nLoop]  = !m_bMoveDown[nLoop];
                    m_bMoveRight[cLoop] = !m_bMoveRight[nLoop];
                    m_bMoveDown[cLoop]  = !m_bMoveDown[nLoop];
                    // Play appropriate sound.
                    m_gsSound.PlaySample(SAMPLE_COLLIDE);
                    // Set sprite modulate color to show impact.
                    m_gsSprites[nLoop].SetModulateColor(1.0f, 0.5f, 0.5f, glfTransparency);
                }
            }
        }

        // Start decrementing the X position once at far-side of screen.
        if (m_gsSprites[nLoop].GetDestX() >= (INTERNAL_RES_X - m_gsSprites[nLoop].GetFrameWidth()))
        {
            m_bMoveRight[nLoop] = FALSE;
            // Play appropriate sound.
            m_gsSound.PlaySample(SAMPLE_IMPACT, 100);
        }

        // Start incrementing the X position once at near-side of screen.
        if (m_gsSprites[nLoop].GetDestX() <= 0)
        {
            m_bMoveRight[nLoop] = TRUE;
            // Play appropriate sound.
            m_gsSound.PlaySample(SAMPLE_IMPACT, 100);
        }

        // Increment/decrement sprite X position.
        if (m_bMoveRight[nLoop])
        {
            m_gsSprites[nLoop].AddDestX(m_fVelocity[nLoop]);
        }
        else
        {
            m_gsSprites[nLoop].AddDestX(-m_fVelocity[nLoop]);
        }

        // Start decrementing the Y position once at bottom of screen.
        if (m_gsSprites[nLoop].GetDestY() >= (INTERNAL_RES_Y - m_gsSprites[nLoop].GetFrameHeight()))
        {
            m_bMoveDown[nLoop] = FALSE;
            // Play appropriate sound.
            m_gsSound.PlaySample(SAMPLE_IMPACT, 100);
        }

        // Start incrementing the Y position once at top of screen.
        if (m_gsSprites[nLoop].GetDestY() <= 0)
        {
            m_bMoveDown[nLoop] = TRUE;
            // Play appropriate sound.
            m_gsSound.PlaySample(SAMPLE_IMPACT, 100);
        }

        // Increment/decrement sprite Y position.
        if (m_bMoveDown[nLoop])
        {
            m_gsSprites[nLoop].AddDestY(m_fVelocity[nLoop]);
        }
        else
        {
            m_gsSprites[nLoop].AddDestY(-m_fVelocity[nLoop]);
        }

        // Render the sprite to the back surface.
        m_gsSprites[nLoop].Render();

        // Move to the next sprite frame.
        m_gsSprites[nLoop].AddFrame(1);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////

    // Display the frames per second at the top left corner.
    m_gsSmallFont.SetText("%0.2f", this->GetCurrentFrameRate());
    m_gsSmallFont.SetDestXY(16, INTERNAL_RES_Y - m_gsSmallFont.GetTextHeight() -16);
    m_gsSmallFont.Render();

    // Display the title of the demo centered at the bottom.
    m_gsLargeFont.SetModulateColor(-1.0f, -1.0f, -1.0f, glfTransparency);
    m_gsLargeFont.SetText("COLLISION");
    m_gsLargeFont.SetDestXY((INTERNAL_RES_X - m_gsLargeFont.GetTextWidth()) / 2, 16);
    m_gsLargeFont.Render();

    /////////////////////////////////////////////////////////////////////////////////////////////

    // Reset display after rendering in 2D.
    m_gsDisplay.EndRender2D();

    // Swap buffers (double buffering) to display results.
    SwapBuffers(this->GetDevice());

    return TRUE;
}


////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_Demo::MenuDemo():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Returns: TRUE if successfull and FALSE if not.
//==============================================================================================

BOOL GS_Demo::MenuDemo()
{

    // Make sure we have a valid display before drawing.
    if (!m_gsDisplay.IsReady())
    {
        return FALSE;
    }

    // Is the game paused?
    if (IsPaused())
    {
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glLoadIdentity();
        m_gsDisplay.BeginRender2D(this->GetWindow());
        m_gsSmallFont.SetModulateColor(1.0f, 1.0f, 1.0f, 1.0f);
        m_gsSmallFont.SetText("PAUSED");
        m_gsSmallFont.SetDestXY((INTERNAL_RES_X - m_gsSmallFont.GetTextWidth())  / 2,
                                (INTERNAL_RES_Y - m_gsSmallFont.GetTextHeight()) / 2);
        m_gsSmallFont.Render();
        m_gsDisplay.EndRender2D();
        SwapBuffers(this->GetDevice());
        return TRUE;
    }

    // Set OpenGL clear color to dark red.
    glClearColor(0.4f, 0.0f, 0.0f, 1.0f);

    // Clear screen and depth buffer.
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Reset the current modelview matrix.
    glLoadIdentity();

    /////////////////////////////////////////////////////////////////////////////////////////////
    // New Code /////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////

    static GLfloat glfRollGrnd  = 0.0f;
    static GLfloat glfRollClds  = 0.0f;
    static GLfloat glfRollSpeed = 0.001f;

    // Create two layers of scrolling clouds in the top half of the screen.
    glBindTexture(GL_TEXTURE_2D, m_gsCloudsTexture.GetID());
    glBegin(GL_QUADS);
    glNormal3f(0.0f, 0.0f, 1.0f);
    glTexCoord2f(1.0f,1.0f+glfRollClds/0.5f);
    glVertex3f( 28.0f,6.0f,0.0f);    // Top right.
    glTexCoord2f(0.0f,1.0f+glfRollClds/0.5f);
    glVertex3f(-28.0f,6.0f,0.0f);    // Top left.
    glTexCoord2f(0.0f,0.0f+glfRollClds/0.5f);
    glVertex3f(-28.0f,-3.0f,-50.0f); // Bottom left.
    glTexCoord2f(1.0f,0.0f+glfRollClds/0.5f);
    glVertex3f( 28.0f,-3.0f,-50.0f); // Bottom right.
    glNormal3f(0.0f, 0.0f, 1.0f);
    glTexCoord2f(0.0f,1.0f+glfRollClds/1.0f);
    glVertex3f( 28.0f,6.0f,0.0f);    // Top right.
    glTexCoord2f(1.0f,1.0f+glfRollClds/1.0f);
    glVertex3f(-28.0f,6.0f,0.0f);    // Top left.
    glTexCoord2f(1.0f,0.0f+glfRollClds/1.0f);
    glVertex3f(-28.0f,-3.0f,-50.0f); // Bottom left.
    glTexCoord2f(0.0f,0.0f+glfRollClds/1.0f);
    glVertex3f( 28.0f,-3.0f,-50.0f); // Bottom right.
    glEnd();

    glfRollClds -= glfRollSpeed;

    // Reset texture once entire length has been scrolled.
    if (glfRollClds < -1.0f + glfRollSpeed)
    {
        glfRollClds = 0.0f;
    }
    else if (glfRollClds >= 1.0f)
    {
        glfRollClds = 0.0f;
    }

    // Create a scrolling landscape in the bottom half of the screen.
    glBindTexture(GL_TEXTURE_2D, m_gsGroundTexture.GetID());
    glBegin(GL_QUADS);
    glNormal3f(0.0f, 0.0f, 1.0f);
    glTexCoord2f(7.0f,4.0f-glfRollGrnd);
    glVertex3f( 28.0f,-3.0f,-50.0f); // Top right.
    glTexCoord2f(0.0f,4.0f-glfRollGrnd);
    glVertex3f(-28.0f,-3.0f,-50.0f); // Top left.
    glTexCoord2f(0.0f,0.0f-glfRollGrnd);
    glVertex3f(-28.0f,-3.0f,0.0f);   // Bottom left.
    glTexCoord2f(7.0f,0.0f-glfRollGrnd);
    glVertex3f( 28.0f,-3.0f,0.0f);   // Bottom right.
    glEnd();
    glBindTexture(GL_TEXTURE_2D, 0);

    glfRollGrnd -= (glfRollSpeed*10);

    // Reset texture once entire length has been scrolled.
    if (glfRollGrnd < -4.0f + glfRollSpeed)
    {
        glfRollGrnd = 0.0f;
    }
    else if (glfRollGrnd >= 4.0f)
    {
        glfRollGrnd = 0.0f;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////

    /////////////////////////////////////////////////////////////////////////////////////////////
    // New Code: Menu Rendering /////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////

    static GLfloat glfTransparency = 1.0f;

    // Setup the display for rendering in 2D.
    m_gsDisplay.BeginRender2D(this->GetWindow());

    // Center menu in screen.
    m_gsMenu.SetDestX((INTERNAL_RES_X -  m_gsMenu.GetWidth()) / 2);
    m_gsMenu.SetDestY((INTERNAL_RES_Y - m_gsMenu.GetHeight()) / 2);

    m_gsMenu.SetModulateColor(-1.0f, -1.0f, -1.0f, glfTransparency);

    // Render the menu to the back surface.
    m_gsMenu.Render();

    static BOOL bWasMouseReleased  = FALSE;
    static BOOL bWasKeyReleased    = TRUE;
    static BOOL bWasButtonReleased    = TRUE;
    static BOOL bMenuHasTitle      = TRUE;
    static int nOptionSelected     = -1;

    int KeyList[3] = { GSK_ENTER, GSK_UP, GSK_DOWN };

    // Were all the keys in the key list released?
    if (TRUE == m_gsKeyboard.AreKeysUp(3, KeyList))
    {
        // Set flag to indicate that all the keys were released.
        bWasKeyReleased = TRUE;
    }

    // Check to see wether a key or button was pressed.
    int nKey = m_gsKeyboard.GetKeyPressed();
    int nButton = m_gsController.GetButtonPressed();

    if( nButton != -1 )
    {
        nKey = nButton;
    }

    // Act depending on key pressed.
    switch (nKey)
    {
    // Was the up key pressed?
    case GSK_UP:
        // Was this key released?
        if (bWasKeyReleased)
        {
            // Highlight the previous option.
            m_gsMenu.HighlightPrev();
            // Play appropriate sound.
            m_gsSound.PlaySample(SAMPLE_OPTION);
            // Key is pressed.
            bWasKeyReleased = FALSE;
        }
        break;
    // Was the down key pressed?
    case GSK_DOWN:
        // Was this key released?
        if (bWasKeyReleased)
        {
            // Highlight the next option.
            m_gsMenu.HighlightNext();
            // Play appropriate sound.
            m_gsSound.PlaySample(SAMPLE_OPTION);
            // Key is pressed.
            bWasKeyReleased = FALSE;
        }
        break;
    // Was the enter key pressed?
    case GSK_ENTER:
        // Was this key released?
        if (bWasKeyReleased)
        {
            // Save the highlighted option.
            nOptionSelected = m_gsMenu.GetHighlight();
            // Play appropriate sound.
            m_gsSound.PlaySample(SAMPLE_SELECT);
            // Key is pressed.
            bWasKeyReleased = FALSE;
        }
        break;
    // Was the page up key pressed?
    case GSK_PAGEUP:
    case GSC_BUTTON_LEFTSHOULDER:
        if (glfTransparency < 1.0f)
        {
            glfTransparency += 0.05f;
        }
        break;
    // Was the page down key pressed?
    case GSK_PAGEDOWN:
    case GSC_BUTTON_RIGHTSHOULDER:
        if (glfTransparency > 0.0f)
        {
            glfTransparency -= 0.05f;
        }
        break;
    // Was the right key pressed?
    case GSK_RIGHT:
    case GSC_BUTTON_DPAD_RIGHT:
        if (glfRollSpeed < 0.1f)
        {
            glfRollSpeed += 0.0001f;
        }
        break;
    // Was the left key pressed?
    case GSK_LEFT:
    case GSC_BUTTON_DPAD_LEFT:
        if (glfRollSpeed > -0.1f)
        {
            glfRollSpeed -= 0.0001f;
        }
        break;
    // Was the home key pressed?
    case GSK_HOME:
    case GSC_BUTTON_START:
        // Reset all variables.
        glfTransparency = 1.0f;
        glfRollSpeed = 0.001f;
        break;
    }
    
    int ButtonList[3] = { GSC_BUTTON_A, GSC_BUTTON_DPAD_UP, GSC_BUTTON_DPAD_DOWN };

    // Were all the buttons in the button list released?
    if (TRUE == m_gsController.AreButtonsUp(3, ButtonList))
    {
        // Set flag to indicate that all the buttons were released.
        bWasButtonReleased = TRUE;
    }

    // Check to see wether a button was pressed.
    int nBufferedButton = m_gsController.GetButtonPressed();    

    // Act depending on button pressed.
    switch (nBufferedButton)
    {
    // Was the up key pressed?
    case GSC_BUTTON_DPAD_UP:
        // Was this key released?
        if (bWasButtonReleased)
        {
            // Highlight the previous option.
            m_gsMenu.HighlightPrev();
            // Play appropriate sound.
            m_gsSound.PlaySample(SAMPLE_OPTION);
            // Key is pressed.
            bWasButtonReleased = FALSE;
        }
        break;
    // Was the down key pressed?
    case GSC_BUTTON_DPAD_DOWN:
        // Was this key released?
        if (bWasButtonReleased)
        {
            // Highlight the next option.
            m_gsMenu.HighlightNext();
            // Play appropriate sound.
            m_gsSound.PlaySample(SAMPLE_OPTION);
            // Key is pressed.
            bWasButtonReleased = FALSE;
        }
        break;
    // Was the enter key pressed?
    case GSC_BUTTON_A:
        // Was this key released?
        if (bWasButtonReleased)
        {
            // Save the highlighted option.
            nOptionSelected = m_gsMenu.GetHighlight();
            // Play appropriate sound.
            m_gsSound.PlaySample(SAMPLE_SELECT);
            // Key is pressed.
            bWasButtonReleased = FALSE;
        }
        break;
    }

    RECT rcDest;

    // Check each option to see if the mouse coordinates are within the option area.
    for (int nLoop = 0; nLoop < m_gsMenu.GetNumOptions(); nLoop++)
    {
        // Determine the screen coordinates of the next option.
        m_gsMenu.GetOptionRect(nLoop, &rcDest);
        // If the mouse coordinates are within the option area.
        if (m_gsCollide.IsCoordInRect(m_gsMouse.GetX(), m_gsMouse.GetY(), rcDest))
        {
            // Highlight the option if it was not highlighted already.
            if (nLoop != m_gsMenu.GetHighlight())
            {
                m_gsMenu.SetHighlight(nLoop);
                // Play appropriate sound.
                m_gsSound.PlaySample(SAMPLE_OPTION);
            }
        }
    }

    // Check if left mouse button was released.
    if (!m_gsMouse.IsLeftPressed())
    {
        bWasMouseReleased = TRUE;
    }

    // Check if the left mouse button was pressed.
    if ((m_gsMouse.IsLeftPressed()) && (bWasMouseReleased))
    {
        // Determine the screen coordinates of the highlighted option.
        m_gsMenu.GetHighlightRect(&rcDest);
        // Check if mouse coordinates are within the area of the highligted option.
        if (m_gsCollide.IsCoordInRect(m_gsMouse.GetX(), m_gsMouse.GetY(), rcDest))
        {
            // Save the highligted option.
            nOptionSelected = m_gsMenu.GetHighlight();
            bWasMouseReleased = FALSE;
            // Play appropriate sound.
            m_gsSound.PlaySample(SAMPLE_SELECT);
        }
    }

    // Process menu command if selected.
    switch (nOptionSelected)
    {
    case 0:
        // Does the menu not have a title?
        if (!bMenuHasTitle)
        {
            // Enable the menu title.
            m_gsMenu.SetTitle("  MAIN MENU  ");
            bMenuHasTitle = TRUE;
            // Change menu option.
            m_gsMenu.SetOption(0, "Disable Title");
        }
        else
        {
            // Disable the menu title.
            m_gsMenu.SetTitle("");
            bMenuHasTitle = FALSE;
            // Change menu option.
            m_gsMenu.SetOption(0, "Enable Title ");
        }
        // Reset selected option.
        nOptionSelected = -1;
        break;
    case 1:
        // Is sound currently paused?
        if (m_gsSound.IsPaused())
        {
            // Enable the sound.
            m_gsSound.Pause(FALSE);
            // Change menu option.
            m_gsMenu.SetOption(1, "Disable Sound");
        }
        else
        {
            // Disable the sound.
            m_gsSound.Pause(TRUE);
            // Change menu option.
            m_gsMenu.SetOption(1, "Enable Sound ");
        }
        // Reset selected option.
        nOptionSelected = -1;
        break;
    case 2:
        // Quit demonstration.
        this->Quit();
        // Reset selected option.
        nOptionSelected = -1;
        return TRUE;
    default:
        break;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////

    // Display the frames per second at the top left corner.
    m_gsSmallFont.SetText("%0.2f", this->GetCurrentFrameRate());
    m_gsSmallFont.SetDestXY(16, INTERNAL_RES_Y - m_gsSmallFont.GetTextHeight() -16);
    m_gsSmallFont.Render();

    // Display the mouse coordinates at the top right corner.
    m_gsSmallFont.SetText("(%ld,%ld)", m_gsMouse.GetX(), m_gsMouse.GetY());
    m_gsSmallFont.SetDestXY(INTERNAL_RES_X - m_gsSmallFont.GetTextWidth() -16,
                            INTERNAL_RES_Y - m_gsSmallFont.GetTextHeight() -16);
    m_gsSmallFont.Render();

    // Display the mouse cursor at the current mouse position.
    m_gsLargeFont.SetText("#");
    m_gsLargeFont.SetDestXY(m_gsMouse.GetX(), m_gsMouse.GetY() -
                            (int)m_gsLargeFont.GetTextHeight() + 1);
    m_gsLargeFont.Render();
    m_gsLargeFont.SetModulateColor(1.0f, 1.0f, 1.0f, 1.0f);

    // Display the title of the demo centered at the bottom.
    m_gsLargeFont.SetModulateColor(-1.0f, -1.0f, -1.0f, glfTransparency);
    m_gsLargeFont.SetText("MENUS");
    m_gsLargeFont.SetDestXY((INTERNAL_RES_X - m_gsLargeFont.GetTextWidth()) / 2, 16);
    m_gsLargeFont.Render();

    /////////////////////////////////////////////////////////////////////////////////////////////

    // Reset display after rendering in 2D.
    m_gsDisplay.EndRender2D();

    // Swap buffers (double buffering) to display results.
    SwapBuffers(this->GetDevice());

    return TRUE;
}


////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_Demo::MapDemo():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Returns: TRUE if successfull and FALSE if not.
//==============================================================================================

BOOL GS_Demo::MapDemo()
{

    // Make sure we have a valid display before drawing.
    if (!m_gsDisplay.IsReady())
    {
        return FALSE;
    }

    // Is the game paused?
    if (IsPaused())
    {
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glLoadIdentity();
        m_gsDisplay.BeginRender2D(this->GetWindow());
        m_gsMap.Render();
        m_gsPlayerSprite.Render();
        m_gsSmallFont.SetModulateColor(1.0f, 1.0f, 1.0f, 1.0f);
        m_gsSmallFont.SetText("PAUSED");
        m_gsSmallFont.SetDestXY((INTERNAL_RES_X  - m_gsSmallFont.GetTextWidth())  / 2,
                                (INTERNAL_RES_Y - m_gsSmallFont.GetTextHeight()) / 2);
        m_gsSmallFont.Render();
        m_gsDisplay.EndRender2D();
        SwapBuffers(this->GetDevice());
        return TRUE;
    }

    // Set OpenGL clear color to dark red.
    glClearColor(0.3f, 0.0f, 0.0f, 1.0f);

    // Clear screen and depth buffer.
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Reset the current modelview matrix.
    glLoadIdentity();

    /////////////////////////////////////////////////////////////////////////////////////////////

    static GLfloat glfRollTex;

    glfRollTex -= (this->GetFrameTime() * 0.00005f);

    // Create two layers of scrolling clouds (the one the invert of the other on the x-axis and
    // the first moving slower than the second) over the entire screen.
    glBindTexture(GL_TEXTURE_2D, m_gsCloudsTexture.GetID());
    glBegin(GL_QUADS);
    glNormal3f(0.0f, 0.0f, 1.0f);
    glTexCoord2f(1.0f,1.0f-glfRollTex/0.5f);
    glVertex3f( 8.0f,6.0f,-5.0f);  // Top right.
    glTexCoord2f(1.0f,0.0f-glfRollTex/0.5f);
    glVertex3f(-8.0f,6.0f,-5.0f);  // Top left.
    glTexCoord2f(0.0f,0.0f-glfRollTex/0.5f);
    glVertex3f(-8.0f,-3.0f,-5.0f); // Bottom left.
    glTexCoord2f(0.0f,1.0f-glfRollTex/0.5f);
    glVertex3f( 8.0f,-3.0f,-5.0f); // Bottom right.
    glNormal3f(0.0f, 0.0f, 1.0f);
    glTexCoord2f(0.0f,1.0f-glfRollTex/1.0f);
    glVertex3f( 8.0f,6.0f,-5.0f);  // Top right.
    glTexCoord2f(0.0f,0.0f-glfRollTex/1.0f);
    glVertex3f(-8.0f,6.0f,-5.0f);  // Top left.
    glTexCoord2f(1.0f,0.0f-glfRollTex/1.0f);
    glVertex3f(-8.0f,-3.0f,-5.0f); // Bottom left.
    glTexCoord2f(1.0f,1.0f-glfRollTex/1.0f);
    glVertex3f( 8.0f,-3.0f,-5.0f); // Bottom right.
    glEnd();

    // Reset texture when scrolled entire length.
    while (glfRollTex <= -1.0f)
    {
        glfRollTex += 1.0f;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////

    static GLfloat glfTransparency = 1.0f;
    static BOOL bWasKeyReleased    = TRUE;
    int    KeyList[3] = { GSK_ENTER, GSK_UP, GSK_DOWN };

    static int nMoveDistance = 4;
    RECT rcPlayer;

    // Get the destination rectangle of the player sprite.
    m_gsPlayerSprite.GetDestRect(&rcPlayer);

    // Set the collision rectangle 75% of the actual.
    m_gsCollide.SetRectPercentXY(&rcPlayer, 75);

    // Were all the keys in the key list released?
    if (TRUE == m_gsKeyboard.AreKeysUp(3, KeyList))
    {
        // Set flag to indicate that all the keys were released.
        bWasKeyReleased = TRUE;
    }

    // Check to see wether a key or button was pressed.
    int nKey = m_gsKeyboard.GetKeyPressed();
    int nButton = m_gsController.GetButtonPressed();

    if( nButton != -1 )
    {
        nKey = nButton;
    }

    // Act depending on key pressed.
    switch (nKey)
    {
    // Was the up key pressed?
    case GSK_UP:
    case GSC_BUTTON_DPAD_UP:
        // Has the player not moved half the height of the clip box?
        if ((m_gsPlayerSprite.GetDestY() + (m_gsPlayerSprite.GetFrameHeight()/2)) <
                (m_gsMap.GetClipBoxBottom() + (m_gsMap.GetClipBoxHeight()/2)))
        {
            // Adjust the player rectangle with the planned move.
            rcPlayer.bottom += nMoveDistance;
            rcPlayer.top    += nMoveDistance;
            // Is the player rectangle not on the specified tile?
            if (!m_gsMap.IsOnTile(rcPlayer, 0))
            {
                // Move the player up.
                m_gsPlayerSprite.AddDestY(nMoveDistance);
            }
            else
            {
                // Move did not take place, reset the player rectangle.
                rcPlayer.bottom -= nMoveDistance;
                rcPlayer.top    -= nMoveDistance;
            }
        }
        else
        {
            // Scroll the map downwards.
            m_gsMap.ScrollY(-nMoveDistance);
            // Is the player rectangle on the specified tile?
            if (m_gsMap.IsOnTile(rcPlayer, 0))
            {
                // Move did not take place, reset the map.
                m_gsMap.ScrollY(+nMoveDistance);
            }
        }
        // Select the appropriate frame.
        m_gsPlayerSprite.SetFrame(0);
        break;
    // Was the down key pressed?
    case GSK_DOWN:
    case GSC_BUTTON_DPAD_DOWN:
        // Has the player not moved half the height of the clip box?
        if ((m_gsPlayerSprite.GetDestY() + (m_gsPlayerSprite.GetFrameHeight()/2)) <
                (m_gsMap.GetClipBoxBottom() + (m_gsMap.GetClipBoxHeight()/2)))
        {
            // Scroll the map upwards.
            m_gsMap.ScrollY(+nMoveDistance);
            // Is the player rectangle on the specified tile?
            if (m_gsMap.IsOnTile(rcPlayer, 0))
            {
                // Move did not take place, reset the map.
                m_gsMap.ScrollY(-nMoveDistance);
            }
        }
        else
        {
            // Adjust the player rectangle with the planned move.
            rcPlayer.bottom -= nMoveDistance;
            rcPlayer.top    -= nMoveDistance;
            // Is the player rectangle not on the specified tile?
            if (!m_gsMap.IsOnTile(rcPlayer, 0))
            {
                // Move the player down.
                m_gsPlayerSprite.AddDestY(-nMoveDistance);
            }
            else
            {
                // Move did not take place, reset the player rectangle.
                rcPlayer.bottom += nMoveDistance;
                rcPlayer.top    += nMoveDistance;
            }
        }
        // Select the appropriate frame.
        m_gsPlayerSprite.SetFrame(2);
        break;
    // Was the right key pressed?
    case GSK_LEFT:
    case GSC_BUTTON_DPAD_LEFT:
        // Has the player not moved half the width of the clip box?
        if ((m_gsPlayerSprite.GetDestX() + (m_gsPlayerSprite.GetFrameWidth()/2)) <
                (m_gsMap.GetClipBoxLeft() + (m_gsMap.GetClipBoxWidth()/2)))
        {
            // Scroll the map to the right.
            m_gsMap.ScrollX(+nMoveDistance);
            // Is the player rectangle on the specified tile?
            if (m_gsMap.IsOnTile(rcPlayer, 0))
            {
                // Move did not take place, reset the map.
                m_gsMap.ScrollX(-nMoveDistance);
            }
        }
        else
        {
            // Adjust the player rectangle with the planned move.
            rcPlayer.left  -= nMoveDistance;
            rcPlayer.right -= nMoveDistance;
            // Is the player rectangle not on the specified tile?
            if (!m_gsMap.IsOnTile(rcPlayer, 0))
            {
                // Move the player left.
                m_gsPlayerSprite.AddDestX(-nMoveDistance);
            }
            else
            {
                // Move did not take place, reset the player rectangle.
                rcPlayer.left  += nMoveDistance;
                rcPlayer.right += nMoveDistance;
            }
        }
        // Select the appropriate frame.
        m_gsPlayerSprite.SetFrame(3);
        break;
    // Was the left key pressed?
    case GSK_RIGHT:
    case GSC_BUTTON_DPAD_RIGHT:
        // Has the player not moved half the width of the clip box?
        if ((m_gsPlayerSprite.GetDestX() + (m_gsPlayerSprite.GetFrameWidth()/2)) <
                (m_gsMap.GetClipBoxLeft() + (m_gsMap.GetClipBoxWidth()/2)))
        {
            // Adjust the player rectangle with the planned move.
            rcPlayer.left  += nMoveDistance;
            rcPlayer.right += nMoveDistance;
            // Is the player rectangle not on the specified tile?
            if (!m_gsMap.IsOnTile(rcPlayer, 0))
            {
                // Move the player right.
                m_gsPlayerSprite.AddDestX(+nMoveDistance);
            }
            else
            {
                // Move did not take place, reset the player rectangle.
                rcPlayer.left  -= nMoveDistance;
                rcPlayer.right -= nMoveDistance;
            }
        }
        else
        {
            // Scroll the map to the left.
            m_gsMap.ScrollX(-nMoveDistance);
            // Is the player rectangle on the specified tile?
            if (m_gsMap.IsOnTile(rcPlayer, 0))
            {
                // Move did not take place, reset the map.
                m_gsMap.ScrollX(+nMoveDistance);
            }
        }
        // Select the appropriate frame.
        m_gsPlayerSprite.SetFrame(1);
        break;
    // Was the page up key pressed?
    case GSK_PAGEUP:
    case GSC_BUTTON_LEFTSHOULDER:
        if (glfTransparency < 1.0f)
        {
            glfTransparency += 0.05f;
        }
        break;
    // Was the page down key pressed?
    case GSK_PAGEDOWN:
    case GSC_BUTTON_RIGHTSHOULDER:
        if (glfTransparency > 0.0f)
        {
            glfTransparency -= 0.05f;
        }
        break;
    // Was the home key pressed?
    case GSK_HOME:
    case GSC_BUTTON_START:
        // Reset all variables.
        glfTransparency = 1.0f;
        m_gsMap.SetMapX(0);
        m_gsMap.SetMapY(0);
        m_gsPlayerSprite.SetDestX(m_gsMap.GetClipBoxLeft()   +  m_gsMap.GetTileWidth());
        m_gsPlayerSprite.SetDestY(m_gsMap.GetClipBoxBottom() + m_gsMap.GetTileHeight());
        break;
    }

    // Setup the display for rendering in 2D.
    m_gsDisplay.BeginRender2D(this->GetWindow());

    /////////////////////////////////////////////////////////////////////////////////////////////
    // New Code /////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////

    static int nOldTile = 2;
    static int nNewTile = 2;
    static int nFrameCounter = 0;

    // Set the modulate color for the map.
    m_gsMap.SetModulateColor(-1.0f, -1.0f, -1.0f, glfTransparency);

    // Render the map.
    m_gsMap.Render();

    // Wait five frames before changing tiles.
    if (nFrameCounter++ >= 5)
    {
        // Replace the old tile with the new tile.
        m_gsMap.ReplaceTileID(nOldTile, nNewTile);
        // Old new-tile becomes the new old-tile.
        nOldTile = nNewTile;
        // Go to next tile.
        nNewTile++;
        // Have we reached the last tile?
        if (nNewTile > 7)
        {
            // Start again from the start.
            nNewTile = 2;
        }
        // Reset fram counter.
        nFrameCounter = 0;
    }

    // Set the modulate color for the player sprite.
    m_gsPlayerSprite.SetModulateColor(-1.0f, -1.0f, -1.0f, glfTransparency);

    // Render the player sprite.
    m_gsPlayerSprite.Render();

    // Set the collision rectangle 50% of the old.
    m_gsCollide.SetRectPercentXY(&rcPlayer, 50);

    // Is the player rectangle in the specified tile?
    if (m_gsMap.IsOnTile(rcPlayer, 10))
    {
        // Reset all variables.
        m_gsMap.SetMapX(0);
        m_gsMap.SetMapY(0);
        m_gsPlayerSprite.SetDestX(m_gsMap.GetClipBoxLeft()   +  m_gsMap.GetTileWidth());
        m_gsPlayerSprite.SetDestY(m_gsMap.GetClipBoxBottom() + m_gsMap.GetTileHeight());
        // Play appropriate sound.
        m_gsSound.PlaySample(SAMPLE_RESIZE);
    }

    /////////////////////////////////////////////////////////////////////////////////////////////

    // Display the frames per second at the top left corner.
    m_gsSmallFont.SetText("%0.2f", this->GetCurrentFrameRate());
    m_gsSmallFont.SetDestXY(16, INTERNAL_RES_Y - m_gsSmallFont.GetTextHeight() -16);
    m_gsSmallFont.Render();

    // Display the title of the demo centered at the bottom.
    m_gsLargeFont.SetModulateColor(-1.0f, -1.0f, -1.0f, glfTransparency);
    m_gsLargeFont.SetText("MAPS");
    m_gsLargeFont.SetDestXY((INTERNAL_RES_X - m_gsLargeFont.GetTextWidth()) / 2, 16);
    m_gsLargeFont.Render();

    /////////////////////////////////////////////////////////////////////////////////////////////

    // Reset display after rendering in 2D.
    m_gsDisplay.EndRender2D();

    // Unbind all textures.
    glBindTexture(GL_TEXTURE_2D, 0);

    // Swap buffers (double buffering) to display results.
    SwapBuffers(this->GetDevice());

    return TRUE;
}


////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_Demo::ParticleDemo():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Returns: TRUE if successfull and FALSE if not.
//==============================================================================================

BOOL GS_Demo::ParticleDemo()
{

    // Make sure we have a valid display before drawing.
    if (!m_gsDisplay.IsReady())
    {
        return FALSE;
    }

    // Is the game paused?
    if (IsPaused())
    {
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glLoadIdentity();
        m_gsDisplay.BeginRender2D(this->GetWindow());
        m_gsParticle.Render();
        m_gsSmallFont.SetModulateColor(1.0f, 1.0f, 1.0f, 1.0f);
        m_gsSmallFont.SetText("PAUSED");
        m_gsSmallFont.SetDestXY((INTERNAL_RES_X - m_gsSmallFont.GetTextWidth())  / 2,
                                (INTERNAL_RES_Y - m_gsSmallFont.GetTextHeight()) / 2);
        m_gsSmallFont.Render();
        m_gsDisplay.EndRender2D();
        SwapBuffers(this->GetDevice());
        return TRUE;
    }

    // Set OpenGL clear color to dark red.
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    // glClearColor(0.3f, 0.0f, 0.0f, 1.0f);

    // Clear screen and depth buffer.
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Reset the current modelview matrix.
    glLoadIdentity();

    /////////////////////////////////////////////////////////////////////////////////////////////

    /*
    static GLfloat glfRollGrnd  = 0.0f;
    static GLfloat glfRollClds  = 0.0f;
    static GLfloat glfRollSpeed = 0.02f;

    // Create two layers of scrolling clouds (the one the invert of the other on the x-axis and
    // the first moving slower than the second) in the top half of the screen.
    glBindTexture(GL_TEXTURE_2D, m_gsCloudsTexture.GetID());
    glBegin(GL_QUADS);
       glNormal3f(0.0f, 0.0f, 1.0f);
       glTexCoord2f(1.0f,1.0f+glfRollClds/0.5f); glVertex3f( 28.0f,6.0f,0.0f);    // Top right.
       glTexCoord2f(0.0f,1.0f+glfRollClds/0.5f); glVertex3f(-28.0f,6.0f,0.0f);    // Top left.
       glTexCoord2f(0.0f,0.0f+glfRollClds/0.5f); glVertex3f(-28.0f,-3.0f,-50.0f); // Bottom left.
       glTexCoord2f(1.0f,0.0f+glfRollClds/0.5f); glVertex3f( 28.0f,-3.0f,-50.0f); // Bottom right.
       glNormal3f(0.0f, 0.0f, 1.0f);
       glTexCoord2f(0.0f,1.0f+glfRollClds/1.0f); glVertex3f( 28.0f,6.0f,0.0f);    // Top right.
       glTexCoord2f(1.0f,1.0f+glfRollClds/1.0f); glVertex3f(-28.0f,6.0f,0.0f);    // Top left.
       glTexCoord2f(1.0f,0.0f+glfRollClds/1.0f); glVertex3f(-28.0f,-3.0f,-50.0f); // Bottom left.
       glTexCoord2f(0.0f,0.0f+glfRollClds/1.0f); glVertex3f( 28.0f,-3.0f,-50.0f); // Bottom right.
    glEnd();

    glfRollClds -= (glfRollSpeed/10.0f);

    // Reset texture once entire length has been scrolled.
    if (glfRollClds < -1.0f + glfRollSpeed) {
       glfRollClds = 0.0f;
       }
    else if (glfRollClds >= 1.0f) {
       glfRollClds = 0.0f;
       }

    // Create a scrolling landscape in the bottom half of the screen.
    glBindTexture(GL_TEXTURE_2D, m_gsGroundTexture.GetID());
    glBegin(GL_QUADS);
       glNormal3f(0.0f, 0.0f, 1.0f);
       glTexCoord2f(7.0f,4.0f-glfRollGrnd); glVertex3f( 28.0f,-3.0f,-50.0f); // Top right.
       glTexCoord2f(0.0f,4.0f-glfRollGrnd); glVertex3f(-28.0f,-3.0f,-50.0f); // Top left.
       glTexCoord2f(0.0f,0.0f-glfRollGrnd); glVertex3f(-28.0f,-3.0f,0.0f);   // Bottom left.
       glTexCoord2f(7.0f,0.0f-glfRollGrnd); glVertex3f( 28.0f,-3.0f,0.0f);   // Bottom right.
    glEnd();
    glBindTexture(GL_TEXTURE_2D, NULL);

    glfRollGrnd -= glfRollSpeed;

    // Reset texture once entire length has been scrolled.
    if (glfRollGrnd < -4.0f + glfRollSpeed) {
       glfRollGrnd = 0.0f;
       }
    else if (glfRollGrnd >= 4.0f) {
       glfRollGrnd = 0.0f;
       }
    */

    /////////////////////////////////////////////////////////////////////////////////////////////

    static GLfloat glfTransparency = 1.0f;

    int nNumParticles = m_gsParticle.GetNumParticles();
    static float fAddScale = 0.100f;

    // Check to see wether a key or button was pressed.
    int nKey = m_gsKeyboard.GetKeyPressed();
    int nButton = m_gsController.GetButtonPressed();

    if( nButton != -1 )
    {
        nKey = nButton;
    }

    // Act depending on key pressed.
    switch (nKey)
    {
    // Was the right key pressed?
    case GSK_RIGHT:
    case GSC_BUTTON_DPAD_RIGHT:
        // Increase the number of particles.
        nNumParticles++;
        if (nNumParticles > MAX_PARTICLES)
        {
            nNumParticles = MAX_PARTICLES;
        }
        m_gsParticle.SetNumParticles(nNumParticles);
        m_gsParticle.Activate();
        m_bIsFirstRun = TRUE;
        break;
    // Was the left key pressed?
    case GSK_LEFT:
    case GSC_BUTTON_DPAD_LEFT:
        // Decrease the number of particles.
        nNumParticles--;
        if (nNumParticles < 0)
        {
            nNumParticles = 0;
        }
        m_gsParticle.SetNumParticles(nNumParticles);
        m_gsParticle.Activate();
        m_bIsFirstRun = TRUE;
        break;
    // Was the down key pressed?
    case GSK_DOWN:
    case GSC_BUTTON_DPAD_DOWN:
        // Decrease the scale.
        fAddScale -= 0.001;
        if (fAddScale < 0.000f)
        {
            fAddScale = 0.000f;
        }
        break;
    // Was the up key pressed?
    case GSK_UP:
    case GSC_BUTTON_DPAD_UP:
        // Increase the scale.
        fAddScale += 0.001;
        if (fAddScale > 0.100f)
        {
            fAddScale = 0.100f;
        }
        break;
    // Was the page up key pressed?
    case GSK_PAGEUP:
    case GSC_BUTTON_LEFTSHOULDER:
        if (glfTransparency < 1.0f)
        {
            glfTransparency += 0.05f;
        }
        break;
    // Was the page down key pressed?
    case GSK_PAGEDOWN:
    case GSC_BUTTON_RIGHTSHOULDER:
        if (glfTransparency > 0.0f)
        {
            glfTransparency -= 0.05f;
        }
        break;
    // Was the home key pressed?
    case GSK_HOME:
    case GSC_BUTTON_START:
        // Reset all variables.
        m_gsParticle.SetNumParticles(100);
        m_gsParticle.Activate();
        fAddScale = 0.100f;
        glfTransparency = 1.0f;
        break;
    }

    // Setup the display for rendering in 2D.
    m_gsDisplay.BeginRender2D(this->GetWindow());

    /////////////////////////////////////////////////////////////////////////////////////////////
    // New Code /////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////

    static float fAddX[MAX_PARTICLES] = { 0.0f };
    static float fAddY[MAX_PARTICLES] = { 0.0f };
    static int   nMovementRateX[MAX_PARTICLES] = { 0 };
    static int   nMovementRateY[MAX_PARTICLES] = { 0 };
    static BOOL  bMoveLeft[MAX_PARTICLES] = { 0 };
    static BOOL  bMoveUp[MAX_PARTICLES]   = { 0 };
    // static BOOL  bIsFirstRun = TRUE;

    // Setup particles for the first time.
    if (m_bIsFirstRun)
    {
        // For each particle.
        for (int nLoop = 0; nLoop < m_gsParticle.GetNumParticles(); nLoop++)
        {
            // Determine a new random movement speed.
            nMovementRateX[nLoop] = 10;
            nMovementRateY[nLoop] = 10;
            // Set horizontal direction randomly.
            if (rand()%2 == 1)
            {
                bMoveLeft[nLoop] = !bMoveLeft[nLoop];
            }
            // Set vertical direction randomly.
            if (rand()%2 == 1)
            {
                bMoveUp[nLoop] = !bMoveUp[nLoop];
            }
            // Determine random particle scale.
            float fScale = 0.1f; // + float(rand()%11) / 5.0f;
            m_gsParticle.SetScaleX(nLoop, fScale);
            m_gsParticle.SetScaleY(nLoop, fScale);
            // Set starting position in the centre of the screen.
            m_gsParticle.SetDestX(nLoop, INTERNAL_RES_X / 2);
            m_gsParticle.SetDestY(nLoop, INTERNAL_RES_Y / 2 - 35);
            // Select a random modulate color for the specified particle.
            m_gsParticle.SetModulateColor(nLoop, 0.0f, 0.0f, 0.0f, 0.0f);
        }
        // Not first run anymore.
        m_bIsFirstRun = FALSE;
    }

    // For each particle.
    for (int nLoop = 0; nLoop < m_gsParticle.GetNumParticles(); nLoop++)
    {
        // Determine the interval required to move the particle at the specified speed, 60
        // times every second at the current frame rate (on the x-axis) and add it to
        // the previous intervals.
        fAddX[nLoop] += (this->GetActionInterval(60) * nMovementRateX[nLoop]);
        // Have the intervals added up enough to take an action?
        while (fAddX[nLoop] >= 1.0f)
        {
            if (bMoveLeft[nLoop])
            {
                // Move the particle 1 pixel left on the x-axis.
                m_gsParticle.AddDestX(nLoop, -1);
            }
            else
            {
                // Move the particle 1 pixel right on the x-axis.
                m_gsParticle.AddDestX(nLoop, +1);
            }
            // One action less to be taken.
            fAddX[nLoop] -= 1.0f;
        }
        // Determine the interval required to move the particle at the specified speed, 60
        // times every second at the current frame rate (on the y-axis) and add it to
        // the previous intervals.
        fAddY[nLoop] += (this->GetActionInterval(60) * nMovementRateY[nLoop]);
        // Have the intervals added up enough to take an action?
        while (fAddY[nLoop] >= 1.0f)
        {
            if (bMoveUp[nLoop])
            {
                // Move the particle 1 pixel up on the y-axis.
                m_gsParticle.AddDestY(nLoop, +1);
            }
            else
            {
                // Move the particle 1 pixel down on the y-axis.
                m_gsParticle.AddDestY(nLoop, -1);
            }
            // One action less to be taken.
            fAddY[nLoop] -= 1.0f;
        }
        // Scale the particle horizontally at the specified rate 60 times every second.
        m_gsParticle.AddScaleX(nLoop, this->GetActionInterval(60) * fAddScale);
        if (m_gsParticle.GetScaleX(nLoop) < 1.0f)
        {
            m_gsParticle.SetScaleX(nLoop, 1.0f);
        }
        // Scale the particle vertically at the specified rate 60 times every second.
        m_gsParticle.AddScaleY(nLoop, this->GetActionInterval(60) * fAddScale);
        if (m_gsParticle.GetScaleY(nLoop) < 1.0f)
        {
            m_gsParticle.SetScaleY(nLoop, 1.0f);
        }
        // Has the specified particle left the screen to the right, left, top or bottom?
        if ((m_gsParticle.GetDestX(nLoop) >= INTERNAL_RES_X)  ||
                (m_gsParticle.GetDestX(nLoop) <= 0 - m_gsParticle.GetScaledWidth(nLoop)) ||
                (m_gsParticle.GetDestY(nLoop) >= INTERNAL_RES_Y) ||
                (m_gsParticle.GetDestY(nLoop) <= 0 - m_gsParticle.GetScaledHeight(nLoop)))
        {
            // Determine a new random movement speed.
            nMovementRateX[nLoop] = rand()%9 + 2;
            nMovementRateY[nLoop] = rand()%9 + 2;
            // Set horizontal direction randomly.
            if (rand()%2 == 1)
            {
                bMoveLeft[nLoop] = !bMoveLeft[nLoop];
            }
            // Set vertical direction randomly.
            if (rand()%2 == 1)
            {
                bMoveUp[nLoop] = !bMoveUp[nLoop];
            }
            // Determine random particle scale.
            float fScale = 0.1f + float(rand()%11) / 5.0f;
            m_gsParticle.SetScaleX(nLoop, fScale);
            m_gsParticle.SetScaleY(nLoop, fScale);
            // Set starting position in the centre of the screen.
            if (bMoveLeft[nLoop])
            {
                m_gsParticle.SetDestX(nLoop, (INTERNAL_RES_X/2) -
                                      (100-10*nMovementRateY[nLoop]));
            }
            else
            {
                m_gsParticle.SetDestX(nLoop, (INTERNAL_RES_X/2) +
                                      (100-10*nMovementRateY[nLoop]));
            }
            m_gsParticle.SetDestY(nLoop, INTERNAL_RES_Y / 2 - 35);
            // Select a random modulate color for the specified particle.
            m_gsParticle.SetModulateColor(nLoop, float((rand()%100)+1) / 100,
                                          float((rand()%100)+1) / 100,
                                          float((rand()%100)+1) / 100,
                                          glfTransparency);
        }
    }

    // Render all active particles.
    m_gsParticle.Render();

    /////////////////////////////////////////////////////////////////////////////////////////////

    // Display the frames per second at the top left corner.
    m_gsSmallFont.SetText("%0.2f", this->GetCurrentFrameRate());
    m_gsSmallFont.SetDestXY(16, INTERNAL_RES_Y - m_gsSmallFont.GetTextHeight() -16);
    m_gsSmallFont.Render();

    // Display the number of particles in the top right corner.
    m_gsSmallFont.SetText("%04d", nNumParticles);
    m_gsSmallFont.SetDestX(INTERNAL_RES_X - m_gsSmallFont.GetTextWidth()  -16);
    m_gsSmallFont.SetDestY(INTERNAL_RES_Y - m_gsSmallFont.GetTextHeight() -16);
    m_gsSmallFont.Render();

    // Display the title of the demo centered at the bottom.
    m_gsLargeFont.SetModulateColor(-1.0f, -1.0f, -1.0f, glfTransparency);
    m_gsLargeFont.SetText("PARTICLES");
    m_gsLargeFont.SetDestXY((INTERNAL_RES_X - m_gsLargeFont.GetTextWidth()) / 2, 16);
    m_gsLargeFont.Render();

    /////////////////////////////////////////////////////////////////////////////////////////////

    // Reset display after rendering in 2D.
    m_gsDisplay.EndRender2D();

    // Unbind all textures.
    glBindTexture(GL_TEXTURE_2D, 0);

    // Swap buffers (double buffering) to display results.
    SwapBuffers(this->GetDevice());

    return TRUE;
}


////////////////////////////////////////////////////////////////////////////////////////////////
// Helper Methods //////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_Demo::SetRenderScaling():
// ---------------------------------------------------------------------------------------------
// Purpose: Set the values used for upscaling or downscaling when rendering
// ---------------------------------------------------------------------------------------------
// Returns: Nothing
//==============================================================================================

void GS_Demo::SetRenderScaling(int nWidth, int nHeight, bool bKeepAspect)
{
    // Is no scaling required?
    if( (nWidth == INTERNAL_RES_X) && (nHeight == INTERNAL_RES_Y) )
    {
        // Reset the default scaling values
        GS_OGLDisplay::SetScaleFactorX(1.0f);
        GS_OGLDisplay::SetScaleFactorY(1.0f);

        GS_OGLDisplay::SetRenderModX(0.0f);
        GS_OGLDisplay::SetRenderModY(0.0f);

        return;
    }

    // Determine the aspect ratio of the internal resolution
    float aspect = (float)INTERNAL_RES_X / (float)INTERNAL_RES_Y;

    // Set up default values for upscaling or downscaling
    float fAspectWidth  = (float)nWidth;
    float fAspectHeight = (float)nHeight;
    float fRenderModX = 0;
    float fRenderModY = 0;

    // Should we keep the aspect ratio?
    if( bKeepAspect )
    {
        // Is the horizontal dimension greater than the vertical?
        if( INTERNAL_RES_X >= INTERNAL_RES_Y )
        {
            // Try to scale the rendering to the given width
            fAspectWidth  = (float)nWidth;
            fAspectHeight = (float)fAspectWidth / aspect;
            fRenderModX = 0;
            fRenderModY = (nHeight - fAspectHeight) / 2.0f;

            // Have we scaled beyond the given height
            if( fAspectHeight > (float)nHeight )
            {
                // Scale the rendering to the given height
                fAspectHeight = (float)nHeight;
                fAspectWidth  = fAspectHeight * aspect;
                fRenderModX = (nWidth - fAspectWidth) / 2.0f;
                fRenderModY = 0;
            }
        }
        else
        {
            // Try to scale the rendering to the given height
            fAspectHeight = (float)nHeight;
            fAspectWidth  = fAspectHeight * aspect;
            fRenderModX = (nWidth - fAspectWidth) / 2.0f;
            fRenderModY = 0;

            // Have we scaled beyond the given width
            if( fAspectWidth > (float)nWidth )
            {
                // Scale the rendering to the given width
                fAspectWidth  = (float)nWidth;
                fAspectHeight = (float)fAspectWidth / aspect;
                fRenderModX = 0;
                fRenderModY = (nHeight - fAspectHeight) / 2.0f;
            }
        }
    }

    // Set the amount with which the rendering coordinates should be modified
    GS_OGLDisplay::SetRenderModX(fRenderModX);
    GS_OGLDisplay::SetRenderModY(fRenderModY);

    // Set the scale factor to effect upscaling or downscaling depending on the resolution
    GS_OGLDisplay::SetScaleFactorX((float)fAspectWidth  / (float)INTERNAL_RES_X);
    GS_OGLDisplay::SetScaleFactorY((float)fAspectHeight / (float)INTERNAL_RES_Y);
}


////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_Demo::GetActionInterval():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Returns: The interval into which any action or actions must be divided in order to be
//          completed in a second at the current frame rate of the application.
//==============================================================================================

float GS_Demo::GetActionInterval(float fActionsPerSecond)
{

    // Convert frame time to seconds and multiply it by the number of actions to determine
    // what fraction of the action needs to be completed in order to complete the action in
    // one second (at the current frame rate).
    return ((this->GetFrameTime() / 1000) * fActionsPerSecond);
}


////////////////////////////////////////////////////////////////////////////////////////////////

// GS_Error::Report("GS_DEMO.CPP", 441, "Break Point!");

// (a, b)(float)((double)(a) / (double)(b))
