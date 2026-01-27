/*============================================================================================+
 | Game System (GS) Library                                                                   |
 |--------------------------------------------------------------------------------------------|
 | FILES: gs_file.cpp, gs_file.h                                                              |
 |--------------------------------------------------------------------------------------------|
 | CLASS: GS_File                                                                             |
 |--------------------------------------------------------------------------------------------|
 | ABOUT: Includes data and functions for reading from or writing to files.                   |
 |--------------------------------------------------------------------------------------------|
 |                                                                                    02/2003 |
 +============================================================================================*/


//==============================================================================================
// Include Game System (GS) header files.
// ---------------------------------------------------------------------------------------------
#include "gs_file.h"
//==============================================================================================


////////////////////////////////////////////////////////////////////////////////////////////////
// Constructor/Destructor Methods //////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_File::GS_File():
// ---------------------------------------------------------------------------------------------
// Purpose: The constructor, initializes class data when class object is created.
// ---------------------------------------------------------------------------------------------
// Returns: Nothing.
//==============================================================================================

GS_File::GS_File()
{
#ifdef GS_PLATFORM_WINDOWS
    m_FileHandle = INVALID_HANDLE_VALUE;
#else
    m_FileHandle = NULL;
#endif
    m_FileMode   = FILE_READ;

    for (int nIndex=0; nIndex < _MAX_PATH; nIndex++)
    {
        m_szDirectoryName[nIndex] = 0;
    }

    m_bIsOpened = FALSE;
}


////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_File::~GS_File():
// ---------------------------------------------------------------------------------------------
// Purpose: The de-structor, de-initializes class data when class object is destroyed.
// ---------------------------------------------------------------------------------------------
// Returns: Nothing.
//==============================================================================================

GS_File::~GS_File()
{

    this->Close();
}


////////////////////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////////////////////
// Filename Methods ////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_File::SetDirectory():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Returns: True if successfull, FALSE if not.
//==============================================================================================

BOOL GS_File::SetDirectory(const char* pszDirectoryName)
{
    strcpy(m_szDirectoryName, pszDirectoryName);

    return TRUE;
}


////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_File::GetFullName():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Returns: True if successfull.
//==============================================================================================

BOOL GS_File::GetFullName(const char* pszFilename, char* pszFullname)
{

    // Add the directory to the filename.
    strcpy(pszFullname, m_szDirectoryName);
    strcat(pszFullname, pszFilename);

#ifndef GS_PLATFORM_WINDOWS
    // Normalize path on non-Windows platforms
    GS_Platform::NormalizePath(pszFullname);
#endif

    return TRUE;
}


////////////////////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////////////////////
// Open/Close Methods. /////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_File::Open():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Returns: True if successfull, FALSE if failed.
//==============================================================================================

BOOL GS_File::Open(const char* pszFilename, FILE_MODE_FLAGS FileMode)
{

    // Exit if no valid filename.
    if (!pszFilename) return FALSE;

    // Close the file if it has already been opened.
    this->Close();

    char szFullname[_MAX_PATH];

    // Get the full pathname (directory + filename) of the file.
    if (FALSE == this->GetFullName(pszFilename, szFullname))
    {
        return FALSE;
    }

#ifdef GS_PLATFORM_WINDOWS
    // Open the file in the specified mode.
    switch (FileMode)
    {
    // If read-mode is specified, open an existing file for reading.
    case FILE_READ:
        m_FileHandle = CreateFile(szFullname, GENERIC_READ, 0, NULL, OPEN_EXISTING,
                                  FILE_ATTRIBUTE_NORMAL, NULL);
        break;
    // If write-mode is specified, create a new file for writing (file will be over-written
    // if it exists).
    case FILE_WRITE:
        m_FileHandle = CreateFile(szFullname, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS,
                                  FILE_ATTRIBUTE_NORMAL, NULL);
        break;
    // Exit if no valid mode was specified.
    default:
        return FALSE;
    }

    // Exit if file could not be opened successfully.
    if (m_FileHandle == INVALID_HANDLE_VALUE)
    {
        return FALSE;
    }
#else
    // Normalize the path
    GS_Platform::NormalizePath(szFullname);
    
    // Open file using standard C functions
    const char* mode = (FileMode == FILE_READ) ? "rb" : "wb";
    m_FileHandle = fopen(szFullname, mode);
    
    if (!m_FileHandle)
    {
        return FALSE;
    }
    
    m_FileMode = FileMode;
    m_bIsOpened = TRUE;
#endif

    return TRUE;
}


