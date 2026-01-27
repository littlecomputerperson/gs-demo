/*============================================================================================+
 | Game System (GS) Library                                                                   |
 |--------------------------------------------------------------------------------------------|
 | FILES: gs_fmod_sound.cpp, gs_fmod_sound.h                                                  |
 |--------------------------------------------------------------------------------------------|
 | CLASS: GS_FmodSound                                                                        |
 |--------------------------------------------------------------------------------------------|
 | ABOUT: ...                                                                                 |
 |--------------------------------------------------------------------------------------------|
 |                                                                                    08/2003 |
 +============================================================================================*/


//==============================================================================================
// Include Game System (GS) header files.
// ---------------------------------------------------------------------------------------------
#include "gs_fmod_sound.h"
//==============================================================================================


////////////////////////////////////////////////////////////////////////////////////////////////
// Constructor/Destructor Methods. /////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_FmodSound::GS_FmodSound():
// ---------------------------------------------------------------------------------------------
// Purpose: The constructor, initializes class data to defaults when class object is created.
// ---------------------------------------------------------------------------------------------
// Returns: Nothing.
//==============================================================================================

GS_FmodSound::GS_FmodSound() {

    m_bIsReady  = FALSE;
    m_bIsPaused = FALSE;

    m_nMasterVolume = 255;

    m_nStreamIndex    = -1;
    m_nStreamChannel = -1;
}


////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_FmodSound::~GS_FmodSound():
// ---------------------------------------------------------------------------------------------
// Purpose: The de-structor, de-initializes class data when class object is destroyed and frees
//             all memory used by it.
// ---------------------------------------------------------------------------------------------
// Returns: Nothing.
//==============================================================================================

GS_FmodSound::~GS_FmodSound() {

    if (m_bIsReady) {
        this->Shutdown();
        }
}


////////////////////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////////////////////
// Initialize/Shutdown Methods /////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_FmodSound::Initialize():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Returns: TRUE if successfull, FALSE if not.
//==============================================================================================

BOOL GS_FmodSound::Initialize(HWND hWnd, BOOL bNoSound) {

    // Exit if invalid window handle.
    if (!hWnd) {
        return FALSE;
        }

    // Shutdown sound if already initialized.
    if (m_bIsReady) {
        this->Shutdown();
        }

    // Check if using the correct version of FMOD.
    if (FSOUND_GetVersion() < FMOD_VERSION) {
        GS_Error::Report("GS_FMOD_SOUND.CPP", 96, "Incorrect version of FMOD!");
        return FALSE;
        }

    // Set the window handle so FMOD knows wether window has focus or not.
    FSOUND_SetHWND(hWnd);

    // Should we enable sound?
    if (FALSE == bNoSound) {
        // Autodetect the output mode and select default sound driver.
        FSOUND_SetOutput(-1);
        FSOUND_SetDriver(0);
        // Try initializing the default device, at 44100hz with 64 channels.
        if (!FSOUND_Init(44100, 64, FSOUND_INIT_USEDEFAULTMIDISYNTH)) {
            GS_Error::Report("GS_FMOD_SOUND.CPP", 110, FMOD_ErrorString(FSOUND_GetError()));
            }
        }

    // No sound was selected or device initialization failed, so set ouput mode to no sound and
    // select an invalid driver which will case the device to be set to a null (nosound) driver.
    FSOUND_SetOutput(FSOUND_OUTPUT_NOSOUND);
    FSOUND_SetDriver(-1);

    // Were we able to initialize the device with no sound?
    if (!FSOUND_Init(44100, 64, FSOUND_INIT_USEDEFAULTMIDISYNTH)) {
        GS_Error::Report("GS_FMOD_SOUND.CPP", 121, FMOD_ErrorString(FSOUND_GetError()));
        return FALSE;
        }

    m_bIsReady = TRUE;

    return m_bIsReady;
}


////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_FmodSound::Shutdown():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Returns: TRUE if successfull, FALSE if not.
//==============================================================================================

BOOL GS_FmodSound::Shutdown() {

    // Can't shutdown if sound not initialized.
    if (!m_bIsReady) {
        return FALSE;
        }

    // Stop streaming sounds
    this->StopStream();

    // Clear all sounds from the lists.
    this->ClearMusicList();
    this->ClearStreamList();
    this->ClearSampleList();

    // Shutdown the FMOD sound system.
    //
    // WARNING: FSOUND_Close() causes system freeze!!! Fixed with new DLL.
    //
    FSOUND_SetPaused(FSOUND_SYSTEMCHANNEL, TRUE);
    FSOUND_Close();

    m_bIsReady  = FALSE;
    m_bIsPaused = FALSE;

    m_nStreamIndex   = -1;
    m_nStreamChannel = -1;

    return TRUE;
}


////////////////////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////////////////////
// Global Sound Methods ////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_FmodSound::Pause():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Returns: TRUE if successfull, FALSE if not.
//==============================================================================================

void GS_FmodSound::Pause(BOOL bPause) {

    // Exit if sound not initialized.
    if ((!m_bIsReady) || (m_bIsPaused == bPause)) {
        return;
        }

    // Pause / unpause all music.
    this->PauseAllMusic(bPause);

    // Pause / unpause stream.
    this->PauseStream(bPause);

    // Pause / unpause all channels.
    this->PauseAllChannels(bPause);

    m_bIsPaused = bPause;
}


////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_FmodSound::SetMasterVolume():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Notes: The FSOUND_SetSFXMasterVolume() sets the volume for all sound effects (samples and
//          streams). Changing the volume of a specific sound effect after a value was set using
//          the FSOUND_SetSFXMasterVolume() method will have the effect of setting that sound
//          effect's volume to a fraction of the master volume (i.e. the sound effect can be
//          no louder than the master volume). Unfortunately this method does not apply to music
//          (mods etc.) as well, so each music's volume has to be set seperately.
// ---------------------------------------------------------------------------------------------
// Returns: Nothing.
//==============================================================================================

void GS_FmodSound::SetMasterVolume(int nVolume) {

    // Exit if sound not initialized.
    if (!m_bIsReady) {
        return;
        }

    // Make sure volume is within range (0 - 255).
    if (nVolume < 0) {
        nVolume = 0;
        }
    if (nVolume > 255) {
        nVolume = 255;
        }

    // Assign master volume.
    m_nMasterVolume = nVolume;

    // For all the music currently in the list.
    for (int nLoop = 0; nLoop < m_gsMusicList.GetNumItems(); nLoop++) {
        // Change volume of the specified music in the list.
        FMUSIC_SetMasterVolume(m_gsMusicList[nLoop], nVolume);
        }

    // Set volume for all sound effects.
    FSOUND_SetSFXMasterVolume(nVolume);
}


////////////////////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////////////////////
// Music Methods ///////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_FmodSound::AddMusic():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Returns: TRUE if successfull, FALSE if not.
//==============================================================================================

