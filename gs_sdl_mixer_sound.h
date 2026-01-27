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


#ifndef GS_SDL_MIXER_SOUND_H
#define GS_SDL_MIXER_SOUND_H


#ifdef GS_USE_SDL_MIXER


//==============================================================================================
// Include Game System (GS) header files.
// ---------------------------------------------------------------------------------------------
#include "gs_object.h"
#include "gs_list.h"
#include "gs_error.h"
#include "gs_platform.h"
//==============================================================================================


//==============================================================================================
// Include SDL2_mixer header files.
// ---------------------------------------------------------------------------------------------
#include <SDL2/SDL_mixer.h>
//==============================================================================================


////////////////////////////////////////////////////////////////////////////////////////////////
// Class Definition. ///////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////


class GS_SDLMixerSound : public GS_Object
{

private:

    BOOL m_bIsReady;        // Whether sound system has been initialized.
    BOOL m_bIsPaused;       // Whether sound system is paused or not.
    
    GS_List<Mix_Music*> m_gsMusicList;  // List of loaded music tracks (MP3, OGG, WAV, etc.).
    GS_List<Mix_Music*> m_gsStreamList; // List of loaded audio streams (same as music in SDL_mixer).
    GS_List<Mix_Chunk*> m_gsSampleList; // List of loaded sound samples (WAV files).
    
    int m_nMasterVolume;  // Master volume for all music, streams and samples (0-255).
    int m_nCurrentMusic;  // Index of currently playing music track (-1 if none).
    int m_nCurrentStream; // Index of currently playing stream (-1 if none).

protected:

    // ...

public:

    // Constructor & destructor.
    GS_SDLMixerSound();
    ~GS_SDLMixerSound();
    
    // Status methods.
    BOOL IsReady()  { return m_bIsReady; }
    BOOL IsPaused() { return m_bIsPaused; }
    
    // Initialize & shutdown methods.
    BOOL Initialize(HWND hWnd, BOOL bNoSound = FALSE);
    BOOL Shutdown();
    
    // Global control methods.
    void Pause(BOOL bPause);
    void SetMasterVolume(int nVolume);
    int  GetMasterVolume() { return m_nMasterVolume; }
    
    // Utility methods.
    static BOOL IsMIDIFile(const char* pszFilename) {
        const char* pExt = strrchr(pszFilename, '.');
        return pExt && (strcasecmp(pExt, ".mid") == 0 || strcasecmp(pExt, ".midi") == 0);
    }
    
    // Music methods (MOD, MP3, OGG, WAV files for background music).
    // Note: MIDI files not supported on Apple platforms.
    BOOL  AddMusic(const char* pszFilename);
    BOOL  PlayMusic(int nIndex, int nVolume = -1, float fSpeed = -1.0f, BOOL bLoop = FALSE);
    BOOL  StopMusic(int nIndex);
    void  StopAllMusic();
    BOOL  IsMusicPlaying(int nIndex);
    void  SetMusicVolume(int nVolume);
    int   GetMusicVolume();
    
    // Stream methods (MP3, OGG, WAV files for streaming audio).
    // Note: In SDL_mixer, streams and music both use Mix_Music*.
    BOOL AddStream(const char* pszFilename);
    int  PlayStream(int nIndex, int nVolume = -1);
    BOOL StopStream();
    BOOL IsStreamPlaying();
    BOOL SetStreamVolume(int nVolume);
    int  GetStreamVolume();
    BOOL PauseStream(BOOL bPause);
    
    // Sample methods (WAV files for sound effects).
    BOOL AddSample(const char* pszFilename);
    int  PlaySample(int nIndex, int nVolume = -1, int nPanning = -1);
    void SetSampleMaster(int nVolume);
    int  GetSampleMaster();
    BOOL SetSampleVolume(int nIndex, int nVolume);
    BOOL SetSamplePanning(int nIndex, int nPanning);
};

////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// Type alias for compatibility with existing GS_FmodSound code.
// ---------------------------------------------------------------------------------------------
typedef GS_SDLMixerSound GS_FmodSound;
//==============================================================================================


#endif // GS_USE_SDL_MIXER
#endif // GS_SDL_MIXER_SOUND_H