////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_File::Close():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Returns: Nothing.
//==============================================================================================

void GS_File::Close()
{
#ifdef GS_PLATFORM_WINDOWS
    if (m_FileHandle != INVALID_HANDLE_VALUE)
    {
        CloseHandle(m_FileHandle);
    }
    m_FileHandle = INVALID_HANDLE_VALUE;
#else
    if (m_FileHandle != NULL)
    {
        fclose(m_FileHandle);
    }
    m_FileHandle = NULL;
#endif

    m_bIsOpened = FALSE;
}


////////////////////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////////////////////
// File-state Methods. //////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_File::IsOpen():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Returns: True if the file has been opened, FALSE if not.
//==============================================================================================

BOOL GS_File::IsOpen()
{

    return m_bIsOpened;
}


////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_File::IsEndOfFile():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Returns: True if the end of file has been reached, FALSE if not.
//==============================================================================================

BOOL GS_File::IsEndOfFile()
{

    // Exit if the file has not been opened.
    if (!m_bIsOpened)
    {
        return TRUE;
    }

    return(this->GetPosition() == this->GetLength());
}


////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_File::Exists():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Returns: True if the file exists, FALSE if not.
//==============================================================================================

BOOL GS_File::Exists(const char* pszFilename)
{

    // Create a temporary file object.
    GS_File gsTempFile;

    // Try to open the file, return FALSE if failed.
    if (!gsTempFile.Open(pszFilename))
    {
        return FALSE;
    }

    // Close the temporary file object.
    gsTempFile.Close();

    return TRUE;
}


////////////////////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////////////////////
// File Get/Set Methods. ///////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_File::GetMode():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Returns: A FILE_MODE_FLAGS value depending on the mode.
//==============================================================================================

FILE_MODE_FLAGS GS_File::GetMode()
{

    return m_FileMode;
}


////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_File::GetLength():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Returns: The length of the file, or 0 if file has not been opened.
//==============================================================================================

unsigned long GS_File::GetLength()
{

    // Exit if the file has not been opened.
    if (!m_bIsOpened)
    {
        return 0;
    }

#ifdef GS_PLATFORM_WINDOWS
    // Save the current file position.
    unsigned long lOldPos = SetFilePointer(m_FileHandle, 0, NULL, FILE_CURRENT);

    // Return 0 if couldn't save current file position
    if (lOldPos == 0xFFFFFFFF)
    {
        return 0;
    }

    // Find the length of the file by setting the file pointer to the end of the file.
    unsigned long lLength = SetFilePointer(m_FileHandle, 0, NULL, FILE_END);

    // Return 0 if couldn't set file pointer to end of file.
    if (lLength == 0xFFFFFFFF)
    {
        return 0;
    }

    // Return the file pointer to it's originale position.
    SetFilePointer(m_FileHandle, lOldPos, NULL, FILE_BEGIN);

    return lLength;
#else
    long lOldPos = ftell(m_FileHandle);
    if (lOldPos == -1)
    {
        return 0;
    }
    if (fseek(m_FileHandle, 0, SEEK_END) != 0)
    {
        return 0;
    }
    long lLength = ftell(m_FileHandle);
    fseek(m_FileHandle, lOldPos, SEEK_SET);
    return (unsigned long)lLength;
#endif
}


////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_File::GetPosition():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Returns: The position of the file pointer in the file, 0 if the file has not been opened.
//==============================================================================================

unsigned long GS_File::GetPosition()
{

    // Exit if the file has not been opened.
    if (!m_bIsOpened)
    {
        return 0;
    }

#ifdef GS_PLATFORM_WINDOWS
    return(SetFilePointer(m_FileHandle, 0, NULL, FILE_CURRENT));
#else
    return (unsigned long)ftell(m_FileHandle);
#endif
}


