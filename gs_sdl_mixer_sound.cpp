/*============================================================================================+
 | Game System (GS) Library                                                                   |
 |--------------------------------------------------------------------------------------------|
 | FILES: gs_sdl_mixer_sound.cpp, gs_sdl_mixer_sound.h                                        |
 |--------------------------------------------------------------------------------------------|
 | CLASS: GS_SDLMixerSound                                                                    |
 |--------------------------------------------------------------------------------------------|
 | ABOUT: SDL2_mixer implementation of the GS audio system. Provides music streaming and      |
 |        sound effect playback using SDL_mixer. This class serves as a drop-in replacement   |
 |        for GS_FmodSound on platforms where FMOD is not available or desired.               |
 |--------------------------------------------------------------------------------------------|
 |                                                                                    01/2026 |
 +============================================================================================*/


#ifdef GS_USE_SDL_MIXER


//==============================================================================================
// Include Game System (GS) header files.
// ---------------------------------------------------------------------------------------------
#include "gs_sdl_mixer_sound.h"
//==============================================================================================


////////////////////////////////////////////////////////////////////////////////////////////////
// Constructor/Destructor Methods. /////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_SDLMixerSound::GS_SDLMixerSound():
// ---------------------------------------------------------------------------------------------
// Purpose: The constructor, initializes class data to defaults when class object is created.
// ---------------------------------------------------------------------------------------------
// Returns: Nothing.
//==============================================================================================

GS_SDLMixerSound::GS_SDLMixerSound() {

    m_bIsReady  = FALSE;
    m_bIsPaused = FALSE;

    m_nMasterVolume = 128;

    m_nCurrentMusic  = -1;
    m_nCurrentStream = -1;
}


////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_SDLMixerSound::~GS_SDLMixerSound():
// ---------------------------------------------------------------------------------------------
// Purpose: The destructor, de-initializes class data when class object is destroyed and frees
//          all memory used by it.
// ---------------------------------------------------------------------------------------------
// Returns: Nothing.
//==============================================================================================

GS_SDLMixerSound::~GS_SDLMixerSound() {

    if (m_bIsReady) {
        this->Shutdown();
    }
}


////////////////////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////////////////////
// Initialize/Shutdown Methods /////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_SDLMixerSound::Initialize():
// ---------------------------------------------------------------------------------------------
// Purpose: Initializes the SDL_mixer audio system with default settings (44.1kHz, stereo,
//          2048 byte buffer). Allocates 64 mixing channels for simultaneous sound effects.
// ---------------------------------------------------------------------------------------------
// Returns: TRUE if successful, FALSE if not.
//==============================================================================================

BOOL GS_SDLMixerSound::Initialize(HWND hWnd, BOOL bNoSound) {

    // Window handle not used by SDL_mixer.
    (void)hWnd;
    
    // Shutdown sound if already initialized.
    if (m_bIsReady) {
        this->Shutdown();
    }
    
    // Should we enable sound?
    if (bNoSound) {
        m_bIsReady = TRUE;
        return TRUE;
    }
    
    // Initialize SDL audio subsystem if not already initialized.
    if (SDL_WasInit(SDL_INIT_AUDIO) == 0) {
        if (SDL_InitSubSystem(SDL_INIT_AUDIO) < 0) {
            GS_Error::Report("GS_SDL_MIXER_SOUND.CPP", 88, "Failed to initialize SDL audio!");
            return FALSE;
        }
    }
    
    // Open the audio device with CD quality (44.1kHz, stereo, 2048 byte chunks).
    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
        GS_Error::Report("GS_SDL_MIXER_SOUND.CPP", 95, "Failed to initialize SDL_mixer!");
        return FALSE;
    }
    
    // Allocate 64 mixing channels for simultaneous sound effect playback.
    Mix_AllocateChannels(64);

    m_bIsReady = TRUE;

    return m_bIsReady;
}


////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_SDLMixerSound::Shutdown():
// ---------------------------------------------------------------------------------------------
// Purpose: Shuts down the SDL_mixer audio system and frees all loaded music, streams, and
//          sample resources.
// ---------------------------------------------------------------------------------------------
// Returns: TRUE if successful, FALSE if not.
//==============================================================================================

