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


#ifndef GS_FMOD_SOUND_H
#define GS_FMOD_SOUND_H


//==============================================================================================
// Include platform abstraction header files.
// ---------------------------------------------------------------------------------------------
#include "gs_platform.h"
//==============================================================================================


//==============================================================================================
// Include FMOD header files.
// ---------------------------------------------------------------------------------------------
#include "fmod.h"
#include "fmod_errors.h"
//==============================================================================================


//==============================================================================================
// Include Game System (GS) header files.
// ---------------------------------------------------------------------------------------------
#include "gs_error.h"
#include "gs_list.h"
#include "gs_object.h"
//==============================================================================================


////////////////////////////////////////////////////////////////////////////////////////////////
// Class Definition. ///////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////


class GS_FmodSound : public GS_Object
{

private:

    BOOL m_bIsReady;        // Wether sound system has been initialized.
    BOOL m_bIsPaused;       // Wether sound system is paused or not.

    GS_List<FMUSIC_MODULE*> m_gsMusicList;  // MOD, S3M ,XM ,IT ,MID ,RMI ,SGT ,FSB files.
    GS_List<FSOUND_STREAM*> m_gsStreamList; // WAV, MP2, MP3, OGG, RAW and others (long).
    GS_List<FSOUND_SAMPLE*> m_gsSampleList; // WAV, MP2, MP3, OGG, RAW and others (short).

    int m_nMasterVolume; // Master volume for all music, streams and samples.

    int m_nStreamIndex;  // Index of currently playing stream.
    int m_nStreamChannel;// Handle of channels used by stream.

protected:

    // ...

public:

    GS_FmodSound();
    ~GS_FmodSound();

    BOOL IsReady()
    {
        return m_bIsReady;
    }
    BOOL IsPaused()
    {
        return m_bIsPaused;
    }

    BOOL Initialize(HWND hWnd, BOOL bNoSound = FALSE);
    BOOL Shutdown();

    void Pause(BOOL bPause);
    void SetMasterVolume(int nVolume);
    int  GetMasterVolume()
    {
        return m_nMasterVolume;
    }

    BOOL  AddMusic(const char* pszFilename);
    BOOL  RemoveMusic(int nIndex);
    BOOL  ClearMusicList();
    int   GetNumMusics();
    BOOL  PlayMusic(int nIndex, int nVolume = -1, float fSpeed = -1.0f, BOOL bLoop = FALSE);
    BOOL  StopMusic(int nIndex);
    void  StopAllMusic();
    BOOL  LoopMusic(int nIndex, BOOL bLoop);
    BOOL  IsMusicPlaying(int nIndex);
    BOOL  IsMusicDone(int nIndex);
    BOOL  PauseMusic(int nIndex, BOOL bPause);
    void  PauseAllMusic(BOOL bPause);
    BOOL  IsMusicPaused(int nIndex);
    BOOL  SetMusicVolume(int nIndex, int nVolume);
    BOOL  SetMusicVolume(int nVolume);
    int   GetMusicVolume(int nIndex);
    BOOL  SetMusicSpeed(int nIndex, float fSpeed);
    void  SetMusicSpeed(float fSpeed);
    float GetMusicSpeed(int nIndex);

    BOOL AddStream(const char* pszFilename);
    BOOL RemoveStream(int nIndex);
    BOOL ClearStreamList();
    int  GetNumStreams();
    int  PlayStream(int nIndex, int nVolume = -1);
    BOOL StopStream();
    BOOL LoopStream(int nIndex, int nLoopTimes);
    BOOL IsStreamPlaying();
    BOOL PauseStream(BOOL bPause);
    BOOL IsStreamPaused();
    BOOL SetStreamVolume(int nVolume);
    int  GetStreamVolume();

    BOOL AddSample(const char* pszFilename);
    BOOL RemoveSample(int nIndex);
    BOOL ClearSampleList();
    int  GetNumSamples();
    int  PlaySample(int nIndex, int nVolume = -1, int nPanning = -1);
    void SetSampleMaster(int nVolume);
    int  GetSampleMaster();
    BOOL SetSampleVolume(int nIndex, int nVolume);
    BOOL SetSampleVolume(int nVolume);
    int  GetSampleVolume(int nIndex);
    BOOL SetSampleFrequency(int nIndex, int nFrequency);
    void SetSampleFrequency(int nFrequency);
    int  GetSampleFrequency(int nIndex);
    BOOL SetSamplePanning(int nIndex, int nPanning = FSOUND_STEREOPAN);
    void SetSamplePanning(int nPanning = FSOUND_STEREOPAN);
    int  GetSamplePanning(int nIndex);
    BOOL SetSamplePriority(int nIndex, int nPriority);
    void SetSamplePriority(int nPriority);
    int  GetSamplePriority(int nIndex);

    BOOL StopChannel(int nChannel);
    BOOL StopAllChannels();
    BOOL IsChannelPlaying(int nChannel);
    BOOL PauseChannel(int nChannel, BOOL bPause);
    BOOL PauseAllChannels(BOOL bPause);
    BOOL IsChannelPaused(int nChannel);
    BOOL SetChannelVolume(int Channel, int nVolume);
    BOOL SetChannelVolume(int nVolume);
    int  GetChannelVolume(int Channel);
    BOOL SetChannelVolumeAbsolute(int Channel, int nVolume);
    BOOL SetChannelVolumeAbsolute(int nVolume);
    BOOL SetChannelFrequency(int Channel, int nFrequency);
    BOOL SetChannelFrequency(int nFrequency);
    int  GetChannelFrequency(int Channel);
    BOOL SetChannelPanning(int Channel, int nPanning = FSOUND_STEREOPAN);
    BOOL SetChannelPanning(int nPanning = FSOUND_STEREOPAN);
    int  GetChannelPanning(int Channel);
    BOOL SetChannelPriority(int Channel, int nPriority);
    BOOL SetChannelPriority(int nPriority);
    int  GetChannelPriority(int Channel);
    BOOL LoopChannel(int nChannel, unsigned int uLoopMode);

    FMUSIC_MODULE* GetMusic (int nIndex);
    FSOUND_STREAM* GetStream(int nIndex);
    FSOUND_SAMPLE* GetSample(int nIndex);
};

////////////////////////////////////////////////////////////////////////////////////////////////

#endif
