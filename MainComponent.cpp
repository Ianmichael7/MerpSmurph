#include "stdafx.h"
#include <windows.h>
#include <fstream>
#include <string>
#include <streambuf>
#include <ctime>
#include <direct.h>
#include <algorithm>
#include "gamepad.h"
#include "JuceLibraryCode/JuceHeader.h"

using namespace std;

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
	string sDataS;
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

/* ----------- animal -----------
//Description: Queue to keep track of animals and their status
//
//Data Structure:
//uSys (int) = ROS function call
//		1 = Flight System
//		2 = Mission System
//		3 = Camera Swivel System
//
//sDataI (int) = Output data to specified system
*/
struct animal
{
	string name;
	int state = 0;
} animals[12];

//CONTROLLER
Gamepad gamepad;

//GLOBAL VARIABLES
bool debug = false;
bool demo = true;
bool queueDemo = false;
bool con = true; //UAS connectivity Tracking
bool ctr = false; //Controller Connectivity Tracking

class prgm
{
public:
	/* -------Telemetry Data------- */
	double battery = 100;
	double altitude = 0.0;
	double temp = 0.0;
	bool sensor1 = 0.0;
	bool sensor2 = 0.0;
	bool sensor3 = 0.0;
	bool sensor4 = 0.0;
						 
	/* -------Modified Telemetry------- */
	bool tempMod = true; //false = Celcius, true = Farhrenheit
	bool altMod = false; //false = Feet, true = Meters
	double altitudeM = 0.0;
	double tempM = 0.0;

	/*--------UAS Commands--------*/
	bool isTalonClosed = false;
	bool isAIControlled = false;

private:
	/* -------Time Data-------*/
	time_t now = time(0);
	tm *tme = localtime(&now);
	int year = 1900 + tme->tm_year;
	int month = 1 + tme->tm_mon;
	int day = tme->tm_mday;
	int hour = tme->tm_hour;
	int min = tme->tm_min;
	int lastSec = 60;

	string dte = to_string(hour) + "-" + to_string(min) + "-" + to_string(month) + "-" + to_string(day) + "-" + to_string(year);

	/* ----Telemetry Modifiers--- */
	string teForm = "t";
	string altForm = "a";

	/* -------Starting Altitude-------- */
	double startAlt = -1;

	/* -------Talon Control----------*/
	double cageHeight = -1;

	/* -------Kill Command Control-------*/
	bool isKilling = false;

	/* -------File Stream------- */
	ofstream lf;

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
	// i (double) = intensity (positive or negative)
	// dx (double) = x-axis directionality
	// dy (double) = y-axis directionality
	//
	//Outputs:
	// bool = complete(0=n, 1=y)
	*/
	bool fSystemController(int b, double i, double dx, double dy)
	{
		if (b == 0 && dx > 0.2)
		{
			cout << "TILT RIGHT" << endl;
		}
		else if (b == 0 && dx < -0.2)
		{
			cout << "TILT LEFT" << endl;
		}
		if (b == 0 && dy > 0.2)
		{
			cout << "TILT UP" << endl;
		}
		else if (b == 0 && dy < -0.2)
		{
			cout << "TILT DOWN" << endl;
		}

		if (b == 1 && i > 0.2)
		{
			cout << "ACCELERATE" << endl;
		}
		else if (b == 1 && i < -0.2)
		{
			cout << "DECELERATE" << endl;
		}
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
		if (!isTalonClosed)
		{
			if (t == 0)
			{
				if (cageHeight == -1) cageHeight = altitude - startAlt;
			}

			//Send command to close talon here

			isTalonClosed = true;
		}
		else
		{
			//Send command to open talon here

			isTalonClosed = true;
		}
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
		editTelemetry();

		//Calculate Current Time
		time_t n = time(0);
		tm *t = localtime(&n);
		int y = 1900 + t->tm_year;
		int m = 1 + t->tm_mon;
		int d = t->tm_mday;
		int h = t->tm_hour;
		int mi = t->tm_min;
		int s = t->tm_sec;

		//Temp Unit
		string de = "C";
		if (tempMod) de = "F";
		else de = "C";

		//Height Unit
		string he = "ft";
		if (altMod) he = "m";
		else he = "ft";

		//Write to Log
		lf.open(".\\log\\HOPPS_" + dte + ".csv", ios::out | ios::app);
		lf << " Date:" << m << "/" << d << "/" << y << ",Time-" << h << ":" << mi << ":" << s
			<< ",Battery%:" << battery
			<< ",Altitude:" << altitudeM << he
			<< ",Tempurature:" << temp << de
			<< ",USensor1:" << sensor1
			<< ",USensor2:" << sensor2
			<< ",USensor3:" << sensor3
			<< ",USensor4:" << sensor4
			<< endl;
		lf.close();
		return true;
	}

