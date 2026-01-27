/*============================================================================================+
 | Game System (GS) Library                                                                   |
 |--------------------------------------------------------------------------------------------|
 | FILES: gs_ini_file.cpp, gs_ini_file.h                                                      |
 |--------------------------------------------------------------------------------------------|
 | CLASS: GS_IniFile                                                                          |
 |--------------------------------------------------------------------------------------------|
 | ABOUT: ...                                                                                 |
 |--------------------------------------------------------------------------------------------|
 |                                                                                    08/2003 |
 +============================================================================================*/


#ifndef GS_INI_FILE_H
#define GS_INI_FILE_H


//==============================================================================================
// Include standard C library header files.
// ---------------------------------------------------------------------------------------------
#include <stdlib.h>
#include <stdio.h>
//==============================================================================================


//==============================================================================================
// Include Game System (GS) header files.
// ---------------------------------------------------------------------------------------------
#include "gs_object.h"
//==============================================================================================


//==============================================================================================
// Include platform abstraction header files.
// ---------------------------------------------------------------------------------------------
#include "gs_platform.h"
//==============================================================================================

#ifndef GS_PLATFORM_WINDOWS
// Platform-specific INI file functions for non-Windows platforms
namespace GS_IniFile_Platform {
    int GetPrivateProfileString(const char* section, const char* key, 
                                const char* defaultValue, char* buffer, 
                                int bufferSize, const char* filename);
    int WritePrivateProfileString(const char* section, const char* key,
                                   const char* value, const char* filename);
}
#endif

//==============================================================================================
// Class constants.
// ---------------------------------------------------------------------------------------------
const int INI_FILE_BUFFER_SIZE = 100;
//==============================================================================================


////////////////////////////////////////////////////////////////////////////////////////////////
// Class Definition. ///////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////

class GS_IniFile : public GS_Object
{

private:

    char m_pszFullname[_MAX_PATH];

    BOOL m_bIsOpen;

    char m_cBuffer[INI_FILE_BUFFER_SIZE];

protected:

    // No protected members.

public:

    GS_IniFile();
    ~GS_IniFile();

    BOOL Open(const char* pszFilename);
    BOOL Close();

    BOOL IsOpen();

    int   ReadInt  (const char* pszSection, const char* pszKey, int nDefaultValue);
    float ReadFloat(const char* pszSection, const char* pszKey, float fDefaultValue);
    const char* ReadString(const char* pszSection, const char* pszKey,
                           const char* pszDefaultValue);

    BOOL WriteInt   (const char* pszSection, const char* pszKey, int nValue);
    BOOL WriteFloat (const char* pszSection, const char* pszKey, float fValue);
    BOOL WriteString(const char* pszSection, const char* pszKey, const char* pszValue);
};

////////////////////////////////////////////////////////////////////////////////////////////////

#endif
