/*============================================================================================+
 | Game System (GS) Library                                                                   |
 |--------------------------------------------------------------------------------------------|
 | FILES: gs_file.cpp, gs_file.h                                                              |
 |--------------------------------------------------------------------------------------------|
 | CLASS: GS_File                                                                             |
 |--------------------------------------------------------------------------------------------|
 | ABOUT: ...                                                                                 |
 |--------------------------------------------------------------------------------------------|
 |                                                                                    02/2003 |
 +============================================================================================*/


#ifndef GS_FILE_H
#define GS_FILE_H


//==============================================================================================
// Include standard C library header files.
// ---------------------------------------------------------------------------------------------
#include <stdlib.h>
#include <stdio.h>
//==============================================================================================


//==============================================================================================
// Include Game System (GS) header files.
// ---------------------------------------------------------------------------------------------
#include "gs_error.h"
#include "gs_object.h"
//==============================================================================================


//==============================================================================================
// Include platform abstraction header files.
// ---------------------------------------------------------------------------------------------
#include "gs_platform.h"
//==============================================================================================


//==============================================================================================
// Error code defines.
// ---------------------------------------------------------------------------------------------
#define FILE_READ_FAILED  0xFFFFFFFF
#define FILE_WRITE_FAILED 0xFFFFFFFF
//==============================================================================================


//==============================================================================================
// File mode flags declaration.
// ---------------------------------------------------------------------------------------------
typedef enum
{
    FILE_READ, // Read from an existing file.
    FILE_WRITE // Write to a new file.
} FILE_MODE_FLAGS;
//==============================================================================================


////////////////////////////////////////////////////////////////////////////////////////////////
// Class Definition. ///////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////


class GS_File : public GS_Object
{

private:

#ifdef GS_PLATFORM_WINDOWS
    HANDLE m_FileHandle;
#else
    FILE* m_FileHandle;
#endif

    FILE_MODE_FLAGS m_FileMode;

    char m_szDirectoryName[_MAX_PATH];

    BOOL m_bIsOpened;

protected:

    // No protected members.

public:

    GS_File();
    ~GS_File();

    BOOL SetDirectory(const char* pszDirectoryName);
    BOOL GetFullName(const char* pszFilename, char* pszFullname);

    BOOL Open(const char* pszFilename, FILE_MODE_FLAGS FileMode = FILE_READ);
    void Close();

    BOOL IsOpen();
    BOOL IsEndOfFile();
    BOOL Exists(const char* pszFilename);

    FILE_MODE_FLAGS GetMode();
    unsigned long GetLength();
    unsigned long GetPosition();
    BOOL SetPosition(unsigned long lPosition);

    unsigned long GetByte();
    unsigned long Read(void* pBuffer, unsigned long lByteCount);
    unsigned long ReadString(char* pBuffer, int nMaxLength);

    BOOL PutByte(unsigned short sByte);
    unsigned long Write(void* pBuffer, unsigned long lByteCount);
    unsigned long __cdecl WriteString(const char* pFormat,...);
};

////////////////////////////////////////////////////////////////////////////////////////////////

#endif
