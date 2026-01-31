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

#include "gs_sdl_controller.h"
#include <string.h>

////////////////////////////////////////////////////////////////////////////////////////////////

const char *GS_Controller::ButtonDescription[GSC_BUTTON_COUNT] =
    {
        "A Button", "B Button", "X Button", "Y Button",
        "Back", "Guide", "Start",
        "Left Stick", "Right Stick",
        "Left Shoulder", "Right Shoulder",
        "D-Pad Up", "D-Pad Down", "D-Pad Left", "D-Pad Right"};

////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////
// Constructor/Destructor Methods //////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////

//==============================================================================================
// GS_Controller::GS_Controller():
// ---------------------------------------------------------------------------------------------
// Purpose: The constructor, initializes controller state arrays and automatically detects
//          and initializes all connected game controllers at startup.
// ---------------------------------------------------------------------------------------------
// Returns: Nothing.
//==============================================================================================

GS_Controller::GS_Controller()
{
    m_nConnectedCount = 0;
    m_nActiveController = 0;
    m_nItemsInBuffer = 0;

    for (int i = 0; i < GS_MAX_CONTROLLERS; i++)
    {
        m_controllers[i].controller = NULL;
        m_controllers[i].instanceID = -1;
        m_controllers[i].isConnected = FALSE;
        m_controllers[i].name[0] = '\0';

        for (int j = 0; j < GSC_BUTTON_COUNT; j++)
        {
            m_controllers[i].buttonPressed[j] = FALSE;
        }

        for (int j = 0; j < GSC_AXIS_COUNT; j++)
        {
            m_controllers[i].axisValue[j] = 0.0f;
        }
    }

    ZeroMemory(m_nButtonBuffer, sizeof(m_nButtonBuffer));

    InitializeControllers();
}

////////////////////////////////////////////////////////////////////////////////////////////////

//==============================================================================================
// GS_Controller::~GS_Controller():
// ---------------------------------------------------------------------------------------------
// Purpose: The destructor, closes all controller handles and cleans up SDL controller
//          subsystem resources.
// ---------------------------------------------------------------------------------------------
// Returns: Nothing.
//==============================================================================================

GS_Controller::~GS_Controller()
{
    CloseControllers();
}

////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////
// Initialization Methods //////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////

//==============================================================================================
// GS_Controller::InitializeControllers():
// ---------------------------------------------------------------------------------------------
// Purpose: Initializes the SDL game controller subsystem and automatically detects and opens
//          all connected controllers (up to GS_MAX_CONTROLLERS).
// ---------------------------------------------------------------------------------------------
// Returns: Nothing.
//==============================================================================================

