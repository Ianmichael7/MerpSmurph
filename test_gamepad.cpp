#include <windows.h>
#include <iostream>
#include "stdafx.h"
#include "gamepad.h"

/* ----------- uDataInput -----------
//Description: Data structure that handles queuing input system data from UAS to be sent off to specified function
//
//Data Structure:
//fSys (int) = ROS function call
//		1 = blah
//		2 = blach
//sDataI (int) = Input that feedsback integers
//sDataS (string) = Input that feedbacks strings/characters
*/
struct uDataInput
{
	int fSys;
	int sData;
	std::string sDataS;
};

/* ----------- uDataOutput -----------
//Description: Data structure that handles queuing output system data to UAS
//
//Data Structure:
//uSys (int) = ROS function call
//		1 = Flight System
//		2 = Mission System
//		3 = Camera Swivel System
//
//sDataI (int) = Output data to specified system
*/
struct uDataOutput
{
	int uSys;
	int uData;
};

bool debug = true;
Gamepad gamepad;

class gui
{
private:
	/* ----------- updateQueue -----------
	//Description: Function that updates queue
	//
	//Inputs: N/A
	//
	//Outputs:
	// bool = complete(0=n, 1=y)
	*/
	bool updateQueue()
	{
		return true;
	}

public:
	/* ----------- createGUI -----------
	//Description: Function that calls openGL and displays GUI
	//
	//Inputs: N/A
	//
	//Outputs:
	// bool = complete(0=n, 1=y)
	*/
	bool createGUI()
	{
		return true;
	}

};

class prgm
{
private:
	/* ----------- prnt -----------
	//Description: Function that loops through watching for controller input to push mapped controls to a queue
	//
	//Inputs:
	//cmdln(int) = which command line we are outputting to
	//		1 = cout (beta testing)
	//		2 = ROS Internal cmd
	//
	//opt(string) = string to be sent to specified command line
	//
	//Outputs: Print Line
	*/
	void prnt(int cmdln, std::string opt)
	{
		if (cmdln == 1) std::cout << opt.c_str() << std::endl;
		else if (cmdln == 2) std::cout << opt.c_str() << std::endl;
		else std::cout << "Unkn output" << std::endl;
	}

	/* ----------- uOutputHandler -----------
	//Description: Function that pushes out commands to UAS over both bluetooth and flight wireless signals
	//
	//Inputs:
	//sys (int) = Internal UAS System Commands ar ebeing sent to
	//	1 = Flight System
	//	2 = Mission System
	//	3 = Camera Swivel System
	//
	//comm (int) = Command for the specified system
	//
	//Outputs:
	//bool = sent(0=n, 1=y)
	*/
	bool uOutputHandler(int sys, int comm)
	{
		return true;
	}

	/* ----------- fSystemController -----------
	//Description: Function that distributes commands to on-board Flight System Controller
	//
	//Inputs:
	// b (int):
	//	0 = axis
	//	1 = trigger
	//  2 = bumper
	// i (double) = intensity
	// dx (double) = x-axis directionality
	// dy (double) = y-axis directionality
	//
	//Outputs:
	// bool = complete(0=n, 1=y)
	*/
	bool fSystemController(int b, double i, double dx, double dy)
	{
		return true;
	}

	/* ----------- mSystemTalon -----------
	//Description: Function that distributes commands to Mission System Talons
	//
	//Inputs:
	// t (int):
	//	0 = Close Talons
	//	1 = Open Talons
	//
	//Outputs: 
	// bool = complete(0=n, 1=y)
	*/
	bool mSystemTalon(int t)
	{
		return true;
	}

	/* ----------- mSystemCamera -----------
	//Description: Function that distributes commands to Mission System Camera Switch to switch between RGB and IR
	//
	//Inputs: N/A
	//
	//Outputs:
	// bool = complete(0=n, 1=y)
	*/
	bool mSystemCamera()
	{
		return true;
	}

	/* ----------- mSystemGimble -----------
	//Description: Function that distributes commands to Mission System Gimble
	//
	//Inputs:
	// g (int):
	//	0 = Move Gimble facing Down
	//	1 = Move Gimble Facing Up
	// i (double) = intensity
	//
	//Outputs:
	// bool = complete(0=n, 1=y)
	*/
	bool mSystemGimble(int g, double i)
	{
		return true;
	}

	/* ----------- doNothing -----------
	//Description: Filler for buttons we have not mapped anything to
	//
	//Inputs: N/A
	//
	//Outputs: N/A
	*/
	void doNothing()
	{
		return;
	}


	/* ----------- printTelemetry -----------
	//Description: Function that outputs telemetry to a user-specified csv file
	//
	//Inputs: N/A
	//
	//Outputs:
	// bool = complete(0=n, 1=y)
	*/
	bool printTelemetry()
	{
		return true;
	}