	/* ----------- telemetryModParser -----------
	//Description: Function that applies user-specified changhes to telemetry
	//
	//Inputs: fm (string) = string representing modification formula to variable
	//
	//Outputs:
	// double = formula applied to variable
	*/
	double telemetryModParser(string fm)
	{
		double v1 = 0;
		double v2 = 0;
		char op1 = '+';
		char op2 = ' ';
		int pos = 0;
		string cur = "";

		while (pos != fm.length())
		{
			while (fm[pos] != '*' && fm[pos] != '/' && pos != fm.length())
			{
				while (fm[pos] != '+' && fm[pos] != '-' && fm[pos] != '*' && fm[pos] != '/' && pos != fm.length())
				{
					if (fm[pos] == 'a' && op1 == '+') v1 += altitude;
					else if (fm[pos] == 'a' && op1 == '-') v1 -= altitude;
					else
					{
						cur += fm[pos];

						if (pos == fm.length() - 1 || (pos != fm.length() - 1 && (fm[pos + 1] == '*' ||
							fm[pos + 1] == '/' || fm[pos + 1] == '+' || fm[pos + 1] == '-')))
						{
							if (op1 == '+') v1 += stod(cur);
							else if (op1 == '-') v1 -= stod(cur);
							cur = "";
						}
					}
					pos++;
				}
				if (pos != fm.length() && fm[pos] == '+')
				{
					op1 = '+';
					pos++;
				}
				else if (pos != fm.length() && fm[pos] == '-')
				{
					op1 = '-';
					pos++;
				}
			}

			op1 = '+';

			if (pos != fm.length() && fm[pos] == '*')
			{
				op2 = '*';
				pos++;
			}
			else if (pos != fm.length() && fm[pos] == '/')
			{
				op2 = '/';
				pos++;
			}

			while (pos != fm.length())
			{
				while (pos != fm.length() && fm[pos] != '+' && fm[pos] != '-' && fm[pos] != '*' && fm[pos] != '/')
				{
					if (fm[pos] == 'a' && op1 == '+') v2 += altitude;
					else if (fm[pos] == 'a' && op1 == '-') v2 -= altitude;
					else
					{
						cur += fm[pos];

						if (pos == fm.length() - 1 || (pos != fm.length() - 1 && (fm[pos + 1] == '*' ||
							fm[pos + 1] == '/' || fm[pos + 1] == '+' || fm[pos + 1] == '-')))
						{
							if (op1 == '+') v2 += stod(cur);
							else if (op1 == '-') v2 -= stod(cur);
							cur = "";
						}
					}
					pos++;
				}
				if (pos != fm.length() && fm[pos] == '+')
				{
					op1 = '+';
					pos++;
				}
				else if (pos != fm.length() && fm[pos] == '-')
				{
					op1 = '-';
					pos++;
				}
				else if (pos != fm.length() && fm[pos] == '*')
				{
					if (op2 == '*') v1 = v1 * v2;
					else if (op2 == '/') v1 = v1 / v2;
					v2 = 0;
					op1 = '+';
					op2 = '*';
					pos++;
				}
				else if (pos != fm.length() && fm[pos] == '/')
				{
					if (op2 == '*') v1 = v1 * v2;
					else if (op2 == '/') v1 = v1 / v2;
					v2 = 0;
					op1 = '+';
					op2 = '/';
					pos++;
				}
				else if (pos == fm.length())
				{
					if (op2 == '*') v1 = v1 * v2;
					else if (op2 == '/') v1 = v1 / v2;
				}
			}
		}
		return v1;
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
		//altitude
		if (altForm == "a") altitudeM = altitude;
		else altitudeM = telemetryModParser(altForm);

		return true;
	}