////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_File::SetPosition():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Returns: True if successfull, FALSE if failed.
//==============================================================================================


BOOL GS_File::SetPosition(unsigned long lPosition)
{

    // Exit if the file has not been opened.
    if (!m_bIsOpened)
    {
        return FALSE;
    }

#ifdef GS_PLATFORM_WINDOWS
    return(SetFilePointer(m_FileHandle, lPosition, NULL, FILE_BEGIN) != 0xFFFFFFFF);
#else
    return (fseek(m_FileHandle, lPosition, SEEK_SET) == 0);
#endif
}


////////////////////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////////////////////
// File Read Methods. //////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_File::GetByte():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Returns: The byte read or FILE_READ_FAILED if failed.
//==============================================================================================

unsigned long GS_File::GetByte()
{

    // Exit if the file has not been opened.
    if (!m_bIsOpened)
    {
        return FILE_READ_FAILED;
    }

    char cByte;
    
#ifdef GS_PLATFORM_WINDOWS
    unsigned long lBytesRead;
    if (!ReadFile(m_FileHandle, &cByte, 1, &lBytesRead, NULL))
    {
        GS_Error::Report("GS_FILE.CPP", 398, "Failed to read from file!");
        return FILE_READ_FAILED;
    }
    if (lBytesRead == 1)
    {
        return ((unsigned long) cByte);
    }
#else
    if (fread(&cByte, 1, 1, m_FileHandle) == 1)
    {
        return ((unsigned long) cByte);
    }
#endif

    GS_Error::Report("GS_FILE.CPP", 407, "Failed to read byte from file!");
    return FILE_READ_FAILED;
}


////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_File::Read():
// ---------------------------------------------------------------------------------------------
// Purpose:
// ---------------------------------------------------------------------------------------------
// Returns: The number of bytes read or FILE_READ_FAILED if failed.
//==============================================================================================

unsigned long GS_File::Read(void* pBuffer, unsigned long lByteCount)
{

    // Exit if the file has not been opened.
    if (!m_bIsOpened)
    {
        return FILE_READ_FAILED;
    }

    if ((pBuffer == 0) || (lByteCount <= 0))
    {
        return 0;
    }

#ifdef GS_PLATFORM_WINDOWS
    unsigned long lBytesRead;
    if (ReadFile(m_FileHandle, pBuffer, lByteCount, &lBytesRead, NULL))
    {
        return lBytesRead;
    }
#else
    size_t bytesRead = fread(pBuffer, 1, lByteCount, m_FileHandle);
    if (bytesRead > 0)
    {
        return (unsigned long)bytesRead;
    }
#endif

    GS_Error::Report("GS_FILE.CPP", 443, "Failed to read from file!");
    return FILE_READ_FAILED;
}


////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_File::ReadString():
// ---------------------------------------------------------------------------------------------
// Purpose:
// ---------------------------------------------------------------------------------------------
// Returns: The number of bytes read or FILE_READ_FAILED if failed.
//==============================================================================================