BOOL GS_FmodSound::AddMusic(const char* pszFilename) {

    // Exit if sound not initialized.
    if (!m_bIsReady) {
        return FALSE;
        }

    FMUSIC_MODULE* pMusic;

    // Load the specified music file.
    pMusic = FMUSIC_LoadSong(pszFilename);

    // Was the music loaded successfully?
    if (!pMusic) {
        GS_Error::Report("GS_FMOD_SOUND.CPP", 280, "Failed to load music file!");
        return FALSE;
        }

    // Set the initial volume of the music.
    FMUSIC_SetMasterVolume(pMusic, m_nMasterVolume);

    // Add the music to the list.
    m_gsMusicList.AddItem(pMusic);

    return TRUE;
}


////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_FmodSound::RemoveMusic():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Returns: Nothing.
//==============================================================================================

BOOL GS_FmodSound::RemoveMusic(int nIndex) {

    // Exit if sound not initialized.
    if (!m_bIsReady) {
        return FALSE;
        }

    // Is index within bounds?
    if ((nIndex >= 0) && (nIndex < m_gsMusicList.GetNumItems())) {
        // Stop playing selected music.
        FMUSIC_StopSong(m_gsMusicList[nIndex]);
        // Free resources used by selected music.
        FMUSIC_FreeSong(m_gsMusicList[nIndex]);
        // Remove selected music from the list.
        m_gsMusicList.RemoveIndex(nIndex);
        // Music successfully removed.
        return TRUE;
        }

    return FALSE;
}


////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_FmodSound::ClearMusicList():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Returns: TRUE if successfull, FALSE if not.
//==============================================================================================

BOOL GS_FmodSound::ClearMusicList() {

    // Exit if sound not initialized.
    if (!m_bIsReady) {
        return FALSE;
        }

    // Stop playing music before clearing the list.
    FMUSIC_StopAllSongs();

    // For all the music currently in the list.
    for (int nLoop = 0; nLoop < m_gsMusicList.GetNumItems(); nLoop++) {
        // Free all the music in the list.
        FMUSIC_FreeSong(m_gsMusicList[nLoop]);
        }

    // Remove all the music from the list.
    m_gsMusicList.RemoveAllItems();

    return TRUE;
}


////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_FmodSound::GetNumMusics():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Returns: An integer number specifying the number of musics currently in the list.
//==============================================================================================

int GS_FmodSound::GetNumMusics() {

    return (m_gsMusicList.GetNumItems());
}


////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_FmodSound::PlayMusic():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Returns: TRUE if successfull, FALSE if not.
//==============================================================================================

BOOL GS_FmodSound::PlayMusic(int nIndex, int nVolume, float fSpeed, BOOL bLoop) {

    // Exit if sound not initialized or paused.
    if ((!m_bIsReady)  || (m_bIsPaused)) {
        return FALSE;
        }


    // Is index within bounds?
    if ((nIndex >= 0) && (nIndex < m_gsMusicList.GetNumItems())) {
        // Have the user specified a valid volume?
        if (nVolume >= 0.0f) {
            // Set the music volume.
            this->SetMusicVolume(nIndex, nVolume);
            }
        // Have the user specified a valid speed?
        if (fSpeed >= 0.0f) {
            // Set the music speed.
            this->SetMusicSpeed(nIndex, fSpeed);
            }
        // Set wether music should loop or not.
        this->LoopMusic(nIndex, bLoop);
        // Stop playing music if already playing.
        this->StopMusic(nIndex);
        // Play the music in the list specified by the index.
        if (FMUSIC_PlaySong(m_gsMusicList[nIndex])) {
            return TRUE;
            }
        }

    // Report error if failed to play music.
    GS_Error::Report("GS_FMOD_SOUND.CPP", 421, "Failed to play music!");

    return FALSE;
}


////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_FmodSound::StopMusic():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Returns: TRUE if successfull, FALSE if not.
//==============================================================================================

BOOL GS_FmodSound::StopMusic(int nIndex) {

    // Exit if sound not initialized.
    if (!m_bIsReady) {
        return FALSE;
        }

    // Is the index within bounds?
    if ((nIndex >= 0) && (nIndex < m_gsMusicList.GetNumItems())) {
        // Is music playing or paused?
        if (FMUSIC_IsPlaying(m_gsMusicList[nIndex])) {
            // Stop playing selected music.
            if (FMUSIC_StopSong(m_gsMusicList[nIndex])) {
                return TRUE;
                }
            }
        }

    return FALSE;
}


////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_FmodSound::StopAllMusic():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Returns: TRUE if successfull, FALSE if not.
//==============================================================================================

void GS_FmodSound::StopAllMusic() {

    // Exit if sound not initialized.
    if (!m_bIsReady) {
        return;
        }

    // Stop playing all music.
    FMUSIC_StopAllSongs();
}

////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_FmodSound::LoopMusic():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Returns: TRUE if successfull, FALSE if not.
//==============================================================================================

BOOL GS_FmodSound::LoopMusic(int nIndex, BOOL bLoop) {

    // Exit if sound not initialized.
    if (!m_bIsReady) {
        return FALSE;
        }

    // Is the index within bounds?
    if ((nIndex >= 0) && (nIndex < m_gsMusicList.GetNumItems())) {
        // Loop the music?
        if (bLoop) {
            // Loop selected music.
            if (!FMUSIC_SetLooping(m_gsMusicList[nIndex], TRUE)) {
                return FALSE;
                }
            }
        else {
            // Don't loop selected music.
            if (!FMUSIC_SetLooping(m_gsMusicList[nIndex], FALSE)) {
                return FALSE;
                }
            }
        }

    return TRUE;
}


////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_FmodSound::IsMusicPlaying():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Returns: TRUE if music is playing or paused, FALSE if it was stopped.
//==============================================================================================

BOOL GS_FmodSound::IsMusicPlaying(int nIndex) {

    // Exit if sound not initialized.
    if (!m_bIsReady) {
        return FALSE;
        }

    // Is the index within bounds?
    if ((nIndex >= 0) && (nIndex < m_gsMusicList.GetNumItems())) {
        // Is music playing or paused?
        if (FMUSIC_IsPlaying(m_gsMusicList[nIndex])) {
            return TRUE;
            }
        }

    return FALSE;
}


////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_FmodSound::IsMusicDone():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Returns: TRUE if music has finished playing, FALSE if not.
//==============================================================================================

BOOL GS_FmodSound::IsMusicDone(int nIndex) {

    // Exit if sound not initialized.
    if (!m_bIsReady) {
        return FALSE;
        }

    // Is the index within bounds?
    if ((nIndex >= 0) && (nIndex < m_gsMusicList.GetNumItems())) {
        // Return wether selected music has finished playing or not.
        return(FMUSIC_IsFinished(m_gsMusicList[nIndex]));
        }

    return FALSE;
}


////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_FmodSound::PauseMusic():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Returns: TRUE if successful, FALSE if failed.
//==============================================================================================