BOOL GS_SDLMixerSound::Shutdown() {

    // Can't shutdown if sound not initialized.
    if (!m_bIsReady) {
        return FALSE;
    }
    
    // Stop all playback.
    this->StopAllMusic();
    this->StopStream();
    
    // Free all loaded music tracks.
    for (int nLoop = 0; nLoop < m_gsMusicList.GetNumItems(); nLoop++) {
        Mix_FreeMusic(m_gsMusicList[nLoop]);
    }
    m_gsMusicList.RemoveAllItems();
    
    // Free all loaded audio streams.
    for (int nLoop = 0; nLoop < m_gsStreamList.GetNumItems(); nLoop++) {
        Mix_FreeMusic(m_gsStreamList[nLoop]);
    }
    m_gsStreamList.RemoveAllItems();
    
    // Free all loaded sound samples.
    for (int nLoop = 0; nLoop < m_gsSampleList.GetNumItems(); nLoop++) {
        Mix_FreeChunk(m_gsSampleList[nLoop]);
    }
    m_gsSampleList.RemoveAllItems();
    
    // Close the audio device.
    Mix_CloseAudio();

    m_bIsReady  = FALSE;
    m_bIsPaused = FALSE;

    m_nCurrentMusic  = -1;
    m_nCurrentStream = -1;

    return TRUE;
}


////////////////////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////////////////////
// Global Sound Methods ////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_SDLMixerSound::Pause():
// ---------------------------------------------------------------------------------------------
// Purpose: Pauses or resumes all audio playback (music, streams, and sound effects).
// ---------------------------------------------------------------------------------------------
// Returns: Nothing.
//==============================================================================================

void GS_SDLMixerSound::Pause(BOOL bPause) {

    // Exit if sound not initialized or already in requested state.
    if (!m_bIsReady || m_bIsPaused == bPause) {
        return;
    }
    
    // Should audio be paused?
    if (bPause) {
        // Pause music playback.
        Mix_PauseMusic();
        // Pause all sound effect channels.
        Mix_Pause(-1);
    }
    else {
        // Resume music playback.
        Mix_ResumeMusic();
        // Resume all sound effect channels.
        Mix_Resume(-1);
    }

    m_bIsPaused = bPause;
}


////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_SDLMixerSound::SetMasterVolume():
// ---------------------------------------------------------------------------------------------
// Purpose: Sets the master volume for both music and sound effects.
// ---------------------------------------------------------------------------------------------
// Notes: In SDL_mixer, music and samples have separate volume controls. This method sets
//        both to the specified volume level.
// ---------------------------------------------------------------------------------------------
// Returns: Nothing.
//==============================================================================================

void GS_SDLMixerSound::SetMasterVolume(int nVolume) {

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

    // Set music volume.
    this->SetMusicVolume(nVolume);

    // Set sample volume.
    this->SetSampleMaster(nVolume);
}


////////////////////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////////////////////
// Music Methods ///////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_SDLMixerSound::AddMusic():
// ---------------------------------------------------------------------------------------------
// Purpose: Loads a music file and adds it to the music list. Supports MP3, OGG, WAV, FLAC,
//          MOD, XM, IT, and other formats supported by SDL_mixer.
// ---------------------------------------------------------------------------------------------
// Notes: MIDI files may not be supported on Apple platforms due to missing native support
//        in SDL_mixer. Consider converting MIDI files to OGG or MP3 for better compatibility.
// ---------------------------------------------------------------------------------------------
// Returns: TRUE if successful, FALSE if not.
//==============================================================================================