unsigned long GS_File::ReadString(char* pBuffer, int nMaxLength)
{
    // Exit if the file has not been opened.
    if (!m_bIsOpened)
    {
        return FILE_READ_FAILED;
    }

    int  nIndex = 0;
    char cChar;

    // Read string one character at a time.
    while (nIndex < nMaxLength)
    {
#ifdef GS_PLATFORM_WINDOWS
        unsigned long lBytesRead;
        // Read a single character from the file.
        if (ReadFile(m_FileHandle, &cChar, 1, &lBytesRead, NULL))
        {
            // If character was read succesfully.
            if (lBytesRead == 1)
            {
#else
        // Read a single character from the file.
        if (fread(&cChar, 1, 1, m_FileHandle) == 1)
        {
#endif
                // End string at the end of a line.
                pBuffer[nIndex] = ((cChar == 10) ? (char)0 : cChar);
                // Move to next character.
                nIndex++;
                // If the end of the line was reached, return the number of characters read.
                if (cChar == 10)
                {
                    return strlen(pBuffer);
                }
                // If the end of the line was not reached and the maximum number of characters
                // have been read, return fail code.
                if (nIndex >= nMaxLength)
                {
                    return FILE_READ_FAILED;
                }
#ifdef GS_PLATFORM_WINDOWS
            }
            else
            {
                GS_Error::Report("GS_FILE.CPP", 493, "Failed to read from file!");
                return FILE_READ_FAILED;
            }
#endif
        }
#ifndef GS_PLATFORM_WINDOWS
        else
        {
            break;
        }
#endif
    }

    GS_Error::Report("GS_FILE.CPP", 499, "Failed to read from file!");
    return FILE_READ_FAILED;
}


////////////////////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////////////////////
// File Write Methods. /////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////


///==============================================================================================
// GS_File::PutByte():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Returns: True if successfull, FALSE if failed.
//==============================================================================================

BOOL GS_File::PutByte(unsigned short sByte)
{

    // Exit if the file has not been opened.
    if (!m_bIsOpened)
    {
        return FALSE;
    }

#ifdef GS_PLATFORM_WINDOWS
    unsigned long lBytesWritten;
    if (WriteFile(m_FileHandle, &sByte, 1, &lBytesWritten, NULL))
    {
        return (lBytesWritten == 1);
    }
#else
    if (fwrite(&sByte, 1, 1, m_FileHandle) == 1)
    {
        return TRUE;
    }
#endif
    
    GS_Error::Report("GS_FILE.CPP", 535, "Failed to write to file!");
    return FALSE;
}


////////////////////////////////////////////////////////////////////////////////////////////////


///==============================================================================================
// GS_File::Write():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Returns: Number of bytes witten or FILE_WRITE_FAILED if failed.
//==============================================================================================

unsigned long GS_File::Write(void* pBuffer, unsigned long lByteCount)
{

    // Exit if the file has not been opened.
    if (!m_bIsOpened)
    {
        return FILE_WRITE_FAILED;
    }

    if ((pBuffer == 0) || (lByteCount == 0))
    {
        return 0;
    }

#ifdef GS_PLATFORM_WINDOWS
    unsigned long lBytesWritten;
    if (WriteFile(m_FileHandle, pBuffer, lByteCount, &lBytesWritten, NULL))
    {
        return lBytesWritten;
    }
#else
    size_t bytesWritten = fwrite(pBuffer, 1, lByteCount, m_FileHandle);
    if (bytesWritten > 0)
    {
        return (unsigned long)bytesWritten;
    }
#endif
    
    GS_Error::Report("GS_FILE.CPP", 571, "Failed to write to file!");
    return FILE_WRITE_FAILED;
}


////////////////////////////////////////////////////////////////////////////////////////////////


///==============================================================================================
// GS_File::WriteString():
// ---------------------------------------------------------------------------------------------
// Purpose: Writes a formatted string no larger than 1000 characters in length to the
//          file at the current file position.
// ---------------------------------------------------------------------------------------------
// Returns: Number of bytes written or FILE_WRITE_FAILED if failed.
//==============================================================================================

unsigned long __cdecl GS_File::WriteString(const char* pFormat,...)
{
    // Exit if the file has not been opened.
    if (!m_bIsOpened)
    {
        return FILE_WRITE_FAILED;
    }

    va_list ArgList;
    static char szString[1000];

    // Create a string from the argument list provided.
    va_start(ArgList, pFormat);
    vsprintf(szString, pFormat, ArgList);
    va_end(ArgList);

    unsigned long lLength = strlen(szString);

    // Don't write anything if no valid string.
    if (lLength == 0)
    {
        return 0 ;
    }

#ifdef GS_PLATFORM_WINDOWS
    unsigned long lBytesWritten;
    if (WriteFile(m_FileHandle, szString, lLength, &lBytesWritten, NULL))
    {
        return lBytesWritten;
    }
#else
    size_t bytesWritten = fwrite(szString, 1, lLength, m_FileHandle);
    if (bytesWritten > 0)
    {
        return (unsigned long)bytesWritten;
    }
#endif
    
    GS_Error::Report("GS_FILE.CPP", 617, "Failed to write to file!");
    return FILE_WRITE_FAILED;
}


////////////////////////////////////////////////////////////////////////////////////////////////
