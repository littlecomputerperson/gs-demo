/*============================================================================================+
 | Game System (GS) Library                                                                   |
 |--------------------------------------------------------------------------------------------|
 | FILES: gs_app.cpp, gs_app.h                                                                |
 |--------------------------------------------------------------------------------------------|
 | CLASS: GS_Application                                                                      |
 |--------------------------------------------------------------------------------------------|
 | ABOUT: ...                                                                                 |
 |--------------------------------------------------------------------------------------------|
 |                                                                                    08/2003 |
 +============================================================================================*/


//==============================================================================================
// Include Game System (GS) header files.
// ---------------------------------------------------------------------------------------------
#include "gs_app.h"
#include "gs_keyboard.h"
//==============================================================================================

//==============================================================================================
// Global access to the application
// ---------------------------------------------------------------------------------------------
GS_Application* g_pGSApp = NULL;
//==============================================================================================

#ifdef GS_PLATFORM_WINDOWS

//==============================================================================================
// Global Window Procedure /////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// WndProc()
// ---------------------------------------------------------------------------------------------
// Purpose: Static messsage handler which passes messages to the application class.
// ---------------------------------------------------------------------------------------------
// Returns: LRESULT depending on message received.
//==============================================================================================

LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{

    // Pass message to application message procedure.
    return g_pGSApp->MsgProc(hWnd, uMsg, wParam, lParam);
}


////////////////////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////////////////////
// Constructor/Destructor Methods //////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////

//==============================================================================================
// GS_Application::GS_Application():
// ---------------------------------------------------------------------------------------------
// Purpose: The constructor, initializes class data to defaults when class object is created.
// ---------------------------------------------------------------------------------------------
// Returns: Nothing.
//==============================================================================================

GS_Application::GS_Application()
{

    g_pGSApp = this;

    m_hInstance = NULL;
    m_hWnd      = NULL;
    m_hDC       = NULL;
    m_hAccel    = NULL;
    m_hMenu     = NULL;

    m_bIsActive   = FALSE;
    m_bIsReady    = FALSE;
    m_bIsPaused   = FALSE;
    m_bIsWindowed = FALSE;

    m_nDisplayWidth  = 0;
    m_nDisplayHeight = 0;
    m_nColorDepth    = 0;

    m_dwWindowStyle   = 0;
    m_dwWindowStyleEx = 0;

    m_rcWindowBounds.left   = 0;
    m_rcWindowBounds.top    = 0;
    m_rcWindowBounds.right  = 0;
    m_rcWindowBounds.bottom = 0;

    m_rcWindowClient.left   = 0;
    m_rcWindowClient.top    = 0;
    m_rcWindowClient.right  = 0;
    m_rcWindowClient.bottom = 0;

    m_fFrameRate = GS_DEFAULT_FPS;
    m_fFrameTime = 0.0f;
    m_fWaitTime  = 0.0f;
}


////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_Application::~GS_Application():
// ---------------------------------------------------------------------------------------------
// Purpose: The de-structor, de-initializes class data when class object is destroyed.
// ---------------------------------------------------------------------------------------------
// Returns: Nothing.
//==============================================================================================

GS_Application::~GS_Application()
{

    if (m_bIsReady)
    {
        Destroy();
    }
}


////////////////////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////////////////////
// Create/Destroy Methods //////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////

//==============================================================================================
// GS_Application::Create():
// ---------------------------------------------------------------------------------------------
// Purpose: Creates and registers the application window, sets up the display and initializes
//          the application timer.
// ---------------------------------------------------------------------------------------------
// Returns: TRUE if successful, FALSE if failed.
//==============================================================================================