BOOL GS_SDLMixerSound::AddMusic(const char* pszFilename) {

    // Exit if sound not initialized.
    if (!m_bIsReady) {
        return FALSE;
    }

    // Check if file is a MIDI file on Apple platforms.
#if defined(__APPLE__)
    const char* pExt = strrchr(pszFilename, '.');
    if (pExt && (strcasecmp(pExt, ".mid") == 0 || strcasecmp(pExt, ".midi") == 0)) {
        GS_Error::Report("GS_SDL_MIXER_SOUND.CPP", 267, "MIDI files are not supported on Apple platforms. Please convert to OGG or MP3.");
        return FALSE;
    }
#endif

    Mix_Music* pMusic;

    // Load the specified music file.
    pMusic = Mix_LoadMUS(pszFilename);

    // Was the music loaded successfully?
    if (!pMusic) {
        char szError[512];
        sprintf(szError, "Failed to load music file '%s': %s", pszFilename, Mix_GetError());
        GS_Error::Report("GS_SDL_MIXER_SOUND.CPP", 267, szError);
        return FALSE;
    }

    // Add the music to the list.
    m_gsMusicList.AddItem(pMusic);

    return TRUE;
}


////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_SDLMixerSound::PlayMusic():
// ---------------------------------------------------------------------------------------------
// Purpose: Plays a loaded music track with optional volume and loop settings.
// ---------------------------------------------------------------------------------------------
// Notes: The fSpeed parameter is ignored as SDL_mixer doesn't support playback speed changes
//        without additional processing.
// ---------------------------------------------------------------------------------------------
// Returns: TRUE if successful, FALSE if not.
//==============================================================================================

BOOL GS_SDLMixerSound::PlayMusic(int nIndex, int nVolume, float fSpeed, BOOL bLoop) {

    // Speed parameter not supported by SDL_mixer.
    (void)fSpeed;
    
    // Exit if sound not initialized or paused.
    if (!m_bIsReady || m_bIsPaused) {
        return FALSE;
    }

    // Is index within bounds?
    if ((nIndex >= 0) && (nIndex < m_gsMusicList.GetNumItems())) {
        // Have the user specified a valid volume?
        if (nVolume >= 0) {
            // Set the music volume (convert 0-255 range to SDL_mixer's 0-128 range).
            this->SetMusicVolume(nVolume);
        }

        // Play the music (-1 for infinite loop, 0 for play once).
        if (Mix_PlayMusic(m_gsMusicList[nIndex], bLoop ? -1 : 0) == -1) {
            GS_Error::Report("GS_SDL_MIXER_SOUND.CPP", 318, "Failed to play music!");
            return FALSE;
        }

        // Remember which music is playing.
        m_nCurrentMusic = nIndex;
        return TRUE;
    }

    return FALSE;
}


////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_SDLMixerSound::StopMusic():
// ---------------------------------------------------------------------------------------------
// Purpose: Stops music playback. Note that in SDL_mixer, only one music track can play at
//          a time, so the nIndex parameter is ignored.
// ---------------------------------------------------------------------------------------------
// Returns: TRUE if successful, FALSE if not.
//==============================================================================================

BOOL GS_SDLMixerSound::StopMusic(int nIndex) {

    // Index parameter not used in SDL_mixer (only one music track plays at a time).
    (void)nIndex;

    // Exit if sound not initialized.
    if (!m_bIsReady) {
        return FALSE;
    }

    // Stop music playback.
    Mix_HaltMusic();

    m_nCurrentMusic = -1;

    return TRUE;
}


////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_SDLMixerSound::StopAllMusic():
// ---------------------------------------------------------------------------------------------
// Purpose: Stops all music playback (same as StopMusic in SDL_mixer).
// ---------------------------------------------------------------------------------------------
// Returns: Nothing.
//==============================================================================================

void GS_SDLMixerSound::StopAllMusic() {

    // Exit if sound not initialized.
    if (!m_bIsReady) {
        return;
    }

    // Stop music playback.
    Mix_HaltMusic();

    m_nCurrentMusic = -1;
}


////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_SDLMixerSound::IsMusicPlaying():
// ---------------------------------------------------------------------------------------------
// Purpose: Checks if any music is currently playing or paused.
// ---------------------------------------------------------------------------------------------
// Returns: TRUE if music is playing or paused, FALSE if stopped.
//==============================================================================================

BOOL GS_SDLMixerSound::IsMusicPlaying(int nIndex) {

    // Index parameter not used in SDL_mixer.
    (void)nIndex;

    // Exit if sound not initialized.
    if (!m_bIsReady) {
        return FALSE;
    }

    // Is music playing or paused?
    return Mix_PlayingMusic();
}


