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


//==============================================================================================
// Include Game System (GS) header files.
// ---------------------------------------------------------------------------------------------
#include "gs_timer.h"
//==============================================================================================


////////////////////////////////////////////////////////////////////////////////////////////////
// Constructor/Destructor Methods //////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_Timer::GS_Timer():
// ---------------------------------------------------------------------------------------------
// Purpose: The constructor, initializes class data when class object is created.
// ---------------------------------------------------------------------------------------------
// Returns: Nothing.
//==============================================================================================

GS_Timer::GS_Timer()
{
#ifdef GS_PLATFORM_WINDOWS
    // Check if the current system supports high frequency performance counters and get the
    // system's performance frequency (if it is available).
    if (QueryPerformanceFrequency((LARGE_INTEGER*) &m_n64PerfFreq))
    {
        // Set flag to indicate that the system supports performance counters.
        m_bHaveCounter = TRUE;
        // Query the performance counter to determine the current number of counts.
        QueryPerformanceCounter((LARGE_INTEGER*) &m_n64StartCount);
    }
    else
    {
        // Set flag to indicate that the system does not support performance counters.
        m_bHaveCounter = FALSE;
        // Get the system time in milliseconds.
        m_dwStartTime = timeGetTime();
    }
#else
    m_lStartTime = GS_Platform::GetTickCount();
#endif
}


////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_Timer::~GS_Timer():
// ---------------------------------------------------------------------------------------------
// Purpose: The destructor, de-initializes class data when class object is destroyed.
// ---------------------------------------------------------------------------------------------
// Returns: Nothing.
//==============================================================================================

GS_Timer::~GS_Timer()
{
    // Does nothing.
}


////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////
// Reset Method ////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_Timer::Reset():
// ---------------------------------------------------------------------------------------------
// Purpose: ...                                                                                 |
// ---------------------------------------------------------------------------------------------
// Returns: Nothing.
//==============================================================================================

void GS_Timer::Reset()
{
#ifdef GS_PLATFORM_WINDOWS
    // If the system supports performance counters.
    if (m_bHaveCounter)
    {
        // Determine a new start count.
        QueryPerformanceCounter((LARGE_INTEGER*) &m_n64StartCount);
    }
    // If the system does not support performance counters.
    else
    {
        // Determine a new start time.
        m_dwStartTime = timeGetTime();
    }
#else
    m_lStartTime = GS_Platform::GetTickCount();
#endif
}


////////////////////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////////////////////
// Timer Methods ///////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_Timer::GetTime():
// ---------------------------------------------------------------------------------------------
// Purpose: ...                                                                                 |
// ---------------------------------------------------------------------------------------------
// Returns: A float value indicating the time, in milliseconds, since the timer was initialized.
//==============================================================================================

float GS_Timer::GetTime()
{
#ifdef GS_PLATFORM_WINDOWS
    __int64 n64CurrentCount;

    // If the system supports performance counters.
    if (m_bHaveCounter)
    {
        // Get the current count of the performance counter.
        QueryPerformanceCounter((LARGE_INTEGER*) &n64CurrentCount);
        // Calculate the time elapsed in milliseconds.
        n64CurrentCount -= m_n64StartCount;
        return ((double(n64CurrentCount) / double(m_n64PerfFreq)) * 1000.0f);
    }
    // If the system does not support performance counters.
    else
    {
        // Calculate the time elapsed in milliseconds.
        return (float(timeGetTime() - m_dwStartTime));
    }
#else
    unsigned long lCurrentTime = GS_Platform::GetTickCount();
    return (float)(lCurrentTime - m_lStartTime);
#endif
}


////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_Timer::Wait():
// ---------------------------------------------------------------------------------------------
// Purpose: ...                                                                                 |
// ---------------------------------------------------------------------------------------------
// Returns: Nothing.
//==============================================================================================

void GS_Timer::Wait(float fWaitTime)
{
    // Add the time to wait to the current time.
    fWaitTime += (this->GetTime());

    // Loop until the current time is equal to or larger than the wait time.
    while (this->GetTime() < fWaitTime)
    {
    }
}


////////////////////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////////////////////////
// Constructor/Destructor Methods //////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_FrameTimer::GS_FrameTimer():
// ---------------------------------------------------------------------------------------------
// Purpose: The constructor, initializes class data when class object is created.
// ---------------------------------------------------------------------------------------------
// Returns: Nothing.
//==============================================================================================

GS_FrameTimer::GS_FrameTimer() : GS_Timer()
{
    m_nFramesTotal     = 0;
    m_fFrameTimesTotal = 0.0f;
    m_fFrameRate       = 0.0f;
}


////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_FrameTimer::~GS_FrameTimer():
// ---------------------------------------------------------------------------------------------
// Purpose: The destructor, de-initializes class data when class object is destroyed.
// ---------------------------------------------------------------------------------------------
// Returns: Nothing.
//==============================================================================================

GS_FrameTimer::~GS_FrameTimer()
{
    // Does nothing.
}


////////////////////////////////////////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////////////////////
// Frame Timer Methods /////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_Timer::GetFrameTime():
// ---------------------------------------------------------------------------------------------
// Purpose: ...                                                                                 |
// ---------------------------------------------------------------------------------------------
// Returns: The time in milliseconds since the last frame was marked.
//==============================================================================================

float GS_FrameTimer::GetFrameTime()
{
    // Calculate the length of time since the last frame.
    return (GS_Timer::GetTime());
}


////////////////////////////////////////////////////////////////////////////////////////////////


//==============================================================================================
// GS_Timer::MarkFrame():
// ---------------------------------------------------------------------------------------------
// Purpose: ...                                                                                 |
// ---------------------------------------------------------------------------------------------
// Returns: Nothing.
//==============================================================================================

void GS_FrameTimer::MarkFrame()
{
    // Get the time elapsed since the last frame.
    float fFrameTime = GS_Timer::GetTime();

    // Reset the timer to start measuring new frame.
    GS_Timer::Reset();

    // Prevent a divide by 0 error.
    if (fFrameTime <= 0.0f)
    {
        fFrameTime = 1.0f;
    }

    // Increment the number of frames completed.
    m_nFramesTotal++;

    // Add the time it took to complete the last frame to the frame times total.
    m_fFrameTimesTotal += (1000.0f / fFrameTime);

    // If the specified number of frames used to calculate the frame rate have been reached.
    if ((m_nFramesTotal >= TOTAL_FRAMES) && (TOTAL_FRAMES > 1))
    {
        // Calculate the frame rate (in frames per seconds).
        if (m_fFrameTimesTotal > 0.0f)
        {
            m_fFrameRate = float(m_fFrameTimesTotal / m_nFramesTotal);
        }
        else
        {
            // Determine the frame rate based on the time it took to complete the last frame.
            m_fFrameRate = (1000.0f / fFrameTime);
        }
        // Reset variables to start measuring the frame rate all over again.
        m_nFramesTotal     = 0;
        m_fFrameTimesTotal = 0.0f;
    }
}


////////////////////////////////////////////////////////////////////////////////////////////////