BOOL GS_Application::Create(int nWidth, int nHeight, int nDepth, BOOL bIsWindowed)
{
    // Has application been created?
    if (m_bIsReady)
    {
        return FALSE;
    }

    // Get application instance.
    if (!m_hInstance)
    {
        m_hInstance = GetModuleHandle(NULL);
    }

    // Were all the required values provided?
    if ((nWidth>0) && (nHeight>0))
    {
        // Set the width and height
        m_nDisplayWidth  = nWidth;
        m_nDisplayHeight = nHeight;

        // Is color depth valid?
        switch(nDepth)
        {
            case 8:  // Fall through.
            case 16: // Fall through.
            case 24: // Fall through.
            case 32: // Fall through.
                // Do nothing, color depth OK.
                m_nColorDepth = nDepth;
                break;
            default:
                m_nColorDepth = GS_DEFAULT_DEPTH;
                break;
        } // end switch(nDepth)

        // Set the mode
        m_bIsWindowed = bIsWindowed;

        // Is it a fullscreen application?
        if( !m_bIsWindowed )
        {
            // Ignore the given settings and use the current screen width and height
            m_nDisplayWidth  = GetSystemMetrics(SM_CXSCREEN);
            m_nDisplayHeight = GetSystemMetrics(SM_CYSCREEN);
        }
    }
    else
    {
        // Setup display with default values.
        m_nDisplayWidth  = GS_DEFAULT_WIDTH;
        m_nDisplayHeight = GS_DEFAULT_HEIGHT;
        m_nColorDepth    = GS_DEFAULT_DEPTH;
        m_bIsWindowed    = GS_DEFAULT_MODE;
    }

    // Setup the windows class.
    WNDCLASSEX wc;
    // Make sure memory is cleared.
    ZeroMemory(&wc, sizeof(WNDCLASSEX));

    // Size of the windows class structure.
    wc.cbSize = sizeof(WNDCLASSEX);
    // Specifies the class style(s).
    wc.style = CS_VREDRAW | CS_HREDRAW | CS_OWNDC;
    // Points to the window procedure.
    wc.lpfnWndProc = WndProc;
    // Specifies no extra window data.
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    // Identifies the instance of the application.
    wc.hInstance = m_hInstance;
    // Identifies the class icon.
    wc.hIcon = LoadIcon(m_hInstance, MAKEINTRESOURCE(IDI_MAIN));
    // Identifies the class cursor.
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    // Set windows background to white.
    wc.hbrBackground = (HBRUSH) (COLOR_BACKGROUND+1);

    /////////////////////////////////////////////////////////////////////////////////////////////
    //###########################################################################################
#ifdef GS_ENABLE_MENU
    // Enable menu only in windowed mode.
    if (m_bIsWindowed)
    {
        wc.lpszMenuName = MAKEINTRESOURCE(IDR_MENU);
    }
#endif
    //###########################################################################################
    /////////////////////////////////////////////////////////////////////////////////////////////

    // Specifies the window class name.
    wc.lpszClassName = "GSWindow";

    // Are we able to register the window class?
    if (RegisterClassEx(&wc) == 0)
    {
        // Report error if failed to register window.
        GS_Error::Report("GS_APP.CPP", 217, "Failed to register application window!");
        return FALSE;
    }

    /////////////////////////////////////////////////////////////////////////////////////////////
    //###########################################################################################
#ifdef GS_ENABLE_ACCELERATORS
    // Load keyboard accelerators.
    m_hAccel = LoadAccelerators(m_hInstance, MAKEINTRESOURCE(IDR_MAIN_ACCEL));
#endif
    //###########################################################################################
    /////////////////////////////////////////////////////////////////////////////////////////////

    /////////////////////////////////////////////////////////////////////////////////////////////
    //###########################################################################################
#ifdef GS_ENABLE_MENU
    // Load the menu.
    m_hMenu = LoadMenu(m_hInstance, MAKEINTRESOURCE(IDR_MENU));
#endif
    //###########################################################################################
    /////////////////////////////////////////////////////////////////////////////////////////////

    RECT rc;

    // Assign the window's required client size to a RECT structure.
    SetRect(&rc, 0, 0, m_nDisplayWidth, m_nDisplayHeight);

    // Assign the window's position to the top left of the screen.
    int nWindowPosX = 0;
    int nWindowPosY = 0;

    // Are we creating a windowed application?
    if (m_bIsWindowed)
    {
        // Specify window style for windowed mode.
        m_dwWindowStyle = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX | WS_VISIBLE;
        m_dwWindowStyleEx = WS_EX_APPWINDOW;
        // Determine the actual size the window has to be to have the required client size, taking
        // into account the style of the window as well as the window's menu in windowed mode.
        //////////////////////////////////////////////////////////////////////////////////////////
        //########################################################################################
#ifdef GS_ENABLE_MENU
        AdjustWindowRectEx(&rc, m_dwWindowStyle, TRUE, m_dwWindowStyleEx);
#else
        AdjustWindowRectEx(&rc, m_dwWindowStyle, FALSE, m_dwWindowStyleEx);
#endif
        //########################################################################################
        //////////////////////////////////////////////////////////////////////////////////////////
        RECT rcTemp;
        // Determine the current desktop dimensions.
        SystemParametersInfo(SPI_GETWORKAREA, 0, &rcTemp, 0);
        // Determine the position of the window centered horizontally on the desktop.
        if ((rc.right - rc.left) < (rcTemp.right - rcTemp.left))
        {
            nWindowPosX = ((rcTemp.right - rcTemp.left) - (rc.right - rc.left)) / 2;
        }
        // Determine the position of the window centered vertically on the desktop.
        if ((rc.bottom - rc.top) < (rcTemp.bottom - rcTemp.top))
        {
            nWindowPosY = ((rcTemp.bottom - rcTemp.top) - (rc.bottom - rc.top)) / 2;
        }
    }
    else
    {
        // Specify window style for fullscreen mode.
        m_dwWindowStyle   = WS_POPUP | WS_VISIBLE;
        m_dwWindowStyleEx = WS_EX_TOPMOST;
        // Determine the actual size the window has to be to have the required client size, taking
        // into account the style of the window.
        AdjustWindowRectEx(&rc, m_dwWindowStyle, FALSE, m_dwWindowStyleEx);
    }

    // Create the application window.
    m_hWnd = CreateWindowEx
    (
        m_dwWindowStyleEx,
        "GSWindow",
        "Game System OpenGL",
        m_dwWindowStyle | WS_CLIPCHILDREN | WS_CLIPSIBLINGS,
        nWindowPosX, nWindowPosY,
        (rc.right - rc.left),
        (rc.bottom - rc.top),
        NULL,
        NULL,
        m_hInstance,
        NULL
    );

    // Has window been created succesfully?
    if (m_hWnd == NULL)
    {
        // Report error if failed to create window.
        GS_Error::Report("GS_APP.CPP", 293, "Failed to create application window!");
        return FALSE;
    }

    // Show the window and update it.
    ShowWindow(m_hWnd, SW_SHOWNORMAL);
    UpdateWindow(m_hWnd);

    // Slightly higher priority.
    SetForegroundWindow(m_hWnd);

    /////////////////////////////////////////////////////////////////////////////////////////////
    //###########################################################################################
#ifdef GS_ENABLE_MODESWITCH
    // Do we want to run in fullscreen mode?
    if (!m_bIsWindowed)
    {
        // Wait a short time before changing to fullscreen mode. This is necesarry because of
        // a bug that causes the application to loose the mouse focus if it is switched to
        // another resolution immediately before or after the new window is created.
        m_gsFrameTimer.Wait(100);
        // Setup device mode for change to fullscreen mode.
        DEVMODE dmDisplayMode;
        memset(&dmDisplayMode, 0, sizeof(dmDisplayMode)); // Make sure memory is cleared.
        // Are we able to enumerate the current display settings?
        if (!EnumDisplaySettings(NULL, ENUM_CURRENT_SETTINGS, &dmDisplayMode))
        {
            // Report error if failed to enumerate display settings.
            GS_Error::Report("GS_APP.CPP", 319, "Failed to enum display settings!");
            return FALSE;
        }
        // dmDisplayMode.dmSize = sizeof(dmDisplayMode);     // Size of the DEVMODE structure.
        // dmDisplayMode.dmPelsWidth  = m_nDisplayWidth;     // Selected screen width.
        // dmDisplayMode.dmPelsHeight = m_nDisplayHeight;    // Selected screen height.
        // dmDisplayMode.dmBitsPerPel = m_nColorDepth;       // Selected color depth.
        dmDisplayMode.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;
        // Are we able to change to fullscreen mode?
        if (ChangeDisplaySettings(&dmDisplayMode, CDS_FULLSCREEN) != DISP_CHANGE_SUCCESSFUL)
        {
            // Report error if failed to change to fullscreen mode.
            GS_Error::Report("GS_APP.CPP", 330, "Failed to enter fullscreen mode!");
            return FALSE;
        }
    }
#endif
    //###########################################################################################
    /////////////////////////////////////////////////////////////////////////////////////////////

    // Save window properties.
    m_dwWindowStyle = GetWindowLong(m_hWnd, GWL_STYLE);
    GetWindowRect(m_hWnd, &m_rcWindowBounds);
    GetClientRect(m_hWnd, &m_rcWindowClient);

    // Get a GDI device context for the application.
    m_hDC = GetDC(m_hWnd);

    // Did we get a valid device context?
    if (!m_hDC)
    {
        // Report error if failed to get a device context.
        GS_Error::Report("GS_APP.CPP", 349, "Failed to get a device context!");
        return FALSE;
    }

    // Can we initialize the game?
    if (TRUE != this->GameInit())
    {
        // Report error if failed to initialize game.
        GS_Error::Report("GS_APP.CPP", 356, "Failed to initialize game!");
        return FALSE;
    }

    // Mark the beginning the first frame.
    m_gsFrameTimer.MarkFrame();

    // Set flags to indicate application has been created successfully.
    m_bIsReady  = TRUE;
    m_bIsActive = TRUE;

    return TRUE;
}