////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_SDLMixerSound::SetMusicVolume():
// ---------------------------------------------------------------------------------------------
// Purpose: Sets the music volume level.
// ---------------------------------------------------------------------------------------------
// Notes: SDL_mixer uses a 0-128 volume range, so we convert from GS's 0-255 range.
// ---------------------------------------------------------------------------------------------
// Returns: Nothing.
//==============================================================================================

void GS_SDLMixerSound::SetMusicVolume(int nVolume) {

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

    // Set music volume (convert 0-255 to SDL_mixer's 0-128 range).
    Mix_VolumeMusic(nVolume * 128 / 255);
}


////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_SDLMixerSound::GetMusicVolume():
// ---------------------------------------------------------------------------------------------
// Purpose: Gets the current music volume level.
// ---------------------------------------------------------------------------------------------
// Returns: The current music volume (0-255).
//==============================================================================================

int GS_SDLMixerSound::GetMusicVolume() {

    // Exit if sound not initialized.
    if (!m_bIsReady) {
        return 0;
    }

    // Get music volume (convert SDL_mixer's 0-128 range to 0-255).
    return Mix_VolumeMusic(-1) * 255 / 128;
}


////////////////////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////////////////////
// Stream Methods //////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_SDLMixerSound::AddStream():
// ---------------------------------------------------------------------------------------------
// Purpose: Loads an audio stream file and adds it to the stream list.
// ---------------------------------------------------------------------------------------------
// Notes: In SDL_mixer, streams and music both use Mix_Music* internally. The distinction is
//        maintained for API compatibility with GS_FmodSound.
//        MIDI files may not be supported on Apple platforms.
// ---------------------------------------------------------------------------------------------
// Returns: TRUE if successful, FALSE if not.
//==============================================================================================

BOOL GS_SDLMixerSound::AddStream(const char* pszFilename) {

    // Exit if sound not initialized.
    if (!m_bIsReady) {
        return FALSE;
    }

    // Check if file is a MIDI file on Apple platforms.
#if defined(__APPLE__)
    const char* pExt = strrchr(pszFilename, '.');
    if (pExt && (strcasecmp(pExt, ".mid") == 0 || strcasecmp(pExt, ".midi") == 0)) {
        GS_Error::Report("GS_SDL_MIXER_SOUND.CPP", 511, "MIDI files are not supported on Apple platforms. Please convert to OGG or MP3.");
        return FALSE;
    }
#endif

    Mix_Music* pStream;

    // Load the specified stream file.
    pStream = Mix_LoadMUS(pszFilename);

    // Was the stream loaded successfully?
    if (!pStream) {
        char szError[512];
        sprintf(szError, "Failed to load stream file '%s': %s", pszFilename, Mix_GetError());
        GS_Error::Report("GS_SDL_MIXER_SOUND.CPP", 511, szError);
        return FALSE;
    }

    // Add the stream to the list.
    m_gsStreamList.AddItem(pStream);

    return TRUE;
}


////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_SDLMixerSound::PlayStream():
// ---------------------------------------------------------------------------------------------
// Purpose: Plays a loaded audio stream with optional volume setting. Streams always loop
//          infinitely in this implementation.
// ---------------------------------------------------------------------------------------------
// Returns: 0 on success, -1 on failure.
//==============================================================================================

int GS_SDLMixerSound::PlayStream(int nIndex, int nVolume) {

    // Exit if sound not initialized or paused.
    if (!m_bIsReady || m_bIsPaused) {
        return -1;
    }

    // Is index within bounds?
    if ((nIndex >= 0) && (nIndex < m_gsStreamList.GetNumItems())) {
        // Have the user specified a valid volume?
        if (nVolume >= 0) {
            // Set the stream volume.
            this->SetStreamVolume(nVolume);
        }

        // Play the stream with infinite looping.
        if (Mix_PlayMusic(m_gsStreamList[nIndex], -1) == -1) {
            GS_Error::Report("GS_SDL_MIXER_SOUND.CPP", 557, "Failed to play stream!");
            return -1;
        }

        // Remember which stream is playing.
        m_nCurrentStream = nIndex;
        return 0;
    }

    return -1;
}


