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


//==============================================================================================
// Include Game System (GS) header files.
// ---------------------------------------------------------------------------------------------
#include "gs_ini_file.h"

#ifndef GS_PLATFORM_WINDOWS
#include <fstream>
#include <sstream>
#include <map>
#include <string>

namespace GS_IniFile_Platform {

// Simple INI file parser
static std::map<std::string, std::map<std::string, std::string>> g_iniCache;
static std::string g_lastIniFile;

static void LoadIniFile(const char* filename) {
    // Normalize the path first
    char normalizedPath[_MAX_PATH];
    strncpy(normalizedPath, filename, _MAX_PATH - 1);
    normalizedPath[_MAX_PATH - 1] = '\0';
    GS_Platform::NormalizePath(normalizedPath);
    
    if (g_lastIniFile == normalizedPath) return;
    
    g_iniCache.clear();
    g_lastIniFile = normalizedPath;
    
    std::ifstream file(normalizedPath);
    if (!file.is_open()) return;
    
    std::string line;
    std::string currentSection;
    
    while (std::getline(file, line)) {
        // Trim whitespace
        size_t start = line.find_first_not_of(" \t\r\n");
        size_t end = line.find_last_not_of(" \t\r\n");
        if (start == std::string::npos) continue;
        line = line.substr(start, end - start + 1);
        
        // Skip comments
        if (line.empty() || line[0] == ';' || line[0] == '#') continue;
        
        // Section header
        if (line[0] == '[' && line[line.length()-1] == ']') {
            currentSection = line.substr(1, line.length()-2);
            continue;
        }
        
        // Key=Value pair
        size_t pos = line.find('=');
        if (pos != std::string::npos) {
            std::string key = line.substr(0, pos);
            std::string value = line.substr(pos + 1);
            
            // Trim key and value
            start = key.find_first_not_of(" \t");
            end = key.find_last_not_of(" \t");
            if (start != std::string::npos) {
                key = key.substr(start, end - start + 1);
            }
            
            start = value.find_first_not_of(" \t");
            end = value.find_last_not_of(" \t");
            if (start != std::string::npos) {
                value = value.substr(start, end - start + 1);
            }
            
            g_iniCache[currentSection][key] = value;
        }
    }
    
    file.close();
}

int GetPrivateProfileString(const char* section, const char* key, 
                            const char* defaultValue, char* buffer, 
                            int bufferSize, const char* filename) {
    if (!buffer || bufferSize <= 0) return 0;
    
    // Normalize the path
    char normalizedPath[_MAX_PATH];
    strncpy(normalizedPath, filename, _MAX_PATH - 1);
    normalizedPath[_MAX_PATH - 1] = '\0';
    GS_Platform::NormalizePath(normalizedPath);
    
    LoadIniFile(normalizedPath);
    
    std::string value = defaultValue ? defaultValue : "";
    
    if (g_iniCache.find(section) != g_iniCache.end()) {
        if (g_iniCache[section].find(key) != g_iniCache[section].end()) {
            value = g_iniCache[section][key];
        }
    }
    
    strncpy(buffer, value.c_str(), bufferSize - 1);
    buffer[bufferSize - 1] = '\0';
    
    return strlen(buffer);
}

int WritePrivateProfileString(const char* section, const char* key,
                               const char* value, const char* filename) {
    // Normalize the path
    char normalizedPath[_MAX_PATH];
    strncpy(normalizedPath, filename, _MAX_PATH - 1);
    normalizedPath[_MAX_PATH - 1] = '\0';
    GS_Platform::NormalizePath(normalizedPath);
    
    LoadIniFile(normalizedPath);
    
    // Update cache
    g_iniCache[section][key] = value;
    
    // Write entire file
    std::ofstream file(normalizedPath);
    if (!file.is_open()) return 0;
    
    for (auto& sec : g_iniCache) {
        file << "[" << sec.first << "]" << std::endl;
        for (auto& kv : sec.second) {
            file << kv.first << "=" << kv.second << std::endl;
        }
        file << std::endl;
    }
    
    file.close();
    return 1;
}

} // namespace GS_IniFile_Platform
#endif // !GS_PLATFORM_WINDOWS

////////////////////////////////////////////////////////////////////////////////////////////////
// Constructor/Destructor Methods //////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_IniFile::GS_IniFile():
// ---------------------------------------------------------------------------------------------
// Purpose: The constructor, initializes class data when class object is created.
// ---------------------------------------------------------------------------------------------
// Returns: Nothing.
//==============================================================================================

GS_IniFile::GS_IniFile()
{
    memset(m_pszFullname, 0, sizeof(m_pszFullname));
    memset(m_cBuffer, 0, sizeof(m_cBuffer));

    m_bIsOpen = FALSE;
}


////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_IniFile::~GS_IniFile():
// ---------------------------------------------------------------------------------------------
// Purpose: The de-structor, de-initializes class data when class object is destroyed.
// ---------------------------------------------------------------------------------------------
// Returns: Nothing.
//==============================================================================================

GS_IniFile::~GS_IniFile()
{

    if (this->IsOpen())
    {
        this->Close();
    }
}


////////////////////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////////////////////
// Open/Close Methods //////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_IniFile::Open():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Returns: True if successfull, FALSE if not.
//==============================================================================================

BOOL GS_IniFile::Open(const char* pszFilename)
{

    if (this->IsOpen())
    {
        this->Close();
    }

    strcpy(m_pszFullname, pszFilename);

    m_bIsOpen = TRUE;

    return TRUE;
}