////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_Application::SetMode():
// ---------------------------------------------------------------------------------------------
// Purpose: Sets the display dimensions, color depth and mode.
// ---------------------------------------------------------------------------------------------
// Returns: TRUE if successfull, FALSE if failed.
//==============================================================================================

BOOL GS_Application::SetMode(int nWidth, int nHeight, int nDepth, BOOL bIsWindowed)
{
    // Has the application not been created?
    if( !m_bIsReady )
    {
        // Were all the required values provided?
        if ((nWidth>0) && (nHeight>0))
        {
            // Set the width and height
            m_nDisplayWidth  = nWidth;
            m_nDisplayHeight = nHeight;

            // Is color depth valid?
            switch(nDepth)
            {
                case 8:  // Fall through.
                case 16: // Fall through.
                case 24: // Fall through.
                case 32: // Fall through.
                    // Do nothing, color depth OK.
                    m_nColorDepth = nDepth;
                    break;
                default:
                    m_nColorDepth = GS_DEFAULT_DEPTH;
                    break;
            } // end switch(nDepth)

            // Set the mode
            m_bIsWindowed = bIsWindowed;

            // Is it a fullscreen application?
            if( !m_bIsWindowed )
            {
                // Ignore the given settings and use the current screen width and height
                m_nDisplayWidth  = GetSystemMetrics(SM_CXSCREEN);
                m_nDisplayHeight = GetSystemMetrics(SM_CYSCREEN);
            }
        }
        else
        {
            // Setup display with default values.
            m_nDisplayWidth  = GS_DEFAULT_WIDTH;
            m_nDisplayHeight = GS_DEFAULT_HEIGHT;
            m_nColorDepth    = GS_DEFAULT_DEPTH;
            m_bIsWindowed    = GS_DEFAULT_MODE;
        }

        // Exit method
        return this->Create(nWidth, nHeight, nDepth, bIsWindowed);
    }

    // Save previous display settings.
    int  nPrevWidth   = m_nDisplayWidth;
    int  nPrevHeight  = m_nDisplayHeight;
    int  nPrevDepth   = m_nColorDepth;
    BOOL bWasWindowed = m_bIsWindowed;

    // Destroy the application
    if( m_bIsActive ) Destroy();

    // Has application been recreated successfully using the new settings?
    if (FALSE == Create(nWidth, nHeight, nDepth, bIsWindowed))
    {
        // Destroy the application.
        Destroy();

        // Report error if failed to change display mode.
        GS_Error::Report("GS_APP.CPP", 425, "Failed to change display mode, using old settings!");

        // Recreate application using the old settings?
        if (FALSE == Create(nPrevWidth, nPrevHeight, nPrevDepth, bWasWindowed))
        {
            GS_Error::Report("GS_APP.CPP", 428, "Failed to recreate window!");
            PostQuitMessage(0);
        }
    }

    return TRUE;
}