////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_SDLMixerSound::StopStream():
// ---------------------------------------------------------------------------------------------
// Purpose: Stops stream playback.
// ---------------------------------------------------------------------------------------------
// Returns: TRUE if successful, FALSE if not.
//==============================================================================================

BOOL GS_SDLMixerSound::StopStream() {

    // Exit if sound not initialized.
    if (!m_bIsReady) {
        return FALSE;
    }

    // Stop stream playback.
    Mix_HaltMusic();

    m_nCurrentStream = -1;

    return TRUE;
}


////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_SDLMixerSound::IsStreamPlaying():
// ---------------------------------------------------------------------------------------------
// Purpose: Checks if a stream is currently playing or paused.
// ---------------------------------------------------------------------------------------------
// Returns: TRUE if stream is playing or paused, FALSE if stopped.
//==============================================================================================

BOOL GS_SDLMixerSound::IsStreamPlaying() {

    // Exit if sound not initialized.
    if (!m_bIsReady) {
        return FALSE;
    }

    // Is stream playing or paused?
    return Mix_PlayingMusic();
}


////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_SDLMixerSound::SetStreamVolume():
// ---------------------------------------------------------------------------------------------
// Purpose: Sets the stream volume level.
// ---------------------------------------------------------------------------------------------
// Returns: TRUE if successful, FALSE if not.
//==============================================================================================

BOOL GS_SDLMixerSound::SetStreamVolume(int nVolume) {

    // Exit if sound not initialized.
    if (!m_bIsReady) {
        return FALSE;
    }

    // Make sure volume is within range (0 - 255).
    if (nVolume < 0) {
        nVolume = 0;
    }
    if (nVolume > 255) {
        nVolume = 255;
    }

    // Set stream volume (convert 0-255 to SDL_mixer's 0-128 range).
    Mix_VolumeMusic(nVolume * 128 / 255);

    return TRUE;
}


////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_SDLMixerSound::GetStreamVolume():
// ---------------------------------------------------------------------------------------------
// Purpose: Gets the current stream volume level.
// ---------------------------------------------------------------------------------------------
// Returns: The current stream volume (0-255).
//==============================================================================================

int GS_SDLMixerSound::GetStreamVolume() {

    // Exit if sound not initialized.
    if (!m_bIsReady) {
        return 0;
    }

    // Get stream volume (convert SDL_mixer's 0-128 range to 0-255).
    return Mix_VolumeMusic(-1) * 255 / 128;
}


////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_SDLMixerSound::PauseStream():
// ---------------------------------------------------------------------------------------------
// Purpose: Pauses or resumes stream playback.
// ---------------------------------------------------------------------------------------------
// Returns: TRUE if successful, FALSE if not.
//==============================================================================================

BOOL GS_SDLMixerSound::PauseStream(BOOL bPause) {

    // Exit if sound not initialized.
    if (!m_bIsReady) {
        return FALSE;
    }

    // Should stream be paused?
    if (bPause) {
        // Pause stream playback.
        Mix_PauseMusic();
    }
    else {
        // Resume stream playback.
        Mix_ResumeMusic();
    }

    return TRUE;
}


////////////////////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////////////////////
// Sample Methods //////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_SDLMixerSound::AddSample():
// ---------------------------------------------------------------------------------------------
// Purpose: Loads a sound sample file and adds it to the sample list. WAV files are
//          recommended for best compatibility.
// ---------------------------------------------------------------------------------------------
// Returns: TRUE if successful, FALSE if not.
//==============================================================================================

BOOL GS_SDLMixerSound::AddSample(const char* pszFilename) {

    // Exit if sound not initialized.
    if (!m_bIsReady) {
        return FALSE;
    }

    Mix_Chunk* pSample;

    // Load the specified sample file.
    pSample = Mix_LoadWAV(pszFilename);

    // Was the sample loaded successfully?
    if (!pSample) {
        GS_Error::Report("GS_SDL_MIXER_SOUND.CPP", 745, "Failed to load sample file!");
        return FALSE;
    }

    // Add the sample to the list.
    m_gsSampleList.AddItem(pSample);

    return TRUE;
}