BOOL GS_FmodSound::PauseMusic(int nIndex, BOOL bPause) {

    // Exit if sound not initialized.
    if (!m_bIsReady) {
        return FALSE;
        }

    // Is the index within bounds?
    if ((nIndex >= 0) && (nIndex < m_gsMusicList.GetNumItems())) {
        // Should music be paused?
        if (bPause) {
            // Is music NOT paused?
            if (!FMUSIC_GetPaused(m_gsMusicList[nIndex])) {
                // Pause selected music.
                if (FMUSIC_SetPaused(m_gsMusicList[nIndex], TRUE)) {
                    return TRUE;
                    }
                }
            }
        else {
            // Is music paused?
            if (FMUSIC_GetPaused(m_gsMusicList[nIndex])) {
                // Unpause selected music.
                if (FMUSIC_SetPaused(m_gsMusicList[nIndex], FALSE)) {
                    return TRUE;
                    }
                }
            }
        }

    return FALSE;
}


////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_FmodSound::PauseAllMusic():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Returns: TRUE if successful, FALSE if failed.
//==============================================================================================

void GS_FmodSound::PauseAllMusic(BOOL bPause) {

    // Exit if sound not initialized.
    if (!m_bIsReady) {
        return;
        }

    int nLoop;

    // Should music be paused?
    if (bPause) {
        // For all the music currently in the list.
        for (nLoop = 0; nLoop < m_gsMusicList.GetNumItems(); nLoop++) {
            // Is music NOT paused?
            if (!FMUSIC_GetPaused(m_gsMusicList[nLoop])) {
                // Pause selected music.
                FMUSIC_SetPaused(m_gsMusicList[nLoop], TRUE);
                }
            }
        }
    else {
        // For all the music currently in the list.
        for (nLoop = 0; nLoop < m_gsMusicList.GetNumItems(); nLoop++) {
            // Is music paused?
            if (FMUSIC_GetPaused(m_gsMusicList[nLoop])) {
                // Unpause selected music.
                FMUSIC_SetPaused(m_gsMusicList[nLoop], FALSE);
                }
            }
        }
}


////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_FmodSound::IsMusicPaused():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Returns: TRUE if paused, FALSE if not.
//==============================================================================================

BOOL GS_FmodSound::IsMusicPaused(int nIndex) {

    // Exit if sound not initialized.
    if (!m_bIsReady) {
        return FALSE;
        }

    // Is the index within bounds?
    if ((nIndex >= 0) || (nIndex < m_gsMusicList.GetNumItems())) {
        // Is selected music paused?
        if (FMUSIC_GetPaused(m_gsMusicList[nIndex])) {
            return TRUE;
            }
        }

    return FALSE;
}


////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_FmodSound::SetMusicVolume():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Returns: TRUE if successful, FALSE if failed.
//==============================================================================================

BOOL GS_FmodSound::SetMusicVolume(int nIndex, int nVolume) {

    // Exit if sound not initialized.
    if (!m_bIsReady) {
        return FALSE;
        }

    // Is volume within range (0 - 255)?
    if (nVolume > 255) {
        nVolume = 255;
        }
    else if (nVolume < 0) {
        nVolume = 0;
        }

    // Is the index within bounds?
    if ((nIndex >= 0) && (nIndex < m_gsMusicList.GetNumItems())) {
        // Change volume of the specified music in the list.
        if (FMUSIC_SetMasterVolume(m_gsMusicList[nIndex], nVolume)) {
            return TRUE;
            }
        }

    return FALSE;
}


////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_FmodSound::SetMusicVolume():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Returns: Nothing.
//==============================================================================================

BOOL GS_FmodSound::SetMusicVolume(int nVolume) {

    // Exit if sound not initialized.
    if (!m_bIsReady) {
        return FALSE;
        }

    // Is volume within range (0 - 255)?
    if (nVolume > 255) {
        nVolume = 255;
        }
    else if (nVolume < 0) {
        nVolume = 0;
        }

    // For all the music currently in the list.
    for (int nLoop = 0; nLoop < m_gsMusicList.GetNumItems(); nLoop++) {
        // Change volume of the specified music in the list.
        if (!FMUSIC_SetMasterVolume(m_gsMusicList[nLoop], nVolume)) {
            return FALSE;
            }
        }

    return TRUE;
}


////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_FmodSound::GetMusicVolume():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Returns: The volume of the specified music or -1 on failure.
//==============================================================================================

int GS_FmodSound::GetMusicVolume(int nIndex) {

    // Exit if sound not initialized.
    if (!m_bIsReady) {
        return -1;
        }

    // Is the index within bounds?
    if ((nIndex >= 0) && (nIndex < m_gsMusicList.GetNumItems())) {
        // Retrieve the volume of the specified music in the list.
        return FMUSIC_GetMasterVolume(m_gsMusicList[nIndex]);
        }

    return -1;
}


////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_FmodSound::SetMusicSpeed():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Returns: TRUE if successful, FALSE if failed.
//==============================================================================================

BOOL GS_FmodSound::SetMusicSpeed(int nIndex, float fSpeed) {

    // Exit if sound not initialized.
    if (!m_bIsReady) {
        return FALSE;
        }

    // Is speed within range (0.0f - 10.0f)?
    if (fSpeed > 10.0f) {
        fSpeed = 10.0f;
        }
    else if (fSpeed < 0.0f) {
        fSpeed = 0.0f;
        }

    // Is the index within bounds?
    if ((nIndex >= 0) && (nIndex < m_gsMusicList.GetNumItems())) {
        // Change speed of the specified music in the list.
        if (FMUSIC_SetMasterSpeed(m_gsMusicList[nIndex], fSpeed)) {
            return TRUE;
            }
        }

    return FALSE;
}


////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_FmodSound::SetMusicSpeed():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Returns: Nothing.
//==============================================================================================

void GS_FmodSound::SetMusicSpeed(float fSpeed) {

    // Exit if sound not initialized.
    if (!m_bIsReady) {
        return;
        }

    // Is speed within range (0.0f - 10.0f)?
    if (fSpeed > 10.0f) {
        fSpeed = 10.0f;
        }
    else if (fSpeed < 0.0f) {
        fSpeed = 0.0f;
        }

    // For all the music currently in the list.
    for (int nLoop = 0; nLoop < m_gsMusicList.GetNumItems(); nLoop++) {
        // Change volume of the specified music in the list.
        FMUSIC_SetMasterSpeed(m_gsMusicList[nLoop], fSpeed);
        }
}


////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_FmodSound::GetMusicSpeed():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Returns: The speed of the music or -1 on failure.
//==============================================================================================

float GS_FmodSound::GetMusicSpeed(int nIndex) {

    // Exit if sound not initialized.
    if (!m_bIsReady) {
        return -1;
        }

    // Is the index within bounds?
    if ((nIndex >= 0) && (nIndex < m_gsMusicList.GetNumItems())) {
        // Retrieve the speed of the specified music in the list.
        return(FMUSIC_GetSpeed(m_gsMusicList[nIndex]));
        }

    return -1;
}