void GS_Controller::InitializeControllers()
{
    if (SDL_InitSubSystem(SDL_INIT_GAMECONTROLLER) < 0)
    {
        return;
    }

    // Auto-detect and initialize all connected controllers
    int numJoysticks = SDL_NumJoysticks();
    for (int i = 0; i < numJoysticks && m_nConnectedCount < GS_MAX_CONTROLLERS; i++)
    {
        if (SDL_IsGameController(i))
        {
            OnControllerAdded(i);
        }
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////

//==============================================================================================
// GS_Controller::CloseControllers():
// ---------------------------------------------------------------------------------------------
// Purpose: Closes all open controller handles and resets connection state.
// ---------------------------------------------------------------------------------------------
// Returns: Nothing.
//==============================================================================================

void GS_Controller::CloseControllers()
{
    for (int i = 0; i < GS_MAX_CONTROLLERS; i++)
    {
        if (m_controllers[i].controller)
        {
            SDL_GameControllerClose(m_controllers[i].controller);
            m_controllers[i].controller = NULL;
            m_controllers[i].isConnected = FALSE;
        }
    }
    m_nConnectedCount = 0;
}

////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////
// Controller Management Methods ///////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////

//==============================================================================================
// GS_Controller::FindControllerIndex():
// ---------------------------------------------------------------------------------------------
// Purpose: Finds the array index of a controller by its SDL joystick instance ID.
// ---------------------------------------------------------------------------------------------
// Returns: Array index (0-3) of the controller, or -1 if not found.
//==============================================================================================

int GS_Controller::FindControllerIndex(SDL_JoystickID instanceID)
{
    for (int i = 0; i < GS_MAX_CONTROLLERS; i++)
    {
        if (m_controllers[i].isConnected && m_controllers[i].instanceID == instanceID)
        {
            return i;
        }
    }
    return -1;
}

////////////////////////////////////////////////////////////////////////////////////////////////

//==============================================================================================
// GS_Controller::FindFreeSlot():
// ---------------------------------------------------------------------------------------------
// Purpose: Finds the next available slot in the controller array for a new connection.
// ---------------------------------------------------------------------------------------------
// Returns: Array index (0-3) of first free slot, or -1 if all slots are occupied.
//==============================================================================================

int GS_Controller::FindFreeSlot()
{
    for (int i = 0; i < GS_MAX_CONTROLLERS; i++)
    {
        if (!m_controllers[i].isConnected)
        {
            return i;
        }
    }
    return -1;
}

////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////
// Update/Reset Methods ////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////

//==============================================================================================
// GS_Controller::Update():
// ---------------------------------------------------------------------------------------------
// Purpose: Placeholder for compatibility with Windows-style input polling. SDL handles
//          controller events in the main event loop, so this method does nothing.
// ---------------------------------------------------------------------------------------------
// Returns: Nothing.
//==============================================================================================

void GS_Controller::Update(HWND hWnd)
{
    (void)hWnd; // SDL handles events in main loop
}

////////////////////////////////////////////////////////////////////////////////////////////////

//==============================================================================================
// GS_Controller::Reset():
// ---------------------------------------------------------------------------------------------
// Purpose: Resets all controller button states, axis values, and clears the input buffer
//          for all connected controllers.
// ---------------------------------------------------------------------------------------------
// Returns: Nothing.
//==============================================================================================

void GS_Controller::Reset()
{
    for (int i = 0; i < GS_MAX_CONTROLLERS; i++)
    {
        ResetController(i);
    }
    ClearBuffer();
}

////////////////////////////////////////////////////////////////////////////////////////////////

//==============================================================================================
// GS_Controller::ResetController():
// ---------------------------------------------------------------------------------------------
// Purpose: Resets button states and axis values for a specific controller without affecting
//          the global input buffer.
// ---------------------------------------------------------------------------------------------
// Returns: Nothing.
//==============================================================================================

void GS_Controller::ResetController(int nControllerIndex)
{
    if (nControllerIndex < 0 || nControllerIndex >= GS_MAX_CONTROLLERS)
        return;

    for (int j = 0; j < GSC_BUTTON_COUNT; j++)
    {
        m_controllers[nControllerIndex].buttonPressed[j] = FALSE;
    }

    for (int j = 0; j < GSC_AXIS_COUNT; j++)
    {
        m_controllers[nControllerIndex].axisValue[j] = 0.0f;
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////
// Connection Status Methods ///////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////

//==============================================================================================
// GS_Controller::IsControllerConnected():
// ---------------------------------------------------------------------------------------------
// Purpose: Checks whether a specific controller slot has a connected controller.
// ---------------------------------------------------------------------------------------------
// Returns: TRUE if controller is connected, FALSE otherwise.
//==============================================================================================

BOOL GS_Controller::IsControllerConnected(int nControllerIndex)
{
    if (nControllerIndex < 0 || nControllerIndex >= GS_MAX_CONTROLLERS)
        return FALSE;

    return m_controllers[nControllerIndex].isConnected;
}

////////////////////////////////////////////////////////////////////////////////////////////////

//==============================================================================================
// GS_Controller::GetControllerName():
// ---------------------------------------------------------------------------------------------
// Purpose: Gets the device name/description of a connected controller as reported by SDL.
// ---------------------------------------------------------------------------------------------
// Returns: Pointer to controller name string, "Disconnected" if not connected, or
//          "Invalid Controller" if index is out of range.
//==============================================================================================

const char *GS_Controller::GetControllerName(int nControllerIndex)
{
    if (nControllerIndex < 0 || nControllerIndex >= GS_MAX_CONTROLLERS)
        return "Invalid Controller";

    if (!m_controllers[nControllerIndex].isConnected)
        return "Disconnected";

    return m_controllers[nControllerIndex].name;
}

////////////////////////////////////////////////////////////////////////////////////////////////

//==============================================================================================
// GS_Controller::SetActiveController():
// ---------------------------------------------------------------------------------------------
// Purpose: Sets which controller should be used as the default for input queries when no
//          specific controller index is provided.
// ---------------------------------------------------------------------------------------------
// Returns: Nothing.
//==============================================================================================

void GS_Controller::SetActiveController(int nControllerIndex)
{
    if (nControllerIndex >= 0 && nControllerIndex < GS_MAX_CONTROLLERS &&
        m_controllers[nControllerIndex].isConnected)
    {
        m_nActiveController = nControllerIndex;
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////
// Event Handling Methods (called from SDL event loop) ////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////

//==============================================================================================
// GS_Controller::OnControllerAdded():
// ---------------------------------------------------------------------------------------------
// Purpose: Called when a new controller is connected. Opens the controller, assigns it to
//          the first available slot, and sets it as active if it's the first controller.
// ---------------------------------------------------------------------------------------------
// Returns: Nothing.
//==============================================================================================

void GS_Controller::OnControllerAdded(int nDeviceIndex)
{
    int slot = FindFreeSlot();
    if (slot == -1)
        return;

    SDL_GameController *controller = SDL_GameControllerOpen(nDeviceIndex);
    if (!controller)
        return;

    m_controllers[slot].controller = controller;
    m_controllers[slot].instanceID = SDL_JoystickInstanceID(SDL_GameControllerGetJoystick(controller));
    m_controllers[slot].isConnected = TRUE;

    const char *name = SDL_GameControllerName(controller);
    if (name)
    {
        strncpy(m_controllers[slot].name, name, sizeof(m_controllers[slot].name) - 1);
        m_controllers[slot].name[sizeof(m_controllers[slot].name) - 1] = '\0';
    }

    m_nConnectedCount++;

    if (m_nConnectedCount == 1)
    {
        m_nActiveController = slot;
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////

//==============================================================================================
// GS_Controller::OnControllerRemoved():
// ---------------------------------------------------------------------------------------------
// Purpose: Called when a controller is disconnected. Closes the controller handle, updates
//          connection state, and switches active controller if necessary.
// ---------------------------------------------------------------------------------------------
// Returns: Nothing.
//==============================================================================================

void GS_Controller::OnControllerRemoved(SDL_JoystickID instanceID)
{
    int index = FindControllerIndex(instanceID);
    if (index == -1)
        return;

    if (m_controllers[index].controller)
    {
        SDL_GameControllerClose(m_controllers[index].controller);
        m_controllers[index].controller = NULL;
    }

    m_controllers[index].isConnected = FALSE;
    m_controllers[index].instanceID = -1;
    m_nConnectedCount--;

    if (m_nActiveController == index && m_nConnectedCount > 0)
    {
        for (int i = 0; i < GS_MAX_CONTROLLERS; i++)
        {
            if (m_controllers[i].isConnected)
            {
                m_nActiveController = i;
                break;
            }
        }
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////

//==============================================================================================
// GS_Controller::OnButtonDown():
// ---------------------------------------------------------------------------------------------
// Purpose: Called when a controller button is pressed. Updates button state array and adds
//          the button press to the buffered input queue.
// ---------------------------------------------------------------------------------------------
// Returns: Nothing.
//==============================================================================================

void GS_Controller::OnButtonDown(SDL_JoystickID instanceID, int nButton)
{

    // Find which controller triggered this event.
    int index = FindControllerIndex(instanceID);
    if (index == -1 || nButton < 0 || nButton >= GSC_BUTTON_COUNT)
        return; // Invalid controller or button.

    // Update the button state to pressed.
    m_controllers[index].buttonPressed[nButton] = TRUE;
    // Add the button press to the buffer for buffered input (using the define value).
    AddButtonToBuffer(GSC_BUTTON_A + nButton);
}

////////////////////////////////////////////////////////////////////////////////////////////////

//==============================================================================================
// GS_Controller::OnButtonUp():
// ---------------------------------------------------------------------------------------------
// Purpose: Called when a controller button is released. Updates button state to not pressed.
// ---------------------------------------------------------------------------------------------
// Returns: Nothing.
//==============================================================================================

void GS_Controller::OnButtonUp(SDL_JoystickID instanceID, int nButton)
{

    // Find which controller triggered this event.
    int index = FindControllerIndex(instanceID);
    if (index == -1 || nButton < 0 || nButton >= GSC_BUTTON_COUNT)
        return; // Invalid controller or button.

    // Update the button state to not pressed.
    m_controllers[index].buttonPressed[nButton] = FALSE;
}

////////////////////////////////////////////////////////////////////////////////////////////////

//==============================================================================================
// GS_Controller::OnAxisMotion():
// ---------------------------------------------------------------------------------------------
// Purpose: Called when a controller analog axis (stick or trigger) changes value. Stores
//          the normalized axis value (-1.0 to 1.0 for sticks, 0.0 to 1.0 for triggers).
// ---------------------------------------------------------------------------------------------
// Returns: Nothing.
//==============================================================================================

void GS_Controller::OnAxisMotion(SDL_JoystickID instanceID, int nAxis, float fValue)
{

    // Find which controller triggered this event.
    int index = FindControllerIndex(instanceID);
    if (index == -1 || nAxis < 0 || nAxis >= GSC_AXIS_COUNT)
        return; // Invalid controller or axis.

    // Update the axis value.
    m_controllers[index].axisValue[nAxis] = fValue;
}

////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////
// Methods for Getting Button Input ////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////

//==============================================================================================
// GS_Controller::IsButtonDown():
// ---------------------------------------------------------------------------------------------
// Purpose: Checks whether a specific button is currently pressed on a controller. Uses the
//          active controller if no specific controller index is provided.
// ---------------------------------------------------------------------------------------------
// Returns: TRUE if button is pressed, FALSE otherwise.
//==============================================================================================

BOOL GS_Controller::IsButtonDown(int nButton, int nControllerIndex)
{

    // Convert button define to array index
    int buttonIndex = nButton - GSC_BUTTON_A;

    // Check if valid button code.
    if (buttonIndex < 0 || buttonIndex >= GSC_BUTTON_COUNT)
        return FALSE;

    // Use active controller if no specific controller specified.
    int controller = (nControllerIndex == -1) ? m_nActiveController : nControllerIndex;
    if (controller < 0 || controller >= GS_MAX_CONTROLLERS)
        return FALSE;

    // Is the controller connected?
    if (!m_controllers[controller].isConnected)
        return FALSE;

    return m_controllers[controller].buttonPressed[buttonIndex];
}

////////////////////////////////////////////////////////////////////////////////////////////////

//==============================================================================================
// GS_Controller::AreButtonsDown():
// ---------------------------------------------------------------------------------------------
// Purpose: Checks whether all specified buttons in an array are currently pressed.
// ---------------------------------------------------------------------------------------------
// Returns: TRUE if all buttons are pressed, FALSE if any button is not pressed.
//==============================================================================================

BOOL GS_Controller::AreButtonsDown(int nNumButtons, int *pButtons, int nControllerIndex)
{

    // Exit if wrong number of buttons.
    if (nNumButtons < 1 || !pButtons)
        return FALSE;

    // Check each button to see whether it is pressed or not.
    for (int i = 0; i < nNumButtons; i++)
    {
        // Has the specified button not been pressed?
        if (!IsButtonDown(*(pButtons + i), nControllerIndex))
            return FALSE;
    }

    // All of the selected buttons were pressed.
    return TRUE;
}

////////////////////////////////////////////////////////////////////////////////////////////////

//==============================================================================================
// GS_Controller::AreButtonsUp():
// ---------------------------------------------------------------------------------------------
// Purpose: Checks whether all specified buttons in an array are currently released.
// ---------------------------------------------------------------------------------------------
// Returns: TRUE if all buttons are released, FALSE if any button is pressed.
//==============================================================================================

BOOL GS_Controller::AreButtonsUp(int nNumButtons, int *pButtons, int nControllerIndex)
{

    // Exit if wrong number of buttons.
    if (nNumButtons < 1 || !pButtons)
        return FALSE;

    // Check each button to see whether it is pressed or not.
    for (int i = 0; i < nNumButtons; i++)
    {
        // Has the specified button been pressed?
        if (IsButtonDown(*(pButtons + i), nControllerIndex))
            return FALSE;
    }

    // None of the selected buttons were pressed.
    return TRUE;
}

////////////////////////////////////////////////////////////////////////////////////////////////

//==============================================================================================
// GS_Controller::GetButtonPressed():
// ---------------------------------------------------------------------------------------------
// Purpose: Gets the button code of the first pressed button found (scanning in order from
//          A button to D-Pad Right).
// ---------------------------------------------------------------------------------------------
// Returns: Button code (0x1000-0x100E) for first pressed button, or -1 if no button pressed.
//==============================================================================================

int GS_Controller::GetButtonPressed(int nControllerIndex)
{

    // Use active controller if no specific controller specified.
    int controller = (nControllerIndex == -1) ? m_nActiveController : nControllerIndex;
    if (controller < 0 || controller >= GS_MAX_CONTROLLERS)
        return -1;

    // Is the controller connected?
    if (!m_controllers[controller].isConnected)
        return -1;

    // Check each button to see whether it is pressed or not.
    for (int i = 0; i < GSC_BUTTON_COUNT; i++)
    {
        // Has a button been pressed?
        if (m_controllers[controller].buttonPressed[i])
        {
            // Return the button code of the first button pressed (using the define value).
            return GSC_BUTTON_A + i;
        }
    }

    return -1;
}

////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////
// Methods for Getting Axis Input //////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////

//==============================================================================================
// GS_Controller::GetAxisValue():
// ---------------------------------------------------------------------------------------------
// Purpose: Gets the current normalized value of a specific analog axis.
// ---------------------------------------------------------------------------------------------
// Returns: Axis value in range -1.0 to 1.0 for sticks, 0.0 to 1.0 for triggers, or 0.0 if
//          invalid axis or controller.
//==============================================================================================

float GS_Controller::GetAxisValue(int nAxis, int nControllerIndex)
{

    // Convert axis define to array index
    int axisIndex = nAxis - GSC_AXIS_LEFTX;

    // Check if valid axis code.
    if (axisIndex < 0 || axisIndex >= GSC_AXIS_COUNT)
        return 0.0f;

    // Use active controller if no specific controller specified.
    int controller = (nControllerIndex == -1) ? m_nActiveController : nControllerIndex;
    if (controller < 0 || controller >= GS_MAX_CONTROLLERS)
        return 0.0f;

    // Is the controller connected?
    if (!m_controllers[controller].isConnected)
        return 0.0f;

    return m_controllers[controller].axisValue[axisIndex];
}

////////////////////////////////////////////////////////////////////////////////////////////////

//==============================================================================================
// GS_Controller::GetLeftStickX():
// ---------------------------------------------------------------------------------------------
// Purpose: Convenience method to get the horizontal position of the left analog stick.
// ---------------------------------------------------------------------------------------------
// Returns: X-axis value in range -1.0 (left) to 1.0 (right).
//==============================================================================================

float GS_Controller::GetLeftStickX(int nControllerIndex)
{

    return GetAxisValue(GSC_AXIS_LEFTX, nControllerIndex);
}

////////////////////////////////////////////////////////////////////////////////////////////////

//==============================================================================================
// GS_Controller::GetLeftStickY():
// ---------------------------------------------------------------------------------------------
// Purpose: Convenience method to get the vertical position of the left analog stick.
// ---------------------------------------------------------------------------------------------
// Returns: Y-axis value in range -1.0 (up) to 1.0 (down).
//==============================================================================================

float GS_Controller::GetLeftStickY(int nControllerIndex)
{

    return GetAxisValue(GSC_AXIS_LEFTY, nControllerIndex);
}

////////////////////////////////////////////////////////////////////////////////////////////////

//==============================================================================================
// GS_Controller::GetRightStickX():
// ---------------------------------------------------------------------------------------------
// Purpose: Convenience method to get the horizontal position of the right analog stick.
// ---------------------------------------------------------------------------------------------
// Returns: X-axis value in range -1.0 (left) to 1.0 (right).
//==============================================================================================

float GS_Controller::GetRightStickX(int nControllerIndex)
{

    return GetAxisValue(GSC_AXIS_RIGHTX, nControllerIndex);
}

////////////////////////////////////////////////////////////////////////////////////////////////

//==============================================================================================
// GS_Controller::GetRightStickY():
// ---------------------------------------------------------------------------------------------
// Purpose: Convenience method to get the vertical position of the right analog stick.
// ---------------------------------------------------------------------------------------------
// Returns: Y-axis value in range -1.0 (up) to 1.0 (down).
//==============================================================================================

float GS_Controller::GetRightStickY(int nControllerIndex)
{

    return GetAxisValue(GSC_AXIS_RIGHTY, nControllerIndex);
}

////////////////////////////////////////////////////////////////////////////////////////////////

//==============================================================================================
// GS_Controller::GetLeftTrigger():
// ---------------------------------------------------------------------------------------------
// Purpose: Convenience method to get the pressure/position of the left trigger.
// ---------------------------------------------------------------------------------------------
// Returns: Trigger value in range 0.0 (not pressed) to 1.0 (fully pressed).
//==============================================================================================

float GS_Controller::GetLeftTrigger(int nControllerIndex)
{

    return GetAxisValue(GSC_AXIS_TRIGGERLEFT, nControllerIndex);
}

////////////////////////////////////////////////////////////////////////////////////////////////

//==============================================================================================
// GS_Controller::GetRightTrigger():
// ---------------------------------------------------------------------------------------------
// Purpose: Convenience method to get the pressure/position of the right trigger.
// ---------------------------------------------------------------------------------------------
// Returns: Trigger value in range 0.0 (not pressed) to 1.0 (fully pressed).
//==============================================================================================

float GS_Controller::GetRightTrigger(int nControllerIndex)
{

    return GetAxisValue(GSC_AXIS_TRIGGERRIGHT, nControllerIndex);
}

////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////
// Buffered Input Methods //////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////

//==============================================================================================
// GS_Controller::AddButtonToBuffer():
// ---------------------------------------------------------------------------------------------
// Purpose: Adds a button press to the buffered input queue (FIFO). If buffer is full, the
//          oldest entry is removed to make room for the new entry.
// ---------------------------------------------------------------------------------------------
// Returns: TRUE if there was space in buffer, FALSE if buffer was full and a button press
//          was discarded.
//==============================================================================================

BOOL GS_Controller::AddButtonToBuffer(int nButton)
{

    BOOL bBufferFull = FALSE;

    // Is buffer full?
    if (m_nItemsInBuffer >= GS_MAX_BUTTON_BUFFER_SIZE)
    {
        // Remove oldest item from the buffer and shift the rest one down.
        for (int i = 0; i < m_nItemsInBuffer - 1; i++)
        {
            m_nButtonBuffer[i] = m_nButtonBuffer[i + 1];
        }
        // Decrease number of items in the buffer.
        m_nItemsInBuffer--;
        // Set last item in buffer to 0.
        m_nButtonBuffer[m_nItemsInBuffer] = 0;
        // Buffer is full, button press was lost.
        bBufferFull = TRUE;
    }

    // Set the last position in the buffer to the button code.
    m_nButtonBuffer[m_nItemsInBuffer] = nButton;
    // Increase number of items in buffer.
    m_nItemsInBuffer++;

    return (!bBufferFull);
}

////////////////////////////////////////////////////////////////////////////////////////////////

//==============================================================================================
// GS_Controller::ClearBuffer():
// ---------------------------------------------------------------------------------------------
// Purpose: Removes all button presses from the buffered input queue.
// ---------------------------------------------------------------------------------------------
// Returns: Nothing.
//==============================================================================================

void GS_Controller::ClearBuffer()
{

    // Remove all items from the button buffer.
    ZeroMemory(m_nButtonBuffer, sizeof(m_nButtonBuffer));
    // Reset the number of items in the buffer.
    m_nItemsInBuffer = 0;
}

////////////////////////////////////////////////////////////////////////////////////////////////

//==============================================================================================
// GS_Controller::GetBufferedButton():
// ---------------------------------------------------------------------------------------------
// Purpose: Retrieves the most recently added button press from the buffered input queue,
//          optionally removing it from the buffer.
// ---------------------------------------------------------------------------------------------
// Returns: Button code for the latest button press, or -1 if buffer is empty.
//==============================================================================================

int GS_Controller::GetBufferedButton(BOOL bRemoveButton)
{

    int nButton = -1;

    // Is there a button code in the buffer?
    if (m_nItemsInBuffer > 0)
    {
        // Get the last added button code.
        nButton = m_nButtonBuffer[m_nItemsInBuffer - 1];
        // Should we remove the button code?
        if (bRemoveButton)
        {
            // Remove the code from the buffer.
            m_nButtonBuffer[m_nItemsInBuffer - 1] = 0;
            m_nItemsInBuffer--;
        }
    }

    return nButton;
}

////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////
// Utility Methods /////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////

//==============================================================================================
// GS_Controller::GetButtonDescription():
// ---------------------------------------------------------------------------------------------
// Purpose: Gets a human-readable text description for a button code.
// ---------------------------------------------------------------------------------------------
// Returns: A character string describing the button (e.g., "A Button", "D-Pad Up"), or
//          "Unknown" if button code is invalid.
//==============================================================================================

const char *GS_Controller::GetButtonDescription(int nButton)
{

    // Convert button define to array index
    int buttonIndex = nButton - GSC_BUTTON_A;

    // Is the button code within bounds?
    if (buttonIndex < 0 || buttonIndex >= GSC_BUTTON_COUNT)
        return "Unknown";

    return ButtonDescription[buttonIndex];
}

////////////////////////////////////////////////////////////////////////////////////////////////

//==============================================================================================
// GS_Controller::EnableRumble():
// ---------------------------------------------------------------------------------------------
// Purpose: Activates haptic feedback (vibration/rumble) on a controller with specified
//          intensity for low and high frequency motors and duration. Uses SDL2's rumble API.
// ---------------------------------------------------------------------------------------------
// Returns: Nothing. Function silently fails if controller doesn't support rumble.
//==============================================================================================

void GS_Controller::EnableRumble(int nControllerIndex, float fLowFreq, float fHighFreq, int nDurationMs)
{

    // Use active controller if no specific controller specified.
    int controller = (nControllerIndex == -1) ? m_nActiveController : nControllerIndex;
    if (controller < 0 || controller >= GS_MAX_CONTROLLERS)
        return;

    // Is the controller connected?
    if (!m_controllers[controller].isConnected || !m_controllers[controller].controller)
        return;

    // Convert normalized values (0.0-1.0) to SDL range (0-65535).
    Uint16 lowFreq = (Uint16)(fLowFreq * 65535.0f);
    Uint16 highFreq = (Uint16)(fHighFreq * 65535.0f);

    // Activate rumble for the specified duration.
    SDL_GameControllerRumble(m_controllers[controller].controller, lowFreq, highFreq, nDurationMs);
}

////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////
// Public Button State Methods (for manual state updates) /////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////

//==============================================================================================
// GS_Controller::SetButtonDown():
// ---------------------------------------------------------------------------------------------
// Purpose: Manually sets a button state to pressed. Can be called from MsgProc when receiving
//          controller button events through the Windows message system.
// ---------------------------------------------------------------------------------------------
// Returns: Nothing.
//==============================================================================================

void GS_Controller::SetButtonDown(int nButton, int nControllerIndex)
{

    // Convert button define to array index
    int buttonIndex = nButton - GSC_BUTTON_A;

    // Check if valid button code.
    if (buttonIndex < 0 || buttonIndex >= GSC_BUTTON_COUNT)
        return;

    // Use active controller if no specific controller specified.
    int controller = (nControllerIndex == -1) ? m_nActiveController : nControllerIndex;
    if (controller < 0 || controller >= GS_MAX_CONTROLLERS)
        return;

    // Is the controller connected?
    if (!m_controllers[controller].isConnected)
        return;

    // Update the button state to pressed.
    m_controllers[controller].buttonPressed[buttonIndex] = TRUE;
}

////////////////////////////////////////////////////////////////////////////////////////////////

//==============================================================================================
// GS_Controller::SetButtonUp():
// ---------------------------------------------------------------------------------------------
// Purpose: Manually sets a button state to released. Can be called from MsgProc when receiving
//          controller button events through the Windows message system.
// ---------------------------------------------------------------------------------------------
// Returns: Nothing.
//==============================================================================================

void GS_Controller::SetButtonUp(int nButton, int nControllerIndex)
{

    // Convert button define to array index
    int buttonIndex = nButton - GSC_BUTTON_A;

    // Check if valid button code.
    if (buttonIndex < 0 || buttonIndex >= GSC_BUTTON_COUNT)
        return;

    // Use active controller if no specific controller specified.
    int controller = (nControllerIndex == -1) ? m_nActiveController : nControllerIndex;
    if (controller < 0 || controller >= GS_MAX_CONTROLLERS)
        return;

    // Is the controller connected?
    if (!m_controllers[controller].isConnected)
        return;

    // Update the button state to not pressed.
    m_controllers[controller].buttonPressed[buttonIndex] = FALSE;
}

////////////////////////////////////////////////////////////////////////////////////////////////