	/* ----------- editTelemetry -----------
	//Description: Function that applies user-specified changhes to telemetry
	//
	//Inputs: N/A
	//
	//Outputs:
	// bool = complete(0=n, 1=y)
	*/
	bool editTelemetry()
	{
		return true;
	}

	/* ----------- killCommand -----------
	//Description: Function that distributes commands to the three main systems to safely shut down
	//
	//Inputs: N/A
	//
	//Outputs: 
	// bool = complete(0=n, 1=y)
	*/
	bool killCommand()
	{
		return true;
	}

	/* ----------- cageCaptureAssist -----------
	//Description: Function that distributes commands to the three main systems to optimally pick up a cage
	//
	//Inputs: N/A
	//
	//Outputs:
	// bool = complete(0=n, 1=y)
	*/
	bool cageCaptureAssist()
	{
		return true;
	}

	/* ----------- cageLandAssist -----------
	//Description: Function that distributes commands to the three main systems to optimally land a cage
	//
	//Inputs: N/A
	//
	//Outputs:
	// bool = complete(0=n, 1=y)
	*/
	bool cageLandAssist()
	{
		return true;
	}


public:

	/* ----------- uInputHandler -----------
	//Description: Function that takes in data sent from UAS to distribute
	//
	//Inputs: N/A
	//
	//Outputs: Received data sent to specified internal function
	*/
	void uInputHandler()
	{
		return;
	}

	/* ----------- controllerHandler -----------
	//Description: Function that goes through possible button changes to push out mapped function calls
	//
	//Inputs: N/A
	//
	//Outputs: N/A
	*/
	void controllerHandler()
	{
		if (debug) std::cout << "Gamepad state has changed" << std::endl;

		// Print out states of left thumbstick
		if (!gamepad.leftStickInDeadzone())
		{
			if (debug) std::cout << "Left thumbstick x: " << gamepad.leftStick_x() << std::endl;
			if (debug) std::cout << "Left thumbstick y: " << gamepad.leftStick_y() << std::endl;
			fSystemController(0, 0, gamepad.leftStick_x(), gamepad.leftStick_y());
		}
		else
			if (debug) std::cout << "Left thumbstick in deadzone" << std::endl;

		// Print out states of right thumbstick
		if (!gamepad.rightStickInDeadzone())
		{
			if (debug) std::cout << "Right thumbstick x: " << gamepad.rightStick_x() << std::endl;
			if (debug) std::cout << "Right thumbstick y: " << gamepad.rightStick_y() << std::endl;
			
			if (gamepad.rightStick_y() > 0) mSystemGimble(1, gamepad.rightStick_y());
			else if (gamepad.rightStick_y() < 0) mSystemGimble(0, -1 * gamepad.rightStick_y());
			fSystemController(2, 0, gamepad.rightStick_x(), 0);
		}
		else
			if (debug) std::cout << "Right thumbstick in deadzone" << std::endl;

		// Print out states of triggers
		if (debug) std::cout << "Left trigger: " << gamepad.leftTrigger() << std::endl;
		if (debug) std::cout << "Right trigger: " << gamepad.rightTrigger() << std::endl;
		double intensity = 0 + gamepad.rightTrigger() - gamepad.leftTrigger();
		fSystemController(1, intensity, 0, 0);

		// Print out states of all buttons
		for (int i = 0; i < 14; i++)
		{
			int iValue = (gamepad.getButtonPressed(i) ? 1 : 0);
			if (debug) std::cout << "Button ID " << i << ": " << iValue << std::endl;
			if (iValue == 1)
			{
				if (i == 0) mSystemTalon(0); //a
				else if (i == 1) killCommand(); //b
				else if (i == 2) mSystemTalon(1); //x
				else if (i == 3) doNothing(); //y
				else if (i == 4) mSystemCamera(); //d-pad up
				else if (i == 5) doNothing(); //d-pad down
				else if (i == 6) cageCaptureAssist(); //d-pad left
				else if (i == 7) cageLandAssist(); //d-pad right
				else if (i == 8) doNothing(); //left bumper
				else if (i == 9) doNothing(); //right bumper
				else if (i == 12) doNothing(); //start
				else if (i == 13) doNothing(); //select
			}
		}
	}
};

int main()
{
	prgm m;

	// Wait for gamepad to be connected
	while (!gamepad.connected())
	{
		std::cout << "Gamepad Not Connected..." << std::endl;
		Sleep(1000);
	}

	while (false)
	{
		std::cout << "UAS Not Connected..." << std::endl;
		Sleep(1000);
	}

	if (debug) std::cout << "Gamepad " << gamepad.getIndex() << " connected" << std::endl;

	// Main loop
	while (true)
	{
		// Check Controller status, if used send out commands
		gamepad.update();
		if (gamepad.hasChanged()) m.controllerHandler();

		// Distribute UAS input
		m.uInputHandler();

		Sleep(100);
	}
}