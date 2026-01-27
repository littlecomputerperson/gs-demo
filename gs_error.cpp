/*============================================================================================+
 | Game System (GS) Library                                                                   |
 |--------------------------------------------------------------------------------------------|
 | FILES: gs_error.cpp, gs_error.h                                                            |
 |--------------------------------------------------------------------------------------------|
 | CLASS: GS_Error                                                                            |
 |--------------------------------------------------------------------------------------------|
 | ABOUT: Includes data and functions for displaying error messages for debugging purposes.   |
 |--------------------------------------------------------------------------------------------|
 |                                                                                    08/2003 |
 +============================================================================================*/


//==============================================================================================
// Include Game System (GS) header files.
// ---------------------------------------------------------------------------------------------
#include "gs_error.h"
//==============================================================================================


////////////////////////////////////////////////////////////////////////////////////////////////
// Report Methods. /////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_Error::Report():
// ---------------------------------------------------------------------------------------------
// Purpose: To report an error by either sending a message containing the name of the file in
//          which the error occured, the line in the file where the error occured, as well as a
//          message detailing the nature of the error to the debugger or by displaying it in a
//          message box.
// ---------------------------------------------------------------------------------------------
// Returns: Nothing.
//==============================================================================================

void GS_Error::Report(LPCTSTR pszFile, int nLine, LPCTSTR pszMessage)
{
    // Create a temporary buffer for the output string.
    char szBuffer[1024];

    // Create the error string.
    sprintf(szBuffer, "%s (%d): %s", pszFile, nLine, pszMessage);

#ifdef GS_DEBUG_MODE
    // Dump the error string to the debugger.
#ifdef GS_PLATFORM_WINDOWS
    OutputDebugString(szBuffer);
#else
    GS_Platform::OutputDebugString(szBuffer);
#endif
#else
#ifdef GS_OUTPUT_TO_FILE
    char szTempString[_MAX_PATH] = { 0 };
    static int nNumErrors = 0;
    GS_IniFile gsIniFile;
    // Determine the full pathname of the INI file.
    GS_Platform::GetCurrentDirectory(_MAX_PATH, szTempString);
    lstrcat(szTempString, "\\");
    lstrcat(szTempString, "errors.ini");
    // Open the INI file.
    gsIniFile.Open(szTempString);
    // Create a heading and increase the number of errors.
    sprintf(szTempString, "Error%03d", nNumErrors++);
    // Write error string.
    gsIniFile.WriteString(szTempString, "ErrorString", szBuffer);
    // Close the INI file.
    gsIniFile.Close();
#else
    // Display the error string in a message box.
#ifdef GS_PLATFORM_WINDOWS
    MessageBox(NULL, szBuffer, "ERROR", MB_OK | MB_ICONERROR);
#else
    MessageBox(NULL, szBuffer, "ERROR", MB_OK | MB_ICONERROR);
#endif
#endif
#endif
}


////////////////////////////////////////////////////////////////////////////////////////////////