////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_Application::Destroy():
// ---------------------------------------------------------------------------------------------
// Purpose: Destroys the application and frees all memory associated with it.
// ---------------------------------------------------------------------------------------------
// Returns: Nothing.
//==============================================================================================

void GS_Application::Destroy()
{

    GameShutdown(); // Do custom shutdown.

    // Set flags to indicate window has been destroyed.
    m_bIsActive = FALSE;
    m_bIsReady  = FALSE;

    ShowWindow(m_hWnd, SW_HIDE);    // Hide the application window.

    // Are we in fullscreen mode?
    if (!m_bIsWindowed)
    {
        ChangeDisplaySettings(NULL, 0); // Return to desktop settings.
    }

    /////////////////////////////////////////////////////////////////////////////////////////////
    //###########################################################################################
#ifdef GS_ENABLE_MENU
    // Destroy the menu (if any).
    DestroyMenu(GetMenu(m_hWnd));
#endif
    //###########################################################################################
    /////////////////////////////////////////////////////////////////////////////////////////////

    // Are we able to release the device context?
    if (m_hDC && !ReleaseDC(m_hWnd, m_hDC))
    {
        // Report error if failed to create window.
        GS_Error::Report("GS_APP.CPP", 475, "Failed to release device context!");
    }
    m_hDC = NULL;

    // Are we able to destroy the application window?
    if (m_hWnd && !DestroyWindow(m_hWnd))
    {
        // Report error if failed to create window.
        GS_Error::Report("GS_APP.CPP", 482, "Failed to destroy application window!");
    }
    m_hWnd=NULL;

    // Are we able to unregister the window class?
    if (!UnregisterClass("GSWindow", m_hInstance))
    {
        // Report error if failed unregister the window class.
        GS_Error::Report("GS_APP.CPP", 489, "Failed to unregister the window class!");
    }
    m_hInstance = NULL;
}


////////////////////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////////////////////
// Message Handling ////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_Application::Run():
// ---------------------------------------------------------------------------------------------
// Purpose: The main loop used to receive and dispatch messages to the windows procedure and run
//          the application in general.
// ---------------------------------------------------------------------------------------------
// Returns: An integer value depending on the message received.
//==============================================================================================