	/* ----------- editTelemetryFormulas (Command Line) -----------
	//Description: Function that allows user to specify commands in the command prompt to change telemetry modifications
	//
	//Inputs: N/A
	//
	//Outputs:
	// bool = complete(0=n, 1=y)
	*/
	bool editTelemetryFormulas()
	{
		string ipt;
		string allowed = "as01234567890+-*/";
		bool cont = true;

		cout << "\nWelcome to command line, type 'help' for a guide on how to use the command line. Type 'exit' to exit out of command line to resume normal operation." << endl;

		while (cont)
		{

			getline(cin, ipt);

			//erase spaces for ease of reading
			ipt.erase(remove(ipt.begin(), ipt.end(), '_'), ipt.end());

			//turn everything to lowercase
			transform(ipt.begin(), ipt.end(), ipt.begin(), ::tolower);

			if (ipt == "")
			{
				cout << "Type 'help' for a guide on how to use the command line." << endl;
				cont = true;
			}
			else if (ipt == "exit")
			{
				cout << "Command line closed, please return to ROS Graphical window." << endl;
				cont = false;
			}
			else if (ipt == "help")
			{
				cout << "\nType 'exit' to exit command line." << endl;
				cout << "Type 'celsius' to change to celsius tempurature readings." << endl;
				cout << "Type 'fahrenheit' to change to fahrenheit tempurature readings." << endl;
				cout << "Type 'meters' to change to meters altitude readings." << endl;
				cout << "Type 'feet' to change to feet altitude readings." << endl;
				cout << "Type 'cageheight' to set known cage height (Otherwise estimated on pickup)." << endl;
				cout << "\nTo change telemetry forumlas, use complete forumlas such as 'x = x * 10'" << endl;
				cout << "Variables to change:\n 'a' = altitude\n 't' = speed" << endl;
				cont = true;
			}
			else if (ipt == "celsius")
			{
				cout << "Temp forumla changed to Celsius. Now exiting to ROS Screen..." << endl;
				tempMod = false;

				cont = false;
			}
			else if (ipt == "fahrenheit")
			{
				cout << "Temp forumla changed to Fahrenheit. Now exiting to ROS Screen..." << endl;
				tempMod = true;
				cont = false;
			}
			else if (ipt == "feet")
			{
				cout << "Altitude formula changed to Feet. Now exiting to ROS Screen..." << endl;
				if (altMod)
				{
					startAlt = startAlt / 0.3048;
					cageHeight = cageHeight / 0.3048;
				}
				altMod = false;
				cont = false;
			}
			else if (ipt == "meters")
			{
				cout << "Altitude changed to Meters. Now exiting to ROS Screen..." << endl;
				if (!altMod)
				{
					startAlt = startAlt * 0.3048;
					cageHeight = cageHeight * 0.3048;
				}
				altMod = true;
				cont = false;
			}
			else if (ipt == "cageheight")
			{
				string he = "feet";
				if (altMod) he = "meters";
				else he = "feet";

				cout << "Please Enter the Cage height in " << he << endl;

				cont = false;
			}
			else
			{
				bool modified = false; //false = speed, true = altitude
				bool readable = true;

				string form = ""; // formula

				if (ipt[0] == 'a' && ipt[1] == '=' && ipt.length() > 2) modified = true;
				else if (ipt[0] == 't' && ipt[1] == '=' && ipt.length() > 2) modified = false;
				else readable = false;

				if (readable)
				{
					//Check for use of only allowed characters
					bool consta = true; //bool to prevent constants
					for (int it = 2; it < ipt.length(); it++)
					{
						if (ipt[it] == 'a' || ipt[it] == 't') consta = false;
						for (int a = 0; a < allowed.length(); a++)
						{
							if (ipt[it] == allowed[a])
							{
								if (ipt[it] == '+' || ipt[it] == '-' || ipt[it] == '*' || ipt[it] == '/')
								{
									if (a == allowed.length() - 1)
									{
										cout << "Cannot end funtion with an operator: " << ipt[it] << endl;
										readable = false;
									}
									else if (ipt[it + 1] == '*' || ipt[it + 1] == '-' || ipt[it + 1] == '*' || ipt[it + 1] == '/')
									{
										cout << "Cannot have two operators next to eachother: " << ipt[it] << ipt[it + 1] << endl;
										readable = false;
									}
								}
								else if (ipt[it] == 'a' || ipt[it] == 't')
								{
									if (a != allowed.length() - 1 && (ipt[it + 1] == 'a' || ipt[it + 1] == 't' || ipt[it + 1] == '1' ||
										ipt[it + 1] == '2' || ipt[it + 1] == '3' || ipt[it + 1] == '4' || ipt[it + 1] == '5' ||
										ipt[it + 1] == '6' || ipt[it + 1] == '7' || ipt[it + 1] == '8' || ipt[it + 1] == '9' ||
										ipt[it + 1] == '0'))
									{
										cout << "Operator required between variables: " << ipt[it] << ipt[it + 1] << endl;
										readable = false;
									}
								}
								else
								{
									if (a != allowed.length() - 1 && (ipt[it + 1] == 'a' || ipt[it + 1] == 't'))
									{
										cout << "Operator required between variables: " << ipt[it] << ipt[it + 1] << endl;
										readable = false;
									}
								}
								form += ipt[it];
								break;
							}
							else if (a == allowed.length() - 1)
							{
								cout << "Unknown Character: " << ipt[it] << endl;
								readable = false;
							}
						}
						if (!readable) break;
					}
					if (readable && consta)
					{
						cout << "Constants not allowed, include variables (ex. 'x = x', 'x = x + 1')" << endl;
						readable = false;
					}
				}

				if (readable && !modified)
				{
					cout << "Tempurature Formula is now: " << form << " ... Now exiting to ROS Screen..." << endl;
					teForm = form;
					cont = false;
				}
				else if (readable && modified)
				{
					cout << "Altitude Formula is now: " << form << " ... Now exiting to ROS Screen..." << endl;
					altForm = form;
					cont = false;
				}

				if (!readable)
				{
					cout << "Unknown Input... type 'help' for options" << endl;
					cont = true;
				}
			}
		}
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
		double i = 0.0;
		double g = 20; // Set variable to start decreasing speed

		if (!altMod) g = g * 0.3048;

		if (isTalonClosed) i = -1 * ((altitude - startAlt - cageHeight) / g);
		else i = -1 * ((altitude - startAlt) / g);

		fSystemController(1, i, 0, 0);

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

	/* ----------- updateTelemetry -----------
	//Description: Function that takes in received telemtry data and applies to private variables
	//
	//Inputs: N/A
	//
	//Outputs:
	// bool = complete(0=n, 1=y)
	*/
	bool updateTelemetry(double a, double t, double s1, double s2, double s3, double s4)
	{
		/* -------Telemetry Data------- */
		sensor1 = s1;
		sensor2 = s2;
		sensor3 = s3;
		sensor4 = s4;

		/* ----Telemetry Modifiers--- */
		if (tempMod) temp = t;
		else temp = (t - 32) / 1.8;

		if (!altMod) altitude = a;
		else altitude = a * 0.3048;

		if (startAlt == -1) startAlt = altitude;

		return true;
	}

public:
	bool q = false;
	int qn = 0;

	/* ----------- setQueue -----------
	//Description: Sets the Queue
	//
	//Inputs: N/A
	//
	//Outputs:
	// bool = complete(0=n, 1=y)
	*/
	bool setQueue()
	{
		animals[0].name = "Elephant (P1)"; animals[1].name = "Gorilla (P1)"; animals[2].name = "Rhinoceros (P1)";
		animals[3].name = "Tiger (P2)"; animals[4].name = "Panda (P2)"; animals[5].name = "Giraffe (P3)";
		animals[6].name = "Lion (P3)"; animals[7].name = "Cheetah (P3)"; animals[8].name = "Zebra (P4)";
		animals[9].name = "Moose (P4)"; animals[10].name = "Warthog (P4)"; animals[11].name = "Hippo (P4)";
		return true;
	}

	/* ----------- updateQueue -----------
	//Description: Function that allows user to update the queue
	//
	//Inputs: N/A
	//
	//Outputs:
	// bool = complete(0=n, 1=y)
	*/
	bool updateQueue()
	{
		// Print out states of right thumbstick
		if (!gamepad.rightStickInDeadzone())
		{
			if (gamepad.rightStick_y() > 0) qn--;
			else if (gamepad.rightStick_y() < 0) qn++;
		}

		if (gamepad.getButtonPressed(1)) { killCommand(); isKilling = true; } //b - KILL COMMAND
		else if (gamepad.getButtonPressed(4)) qn--; //d-pad up - MOVE ONE UP QUEUE (OR LOOP TO BOTTOM)
		else if (gamepad.getButtonPressed(5)) qn++; //d-pad down - MOVE ONE DOWN QUEUE (OR LOOP TO TOP)

		//Loop Queue
		if (qn == -1) qn = 11;
		else if (qn == 12) qn = 0;

		if (gamepad.getButtonPressed(1)) //a
		{
			if (animals[q].state == 0) animals[q].state = 1; // CHANGE Safe to Danger
			else if (animals[q].state == 1) animals[q].state = 2; // CHANGE Danger to Rescued
			else if (animals[q].state == 2) animals[q].state = 3; // CHANGE Rescued to N/A
			else if (animals[q].state == 3) animals[q].state = 0; // CHANGE N/A to Safe
		}

		if ((gamepad.getButtonPressed(3) ? 1 : 0) == 1) q = false; //y - EXIT QUEUE

		return true;
	}

	/* ----------- buildQueue -----------
	//Description: Builds string with animal name and it's current status
	//
	//Inputs: N/A
	//
	//Outputs:
	// bool = complete(0=n, 1=y)
	*/
	string buildQueue(int i)
	{
		if (i < 0 || i > 11) return "_";

		string qs = "Safe";
		if (animals[i].state == 1) qs = "Danger";
		else if (animals[i].state == 2) qs = "Rescued";
		else if (animals[i].state == 3) qs = "N/A";

		return animals[i].name + " - " + qs;
	}

	/* ----------- uInputHandler -----------
	//Description: Function that takes in data sent from UAS to distribute
	//
	//Inputs: N/A
	//
	//Outputs: Received data sent to specified internal function
	*/
	void uInputHandler()
	{
		//Update Internal variables with received telemetry data
		//updateTelemetry(0, 0, 0, 0, 0, 0);

		if (isKilling) killCommand();

		//Print to the log every 5 seconds
		time_t n = time(0);
		tm *t = localtime(&n);
		if (t->tm_sec != lastSec && t->tm_sec % 1 == 0)
		{
			printTelemetry();
			lastSec = t->tm_sec;
		}
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
		// Left thumbstick
		if (!gamepad.leftStickInDeadzone()) fSystemController(0, 0, gamepad.leftStick_x(), gamepad.leftStick_y());

		// Right thumbstick
		if (!gamepad.rightStickInDeadzone())
		{
			if (gamepad.rightStick_y() > 0) mSystemGimble(1, gamepad.rightStick_y());
			else if (gamepad.rightStick_y() < 0) mSystemGimble(0, -1 * gamepad.rightStick_y());
			fSystemController(2, 0, gamepad.rightStick_x(), 0);
		}

		// Triggers
		double intensity = 0 + gamepad.rightTrigger() - gamepad.leftTrigger();
		if (intensity != 0) fSystemController(1, intensity, 0, 0);

		// All buttons
		for (int i = 0; i < 14; i++)
		{
			bool iValue = gamepad.getButtonPressed(i);
			if (iValue)
			{
				if (i == 0) mSystemTalon(0); //a - CLOSE TALON
				else if (i == 1) { killCommand(); isKilling = true; }//b - KILL COMMAND
				else if (i == 2) mSystemTalon(1); //x - OPEN TALON
				else if (i == 3) q = true; //y - QUEUE EDITABLE
				else if (i == 4) mSystemCamera(); //d-pad up - SWITCH CAMERAS
				else if (i == 5) doNothing(); //d-pad down - DO NOTHING
				else if (i == 6) cageCaptureAssist(); //d-pad left - CAGE CAPTURE ASSIST
				else if (i == 7) cageLandAssist(); //d-pad right - CAGE LAND ASSIST
				else if (i == 8) doNothing(); //left bumper - DO NOTHING
				else if (i == 9) doNothing(); //right bumper - DO NOTHING
				else if (i == 12) doNothing(); //start - DO NOTHING
				else if (i == 13) doNothing();  //editTelemetryFormulas(); //select - OPEN CMD LINE
			}
		}
	}
};

class MainContentComponent   : public OpenGLAppComponent
{
public:
	prgm m;

