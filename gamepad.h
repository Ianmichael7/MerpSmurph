#include <windows.h>
#include <iostream>

#ifndef GAMEPAD_H
#define GAMEPAD_H

#include <Xinput.h>

// XInput button values
static const WORD XINPUT_Buttons[] = {
	XINPUT_GAMEPAD_A,
	XINPUT_GAMEPAD_B,
	XINPUT_GAMEPAD_X,
	XINPUT_GAMEPAD_Y,
	XINPUT_GAMEPAD_DPAD_UP,
	XINPUT_GAMEPAD_DPAD_DOWN,
	XINPUT_GAMEPAD_DPAD_LEFT,
	XINPUT_GAMEPAD_DPAD_RIGHT,
	XINPUT_GAMEPAD_LEFT_SHOULDER,
	XINPUT_GAMEPAD_RIGHT_SHOULDER,
	XINPUT_GAMEPAD_LEFT_THUMB,
	XINPUT_GAMEPAD_RIGHT_THUMB,
	XINPUT_GAMEPAD_START,
	XINPUT_GAMEPAD_BACK
};

// XInput Button IDs
struct XButtonIDs
{
	// Function prototypes

	XButtonIDs(); // Default constructor

				  // Member variables
	int A, B, X, Y;
	int dpadUp, dpadDown, dpadLeft, dpadRight;
	int leftShoulder, rightShoulder;
	int leftThumbstick, rightThumbstick;
	int start;
	int back;
};

class Gamepad
{
public:
	// Function prototypes

	// Constructors
	Gamepad();
	Gamepad(int iIndex);

	void update(); // Update gamepad state
	void refreshState(); // Update button states for next frame

						 // Analog stick functions
						 // Return true if stick is inside deadzone, false if outside
	bool leftStickInDeadzone();
	bool rightStickInDeadzone();

	float leftStick_x();  // Return x axis of left stick from -1 to 1
	float leftStick_y();  // Return y axis of left stick from -1 to 1
	float rightStick_x(); // Return x axis of right stick from -1 to 1
	float rightStick_y(); // Return y axis of right stick from -1 to 1

						  // Trigger functions
	float leftTrigger();  // Return value of left trigger from -1 to 1
	float rightTrigger(); // Return value of right trigger from -1 to 1

						  // Button functions
	bool getButtonPressed(int iButton); // Returns true if pressed, false if not
	bool getButtonDown(int iButton);    // Returns true if pressed in frame

										// Utility functions
	XINPUT_STATE getState(); // Return gamepad state
	int getIndex();          // Return gamepad index
	bool connected();        // Return true if gamepad is connected
	bool hasChanged();       // Return true if gamepad state has changed

private:
	// Member variables
	XINPUT_STATE m_State; // Current gamepad state
	int m_iGamepadIndex;  // Gamepad index (0, 1, 2, 3)
	DWORD dwPacketNumberPrev; // Previous state packet number

	static const int buttonCount = 14; // Total gamepad buttons
	bool bPrevButtonStates[buttonCount]; // Previous frame button states
	bool bButtonStates[buttonCount]; // Current frame button states

									 // Buttons pressed on current frame
	bool bGamepadButtonsDown[buttonCount];

	// Utility functions
	void zeroButtons(); // Zero the button state arrays
};

#endif // GAMEPAD_H

// Externally define the XButtonIDs struct as XButtons
extern XButtonIDs XButtons;