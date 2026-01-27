/*============================================================================================+
 | Game System (GS) Library                                                                   |
 |--------------------------------------------------------------------------------------------|
 | FILES: gs_timer.cpp, gs_timer.h                                                            |
 |--------------------------------------------------------------------------------------------|
 | CLASS: GS_Timer, GS_FrameTimer                                                             |
 |--------------------------------------------------------------------------------------------|
 | ABOUT: ...                                                                                 |
 |--------------------------------------------------------------------------------------------|
 |                                                                                    02/2003 |
 +============================================================================================*/

#ifndef GS_TIMER_H
#define GS_TIMER_H


//==============================================================================================
// Include platform abstraction header files.
// ---------------------------------------------------------------------------------------------
#include "gs_platform.h"
//==============================================================================================

#ifdef GS_PLATFORM_WINDOWS
//==============================================================================================
// Link to Windows multimedia library (Windows only).
// ---------------------------------------------------------------------------------------------
#pragma comment(lib,"winmm.lib")
//==============================================================================================

//==============================================================================================
// Include Windows header files.
// ---------------------------------------------------------------------------------------------
#include <mmsystem.h>
//==============================================================================================
#endif

//==============================================================================================
// Include Game System (GS) header files.
// ---------------------------------------------------------------------------------------------
#include "gs_object.h"
//==============================================================================================


////////////////////////////////////////////////////////////////////////////////////////////////
// Class Definition. ///////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////


class GS_Timer : public GS_Object
{

private:

#ifdef GS_PLATFORM_WINDOWS
    BOOL    m_bHaveCounter;  // Whether the system supports performance counters.
    __int64 m_n64PerfFreq;   // The frequency (counts per second) of the performance counter.
    __int64 m_n64StartCount; // The initial number of counts of the performance counter.
    DWORD   m_dwStartTime;   // The initial start time of the multi-media counter.
#else
    unsigned long m_lStartTime; // Start time in milliseconds (SDL)
#endif

protected:

    // No protected members.

public:

    GS_Timer();
    ~GS_Timer();

    void  Reset();
    float GetTime();
    void  Wait(float fWaitTime);
};


////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// The frame-rate is determined by dividing a number of frames by the time (in seconds) it took
// to complete them. The value below indicates the number of frames that will be used to
// calculate the frame rate.
// ---------------------------------------------------------------------------------------------
#define TOTAL_FRAMES 30
//==============================================================================================


////////////////////////////////////////////////////////////////////////////////////////////////
// Class Definition. ///////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////


class GS_FrameTimer : public GS_Timer
{

private:

    int   m_nFramesTotal;     // The number of frames completed.
    float m_fFrameTimesTotal; // The total time it took to complete m_nFramesCompleted.
    float m_fFrameRate;       // The current frame rate in frames per second.

protected:

    // No protected members.

public:

    GS_FrameTimer();
    ~GS_FrameTimer();

    void MarkFrame();
    float GetFrameTime();
    float GetFrameRate()
    {
        return m_fFrameRate;
    };
};

////////////////////////////////////////////////////////////////////////////////////////////////

#endif