	ScopedPointer<Label> queueLabel, powerLabel, systemLabel, flightLabel, powerListing, 
		systemListing, flightListing, talonStatus, sensorLabel, sensorListing, telLabel, opLabel,
		a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, t1, t2, t3, t4, o1, o2, r1, r2, upd, s1, 
		s2, s3, s4, tt1, tt2;

	ScopedPointer<TextButton> quitButton;
	Path internalPath1;

	File f1 = "drone.jpg";
	Image drone = ImageFileFormat::loadFrom(f1);

	File f2 = "video.jpg";
	Image vid = ImageFileFormat::loadFrom(f2);

	int selY = 55;

    MainContentComponent()
    {
		m.setQueue();

		mkdir(".\\log");

		//Screen Updater
		OpenGLAppComponent::addAndMakeVisible(upd = new Label(String(), TRANS("o")));
		upd->setFont(Font(15.00f, Font::bold)); upd->setColour(Label::textColourId, Colours::white);
		upd->setBounds(1100, 205, 100, 15); upd->setJustificationType(Justification::centred);

		//Queue Label
		OpenGLAppComponent::addAndMakeVisible(queueLabel = new Label(String(),TRANS("Queue")));
		queueLabel->setFont(Font(25.00f, Font::bold)); queueLabel->setColour(Label::textColourId, Colours::darkgrey); queueLabel->setBounds(1100, 10, 300, 25);

		//Animal Queue Listing
		OpenGLAppComponent::addAndMakeVisible(a0 = new Label(String(), TRANS(m.buildQueue(0))));
		a0->setFont(Font(15.00f, Font::bold)); a0->setColour(Label::textColourId, Colours::green); 
		a0->setBounds(1050, 40, 200, 15); a0->setJustificationType(Justification::centred);
		OpenGLAppComponent::addAndMakeVisible(a1 = new Label(String(), TRANS(m.buildQueue(1))));
		a1->setFont(Font(15.00f, Font::bold)); a1->setColour(Label::textColourId, Colours::green); 
		a1->setBounds(1050, 55, 200, 15); a1->setJustificationType(Justification::centred);
		OpenGLAppComponent::addAndMakeVisible(a2 = new Label(String(), TRANS(m.buildQueue(2))));
		a2->setFont(Font(15.00f, Font::bold)); a2->setColour(Label::textColourId, Colours::green); 
		a2->setBounds(1050, 70, 200, 15); a2->setJustificationType(Justification::centred);
		OpenGLAppComponent::addAndMakeVisible(a3 = new Label(String(), TRANS(m.buildQueue(3))));
		a3->setFont(Font(15.00f, Font::bold)); a3->setColour(Label::textColourId, Colours::green); 
		a3->setBounds(1050, 85, 200, 15); a3->setJustificationType(Justification::centred);
		OpenGLAppComponent::addAndMakeVisible(a4 = new Label(String(), TRANS(m.buildQueue(4))));
		a4->setFont(Font(15.00f, Font::bold)); a4->setColour(Label::textColourId, Colours::green); 
		a4->setBounds(1050, 100, 200, 15); a4->setJustificationType(Justification::centred);
		OpenGLAppComponent::addAndMakeVisible(a5 = new Label(String(), TRANS(m.buildQueue(5))));
		a5->setFont(Font(15.00f, Font::bold)); a5->setColour(Label::textColourId, Colours::green); 
		a5->setBounds(1050, 115, 200, 15); a5->setJustificationType(Justification::centred);
		OpenGLAppComponent::addAndMakeVisible(a6 = new Label(String(), TRANS(m.buildQueue(6))));
		a6->setFont(Font(15.00f, Font::bold)); a6->setColour(Label::textColourId, Colours::green); 
		a6->setBounds(1050, 130, 200, 15); a6->setJustificationType(Justification::centred);
		OpenGLAppComponent::addAndMakeVisible(a7 = new Label(String(), TRANS(m.buildQueue(7))));
		a7->setFont(Font(15.00f, Font::bold)); a7->setColour(Label::textColourId, Colours::green); 
		a7->setBounds(1050, 145, 200, 15); a7->setJustificationType(Justification::centred);
		OpenGLAppComponent::addAndMakeVisible(a8 = new Label(String(), TRANS(m.buildQueue(8))));
		a8->setFont(Font(15.00f, Font::bold)); a8->setColour(Label::textColourId, Colours::green); 
		a8->setBounds(1050, 160, 200, 15); a8->setJustificationType(Justification::centred);
		OpenGLAppComponent::addAndMakeVisible(a9 = new Label(String(), TRANS(m.buildQueue(9))));
		a9->setFont(Font(15.00f, Font::bold)); a9->setColour(Label::textColourId, Colours::green); 
		a9->setBounds(1050, 175, 200, 15); a9->setJustificationType(Justification::centred);
		OpenGLAppComponent::addAndMakeVisible(a10 = new Label(String(), TRANS(m.buildQueue(10))));
		a10->setFont(Font(15.00f, Font::bold)); a10->setColour(Label::textColourId, Colours::green); 
		a10->setBounds(1050, 190, 200, 15); a10->setJustificationType(Justification::centred);
		OpenGLAppComponent::addAndMakeVisible(a11 = new Label(String(), TRANS(m.buildQueue(11))));
		a11->setFont(Font(15.00f, Font::bold)); a11->setColour(Label::textColourId, Colours::green); 
		a11->setBounds(1050, 205, 200, 15); a11->setJustificationType(Justification::centred);

		//Power Status Label
		OpenGLAppComponent::addAndMakeVisible(powerLabel = new Label(String(), TRANS("Power Status:")));
		powerLabel->setFont(Font(25.00f, Font::bold)); powerLabel->setColour(Label::textColourId, Colours::darkgrey); 
		powerLabel->setBounds(1000, 250, 300, 25); powerLabel->setJustificationType(Justification::centred);

		//Power Status Listing
		OpenGLAppComponent::addAndMakeVisible(powerListing = new Label(String(), TRANS("System: ON  Battery: 100%")));
		powerListing->setFont(Font(20.00f, Font::bold)); powerListing->setColour(Label::textColourId, Colours::green);
		powerListing->setBounds(1050, 280, 200, 20); powerListing->setJustificationType(Justification::centred);
		
		//System Status Label
		OpenGLAppComponent::addAndMakeVisible(systemLabel = new Label(String(), TRANS("System Status:")));
		systemLabel->setFont(Font(25.00f, Font::bold)); systemLabel->setColour(Label::textColourId, Colours::darkgrey); 
		systemLabel->setBounds(1000, 310, 300, 25); systemLabel->setJustificationType(Justification::centred);

		//System Status Listing
		OpenGLAppComponent::addAndMakeVisible(systemListing = new Label(String(), TRANS("Safe")));
		systemListing->setFont(Font(20.00f, Font::bold)); systemListing->setColour(Label::textColourId, Colours::green);
		systemListing->setBounds(1050, 340, 200, 20); systemListing->setJustificationType(Justification::centred);

		//Flight Status Label
		OpenGLAppComponent::addAndMakeVisible(flightLabel = new Label(String(), TRANS("Flight Status:")));
		flightLabel->setFont(Font(25.00f, Font::bold)); flightLabel->setColour(Label::textColourId, Colours::darkgrey); 
		flightLabel->setBounds(1000, 370, 300, 25); flightLabel->setJustificationType(Justification::centred);

		//Flight Status Listing
		OpenGLAppComponent::addAndMakeVisible(flightListing = new Label(String(), TRANS("Operator Controlled")));
		flightListing->setFont(Font(20.00f, Font::bold)); flightListing->setColour(Label::textColourId, Colours::green);
		flightListing->setBounds(1050, 400, 200, 15); flightListing->setJustificationType(Justification::centred);

		//Talon Status Listing
		OpenGLAppComponent::addAndMakeVisible(talonStatus = new Label(String(), TRANS("(No Cage)")));
		talonStatus->setFont(Font(15.00f, Font::bold)); talonStatus->setColour(Label::textColourId, Colours::green);
		talonStatus->setBounds(1050, 415, 200, 15); talonStatus->setJustificationType(Justification::centred);

		//Sensor Status Label
		OpenGLAppComponent::addAndMakeVisible(sensorLabel = new Label(String(), TRANS("Sensor Status:")));
		sensorLabel->setFont(Font(25.00f, Font::bold)); sensorLabel->setColour(Label::textColourId, Colours::darkgrey);
		sensorLabel->setBounds(1000, 450, 300, 25); sensorLabel->setJustificationType(Justification::centred);

		//Sensor Status Listing
		OpenGLAppComponent::addAndMakeVisible(sensorListing = new Label(String(), TRANS("NO OBSTRUCTIONS")));
		sensorListing->setFont(Font(25.00f, Font::bold)); sensorListing->setColour(Label::textColourId, Colours::green);
		sensorListing->setBounds(1000, 480, 300, 25); sensorListing->setJustificationType(Justification::centred);

		//Telemetry Status Label
		OpenGLAppComponent::addAndMakeVisible(telLabel = new Label(String(), TRANS("Telemetry:")));
		telLabel->setFont(Font(25.00f, Font::bold)); telLabel->setColour(Label::textColourId, Colours::darkgrey);
		telLabel->setBounds(500, 525, 200, 25);

		//Telemetry Label / Listing 1 (Alt)
		OpenGLAppComponent::addAndMakeVisible(t1 = new Label(String(), TRANS("Altitude: 15ft")));
		t1->setFont(Font(20.00f, Font::bold)); t1->setColour(Label::textColourId, Colours::black);
		t1->setBounds(500, 580, 200, 25);

		//Telemetry Label / Listing 2 (Outside Temp)
		OpenGLAppComponent::addAndMakeVisible(t2 = new Label(String(), TRANS("Air Tempurature: 80F")));
		t2->setFont(Font(20.00f, Font::bold)); t2->setColour(Label::textColourId, Colours::black);
		t2->setBounds(500, 630, 200, 25);

		//Operator Assistance Status Label
		OpenGLAppComponent::addAndMakeVisible(opLabel = new Label(String(), TRANS("Operator Assistance:")));
		opLabel->setFont(Font(25.00f, Font::bold)); opLabel->setColour(Label::textColourId, Colours::darkgrey);
		opLabel->setBounds(20, 525, 250, 25);

		//OA Pick-Up Lable
		OpenGLAppComponent::addAndMakeVisible(o1 = new Label(String(), TRANS("Pick-Up Cage:")));
		o1->setFont(Font(20.00f, Font::bold)); o1->setColour(Label::textColourId, Colours::black);
		o1->setBounds(20, 580, 200, 25);

		//OA Landing Lable
		OpenGLAppComponent::addAndMakeVisible(o2 = new Label(String(), TRANS("Land Cage:")));
		o2->setFont(Font(20.00f, Font::bold)); o2->setColour(Label::textColourId, Colours::black);
		o2->setBounds(20, 630, 200, 25);

		//OA Pick-Up Listing
		OpenGLAppComponent::addAndMakeVisible(r1 = new Label(String(), TRANS("Possible")));
		r1->setFont(Font(20.00f, Font::bold)); r1->setColour(Label::textColourId, Colours::green);
		r1->setBounds(180, 580, 200, 25);

		//OA Landing Listing
		OpenGLAppComponent::addAndMakeVisible(r2 = new Label(String(), TRANS("Impossible")));
		r2->setFont(Font(20.00f, Font::bold)); r2->setColour(Label::textColourId, Colours::red);
		r2->setBounds(180, 630, 200, 25);

		//Front Sensor
		OpenGLAppComponent::addAndMakeVisible(s1 = new Label(String(), TRANS("X")));
		s1->setFont(Font(20.00f, Font::bold)); s1->setColour(Label::textColourId, Colours::green);
		s1->setBounds(1136, 532, 200, 25);

		//LEFT SENSOR
		OpenGLAppComponent::addAndMakeVisible(s2 = new Label(String(), TRANS("X")));
		s2->setFont(Font(20.00f, Font::bold)); s2->setColour(Label::textColourId, Colours::green);
		s2->setBounds(1056, 597, 200, 25);

		//RIGHT SENSOR
		OpenGLAppComponent::addAndMakeVisible(s3 = new Label(String(), TRANS("X")));
		s3->setFont(Font(20.00f, Font::bold)); s3->setColour(Label::textColourId, Colours::green);
		s3->setBounds(1218, 597, 200, 25);

		//BACK SENSOR
		OpenGLAppComponent::addAndMakeVisible(s4 = new Label(String(), TRANS("X")));
		s4->setFont(Font(20.00f, Font::bold)); s4->setColour(Label::textColourId, Colours::green);
		s4->setBounds(1136, 667, 200, 25);

		//OA Landing Listing
		OpenGLAppComponent::addAndMakeVisible(tt1 = new Label(String(), TRANS("X")));
		tt1->setFont(Font(20.00f, Font::bold)); tt1->setColour(Label::textColourId, Colours::green);
		tt1->setBounds(159, 580, 200, 25);

		//OA Landing Listing
		OpenGLAppComponent::addAndMakeVisible(tt2 = new Label(String(), TRANS("X")));
		tt2->setFont(Font(20.00f, Font::bold)); tt2->setColour(Label::textColourId, Colours::red);
		tt2->setBounds(159, 630, 200, 25);

		OpenGLAppComponent::setSize (1280, 720);
    }

