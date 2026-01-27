/*=============================================================================================+
 | Game System (GS) Library                                                                    |
 |---------------------------------------------------------------------------------------------|
 | FILES: gs_main.cpp, gs_main.h                                                               |
 |---------------------------------------------------------------------------------------------|
 | CLASS: None                                                                                 |
 |---------------------------------------------------------------------------------------------|
 | ABOUT: ...                                                                                  |
 |---------------------------------------------------------------------------------------------|
 |                                                                                     08/2003 |
 +=============================================================================================*/


#include "gs_main.h"


//==============================================================================================
// External global variable
// ---------------------------------------------------------------------------------------------
extern GS_Application* g_pGSApp;
// ---------------------------------------------------------------------------------------------


#ifdef GS_PLATFORM_WINDOWS
//==============================================================================================
// WinMain():
// ---------------------------------------------------------------------------------------------
// Purpose: The main entry point into the application, creates a window and runs the game.
// ---------------------------------------------------------------------------------------------
// Returns: Result depending on the exit return value.
//==============================================================================================

int APIENTRY WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
    // Has an instance of the GS_Application class been created?
    if (!g_pGSApp)
    {
        // Report error if failed to create application.
        GS_Error::Report("GS_MAIN.CPP", 44, "Failed to start application, no instance created!");
        // Exit application.
        return 0;
    }

    // Run the application.
    return g_pGSApp->Run();
}
#else
//==============================================================================================
// main(): SDL entry point
// ---------------------------------------------------------------------------------------------
// Purpose: The main entry point for non-Windows platforms, initializes SDL and runs the game.
// ---------------------------------------------------------------------------------------------
// Returns: Result depending on the exit return value.
//==============================================================================================

int main(int argc, char* argv[])
{
    (void)argc;
    (void)argv;
    
    // Has an instance of the GS_Application class been created?
    if (!g_pGSApp)
    {
        // Report error if failed to create application.
        GS_Error::Report("GS_MAIN.CPP", 44, "Failed to start application, no instance created!");
        // Exit application.
        return 0;
    }

    // Run the application.
    return g_pGSApp->Run();
}
#endif

////////////////////////////////////////////////////////////////////////////////////////////////