////////////////////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////////////////////
// Stream Methods //////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_FmodSound::AddStream():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Returns: TRUE if successfull, FALSE if not.
//==============================================================================================

BOOL GS_FmodSound::AddStream(const char* pszFilename) {

    // Exit if sound not initialized.
    if (!m_bIsReady) {
        return FALSE;
        }

    FSOUND_STREAM* pStream;

    // Create a stream using the specified file.
    pStream = FSOUND_Stream_Open(pszFilename, FSOUND_LOOP_NORMAL, 0, 0);

    // Was a stream created successfully?
    if (!pStream) {
        GS_Error::Report("GS_FMOD_SOUND.CPP", 932, "Failed to load stream file!");
        return FALSE;
        }

    // Add stream to list.
    m_gsStreamList.AddItem(pStream);

    return TRUE;
}


////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_FmodSound::RemoveStream():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Returns: TRUE if successful, FALSE if failed.
//==============================================================================================

BOOL GS_FmodSound::RemoveStream(int nIndex) {

    // Exit if sound not initialized.
    if (!m_bIsReady) {
        return FALSE;
        }

    // Is index within bounds?
    if ((nIndex >= 0) && (nIndex < m_gsStreamList.GetNumItems())) {
        // Stop playing stream (if any).
        this->StopStream();
        // Free resources used by selected stream.
        FSOUND_Stream_Close(m_gsStreamList[nIndex]);
        // Remove selected stream from the list.
        m_gsStreamList.RemoveIndex(nIndex);
        // Stream successfully removed.
        return TRUE;
        }

    return FALSE;
}


////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_FmodSound::ClearStreamList():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Returns: TRUE if successfull, FALSE if not.
//==============================================================================================

BOOL GS_FmodSound::ClearStreamList() {

    // Exit if sound not initialized.
    if (!m_bIsReady) {
        return FALSE;
        }

    // Stop playing stream (if any).
    this->StopStream();

    // Free all the streams in the list.
    for (int nLoop = 0; nLoop < m_gsStreamList.GetNumItems(); nLoop++) {
        FSOUND_Stream_Close(m_gsStreamList[nLoop]);
        }

    // Remove all the streams from the list.
    m_gsStreamList.RemoveAllItems();

    return TRUE;
}



////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_FmodSound::GetNumStreams():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Returns: An integer number specifying the number of musics currently in the list.
//==============================================================================================

int GS_FmodSound::GetNumStreams() {

    return (m_gsStreamList.GetNumItems());
}


////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_FmodSound::PlayStream():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Returns: The channel handle for the stream, -1 if failed.
//==============================================================================================

int GS_FmodSound::PlayStream(int nIndex, int nVolume) {

    // Exit if sound not initialized or paused.
    if ((!m_bIsReady) || (m_bIsPaused)) {
        return -1;
        }

    // Stop stream if it's already playing.
    this->StopStream();

    // Make sure index is within bounds.
    if ((nIndex >= 0) && (nIndex < m_gsStreamList.GetNumItems())) {
        // Play the stream in the list specified by the index.
        m_nStreamChannel = FSOUND_Stream_Play(FSOUND_FREE, m_gsStreamList[nIndex]);
        // Was opening stream successful?
        if (-1 != m_nStreamChannel) {
            // Remember which stream is playing.
            m_nStreamIndex = nIndex;
            // Have the user specified a valid volume?
            if (nVolume >= 0) {
                // Set the stream volume.
                this->SetStreamVolume(nVolume);
                }
            return m_nStreamChannel;
            }
        }

    // Report error if could not play stream.
    GS_Error::Report("GS_FMOD_SOUND.CPP", 1067, "Failed to play stream!");

    return -1;
}


////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_FmodSound::StopStream():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Returns: TRUE if successfull, FALSE if not.
//==============================================================================================

BOOL GS_FmodSound::StopStream() {

    // Exit if sound not initialized.
    if (!m_bIsReady) {
        return FALSE;
        }

    // Has any stream playing?
    if (m_nStreamIndex >= 0) {
        // Is stream playing or paused?
        if (FSOUND_IsPlaying(m_nStreamChannel)) {
            // Was stopping stream successful?
            if (FSOUND_Stream_Stop(m_gsStreamList[m_nStreamIndex])) {
                // Reset stream index and channel.
                m_nStreamIndex    = -1;
                m_nStreamChannel = -1;
                return TRUE;
                }
            }
        }

    return FALSE;
}


////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_FmodSound::LoopStream():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Returns: TRUE if successfull, FALSE if not.
//==============================================================================================

BOOL GS_FmodSound::LoopStream(int nIndex, int nLoopTimes) {

    // Exit if sound not initialized.
    if (!m_bIsReady) {
        return FALSE;
        }

    // Make sure index is within bounds.
    if ((nIndex >= 0) && (nIndex < m_gsStreamList.GetNumItems())) {
        // Set the number of times the stream will be looped (-1 for infinite, 0 for none).
        if (FSOUND_Stream_SetLoopCount(m_gsStreamList[nIndex], nLoopTimes)) {
            return TRUE;
            }
        }

    return FALSE;
}


////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_FmodSound::IsStreamPlaying():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Returns: TRUE if stream is playing or paused, FALSE if stream was stopped.
//==============================================================================================

BOOL GS_FmodSound::IsStreamPlaying() {

    // Exit if sound not initialized.
    if (!m_bIsReady) {
        return FALSE;
        }

    // Has any stream playing?
    if (m_nStreamIndex >= 0) {
        // Is stream playing or paused?
        if (FSOUND_IsPlaying(m_nStreamChannel)) {
            return TRUE;
            }
        }

    return FALSE;
}


////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_FmodSound::PauseStream():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Returns: TRUE if successful, FALSE if not.
//==============================================================================================

BOOL GS_FmodSound::PauseStream(BOOL bPause) {

    // Exit if sound not initialized.
    if (!m_bIsReady) {
        return FALSE;
        }

    // Is the index within bounds?
    if (m_nStreamIndex >= 0) {
        // Should stream be paused?
        if (bPause) {
            // Is stream NOT paused?
            if (!FSOUND_GetPaused(m_nStreamChannel)) {
                // Pause currently playing stream.
                if (FSOUND_SetPaused(m_nStreamChannel, TRUE)) {
                    return TRUE;
                  }
                }
            }
        else {
            // Is stream paused?
            if (FSOUND_GetPaused(m_nStreamChannel)) {
                // Unpause currently playing stream.
                if (FSOUND_SetPaused(m_nStreamChannel, FALSE)) {
                    return TRUE;
                    }
                }
            }
        }

    return FALSE;
}


////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_FmodSound::IsStreamPaused():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Returns: TRUE if stream is paused, FALSE if not.
//==============================================================================================

