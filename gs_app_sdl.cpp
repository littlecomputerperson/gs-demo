#ifndef GS_PLATFORM_WINDOWS

#include "gs_app.h"
#include "gs_platform.h"
#include <SDL2/SDL.h>

// Global access to the application
extern GS_Application* g_pGSApp;

////////////////////////////////////////////////////////////////////////////////////////////////
// SDL2-Specific Implementation ////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////

BOOL GS_Application::Create(int nWidth, int nHeight, int nDepth, BOOL bIsWindowed) {
    if (m_bIsReady) {
        return FALSE;
    }

    // Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS) < 0) {
        GS_Error::Report("GS_APP_SDL.CPP", 21, "Failed to initialize SDL!");
        return FALSE;
    }

    // Set OpenGL attributes
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 16);
    
    // Set display parameters
    m_nDisplayWidth = (nWidth > 0) ? nWidth : GS_DEFAULT_WIDTH;
    m_nDisplayHeight = (nHeight > 0) ? nHeight : GS_DEFAULT_HEIGHT;
    m_nColorDepth = (nDepth == 16 || nDepth == 24 || nDepth == 32) ? nDepth : GS_DEFAULT_DEPTH;
    m_bIsWindowed = bIsWindowed;

    // Create window
    Uint32 flags = SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN;
    if (!m_bIsWindowed) {
        flags |= SDL_WINDOW_FULLSCREEN;
        // Use desktop resolution for fullscreen
        SDL_DisplayMode dm;
        if (SDL_GetDesktopDisplayMode(0, &dm) == 0) {
            m_nDisplayWidth = dm.w;
            m_nDisplayHeight = dm.h;
        }
    } else {
        flags |= SDL_WINDOW_RESIZABLE;
    }

    m_pWindow = SDL_CreateWindow(
        "Game System OpenGL",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        m_nDisplayWidth,
        m_nDisplayHeight,
        flags
    );

    if (!m_pWindow) {
        GS_Error::Report("GS_APP_SDL.CPP", 60, "Failed to create SDL window!");
        SDL_Quit();
        return FALSE;
    }

    // Create OpenGL context
    m_glContext = SDL_GL_CreateContext(m_pWindow);
    if (!m_glContext) {
        GS_Error::Report("GS_APP_SDL.CPP", 68, "Failed to create OpenGL context!");
        SDL_DestroyWindow(m_pWindow);
        SDL_Quit();
        return FALSE;
    }

    // Enable VSync
    SDL_GL_SetSwapInterval(1);

    // Initialize game
    if (TRUE != this->GameInit()) {
        GS_Error::Report("GS_APP_SDL.CPP", 80, "Failed to initialize game!");
        return FALSE;
    }

    m_gsFrameTimer.MarkFrame();
    m_bIsReady = TRUE;
    m_bIsActive = TRUE;

    return TRUE;
}

void GS_Application::Destroy() {
    GameShutdown();
    
    m_bIsActive = FALSE;
    m_bIsReady = FALSE;

    if (m_glContext) {
        SDL_GL_DeleteContext(m_glContext);
        m_glContext = NULL;
    }

    if (m_pWindow) {
        SDL_DestroyWindow(m_pWindow);
        m_pWindow = NULL;
    }

    SDL_Quit();
}

INT GS_Application::Run() {
    SDL_Event event;
    bool running = true;

    // Calculate wait time
    if (m_fFrameRate > 0.0f) {
        m_fWaitTime = 1000.0f / m_fFrameRate;
    } else {
        m_fWaitTime = 0.0f;
    }

    while (running) {
        // Process events
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
                case SDL_QUIT:
                    running = false;
                    break;
                    
                case SDL_WINDOWEVENT:
                    if (event.window.event == SDL_WINDOWEVENT_FOCUS_LOST) {
                        if (m_bIsActive && m_bIsReady) {
                            GameRelease();
                            m_bIsActive = FALSE;
                        }
                    } else if (event.window.event == SDL_WINDOWEVENT_FOCUS_GAINED) {
                        if (!m_bIsActive && m_bIsReady) {
                            GameRestore();
                            m_gsFrameTimer.MarkFrame();
                            m_bIsActive = TRUE;
                        }
                    }
                    break;
                    
                case SDL_KEYDOWN:
                    if (event.key.keysym.sym == SDLK_RETURN && 
                        (event.key.keysym.mod & KMOD_ALT)) {
                        this->OnChangeMode();
                    }
                    break;
            }
        }

        // Render frame
        if (m_bIsActive && m_bIsReady) {
            if ((m_fFrameTime = m_gsFrameTimer.GetFrameTime()) >= m_fWaitTime) {
                m_gsFrameTimer.MarkFrame();
                
                if (TRUE != GameLoop()) {
                    GS_Error::Report("GS_APP_SDL.CPP", 165, "Error running main game loop!");
                    running = false;
                }
                
                SDL_GL_SwapWindow(m_pWindow);
            }
        } else if (!m_bIsActive) {
            SDL_WaitEvent(NULL);
        }
    }

    this->Destroy();
    return 0;
}

LRESULT GS_Application::MsgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    // Stub for compatibility - actual event handling done in Run()
    return 0;
}

BOOL GS_Application::SetMode(int nWidth, int nHeight, int nDepth, BOOL bIsWindowed) {
    if (!m_bIsReady) {
        return this->Create(nWidth, nHeight, nDepth, bIsWindowed);
    }

    // Save previous settings
    int nPrevWidth = m_nDisplayWidth;
    int nPrevHeight = m_nDisplayHeight;
    int nPrevDepth = m_nColorDepth;
    BOOL bWasWindowed = m_bIsWindowed;

    Destroy();

    if (FALSE == Create(nWidth, nHeight, nDepth, bIsWindowed)) {
        GS_Error::Report("GS_APP_SDL.CPP", 202, "Failed to change display mode!");
        Create(nPrevWidth, nPrevHeight, nPrevDepth, bWasWindowed);
    }

    return TRUE;
}

#endif // !GS_PLATFORM_WINDOWS