INT GS_Application::Run()
{

    MSG msg;
    msg.message = WM_NULL;

    // Determine the time to wait (in milliseconds) before processing next frame.
    if (m_fFrameRate > 0.0f)
    {
        m_fWaitTime = 1000.0f / m_fFrameRate;
    }
    else
    {
        m_fWaitTime = 0.0f;
    }

    // Recieve and process windows messages.
    while (WM_QUIT != msg.message)
    {
        // Check if there is a message (without removing it from queue).
        if (PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE))
        {
            // If there was a message, get it (remove it from queue).
            GetMessage(&msg, NULL, 0, 0);
            ///////////////////////////////////////////////////////////////////////////////////////
            //#####################################################################################
#ifdef GS_ENABLE_ACCELERATORS
            // Process accelerator keys for menu commands.
            if (0 == TranslateAccelerator(m_hWnd, m_hAccel, &msg))
            {
                // Translate and dispatch the message.
                TranslateMessage(&msg);
                DispatchMessage(&msg);
            }
#else
            // Translate and dispatch the message.
            TranslateMessage(&msg);
            DispatchMessage(&msg);
#endif
            //#####################################################################################
            ///////////////////////////////////////////////////////////////////////////////////////
        }
        else
        {
            // Render a frame during idle time.
            if (m_bIsActive && m_bIsReady)
            {
                // Wait for the minimum time required by frame-rate.
                if ((m_fFrameTime = m_gsFrameTimer.GetFrameTime()) >= m_fWaitTime)
                {
                    // Mark the next frame to be measured.
                    m_gsFrameTimer.MarkFrame();
                    // Run game loop once each frame, check if successfull.
                    if (TRUE != GameLoop())
                    {
                        GS_Error::Report("GS_APP.CPP", 557, "Error running main game loop!");
                        PostQuitMessage(0);
                    }
                }
            }
            else if (!m_bIsActive)
            {
                // Wait for new messages.
                WaitMessage();
            }
        }
    } // end while (WM_QUIT != msg.message...

    // Free all resources used by application.
    this->Destroy();

    // Show average framerate.
    // ShowFrameRate();

    return (INT) msg.wParam;
}


////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_Application::MsgProc():
// ---------------------------------------------------------------------------------------------
// Purpose: The applications message handler used to take the appropriate action depending on
//          the message received.
// ---------------------------------------------------------------------------------------------
// Returns: LRESULT depending on message received.
//==============================================================================================

LRESULT GS_Application::MsgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{

    PAINTSTRUCT paintStruct;
    HDC hDC;

    switch (uMsg)
    {

    // Received when a window belonging to a different application than the active window is
    // about to be activated. The message is sent to the application whose window is being
    // activated and to the application whose window is being deactivated.
    case WM_ACTIVATEAPP:
        // Is application about to lose focus?
        if ((LOWORD(wParam)==FALSE) && (m_bIsActive) && (m_bIsReady))
        {
            // Release the game when application loses focus.
            if (GameRelease() != TRUE)
            {
                // Report error if not able to release the game.
                GS_Error::Report("GS_APP.CPP", 607, "Unable to release game!");
                PostQuitMessage(0);
            }
            ////////////////////////////////////////////////////////////////////////////////////
            //##################################################################################
#ifdef GS_ENABLE_MODESWITCH
            // Is it a fullscreen application?
            // if (!m_bIsWindowed) {
            // Return to desktop settings.
            // ChangeDisplaySettings(NULL, 0);
            // Minimize the window.
            // ShowWindow(m_hWnd, SW_MINIMIZE);
            // }
#endif
            //##################################################################################
            ////////////////////////////////////////////////////////////////////////////////////
            // Set flag to indicate application is NOT active.
            m_bIsActive = FALSE;
        }
        // Is application about to receive focus?
        else if ((LOWORD(wParam)==TRUE) && (!m_bIsActive) && (m_bIsReady))
        {
            ////////////////////////////////////////////////////////////////////////////////////
            //##################################################################################
#ifdef GS_ENABLE_MODESWITCH
            // Is it a fullscreen application?
            // if ((!m_bIsWindowed) && (!m_bIsActive)) {
            // Setup device mode for change to fullscreen mode.
            // DEVMODE dmDisplayMode;
            // memset(&dmDisplayMode, 0, sizeof(dmDisplayMode));
            // dmDisplayMode.dmSize = sizeof(dmDisplayMode);
            // dmDisplayMode.dmPelsWidth  = m_nDisplayWidth;
            // dmDisplayMode.dmPelsHeight = m_nDisplayHeight;
            // dmDisplayMode.dmBitsPerPel = m_nColorDepth;
            // dmDisplayMode.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;
            // Change back to fullscreen mode.
            // ChangeDisplaySettings(&dmDisplayMode, CDS_FULLSCREEN);
            // Restore the window.
            // ShowWindow(m_hWnd, SW_RESTORE);
            // }
#endif
            //##################################################################################
            ////////////////////////////////////////////////////////////////////////////////////
            // Restore the game when application regains focus.
            if (GameRestore() != TRUE)
            {
                // Report error if not able to restore the game.
                GS_Error::Report("GS_APP.CPP", 652, "Unable to restore game!");
                PostQuitMessage(0);
            }
            // Mark the beginning of a new frame.
            m_gsFrameTimer.MarkFrame();
            // Set flag to indicate window IS active.
            m_bIsActive = TRUE;
        }
        break;

    // Received when the user selects a command item from a menu, when a control sends a
    // notification message to its parent window, or when an accelerator keystroke is
    // translated.
    case WM_COMMAND:
        // Check for message sent by accelerator.
        switch(LOWORD(wParam))
        {
        // The user pressed Alt+X or selected quit from menu.
        case IDM_QUIT:
            // Post a quit message in the message queue.
            PostQuitMessage(0);
            return 0L;
        // The user pressed ALT+ENTER or selected toggle fullscreen from menu.
        case IDM_CHANGEMODE:
            // Call the function for changing modes
            this->OnChangeMode();
            return 0L;
        } // end switch(LOWORD(wParam...
        break;

    // Received when window is closed.
    case WM_CLOSE:
        // Post a quit message in the message queue.
        PostQuitMessage(0);
        return 0L;

    // Received when Windows or another application makes a request to paint a portion of
    // an application's window.
    case WM_PAINT:
        // Tell windows we want to update the window.
        hDC = BeginPaint(hWnd, &paintStruct);
        // ...
        // Do GDI drawing here.
        // ...
        EndPaint(hWnd, &paintStruct);
        DeleteDC(hDC);
        break;

    // Received when the user chooses a command from the window menu or when the user chooses
    // the Maximize button or Minimize button.
    case WM_SYSCOMMAND:
        // Prevent power loss, resizing etc. in full-screen mode.
        switch (wParam)
        {
        case SC_MAXIMIZE:
        case SC_MOVE:
        case SC_SIZE:
        case SC_KEYMENU:
        case SC_MONITORPOWER:
            if (!m_bIsWindowed)
                return TRUE;
        }
        break;

    } // end switch (uMsg...


    // Provide default processing for unprocessed messages.
    return DefWindowProc(hWnd, uMsg, wParam, lParam);
}