BOOL GS_FmodSound::IsStreamPaused() {

    // Exit if sound not initialized.
    if (!m_bIsReady) {
        return FALSE;
        }

    // Is the index within bounds?
    if (m_nStreamIndex >= 0) {
        // Is stream paused?
        if (FSOUND_GetPaused(m_nStreamChannel)) {
            return TRUE;
            }
        }

    return FALSE;
}


////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_FmodSound::SetStreamVolume():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Returns: TRUE if successful, FALSE if not.
//==============================================================================================

BOOL GS_FmodSound::SetStreamVolume(int nVolume) {

    // Exit if sound not initialized or no stream playing.
    if ((!m_bIsReady) || (m_nStreamIndex<0)) {
        return FALSE;
        }

    // Is volume within range (0 - 255)?
    if (nVolume > 255) {
        nVolume = 255;
        }
    else if (nVolume < 0) {
        nVolume = 0;
        }

    // Change volume of the currently playing stream.
    if (FSOUND_SetVolume(m_nStreamChannel, nVolume)) {
        return TRUE;
        }

    return FALSE;
}


////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_FmodSound::GetStreamVolume():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Returns: The volume of the specified stream or -1 on failure.
//==============================================================================================

int GS_FmodSound::GetStreamVolume() {

    // Exit if sound not initialized or no stream playing.
    if ((!m_bIsReady) || (m_nStreamIndex<0)) {
        return -1;
        }

    return FSOUND_GetVolume(m_nStreamChannel);
}


////////////////////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////////////////////
// Sample Methods //////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_FmodSound::AddSample():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Returns: TRUE if successfull, FALSE if not.
//==============================================================================================

BOOL GS_FmodSound::AddSample(const char* pszFilename) {

    // Exit if sound not initialized.
    if (!m_bIsReady) {
        return FALSE;
        }

    FSOUND_SAMPLE* pSample;

    // Load a sound sample from file.
    pSample = FSOUND_Sample_Load(FSOUND_FREE,pszFilename,FSOUND_NORMAL | FSOUND_LOOP_OFF,0,0);

    // Was the sample loaded successfully?
    if (!pSample) {
        GS_Error::Report("GS_FMOD_SOUND.CPP", 1331, "Failed to load sample file!");
        return FALSE;
        }

    // Add sample to list.
    m_gsSampleList.AddItem(pSample);

    return TRUE;

}


////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_FmodSound::RemoveSample():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Returns: TRUE if successful, FALSE if failed.
//==============================================================================================

BOOL GS_FmodSound::RemoveSample(int nIndex) {

    // Exit if sound not initialized.
    if (!m_bIsReady) {
        return FALSE;
        }

    // Is index within bounds?
    if ((nIndex >= 0) && (nIndex < m_gsSampleList.GetNumItems())) {
        // Stop playing all samples.
        FSOUND_StopSound(FSOUND_ALL);
        // Free resources used by selected sample.
        FSOUND_Sample_Free(m_gsSampleList[nIndex]);
        // Remove selected sample from the list.
        m_gsSampleList.RemoveIndex(nIndex);
        // Sample successfully removed.
        return TRUE;
        }

    return FALSE;
}


////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_FmodSound::ClearSampleList():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Returns: TRUE if successfull, FALSE if not.
//==============================================================================================

BOOL GS_FmodSound::ClearSampleList() {

    // Exit if sound not initialized.
    if (!m_bIsReady) {
        return FALSE;
        }

    // Stop playing all samples.
    FSOUND_StopSound(FSOUND_ALL);

    // Free all the samples in the list.
    for (int nLoop = 0; nLoop < m_gsSampleList.GetNumItems(); nLoop++) {
        FSOUND_Sample_Free(m_gsSampleList[nLoop]);
        }

    // Remove all the samples from the list.
    m_gsSampleList.RemoveAllItems();

    return TRUE;
}


////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_FmodSound::GetNumSamples():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Returns: An integer number specifying the number of samples currently in the list.
//==============================================================================================

int GS_FmodSound::GetNumSamples() {

    return (m_gsSampleList.GetNumItems());
}


////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_FmodSound::PlaySample():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Returns: The channel handle for the stream, -1 if failed.
//==============================================================================================

int GS_FmodSound::PlaySample(int nIndex, int nVolume, int nPanning) {

    // Exit if sound not initialized or paused.
    if ((!m_bIsReady) || (m_bIsPaused)) {
        return -1;
        }

    // Is the index within bounds?
    if ((nIndex >= 0) && (nIndex < m_gsSampleList.GetNumItems())) {
        // Have the user specified a valid volume?
        if (nVolume >= 0) {
            // Set the sample volume.
            this->SetSampleVolume(nIndex, nVolume);
            }
        // Have the user specified a valid panning?
        if (nPanning >= 0) {
            // Set the sample panning.
            this->SetSamplePanning(nIndex, nPanning);
            }
        // Play the sample in the list specified by the index.
        int nSampleChannel = FSOUND_PlaySound(FSOUND_FREE, m_gsSampleList[nIndex]);
        // Was playing sample successful?
        if (-1 != nSampleChannel) {
            return nSampleChannel;
            }
        }

    // Report error if could not play sample.
    GS_Error::Report("GS_FMOD_SOUND.CPP", 1466, "Failed to play sample!");

    return -1;
}


////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_FmodSound::SetSampleMaster():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Returns: Nothing.
//==============================================================================================

void GS_FmodSound::SetSampleMaster(int nVolume) {

    // Is volume within range (0 - 255)?
    if (nVolume > 255) {
        nVolume = 255;
        }
    else if (nVolume < 0) {
        nVolume = 0;
        }

    // Set master sample volume.
    FSOUND_SetSFXMasterVolume(nVolume);
}


////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_FmodSound::GetSampleMaster():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Returns: The master sample volume.
//==============================================================================================

int GS_FmodSound::GetSampleMaster() {

    return FSOUND_GetSFXMasterVolume();
}


////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_FmodSound::SetSampleVolume():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Returns: TRUE if successfull, FALSE if not.
//==============================================================================================

BOOL GS_FmodSound::SetSampleVolume(int nIndex, int nVolume) {

    // Exit if sound not initialized.
    if (!m_bIsReady) {
        return FALSE;
        }

    // Is volume within range (0 - 255)?
    if (nVolume > 255) {
        nVolume = 255;
        }
    else if (nVolume < 0) {
        nVolume = 0;
        }

    // Is the index within bounds?
    if ((nIndex >= 0) && (nIndex < m_gsSampleList.GetNumItems())) {
        // Change volume of the specified sample in the list.
        if (FSOUND_Sample_SetDefaults(m_gsSampleList[nIndex], -1, nVolume, -1, -1)) {
            return TRUE;
            }
        }

    return FALSE;
}


////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_FmodSound::SetSampleVolume():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Returns: Nothing.
//==============================================================================================