////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_IniFile::Close():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Returns: True if successfull, FALSE if not.
//==============================================================================================

BOOL GS_IniFile::Close()
{

    m_bIsOpen = FALSE;

    return TRUE;
}


////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_IniFile::IsOpen():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Returns: True if open, FALSE if not.
//==============================================================================================

BOOL GS_IniFile::IsOpen()
{

    return m_bIsOpen;
}


////////////////////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////////////////////
// Read Methods ////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_IniFile::ReadInt():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Returns: An integer value indicating the value read or the default value if unsuccessfull.
//==============================================================================================

int GS_IniFile::ReadInt(const char* pszSection, const char* pszKey, int nDefaultValue)
{
    if (!this->IsOpen())
    {
        return nDefaultValue;
    }

    char cTempBuffer[50];

    sprintf(cTempBuffer, "%i", nDefaultValue);

#ifdef GS_PLATFORM_WINDOWS
    GetPrivateProfileString(pszSection, pszKey, cTempBuffer, m_cBuffer,INI_FILE_BUFFER_SIZE - 2,
                            m_pszFullname);
#else
    GS_IniFile_Platform::GetPrivateProfileString(pszSection, pszKey, cTempBuffer, m_cBuffer,
                                                  INI_FILE_BUFFER_SIZE - 2, m_pszFullname);
#endif

    int nValue;

    if (sscanf(m_cBuffer, "%i", &nValue) == 1)
    {
        return nValue;
    }
    else
    {
        return nDefaultValue;
    }
}


////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_IniFile::ReadFloat():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Returns: A float value indicating the value read or the default value if unsuccessfull.
//==============================================================================================

float GS_IniFile::ReadFloat(const char* pszSection, const char* pszKey, float fDefaultValue)
{
    if (!this->IsOpen())
    {
        return fDefaultValue;
    }

    char cTempBuffer[50];

    sprintf(cTempBuffer, "%.2f", fDefaultValue);

#ifdef GS_PLATFORM_WINDOWS
    GetPrivateProfileString(pszSection, pszKey, cTempBuffer, m_cBuffer, INI_FILE_BUFFER_SIZE - 2,
                            m_pszFullname);
#else
    GS_IniFile_Platform::GetPrivateProfileString(pszSection, pszKey, cTempBuffer, m_cBuffer,
                                                  INI_FILE_BUFFER_SIZE - 2, m_pszFullname);
#endif

    float fValue;

    if (sscanf(m_cBuffer, "%f", &fValue) == 1)
    {
        return fValue;
    }
    else
    {
        return fDefaultValue;
    }
}


////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_IniFile::ReadString():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Returns: A string value indicating the string read or the default string if unsuccessfull.
//==============================================================================================

const char* GS_IniFile::ReadString(const char* pszSection, const char* pszKey,
                                   const char* pszDefaultValue)
{
    if (!this->IsOpen())
    {
        return pszDefaultValue;
    }

#ifdef GS_PLATFORM_WINDOWS
    GetPrivateProfileString(pszSection, pszKey, pszDefaultValue, m_cBuffer,
                            INI_FILE_BUFFER_SIZE - 2, m_pszFullname);
#else
    GS_IniFile_Platform::GetPrivateProfileString(pszSection, pszKey, pszDefaultValue, m_cBuffer,
                                                  INI_FILE_BUFFER_SIZE - 2, m_pszFullname);
#endif

    return m_cBuffer;
}


////////////////////////////////////////////////////////////////////////////////////////////////
// Write Methods ///////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_IniFile::WriteInt():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Returns: True if successfull, FALSE if not.
//==============================================================================================

BOOL GS_IniFile::WriteInt(const char* pszSection, const char* pszKey, int nValue)
{
    if (!this->IsOpen())
    {
        return FALSE;
    }

    char cTempBuffer[50];

    sprintf(cTempBuffer, "%i", nValue);

#ifdef GS_PLATFORM_WINDOWS
    return (WritePrivateProfileString(pszSection, pszKey, cTempBuffer, m_pszFullname) != 0);
#else
    return (GS_IniFile_Platform::WritePrivateProfileString(pszSection, pszKey, cTempBuffer, m_pszFullname) != 0);
#endif
}


////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_IniFile::WriteFloat():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Returns: True if successfull, FALSE if not.
//==============================================================================================

BOOL GS_IniFile::WriteFloat(const char* pszSection, const char* pszKey, float fValue)
{
    if (!this->IsOpen())
    {
        return FALSE;
    }

    char cTempBuffer[50];

    sprintf(cTempBuffer, "%.2f", fValue);

#ifdef GS_PLATFORM_WINDOWS
    return (WritePrivateProfileString(pszSection, pszKey, cTempBuffer, m_pszFullname) != 0);
#else
    return (GS_IniFile_Platform::WritePrivateProfileString(pszSection, pszKey, cTempBuffer, m_pszFullname) != 0);
#endif
}


////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_IniFile::WriteString():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Returns: True if successfull, FALSE if not.
//==============================================================================================

BOOL GS_IniFile::WriteString(const char* pszSection, const char* pszKey, const char* pszValue)
{
    if (!this->IsOpen())
    {
        return FALSE;
    }

#ifdef GS_PLATFORM_WINDOWS
    return (WritePrivateProfileString(pszSection, pszKey, pszValue, m_pszFullname) != 0);
#else
    return (GS_IniFile_Platform::WritePrivateProfileString(pszSection, pszKey, pszValue, m_pszFullname) != 0);
#endif
}


////////////////////////////////////////////////////////////////////////////////////////////////