////////////////////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////////////////////
// Utility Methods /////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_Application::SetFrameRate():
// ---------------------------------------------------------------------------------------------
// Purpose: Sets a new frame rate for the application.
// ---------------------------------------------------------------------------------------------
// Returns: Nothing.
//==============================================================================================

void GS_Application::SetFrameRate(float fFrameRate)
{

    // Assign the new frame rate.
    m_fFrameRate = fFrameRate;

    // Determine the time to wait (in milliseconds) before processing next frame.
    if (m_fFrameRate > 0.0f)
    {
        m_fWaitTime = 1000.0f / m_fFrameRate;
    }
    else
    {
        m_fWaitTime = 0.0f;
    }
}


////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_Application::ShowFrameRate():
// ---------------------------------------------------------------------------------------------
// Purpose: To show a message box displaying the running time and average frame-rate of the
//          application.
// ---------------------------------------------------------------------------------------------
// Returns: Nothing.
//==============================================================================================

void GS_Application::ShowFrameRate()
{

    char outputString[128] = { 0 };

    // Convert the frame-rate information into a string that can be displayed in a message box.
    sprintf(outputString, "%s%0.2f%s", "The program ran at an average of ",
            m_gsFrameTimer.GetFrameRate(), " frames per second.");

    // Display frame-rate information.
    MessageBox(NULL, outputString, "GAME INFORMATION", MB_OK | MB_ICONINFORMATION);
}


////////////////////////////////////////////////////////////////////////////////////////////////

#else

//==============================================================================================
// SDL Implementation
// ---------------------------------------------------------------------------------------------

#ifdef __APPLE__
#include <mach-o/dyld.h>
#include <unistd.h>
#include <libgen.h>
#include <CoreFoundation/CoreFoundation.h>
#endif

GS_Application::GS_Application()
{
    g_pGSApp = this;

    m_pWindow = NULL;
    m_glContext = NULL;

    m_bIsActive   = FALSE;
    m_bIsReady    = FALSE;
    m_bIsPaused   = FALSE;
    m_bIsWindowed = TRUE;

    m_nDisplayWidth  = 0;
    m_nDisplayHeight = 0;
    m_nColorDepth    = 0;

    m_dwWindowStyle   = 0;
    m_dwWindowStyleEx = 0;

    memset(&m_rcWindowBounds, 0, sizeof(RECT));
    memset(&m_rcWindowClient, 0, sizeof(RECT));

    m_fFrameRate = GS_DEFAULT_FPS;
    m_fFrameTime = 0.0f;
    m_fWaitTime  = 0.0f;
}

GS_Application::~GS_Application()
{

    if (m_bIsReady)
    {
        Destroy();
    }
}