BOOL GS_FmodSound::SetSampleVolume(int nVolume) {

    // Exit if sound not initialized.
    if (!m_bIsReady) {
        return FALSE;
        }

    // Is volume within range (0 - 255)?
    if (nVolume > 255) {
        nVolume = 255;
        }
    else if (nVolume < 0) {
        nVolume = 0;
        }

    // For all the samples currently in the list.
    for (int nLoop = 0; nLoop < m_gsSampleList.GetNumItems(); nLoop++) {
        // Change volume of the specified sample in the list.
        if (!FSOUND_Sample_SetDefaults(m_gsSampleList[nLoop], -1, nVolume, -1, -1)) {
            return FALSE;
            }
        }

    return TRUE;
}


////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_FmodSound::GetSampleVolume():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Returns: The volume of the specified sample or -1 on failure.
//==============================================================================================

int GS_FmodSound::GetSampleVolume(int nIndex) {

    // Exit if sound not initialized.
    if (!m_bIsReady) {
        return -1;
        }

    int nVolume;

    // Is the index within bounds?
    if ((nIndex >= 0) && (nIndex < m_gsSampleList.GetNumItems())) {
        // Get the volume of the specified sample in the list.
        if (FSOUND_Sample_GetDefaults(m_gsSampleList[nIndex], NULL, &nVolume, NULL, NULL)) {
            return nVolume;
            }
        }

    return -1;
}


////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_FmodSound::SetSampleFrequency():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Returns: TRUE if successfull, FALSE if not.
//==============================================================================================

BOOL GS_FmodSound::SetSampleFrequency(int nIndex, int nFrequency) {

    // Exit if sound not initialized.
    if (!m_bIsReady) {
        return FALSE;
        }

    // Is the index within bounds?
    if ((nIndex >= 0) && (nIndex < m_gsSampleList.GetNumItems())) {
        // Change frequency of the specified sample in the list.
        if (FSOUND_Sample_SetDefaults(m_gsSampleList[nIndex], nFrequency, -1, -1, -1)) {
            return TRUE;
            }
        }

    return FALSE;
}


////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_FmodSound::SetSampleFrequency():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Returns: Nothing.
//==============================================================================================

void GS_FmodSound::SetSampleFrequency(int nFrequency) {

    // Exit if sound not initialized.
    if (!m_bIsReady) {
        return;
        }

    // For all the samples currently in the list.
    for (int nLoop = 0; nLoop < m_gsSampleList.GetNumItems(); nLoop++) {
        // Change frequency of the specified sample in the list.
        FSOUND_Sample_SetDefaults(m_gsSampleList[nLoop], nFrequency, -1, -1, -1);
        }
}


////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_FmodSound::GetSampleFrequency():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Returns: The frequency of the specified sample or -1 on failure.
//==============================================================================================

int GS_FmodSound::GetSampleFrequency(int nIndex) {

    // Exit if sound not initialized.
    if (!m_bIsReady) {
        return -1;
        }

    int nFrequency;

    // Is the index within bounds?
    if ((nIndex >= 0) && (nIndex < m_gsSampleList.GetNumItems())) {
        // Get frequency of the specified sample in the list.
        if (FSOUND_Sample_GetDefaults(m_gsSampleList[nIndex], &nFrequency, NULL, NULL, NULL)) {
            return nFrequency;
            }
        }

    return -1;
}


////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_FmodSound::SetSamplePanning():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Returns: TRUE if successfull, FALSE if not.
//==============================================================================================

BOOL GS_FmodSound::SetSamplePanning(int nIndex, int nPanning) {

    // Exit if sound not initialized.
    if (!m_bIsReady) {
        return FALSE;
        }

    // Is panning within range (0 - 255)?
    if (nPanning > 255) {
        nPanning = 255;
        }
    else if (nPanning < 0) {
        nPanning = 0;
        }

    // Is the index within bounds?
    if ((nIndex >= 0) && (nIndex < m_gsSampleList.GetNumItems())) {
        // Change panning of the specified sample in the list.
        if (FSOUND_Sample_SetDefaults(m_gsSampleList[nIndex], -1, -1, nPanning, -1)) {
            return TRUE;
            }
        }

    return FALSE;
}


////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_FmodSound::SetSamplePanning():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Returns: Nothing.
//==============================================================================================

void GS_FmodSound::SetSamplePanning(int nPanning) {

    // Exit if sound not initialized.
    if (!m_bIsReady) {
        return;
        }

    // Is panning within range (0 - 255)?
    if (nPanning > 255) {
        nPanning = 255;
        }
    else if (nPanning < 0) {
        nPanning = 0;
        }

    // For all the samples currently in the list.
    for (int nLoop = 0; nLoop < m_gsSampleList.GetNumItems(); nLoop++) {
        // Change panning of the specified sample in the list.
        FSOUND_Sample_SetDefaults(m_gsSampleList[nLoop], -1, -1, nPanning, -1);
        }
}


////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_FmodSound::GetSamplePanning():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Returns: The panning of the specified sample or -1 on failure.
//==============================================================================================

int GS_FmodSound::GetSamplePanning(int nIndex) {

    // Exit if sound not initialized.
    if (!m_bIsReady) {
        return -1;
        }

    int nPanning;

    // Is the index within bounds?
    if ((nIndex >= 0) && (nIndex < m_gsSampleList.GetNumItems())) {
        // Get panning of the specified sample in the list.
        if (FSOUND_Sample_GetDefaults(m_gsSampleList[nIndex], NULL, NULL, &nPanning, NULL)) {
            return nPanning;
            }
        }

    return -1;
}


////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_FmodSound::SetSamplePriority():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Returns: TRUE if successfull, FALSE if not.
//==============================================================================================

BOOL GS_FmodSound::SetSamplePriority(int nIndex, int nPriority) {

    // Exit if sound not initialized.
    if (!m_bIsReady) {
        return FALSE;
        }

    // Is priority within range (0 - 255)?
    if (nPriority > 255) {
        nPriority = 255;
        }
    else if (nPriority < 0) {
        nPriority = 0;
        }

    // Is the index within bounds?
    if ((nIndex >= 0) && (nIndex < m_gsSampleList.GetNumItems())) {
        // Change priority of the specified sample in the list.
        if (FSOUND_Sample_SetDefaults(m_gsSampleList[nIndex], -1, -1, -1, nPriority)) {
            return TRUE;
            }
        }

    return FALSE;
}


////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_FmodSound::SetSamplePriority():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Returns: Nothing.
//==============================================================================================

void GS_FmodSound::SetSamplePriority(int nPriority) {

    // Exit if sound not initialized.
    if (!m_bIsReady) {
        return;
        }

    // Is priority within range (0 - 255)?
    if (nPriority > 255) {
        nPriority = 255;
        }
    else if (nPriority < 0) {
        nPriority = 0;
        }

    // For all the samples currently in the list.
    for (int nLoop = 0; nLoop < m_gsSampleList.GetNumItems(); nLoop++) {
        // Change priority of the specified sample in the list.
        FSOUND_Sample_SetDefaults(m_gsSampleList[nLoop], -1, -1, -1, nPriority);
        }
}