////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_SDLMixerSound::PlaySample():
// ---------------------------------------------------------------------------------------------
// Purpose: Plays a loaded sound sample with optional volume and panning.
// ---------------------------------------------------------------------------------------------
// Notes: The panning parameter is currently ignored as SDL_mixer's panning requires a more
//        complex channel-based approach.
// ---------------------------------------------------------------------------------------------
// Returns: The channel number playing the sample, or -1 on error.
//==============================================================================================

int GS_SDLMixerSound::PlaySample(int nIndex, int nVolume, int nPanning) {

    // Panning parameter not currently implemented.
    (void)nPanning;
    
    // Exit if sound not initialized or paused.
    if (!m_bIsReady || m_bIsPaused) {
        return -1;
    }

    // Is index within bounds?
    if ((nIndex >= 0) && (nIndex < m_gsSampleList.GetNumItems())) {
        // Have the user specified a valid volume?
        if (nVolume >= 0) {
            // Set the sample volume.
            this->SetSampleVolume(nIndex, nVolume);
        }

        // Play the sample on the first available channel.
        return Mix_PlayChannel(-1, m_gsSampleList[nIndex], 0);
    }

    return -1;
}


////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_SDLMixerSound::SetSampleMaster():
// ---------------------------------------------------------------------------------------------
// Purpose: Sets the master volume for all sound effects.
// ---------------------------------------------------------------------------------------------
// Returns: Nothing.
//==============================================================================================

void GS_SDLMixerSound::SetSampleMaster(int nVolume) {

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

    // Set master volume for all channels (convert 0-255 to SDL_mixer's 0-128 range).
    Mix_Volume(-1, nVolume * 128 / 255);
}


////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_SDLMixerSound::GetSampleMaster():
// ---------------------------------------------------------------------------------------------
// Purpose: Gets the master volume for all sound effects.
// ---------------------------------------------------------------------------------------------
// Returns: The master sample volume (0-255).
//==============================================================================================

int GS_SDLMixerSound::GetSampleMaster() {

    // Exit if sound not initialized.
    if (!m_bIsReady) {
        return 0;
    }

    // Get master volume for all channels (convert SDL_mixer's 0-128 range to 0-255).
    return Mix_Volume(-1, -1) * 255 / 128;
}


////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_SDLMixerSound::SetSampleVolume():
// ---------------------------------------------------------------------------------------------
// Purpose: Sets the volume for a specific sample.
// ---------------------------------------------------------------------------------------------
// Returns: TRUE if successful, FALSE if not.
//==============================================================================================

BOOL GS_SDLMixerSound::SetSampleVolume(int nIndex, int nVolume) {

    // Exit if sound not initialized.
    if (!m_bIsReady) {
        return FALSE;
    }

    // Make sure volume is within range (0 - 255).
    if (nVolume < 0) {
        nVolume = 0;
    }
    if (nVolume > 255) {
        nVolume = 255;
    }

    // Is index within bounds?
    if ((nIndex >= 0) && (nIndex < m_gsSampleList.GetNumItems())) {
        // Set sample volume (convert 0-255 to SDL_mixer's 0-128 range).
        Mix_VolumeChunk(m_gsSampleList[nIndex], nVolume * 128 / 255);
        return TRUE;
    }

    return FALSE;
}


////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_SDLMixerSound::SetSamplePanning():
// ---------------------------------------------------------------------------------------------
// Purpose: Sets the panning for a specific sample.
// ---------------------------------------------------------------------------------------------
// Notes: Currently not fully implemented as SDL_mixer requires a channel-based approach for
//        panning, which is more complex than the sample-based approach used by FMOD.
// ---------------------------------------------------------------------------------------------
// Returns: TRUE (placeholder).
//==============================================================================================

BOOL GS_SDLMixerSound::SetSamplePanning(int nIndex, int nPanning) {

    // Panning not currently implemented.
    (void)nIndex;
    (void)nPanning;

    // SDL_mixer panning requires channel-based approach.
    // This would need to be implemented using Mix_SetPanning() on active channels.
    
    return TRUE;
}


////////////////////////////////////////////////////////////////////////////////////////////////


#endif // GS_USE_SDL_MIXER