BOOL GS_Application::Create(int nWidth, int nHeight, int nDepth, BOOL bIsWindowed)
{
    if (m_bIsReady)
    {
        return FALSE;
    }

#ifdef __APPLE__
    // On macOS, change working directory to the Resources folder in the bundle
    // This fixes asset loading when launched from Finder
    CFBundleRef mainBundle = CFBundleGetMainBundle();
    if (mainBundle)
    {
        CFURLRef resourcesURL = CFBundleCopyResourcesDirectoryURL(mainBundle);
        if (resourcesURL)
        {
            char resourcesPath[PATH_MAX];
            if (CFURLGetFileSystemRepresentation(resourcesURL, TRUE, (UInt8*)resourcesPath, PATH_MAX))
            {
                chdir(resourcesPath);
            }
            CFRelease(resourcesURL);
        }
    }
    else
    {
        // Fallback to executable directory if not running as bundle
        char exePath[1024];
        uint32_t size = sizeof(exePath);
        if (_NSGetExecutablePath(exePath, &size) == 0)
        {
            char* dirPath = dirname(exePath);
            chdir(dirPath);
        }
    }
#endif

    // Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS) < 0)
    {
        GS_Error::Report("GS_APP.CPP", 50, "Failed to initialize SDL!");
        return FALSE;
    }

    // Set OpenGL attributes
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

    m_nDisplayWidth  = (nWidth > 0) ? nWidth : GS_DEFAULT_WIDTH;
    m_nDisplayHeight = (nHeight > 0) ? nHeight : GS_DEFAULT_HEIGHT;
    m_nColorDepth    = (nDepth > 0) ? nDepth : GS_DEFAULT_DEPTH;
    m_bIsWindowed    = bIsWindowed;

    // Get desktop dimensions for fullscreen mode
    if (!bIsWindowed)
    {
        SDL_DisplayMode dm;
        if (SDL_GetDesktopDisplayMode(0, &dm) == 0)
        {
            m_nDisplayWidth = dm.w;
            m_nDisplayHeight = dm.h;
        }
    }

    Uint32 flags = SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN;
    if (!bIsWindowed)
    {
        flags |= SDL_WINDOW_FULLSCREEN_DESKTOP;
    }

    m_pWindow = SDL_CreateWindow("Game System OpenGL",
                                  SDL_WINDOWPOS_CENTERED,
                                  SDL_WINDOWPOS_CENTERED,
                                  m_nDisplayWidth,
                                  m_nDisplayHeight,
                                  flags);

    if (!m_pWindow)
    {
        GS_Error::Report("GS_APP.CPP", 70, "Failed to create SDL window!");
        SDL_Quit();
        return FALSE;
    }

    // Create OpenGL context
    m_glContext = SDL_GL_CreateContext(m_pWindow);
    if (!m_glContext)
    {
        GS_Error::Report("GS_APP.CPP", 77, "Failed to create OpenGL context!");
        SDL_DestroyWindow(m_pWindow);
        SDL_Quit();
        return FALSE;
    }

    // Make context current
    SDL_GL_MakeCurrent(m_pWindow, m_glContext);

    // Set up client rect
    m_rcWindowClient.left = 0;
    m_rcWindowClient.top = m_nDisplayHeight;
    m_rcWindowClient.right = m_nDisplayWidth;
    m_rcWindowClient.bottom = 0;

    if (TRUE != this->GameInit())
    {
        GS_Error::Report("GS_APP.CPP", 95, "Failed to initialize game!");
        return FALSE;
    }

    m_gsFrameTimer.MarkFrame();
    m_bIsReady  = TRUE;
    m_bIsActive = TRUE;

    return TRUE;
}

BOOL GS_Application::SetMode(int nWidth, int nHeight, int nDepth, BOOL bIsWindowed)
{
    if (!m_bIsReady)
    {
        return this->Create(nWidth, nHeight, nDepth, bIsWindowed);
    }

    int  nPrevWidth   = m_nDisplayWidth;
    int  nPrevHeight  = m_nDisplayHeight;
    int  nPrevDepth   = m_nColorDepth;
    BOOL bWasWindowed = m_bIsWindowed;

    if (m_bIsActive) Destroy();

    if (FALSE == Create(nWidth, nHeight, nDepth, bIsWindowed))
    {
        Destroy();
        GS_Error::Report("GS_APP.CPP", 105, "Failed to change display mode!");
        Create(nPrevWidth, nPrevHeight, nPrevDepth, bWasWindowed);
    }

    return TRUE;
}

void GS_Application::Destroy()
{
    GameShutdown();

    m_bIsActive = FALSE;
    m_bIsReady  = FALSE;

    if (m_glContext)
    {
        SDL_GL_DeleteContext(m_glContext);
        m_glContext = NULL;
    }

    if (m_pWindow)
    {
        SDL_DestroyWindow(m_pWindow);
        m_pWindow = NULL;
    }

    SDL_Quit();
}