////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_FmodSound::GetSamplePriority():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Returns: The priority of the specified sample or -1 on failure.
//==============================================================================================

int GS_FmodSound::GetSamplePriority(int nIndex) {

    // Exit if sound not initialized.
    if (!m_bIsReady) {
        return -1;
        }

    int nPriority;

    // Is the index within bounds?
    if ((nIndex >= 0) && (nIndex < m_gsSampleList.GetNumItems())) {
        // Get priority of the specified sample in the list.
        if (FSOUND_Sample_GetDefaults(m_gsSampleList[nIndex], NULL, NULL, NULL, &nPriority)) {
            return nPriority;
            }
        }

    return -1;
}


////////////////////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////////////////////
// Channel Methods /////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_FmodSound::StopChannel():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Returns: TRUE if successfull, FALSE if not.
//==============================================================================================

BOOL GS_FmodSound::StopChannel(int nChannel) {

    // Exit if sound not initialized.
    if (!m_bIsReady) {
        return FALSE;
        }

    // Was stopping channel successful?
    if (FSOUND_StopSound(nChannel)) {
        return TRUE;
        }

    return FALSE;
}


////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_FmodSound::StopAllChannels():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Returns: TRUE if successfull, FALSE if not.
//==============================================================================================

BOOL GS_FmodSound::StopAllChannels() {

    // Exit if sound not initialized.
    if (!m_bIsReady) {
        return FALSE;
        }

    // Stop playing all channels.
    if (FSOUND_StopSound(FSOUND_ALL)) {
        return TRUE;
        }

    return FALSE;
}


////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_FmodSound::IsChannelPlaying():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Returns: TRUE if channel is playing, FALSE if not.
//==============================================================================================

BOOL GS_FmodSound::IsChannelPlaying(int nChannel) {

    // Exit if sound not initialized.
    if (!m_bIsReady) {
        return FALSE;
        }

    // Is channel playing?
    if (FSOUND_IsPlaying(nChannel)) {
        return TRUE;
        }

    return FALSE;
}


////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_FmodSound::PauseChannel():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Returns: TRUE if successfull, FALSE if not.
//==============================================================================================

BOOL GS_FmodSound::PauseChannel(int nChannel, BOOL bPause) {

    // Exit if sound not initialized.
    if (!m_bIsReady) {
        return FALSE;
        }

    // Should channel be paused?
    if (bPause) {
        // Is channel not paused?
        if (!FSOUND_GetPaused(nChannel)) {
            // Pause channel.
            if (FSOUND_SetPaused(nChannel, TRUE)) {
                return TRUE;
                }
            }
        }
    else {
        // Is channel paused?
        if (FSOUND_GetPaused(nChannel)) {
            // Unpause channel.
            if (FSOUND_SetPaused(nChannel, FALSE)) {
                return TRUE;
                }
            }
        }

    return FALSE;
}


////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_FmodSound::PauseAllChannels():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Returns: TRUE if successfull, FALSE if not.
//==============================================================================================

BOOL GS_FmodSound::PauseAllChannels(BOOL bPause) {

    // Exit if sound not initialized.
    if (!m_bIsReady) {
        return FALSE;
        }

    // Should channels be paused?
    if (bPause) {
        // Pause all channels.
        if (FSOUND_SetPaused(FSOUND_ALL, TRUE)) {
            return TRUE;
            }
        }
    else {
        // Unpause all channels.
        if (FSOUND_SetPaused(FSOUND_ALL, FALSE)) {
            return TRUE;
            }
        }

    return FALSE;
}


////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_FmodSound::IsChannelPaused():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Returns: TRUE if channel is paused, FALSE if not.
//==============================================================================================

BOOL GS_FmodSound::IsChannelPaused(int nChannel) {

    // Exit if sound not initialized.
    if (!m_bIsReady) {
        return FALSE;
        }

    // Is channel paused?
    if (FSOUND_GetPaused(nChannel)) {
        return TRUE;
        }

    return FALSE;
}


////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_FmodSound::SetChannelVolume():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Returns: TRUE if successfull, FALSE if not.
//==============================================================================================

BOOL GS_FmodSound::SetChannelVolume(int nChannel, int nVolume) {

    // Exit if sound not initialized.
    if (!m_bIsReady) {
        return FALSE;
        }

    // Is volume within range (0 - 255)?
    if (nVolume > 255) {
        nVolume = 255;
        }
    else if (nVolume < 0) {
        nVolume = 0;
        }

    // Change volume of the specified channel.
    if (FSOUND_SetVolume(nChannel, nVolume)) {
        return TRUE;
        }

    return FALSE;
}


////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_FmodSound::SetChannelVolume():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Returns: TRUE if successfull, FALSE if not.
//==============================================================================================

BOOL GS_FmodSound::SetChannelVolume(int nVolume) {

    // Exit if sound not initialized.
    if (!m_bIsReady) {
        return FALSE;
        }

    // Is volume within range (0 - 255)?
    if (nVolume > 255) {
        nVolume = 255;
        }
    else if (nVolume < 0) {
        nVolume = 0;
        }

    // Change volume of all the channels.
    if (FSOUND_SetVolume(FSOUND_ALL, nVolume)) {
        return TRUE;
        }

    return FALSE;
}



////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_FmodSound::GetChannelVolume():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Returns: The volume of the specified channel or 0 on failure.
//==============================================================================================

int GS_FmodSound::GetChannelVolume(int nChannel) {

    // Exit if sound not initialized.
    if (!m_bIsReady) {
        return 0;
        }

    // Get volume of the specified channel.
    return(FSOUND_GetVolume(nChannel));
}


////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_FmodSound::SetChannelVolumeAbsolute():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Returns: TRUE if successfull, FALSE if not.
//==============================================================================================

BOOL GS_FmodSound::SetChannelVolumeAbsolute(int nChannel, int nVolume) {

    // Exit if sound not initialized.
    if (!m_bIsReady) {
        return FALSE;
        }

    // Is volume within range (0 - 255)?
    if (nVolume > 255) {
        nVolume = 255;
        }
    else if (nVolume < 0) {
        nVolume = 0;
        }

    // Change volume of the specified channel.
    if (FSOUND_SetVolumeAbsolute(nChannel, nVolume)) {
        return TRUE;
        }

    return FALSE;
}


////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_FmodSound::SetChannelVolumeAbsolute():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Returns: TRUE if successfull, FALSE if not.
//==============================================================================================

BOOL GS_FmodSound::SetChannelVolumeAbsolute(int nVolume) {

    // Exit if sound not initialized.
    if (!m_bIsReady) {
        return FALSE;
        }

    // Is volume within range (0 - 255)?
    if (nVolume > 255) {
        nVolume = 255;
        }
    else if (nVolume < 0) {
        nVolume = 0;
        }

    // Change volume of all the channels.
    if (FSOUND_SetVolumeAbsolute(FSOUND_ALL, nVolume)) {
        return TRUE;
        }

    return FALSE;
}