    ~MainContentComponent()
    {
		queueLabel = nullptr;
		quitButton = nullptr;
        shutdownOpenGL();
    }

    void initialise() override {}

    void shutdown() override {}

    void render() override
    {
        OpenGLHelpers::clear (Colours::white);
    }

	Colour setQueueColor(int i)
	{
		if (animals[i].state == 1) return Colours::red;
		else if (animals[i].state == 2) return Colours::purple;
		else if (animals[i].state == 3) return Colours::blue;
		return Colours::green;
	}

	string updater = "o";
	string tempp = " ";
	string altt = " ";
	string powerr = " ";
	bool tup = true;
	bool aup= true;

	//TEMPORARY
	int lastSec = 60;

    void paint (Graphics& g) override
    {
		updater += "o";
		if (updater == "oooo") updater = "o";

		// Look for gamepad to be connected
		if(!gamepad.connected() && !demo)
		{
			if (debug) cout << "Gamepad Not Connected..." << endl;
			systemListing->setColour(Label::textColourId, Colours::red);
			systemListing->setText("Gamepad Disconnect", sendNotification);
			ctr = false;
		}
		else if (gamepad.connected() && !ctr)
		{
			if (debug) cout << "Gamepad " << gamepad.getIndex() << " connected" << endl;
			systemListing->setColour(Label::textColourId, Colours::green);
			systemListing->setText("Safe", sendNotification);

			ctr = true;
		}
		else if (demo)
		{
			systemListing->setColour(Label::textColourId, Colours::green);
			systemListing->setText("Safe", sendNotification);

			ctr = true;
		}

		// Look for UAS to be connected if not
		if(!con)
		{
			con = false;
			cout << "UAS Not Connected..." << endl;
			systemListing->setColour(Label::textColourId, Colours::red);
			systemListing->setText("UAS Disconnect", sendNotification);
		}
		else if (con && gamepad.connected())
		{
			cout << "UAS Connected!" << endl;
			systemListing->setColour(Label::textColourId, Colours::green);
			systemListing->setText("Safe", sendNotification);
			con = true;
		}

		if (gamepad.connected())
		{
			if (m.q)
			{
				if (gamepad.update()) m.updateQueue();
			}
			else if (gamepad.update()) m.controllerHandler();
		}

		// Distribute UAS input
		m.uInputHandler();

		//DRAW 'VIDEO' SAMPLE PICTURE
		g.drawImageAt(vid, 5, 5);

		//DRAW DRONE PICTURE
		g.drawImageAt(drone, 1070, 550);

		//FRONT SENSOR
		if (m.sensor1) s1->setColour(Label::textColourId, Colours::red);
		else s1->setColour(Label::textColourId, Colours::green);
		s1->setText("X", sendNotification);

		//LEFT SENSOR
		if (m.sensor2) s2->setColour(Label::textColourId, Colours::red);
		else s2->setColour(Label::textColourId, Colours::green);
		s2->setText("X", sendNotification);

		//RIGHT SENSOR
		if (m.sensor3) s3->setColour(Label::textColourId, Colours::red);
		else s3->setColour(Label::textColourId, Colours::green);
		s3->setText("X", sendNotification);

		//BACK SENSOR
		if (m.sensor4) s4->setColour(Label::textColourId, Colours::red);
		else s4->setColour(Label::textColourId, Colours::green);
		s4->setText("X", sendNotification);

		//Update Altitude
		altt = "Altitude: ";
		altt += to_string((int)m.altitudeM);
		if (m.altMod) altt += "m";
		else altt += "ft";
		t1->setText(altt, sendNotification);

		//Update Tempurature
		tempp = "Air Tempurature: ";
		tempp += to_string((int)m.temp);
		if (m.tempMod) tempp += "F";
		else tempp += "C";
		t2->setText(tempp, sendNotification);

		//Update Talon Status talonStatus
		if (m.isTalonClosed) talonStatus->setText("(CAGE)", sendNotification);
		else talonStatus->setText("(NO CAGE)", sendNotification);

		//Update PICK-UP INDICATOR
		if (!(m.sensor1 || m.sensor2 || m.sensor3 || m.sensor4) && con && !m.isTalonClosed && !m.isAIControlled)
		{
			g.setColour(Colours::green);
			r1->setColour(Label::textColourId, Colours::green);
			tt1->setColour(Label::textColourId, Colours::green);
			r1->setText("POSSIBLE", sendNotification);
			tt1->setText("X", sendNotification);
		}
		else
		{
			g.setColour(Colours::red);
			r1->setColour(Label::textColourId, Colours::red);
			tt1->setColour(Label::textColourId, Colours::red);
			r1->setText("IMPOSSIBLE", sendNotification);
			tt1->setText("X", sendNotification);
		}

		//Update LAND INDICATOR
		if (!(m.sensor1 || m.sensor2 || m.sensor3 || m.sensor4) && con && m.isTalonClosed && !m.isAIControlled)
		{
			g.setColour(Colours::green);
			r2->setColour(Label::textColourId, Colours::green);
			tt2->setColour(Label::textColourId, Colours::green);
			r2->setText("POSSIBLE", sendNotification);
			tt2->setText("X", sendNotification);
		}
		else
		{
			g.setColour(Colours::red);
			r2->setColour(Label::textColourId, Colours::red);
			tt2->setColour(Label::textColourId, Colours::red);
			r2->setText("IMPOSSIBLE", sendNotification);
			tt2->setText("X", sendNotification);
		}

		//Update Sensor Listing
		if (m.sensor1 || m.sensor2 || m.sensor3 || m.sensor4)
		{
			sensorListing->setColour(Label::textColourId, Colours::red);
			sensorListing->setText("OBSTRUCTION", sendNotification);
			sensorListing->repaint();
		}
		else
		{
			sensorListing->setColour(Label::textColourId, Colours::green);
			sensorListing->setText("NO OBSTRUCTION", sendNotification);
			sensorListing->repaint();
		}

		//Update UAS Power
		if (con)
		{
			if (m.battery <= 30.49)
			{
				systemListing->setColour(Label::textColourId, Colours::red);
				systemListing->setText("LOW BATTERY!", sendNotification);
				powerListing->setColour(Label::textColourId, Colours::orange);
				if (m.battery <= 5.49) powerListing->setColour(Label::textColourId, Colours::red);
			}
			else powerListing->setColour(Label::textColourId, Colours::green);
			powerr = "System: ON  ";
		}
		else
		{
			powerListing->setColour(Label::textColourId, Colours::red);
			powerr = "System: OFF  ";
		}
		powerr += to_string((int)m.battery) + "%";
		powerListing->setText(powerr, sendNotification);

		//Update Pilot Control m.isAIControlled 
		if (m.isAIControlled)
		{
			flightListing->setColour(Label::textColourId, Colours::cornflowerblue);
			talonStatus->setColour(Label::textColourId, Colours::cornflowerblue);
			flightListing->setText("Operator Assistance", sendNotification);
		}
		else
		{
			flightListing->setColour(Label::textColourId, Colours::green);
			talonStatus->setColour(Label::textColourId, Colours::green);
			flightListing->setText("Operator Controlled", sendNotification);
		}

		//QUEUE BORDER (Black whie not in use, Turns Green when in use)
		if (m.q) g.setColour(Colours::green);
		else g.setColour(Colours::black);

		g.drawLine(1030, 38, 1265, 38, 4);
		g.drawRect(1030, 5, 235, 230, 4);

		//QUEUE SELECTION LINE (White while not in use, Turns purple when in use)
		if(m.qn == 0) selY = 55;
		else if (m.qn == 1) selY = 70;
		else if (m.qn == 2) selY = 85;
		else if (m.qn == 3) selY = 100;
		else if (m.qn == 4) selY = 115;
		else if (m.qn == 5) selY = 130;
		else if (m.qn == 6) selY = 145;
		else if (m.qn == 7) selY = 160;
		else if (m.qn == 8) selY = 175;
		else if (m.qn == 9) selY = 190;
		else if (m.qn == 10) selY = 205;
		else if (m.qn == 11) selY = 220;

		if (m.q) g.setColour(Colours::purple);
		else g.setColour(Colours::white);

		g.drawLine(1045, selY, 1250, selY, 2);

		//BORDERS
		g.setColour(Colours::black);
		g.drawLine(1030, 307, 1265, 307, 4);
		g.drawLine(1030, 365, 1265, 365, 4);
		g.drawLine(1030, 440, 1265, 440, 4);
		g.drawLine(475, 525, 475, 715, 4);
		g.drawLine(1012, 525, 1012, 715, 4);
		g.drawRect(1137, 535, 20, 20, 2);
		g.drawRect(1057, 600, 20, 20, 2);
		g.drawRect(1219, 600, 20, 20, 2);
		g.drawRect(1137, 670, 20, 20, 2);
		g.drawRect(160, 583, 20, 20, 2);
		g.drawRect(160, 633, 20, 20, 2);

		//Update GUI Queue
		a0->setColour(Label::textColourId, Colours::white);
		a0->setText(updater, sendNotification);
		a0->setColour(Label::textColourId, setQueueColor(0));
		a0->setText(m.buildQueue(0), sendNotification);
		a1->setColour(Label::textColourId, setQueueColor(1));
		a1->setText(m.buildQueue(1), sendNotification);
		a2->setColour(Label::textColourId, setQueueColor(2));
		a2->setText(m.buildQueue(2), sendNotification);
		a3->setColour(Label::textColourId, setQueueColor(3));
		a3->setText(m.buildQueue(3), sendNotification);
		a4->setColour(Label::textColourId, setQueueColor(4));
		a4->setText(m.buildQueue(4), sendNotification);
		a5->setColour(Label::textColourId, setQueueColor(5));
		a5->setText(m.buildQueue(5), sendNotification);
		a6->setColour(Label::textColourId, setQueueColor(6));
		a6->setText(m.buildQueue(6), sendNotification);
		a7->setColour(Label::textColourId, setQueueColor(7));
		a7->setText(m.buildQueue(7), sendNotification);
		a8->setColour(Label::textColourId, setQueueColor(8));
		a8->setText(m.buildQueue(8), sendNotification);
		a9->setColour(Label::textColourId, setQueueColor(9));
		a9->setText(m.buildQueue(9), sendNotification);
		a10->setColour(Label::textColourId, setQueueColor(10));
		a10->setText(m.buildQueue(10), sendNotification);
		a11->setColour(Label::textColourId, setQueueColor(11));
		a11->setText(m.buildQueue(11), sendNotification);

		Sleep(20);

		//TEMPORARY DEMO VIDEO CODE - COMMENT IF NOT FOR VIDEO DEMO!
		if (demo)
		{
			if (tup) m.temp += 0.03;
			else m.temp -= 0.03;
			if (m.temp >= 90) tup = false;
			else if (m.temp <= 80) { m.temp = 80; tup = true; }

			if (aup) m.altitude += 0.03;
			else m.altitude -= 0.03;
			if (m.altitude >= 20) aup = false;
			else if (m.altitude <= 0) aup = true;

			if (m.altitude >= 19.9) m.isAIControlled = !m.isAIControlled;
			if (m.isAIControlled)
			{
				m.q = true;
				if (fmod(m.altitude,3) == 0)
				{
					if (animals[0].state == 0) animals[0].state = 1; // CHANGE Safe to Danger
					else if (animals[0].state == 1) animals[0].state = 2; // CHANGE Danger to Rescued
					else if (animals[0].state == 2) animals[0].state = 3; // CHANGE Rescued to N/A
					else if (animals[0].state == 3) animals[0].state = 0; // CHANGE N/A to Safe
				}
			}
			else m.q = false;

			if (m.altitude > 15) m.sensor1 = true;
			else m.sensor1 = false;

			m.battery -= 0.02;
			if (m.battery <= 0) m.battery = 100;
		}
    }

    void resized() override {}
};

Component* createMainContentComponent()
{
	return new MainContentComponent();
}