INT GS_Application::Run()
{
    SDL_Event event;
    bool running = true;

    if (m_fFrameRate > 0.0f)
    {
        m_fWaitTime = 1000.0f / m_fFrameRate;
    }
    else
    {
        m_fWaitTime = 0.0f;
    }

    // Enable text input for WM_CHAR simulation
    SDL_StartTextInput();

    while (running)
    {
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_QUIT)
            {
                running = false;
            }
            else if (event.type == SDL_WINDOWEVENT)
            {
                if (event.window.event == SDL_WINDOWEVENT_FOCUS_LOST)
                {
                    if (m_bIsActive && m_bIsReady)
                    {
                        GameRelease();
                        m_bIsActive = FALSE;
                    }
                }
                else if (event.window.event == SDL_WINDOWEVENT_FOCUS_GAINED)
                {
                    if (!m_bIsActive && m_bIsReady)
                    {
                        GameRestore();
                        m_gsFrameTimer.MarkFrame();
                        m_bIsActive = TRUE;
                    }
                }
            }
            else if (event.type == SDL_TEXTINPUT)
            {
                // Simulate WM_CHAR for text input
                // SDL_TEXTINPUT provides UTF-8 text, convert to ASCII for WM_CHAR
                if (event.text.text[0] != '\0')
                {
                    unsigned char ch = (unsigned char)event.text.text[0];
                    MsgProc((HWND)m_pWindow, WM_CHAR, ch, 0);
                }
            }
            else if (event.type == SDL_KEYDOWN)
            {
                // Convert SDL scancode to GSK virtual key code
                int gskKey = GS_Keyboard::MapSDLKey(event.key.keysym.scancode);
                if (gskKey != GSK_NONE)
                {
                    MsgProc((HWND)m_pWindow, WM_KEYDOWN, gskKey, 0);
                    
                    // For special keys that don't generate SDL_TEXTINPUT, simulate WM_CHAR
                    if (event.key.keysym.sym == SDLK_RETURN ||
                        event.key.keysym.sym == SDLK_ESCAPE ||
                        event.key.keysym.sym == SDLK_BACKSPACE ||
                        event.key.keysym.sym == SDLK_TAB ||
                        event.key.keysym.sym == SDLK_DELETE)
                    {
                        MsgProc((HWND)m_pWindow, WM_CHAR, gskKey, 0);
                    }
                }
            }
            else if (event.type == SDL_KEYUP)
            {
                // Convert SDL scancode to GSK virtual key code
                int gskKey = GS_Keyboard::MapSDLKey(event.key.keysym.scancode);
                if (gskKey != GSK_NONE)
                {
                    MsgProc((HWND)m_pWindow, WM_KEYUP, gskKey, 0);
                }
            }
            else if (event.type == SDL_MOUSEMOTION)
            {
                // Pass SDL_MOUSEMOTION to MsgProc as WM_MOUSEMOVE
                MsgProc((HWND)m_pWindow, WM_MOUSEMOVE, 0, (event.motion.x & 0xFFFF) | (event.motion.y << 16));
            }
            else if (event.type == SDL_MOUSEBUTTONDOWN)
            {
                if (event.button.button == SDL_BUTTON_LEFT)
                    MsgProc((HWND)m_pWindow, WM_LBUTTONDOWN, 0, 0);
                else if (event.button.button == SDL_BUTTON_RIGHT)
                    MsgProc((HWND)m_pWindow, WM_RBUTTONDOWN, 0, 0);
                else if (event.button.button == SDL_BUTTON_MIDDLE)
                    MsgProc((HWND)m_pWindow, WM_MBUTTONDOWN, 0, 0);
            }
            else if (event.type == SDL_MOUSEBUTTONUP)
            {
                if (event.button.button == SDL_BUTTON_LEFT)
                    MsgProc((HWND)m_pWindow, WM_LBUTTONUP, 0, 0);
                else if (event.button.button == SDL_BUTTON_RIGHT)
                    MsgProc((HWND)m_pWindow, WM_RBUTTONUP, 0, 0);
                else if (event.button.button == SDL_BUTTON_MIDDLE)
                    MsgProc((HWND)m_pWindow, WM_MBUTTONUP, 0, 0);
            }
        }

        if (m_bIsActive && m_bIsReady)
        {
            if ((m_fFrameTime = m_gsFrameTimer.GetFrameTime()) >= m_fWaitTime)
            {
                m_gsFrameTimer.MarkFrame();
                if (TRUE != GameLoop())
                {
                    GS_Error::Report("GS_APP.CPP", 165, "Error running main game loop!");
                    running = false;
                }
            }
            else
            {
                // Small delay to prevent busy-waiting
                SDL_Delay(1);
            }
        }
        else
        {
            SDL_Delay(10);
        }
    }

    SDL_StopTextInput();
    this->Destroy();
    return 0;
}

LRESULT GS_Application::MsgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    (void)hWnd;
    (void)lParam;

    // Handle keyboard and mouse events in SDL mode
    switch (uMsg)
    {
    case WM_KEYDOWN:
    case WM_KEYUP:
    case WM_MOUSEMOVE:
    case WM_LBUTTONDOWN:
    case WM_MBUTTONDOWN:
    case WM_RBUTTONDOWN:
    case WM_LBUTTONUP:
    case WM_MBUTTONUP:
    case WM_RBUTTONUP:
        // Child classes will handle these
        break;
    }

    return 0;
}

void GS_Application::SetFrameRate(float fFrameRate)
{
    m_fFrameRate = fFrameRate;

    if (m_fFrameRate > 0.0f)
    {
        m_fWaitTime = 1000.0f / m_fFrameRate;
    }
    else
    {
        m_fWaitTime = 0.0f;
    }
}

void GS_Application::ShowFrameRate()
{
    char outputString[128] = { 0 };
    sprintf(outputString, "The program ran at an average of %0.2f frames per second.",
            m_gsFrameTimer.GetFrameRate());
    MessageBox(NULL, outputString, "GAME INFORMATION", MB_OK | MB_ICONINFORMATION);
}

#endif // !GS_PLATFORM_WINDOWS