////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_FmodSound::SetChannelFrequency():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Returns: TRUE if successfull, FALSE if not.
//==============================================================================================

BOOL GS_FmodSound::SetChannelFrequency(int nChannel, int nFrequency) {

    // Exit if sound not initialized.
    if (!m_bIsReady) {
        return FALSE;
        }

    // Change frequency of the specified channel.
    if (FSOUND_SetFrequency(nChannel, nFrequency)) {
        return TRUE;
        }

    return FALSE;
}


////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_FmodSound::SetChannelFrequency():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Returns: TRUE if successfull, FALSE if not.
//==============================================================================================

BOOL GS_FmodSound::SetChannelFrequency(int nFrequency) {

    // Exit if sound not initialized.
    if (!m_bIsReady) {
        return FALSE;
        }

    // Change frequency of all the channels.
    if (FSOUND_SetFrequency(FSOUND_ALL, nFrequency)) {
        return TRUE;
        }

    return FALSE;
}



////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_FmodSound::GetChannelFrequency():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Returns: The frequency of the specified channel or 0 on failure.
//==============================================================================================

int GS_FmodSound::GetChannelFrequency(int nChannel) {

    // Exit if sound not initialized.
    if (!m_bIsReady) {
        return 0;
        }

    // Get frequency of the specified channel.
    return(FSOUND_GetFrequency(nChannel));
}


////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_FmodSound::SetChannelPanning():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Returns: TRUE if successfull, FALSE if not.
//==============================================================================================

BOOL GS_FmodSound::SetChannelPanning(int nChannel, int nPanning) {

    // Exit if sound not initialized.
    if (!m_bIsReady) {
        return FALSE;
        }

    // Is panning within range (0 - 255)?
    if (nPanning > 255) {
        nPanning = 255;
        }
    else if (nPanning < 0) {
        nPanning = 0;
        }

    // Change panning of the specified channel.
    if (FSOUND_SetPan(nChannel, nPanning)) {
        return TRUE;
        }

    return FALSE;
}


////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_FmodSound::SetChannelPanning():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Returns: TRUE if successfull, FALSE if not.
//==============================================================================================

BOOL GS_FmodSound::SetChannelPanning(int nPanning) {

    // Exit if sound not initialized.
    if (!m_bIsReady) {
        return FALSE;
        }

    // Is panning within range (0 - 255)?
    if (nPanning > 255) {
        nPanning = 255;
        }
    else if (nPanning < 0) {
        nPanning = 0;
        }

    // Change panning of all the channels.
    if (FSOUND_SetPan(FSOUND_ALL, nPanning)) {
        return TRUE;
        }

    return FALSE;
}



////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_FmodSound::GetChannelPanning():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Returns: The panning of the specified channel or 0 on failure.
//==============================================================================================

int GS_FmodSound::GetChannelPanning(int nChannel) {

    // Exit if sound not initialized.
    if (!m_bIsReady) {
        return 0;
        }

    // Get panning of the specified channel.
    return(FSOUND_GetPan(nChannel));
}


////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_FmodSound::SetChannelPriority():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Returns: TRUE if successfull, FALSE if not.
//==============================================================================================

BOOL GS_FmodSound::SetChannelPriority(int nChannel, int nPriority) {

    // Exit if sound not initialized.
    if (!m_bIsReady) {
        return FALSE;
        }

    // Is priority within range (0 - 255)?
    if (nPriority > 255) {
        nPriority = 255;
        }
    else if (nPriority < 0) {
        nPriority = 0;
        }

    // Change priority of the specified channel.
    if (FSOUND_SetPriority(nChannel, nPriority)) {
        return TRUE;
        }

    return FALSE;
}


////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_FmodSound::SetChannelPriority():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Returns: TRUE if successfull, FALSE if not.
//==============================================================================================

BOOL GS_FmodSound::SetChannelPriority(int nPriority) {

    // Exit if sound not initialized.
    if (!m_bIsReady) {
        return FALSE;
        }

    // Is priority within range (0 - 255)?
    if (nPriority > 255) {
        nPriority = 255;
        }
    else if (nPriority < 0) {
        nPriority = 0;
        }

    // Change priority of all the channels.
    if (FSOUND_SetPriority(FSOUND_ALL, nPriority)) {
        return TRUE;
        }

    return FALSE;
}



////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_FmodSound::GetChannelPriority():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Returns: The priority of the specified channel or 0 on failure.
//==============================================================================================

int GS_FmodSound::GetChannelPriority(int nChannel) {

    // Exit if sound not initialized.
    if (!m_bIsReady) {
        return 0;
        }

    // Get priority of the specified channel.
    return(FSOUND_GetPriority(nChannel));
}


////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_FmodSound::LoopChannel():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Returns: TRUE if successfull, FALSE if not.
//==============================================================================================

BOOL GS_FmodSound::LoopChannel(int nChannel, unsigned int uLoopMode) {

    // Exit if sound not initialized.
    if (!m_bIsReady) {
        return FALSE;
        }

    // Set loop mode to FSOUND_LOOP_NORMAL, FSOUND_LOOP_BIDI or FSOUND_LOOP_OFF.
    if (FSOUND_SetLoopMode(nChannel, uLoopMode)) {
        return TRUE;
        }

    return FALSE;
}


////////////////////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_FmodSound::GetMusic():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Returns: FMOD music handle if successful, NULL if failed.
//==============================================================================================

FMUSIC_MODULE* GS_FmodSound::GetMusic (int nIndex) {

    // Exit if sound not initialized.
    if (!m_bIsReady) {
        return NULL;
        }

    // Is index within bounds?
    if ((nIndex >= 0) && (nIndex < m_gsMusicList.GetNumItems())) {
        // Return music handle.
        return m_gsMusicList[nIndex];
        }

    return NULL;
}


////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_FmodSound::GetStream():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Returns: FMOD stream handle if successful, NULL if failed.
//==============================================================================================

FSOUND_STREAM* GS_FmodSound::GetStream(int nIndex) {

    // Exit if sound not initialized.
    if (!m_bIsReady) {
        return NULL;
        }

    // Is index within bounds?
    if ((nIndex >= 0) && (nIndex < m_gsStreamList.GetNumItems())) {
        // Return stream handle.
        return m_gsStreamList[nIndex];
        }

    return NULL;
}


////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_FmodSound::GetSample():
// ---------------------------------------------------------------------------------------------
// Purpose: ...
// ---------------------------------------------------------------------------------------------
// Returns: FMOD sample handle if successful, NULL if failed.
//==============================================================================================

FSOUND_SAMPLE* GS_FmodSound::GetSample(int nIndex) {

    // Exit if sound not initialized.
    if (!m_bIsReady) {
        return NULL;
        }

    // Is index within bounds?
    if ((nIndex >= 0) && (nIndex < m_gsSampleList.GetNumItems())) {
        // Return sample handle.
        return m_gsSampleList[nIndex];
        }

    return NULL;
}


////////////////////////////////////////////////////////////////////////////////////////////////
