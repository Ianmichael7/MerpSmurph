#include <windows.h>
#include <iostream>
#include "stdafx.h"
#include "gamepad.h"

#define SHORT_MAX_AS_FLOAT 32768.0f
#define BYTE_MAX_AS_FLOAT 255.0f

// Link the 'Xinput' library
#pragma comment(lib, "Xinput.lib")

// Define the XButtonIDs struct as XButtons
XButtonIDs XButtons;

// XButtonIDs Default constructor
XButtonIDs::XButtonIDs()
{
	// These values are used to index the XINPUT_Buttons array,
	// accessing the matching XINPUT button value

	A = 0;
	B = 1;
	X = 2;
	Y = 3;

	dpadUp = 4;
	dpadDown = 5;
	dpadLeft = 6;
	dpadRight = 7;

	leftShoulder = 8;
	rightShoulder = 9;

	leftThumbstick = 10;
	rightThumbstick = 11;

	start = 12;
	back = 13;
}

// Default constructor
Gamepad::Gamepad()
{
	// Set gamepad index and zero button arrays
	m_iGamepadIndex = 0;
	zeroButtons();
}

// Overload constructor
Gamepad::Gamepad(int iIndex)
{
	// Set gamepad index and zero button arrays
	m_iGamepadIndex = iIndex;
	zeroButtons();
}

// Update gamepad state
void Gamepad::update()
{
	dwPacketNumberPrev = m_State.dwPacketNumber; // save off the previous state number
	m_State = getState(); // obtain current gamepad state

	for (int i = 0; i < buttonCount; i++)
	{
		// Set button state for current frame
		bButtonStates[i] = (m_State.Gamepad.wButtons & XINPUT_Buttons[i]) == XINPUT_Buttons[i];

		// Set DOWN state for current frame
		bGamepadButtonsDown[i] = !bPrevButtonStates[i] && bButtonStates[i];
	}
}

// Update button states for next frame
void Gamepad::refreshState()
{
	memcpy(bPrevButtonStates, bButtonStates, sizeof(bPrevButtonStates));
}

// Return true if left stick is inside deadzone
bool Gamepad::leftStickInDeadzone()
{
	// Obtain the x and y axes of the stick
	short sX = m_State.Gamepad.sThumbLX;
	short sY = m_State.Gamepad.sThumbLY;

	// x axis is outside of deadzone
	if (sX > XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE || sX < -XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE)
		return false;

	// y axis is outside of deadzone
	if (sY > XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE || sY < -XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE)
		return false;

	// Both axes inside of deadzone
	return true;
}

// Return true if right stick is inside deadzone
bool Gamepad::rightStickInDeadzone()
{
	// Obtain the x and y axes of the stick
	short sX = m_State.Gamepad.sThumbRX;
	short sY = m_State.Gamepad.sThumbRY;

	// x axis is outside of deadzone
	if (sX > XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE || sX < -XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE)
		return false;

	// y axis is outside of deadzone
	if (sY > XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE || sY < -XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE)
		return false;

	// Both axes inside of deadzone
	return true;
}

// Return x axis of left stick from -1 to 1
float Gamepad::leftStick_x()
{
	// Obtain x axis of left stick
	short sX = m_State.Gamepad.sThumbLX;

	// Return axis value, converted to a float
	return (static_cast<float> (sX) / SHORT_MAX_AS_FLOAT);
}

// Return y axis of left stick from -1 to 1
float Gamepad::leftStick_y()
{
	// Obtain x axis of left stick
	short sY = m_State.Gamepad.sThumbLY;

	// Return axis value, converted to a float
	return (static_cast<float> (sY) / SHORT_MAX_AS_FLOAT);
}

// Return x axis of right stick from -1 to 1
float Gamepad::rightStick_x()
{
	// Obtain x axis of left stick
	short sX = m_State.Gamepad.sThumbRX;

	// Return axis value, converted to a float
	return (static_cast<float> (sX) / SHORT_MAX_AS_FLOAT);
}

// Return y axis of right stick from -1 to 1
float Gamepad::rightStick_y()
{
	// Obtain x axis of left stick
	short sY = m_State.Gamepad.sThumbRY;

	// Return axis value, converted to a float
	return (static_cast<float> (sY) / SHORT_MAX_AS_FLOAT);
}

// Return value of left trigger from -1 to 1
float Gamepad::leftTrigger()
{
	// Obtain value of left trigger
	BYTE trigger = m_State.Gamepad.bLeftTrigger;

	if (trigger > XINPUT_GAMEPAD_TRIGGER_THRESHOLD)
		return trigger / BYTE_MAX_AS_FLOAT;

	return 0.0f; // Trigger not pressed
}

// Return value of right trigger from -1 to 1
float Gamepad::rightTrigger()
{
	// Obtain value of left trigger
	BYTE trigger = m_State.Gamepad.bRightTrigger;

	if (trigger > XINPUT_GAMEPAD_TRIGGER_THRESHOLD)
		return trigger / BYTE_MAX_AS_FLOAT;

	return 0.0f; // Trigger not pressed
}

// Return true if button is pressed, false if not
bool Gamepad::getButtonPressed(int iButton)
{
	if (m_State.Gamepad.wButtons & XINPUT_Buttons[iButton])
		return true;

	return false;
}

// Frame-specific version of getButtonPressed function
bool Gamepad::getButtonDown(int iButton)
{
	return bGamepadButtonsDown[iButton];
}

// Return gamepad state
XINPUT_STATE Gamepad::getState()
{
	// Temporary XINPUT_STATE to return
	XINPUT_STATE gamepadState;

	// Zero memory
	ZeroMemory(&gamepadState, sizeof(XINPUT_STATE));

	// Get state of gamepad
	XInputGetState(m_iGamepadIndex, &gamepadState);

	// Return the gamepad state
	return gamepadState;
}

// Return gamepad index
int Gamepad::getIndex()
{
	return m_iGamepadIndex;
}

// Return true if gamepad is connected
bool Gamepad::connected()
{
	// Zero memory
	ZeroMemory(&m_State, sizeof(XINPUT_STATE));

	// Get the state of the gamepad
	DWORD Result = XInputGetState(m_iGamepadIndex, &m_State);

	if (Result == ERROR_SUCCESS)
		return true;  // The gamepad is connected
	else
		return false; // The gamepad is not connected
}

// Return true if gamepad state has changed
bool Gamepad::hasChanged()
{
	return (dwPacketNumberPrev != m_State.dwPacketNumber);
}

// Zero the button arrays
void Gamepad::zeroButtons()
{
	// Iterate through all gamepad buttons
	for (int i = 0; i < buttonCount; i++)
	{
		bPrevButtonStates[i] = false;
		bButtonStates[i] = false;
		bGamepadButtonsDown[i] = false;
	}
}
