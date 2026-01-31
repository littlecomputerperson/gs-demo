/*============================================================================================+
 | Game System (GS) Library                                                                   |
 |--------------------------------------------------------------------------------------------|
 | FILES: gs_sdl_controller.cpp, gs_sdl_controller.h                                          |
 |--------------------------------------------------------------------------------------------|
 | CLASS: GS_Controller                                                                       |
 |--------------------------------------------------------------------------------------------|
 | ABOUT: SDL2-based game controller/gamepad support for cross-platform input handling.       |
 |        Provides automatic detection and initialization of up to 4 controllers with         |
 |        support for buttons, analog sticks, triggers, and haptic feedback.                  |
 |--------------------------------------------------------------------------------------------|
 |                                                                                    01/2026 |
 +============================================================================================*/

#ifndef GS_SDL_CONTROLLER_H
#define GS_SDL_CONTROLLER_H

//==============================================================================================
// Include Game System (GS) header files.
// ---------------------------------------------------------------------------------------------
#include "gs_object.h"
//==============================================================================================

//==============================================================================================
// Include platform abstraction header files.
// ---------------------------------------------------------------------------------------------
#include "gs_platform.h"
//==============================================================================================

//==============================================================================================
// Buffer defines.
// ---------------------------------------------------------------------------------------------
#define GS_MAX_CONTROLLERS 4
#define GS_MAX_BUTTON_BUFFER_SIZE 10
//==============================================================================================

//==============================================================================================
// Controller button defines.
// ---------------------------------------------------------------------------------------------
#define GSC_BUTTON_A 0x1000
#define GSC_BUTTON_B 0x1001
#define GSC_BUTTON_X 0x1002
#define GSC_BUTTON_Y 0x1003
#define GSC_BUTTON_BACK 0x1004
#define GSC_BUTTON_GUIDE 0x1005
#define GSC_BUTTON_START 0x1006
#define GSC_BUTTON_LEFTSTICK 0x1007
#define GSC_BUTTON_RIGHTSTICK 0x1008
#define GSC_BUTTON_LEFTSHOULDER 0x1009
#define GSC_BUTTON_RIGHTSHOULDER 0x100A
#define GSC_BUTTON_DPAD_UP 0x100B
#define GSC_BUTTON_DPAD_DOWN 0x100C
#define GSC_BUTTON_DPAD_LEFT 0x100D
#define GSC_BUTTON_DPAD_RIGHT 0x100E
#define GSC_BUTTON_COUNT 15
//==============================================================================================

//==============================================================================================
// Controller axis defines.
// ---------------------------------------------------------------------------------------------
#define GSC_AXIS_LEFTX 0x1010
#define GSC_AXIS_LEFTY 0x1011
#define GSC_AXIS_RIGHTX 0x1012
#define GSC_AXIS_RIGHTY 0x1013
#define GSC_AXIS_TRIGGERLEFT 0x1014
#define GSC_AXIS_TRIGGERRIGHT 0x1015
#define GSC_AXIS_COUNT 6
//==============================================================================================

////////////////////////////////////////////////////////////////////////////////////////////////
// Class Definition. ///////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////

class GS_Controller : public GS_Object
{

private:
    struct ControllerState
    {
        SDL_GameController *controller;       // SDL controller handle.
        SDL_JoystickID instanceID;            // Unique instance ID for this controller.
        BOOL buttonPressed[GSC_BUTTON_COUNT]; // Current button states.
        float axisValue[GSC_AXIS_COUNT];      // Current axis values (-1.0 to 1.0).
        BOOL isConnected;                     // Whether controller is connected.
        char name[256];                       // Controller name/description.
    };

    ControllerState m_controllers[GS_MAX_CONTROLLERS]; // Array of controller states.
    int m_nConnectedCount;                             // Number of connected controllers.

    int m_nButtonBuffer[GS_MAX_BUTTON_BUFFER_SIZE]; // The button buffer for buffered input.
    int m_nItemsInBuffer;                           // The number of items in the button buffer.
    int m_nActiveController;                        // Currently active controller index.

    static const char *ButtonDescription[GSC_BUTTON_COUNT]; // An array containing text
                                                            // descriptions of each button.

    void InitializeControllers();                       // Detects and initializes all connected controllers.
    void CloseControllers();                            // Closes all controller handles.
    int FindControllerIndex(SDL_JoystickID instanceID); // Finds controller by instance ID.
    int FindFreeSlot();                                 // Finds next available controller slot.

protected:
    // No protected members.

public:
    GS_Controller();
    ~GS_Controller();

    void Update(HWND hWnd);
    void Reset();
    void ResetController(int nControllerIndex);

    // Connection management methods.
    int GetConnectedCount()
    {
        return m_nConnectedCount;
    }
    BOOL IsControllerConnected(int nControllerIndex);
    const char *GetControllerName(int nControllerIndex);
    void SetActiveController(int nControllerIndex);
    int GetActiveController()
    {
        return m_nActiveController;
    }

    // Event handling methods (called from main event loop).
    void OnControllerAdded(int nDeviceIndex);
    void OnControllerRemoved(SDL_JoystickID instanceID);
    void OnButtonDown(SDL_JoystickID instanceID, int nButton);
    void OnButtonUp(SDL_JoystickID instanceID, int nButton);
    void OnAxisMotion(SDL_JoystickID instanceID, int nAxis, float fValue);

    // Public methods for setting button state (can be called from MsgProc)
    void SetButtonDown(int nButton, int nControllerIndex = -1);
    void SetButtonUp(int nButton, int nControllerIndex = -1);

    // Button state query methods.
    BOOL IsButtonDown(int nButton, int nControllerIndex = -1);
    BOOL AreButtonsDown(int nNumButtons, int *pButtons, int nControllerIndex = -1);
    BOOL AreButtonsUp(int nNumButtons, int *pButtons, int nControllerIndex = -1);
    int GetButtonPressed(int nControllerIndex = -1);

    // Axis state query methods.
    float GetAxisValue(int nAxis, int nControllerIndex = -1);
    float GetLeftStickX(int nControllerIndex = -1);
    float GetLeftStickY(int nControllerIndex = -1);
    float GetRightStickX(int nControllerIndex = -1);
    float GetRightStickY(int nControllerIndex = -1);
    float GetLeftTrigger(int nControllerIndex = -1);
    float GetRightTrigger(int nControllerIndex = -1);

    // Buffered input methods.
    int *GetBuffer()
    {
        return m_nButtonBuffer;
    }
    int GetBufferSize()
    {
        return m_nItemsInBuffer;
    }
    BOOL AddButtonToBuffer(int nButton);
    void ClearBuffer();
    int GetBufferedButton(BOOL bRemoveButton = TRUE);

    // Utility methods.
    const char *GetButtonDescription(int nButton);
    void EnableRumble(int nControllerIndex, float fLowFreq, float fHighFreq, int nDurationMs);
};

////////////////////////////////////////////////////////////////////////////////////////////////

#endif
