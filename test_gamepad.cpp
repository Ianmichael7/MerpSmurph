#include "stdafx.h"
#include <windows.h>
#include <fstream>
#include <string>
#include <streambuf>
#include <ctime>
#include <direct.h>
#include <algorithm>
#include "gamepad.h"
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

//GLOBAL VARIABLES
bool debug = false;
Gamepad gamepad;
ifstream istrm("blah.txt", ios::binary);

bool con = false; //UAS connectivity Tracking
bool ctr = false; //Controller Connectivity Tracking

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
	/* ----------- updateGUI -----------
	//Description: Function that pushes GUI updates
	//
	//Inputs: N/A
	//
	//Outputs:
	// bool = complete(0=n, 1=y)
	*/
	bool updateGUI()
	{
		return true;
	}
	
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

	/* -------Telemetry Data------- */
	double altitude = 0.0;
	double temp = 0.0;
	double speed = 0.0;
	double gyroX = 0.0;
	double gyroY = 0.0;
	double gyroZ = 0.0;
	double sensor1 = 0.0;
	double sensor2 = 0.0;
	double sensor3 = 0.0;
	double sensor4 = 0.0;

	/* ----Telemetry Modifiers--- */
	bool tempMod = false; //false = Celcius, true = Farhrenheit
	bool altMod = false; //false = Feet, true = Meters
	string spForm = "s";
	string altForm = "a";

	/* -------Modified Telemetry------- */
	double altitudeM = 0.0;
	double speedM = 0.0;

	/* -------Starting Altitude-------- */
	double startAlt = -1;

	/* -------Talon Control----------*/
	bool isTalonClosed = false;
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
		if (t == 0)
		{
			if (cageHeight == -1) cageHeight = altitude - startAlt;
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
			<< ",Altitude:" << altitudeM << "ft"
			<< ",Speed:" << speedM 
			<< ",Tempurature:" << temp << de
			<< ",Gyro X-Axis:" << gyroX 
			<< ",Gyro Y-Axis:" << gyroY 
			<< ",Gyro Z-Axis:" << gyroZ 
			<< ",U Sensor 1:" << sensor1 
			<< ",U Sensor 2:" << sensor2 
			<< ",U Sensor 3:" << sensor3 
			<< ",U Sensor 4:" << sensor4 
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
					else if (fm[pos] == 's' && op1 == '+') v1 += speed;
					else if (fm[pos] == 'a' && op1 == '-') v1 -= altitude;
					else if (fm[pos] == 's' && op1 == '-') v1 -= speed;
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
					else if (fm[pos] == 's' && op1 == '+') v2 += speed;
					else if (fm[pos] == 'a' && op1 == '-') v2 -= altitude;
					else if (fm[pos] == 's' && op1 == '-') v2 -= speed;
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

		//speed
		if (spForm == "s") speedM = speed;
		else speedM = telemetryModParser(spForm);

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
				cout << "Variables to change:\n 'a' = altitude\n 's' = speed" << endl;
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
				else if (ipt[0] == 's' && ipt[1] == '=' && ipt.length() > 2) modified = false;
				else readable = false;

				if (readable)
				{
					//Check for use of only allowed characters
					bool consta = true; //bool to prevent constants
					for (int it = 2; it < ipt.length(); it++)
					{
						if (ipt[it] == 'a' || ipt[it] == 's') consta = false;
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
								else if (ipt[it] == 'a' || ipt[it] == 's')
								{
									if (a != allowed.length() - 1 && (ipt[it + 1] == 'a' || ipt[it + 1] == 's' || ipt[it + 1] == '1' || 
										ipt[it + 1] == '2' ||ipt[it + 1] == '3' || ipt[it + 1] == '4' || ipt[it + 1] == '5' || 
										ipt[it + 1] == '6' || ipt[it + 1] == '7' || ipt[it + 1] == '8' || ipt[it + 1] == '9' || 
										ipt[it + 1] == '0'))
									{
										cout << "Operator required between variables: " << ipt[it] << ipt[it + 1] << endl;
										readable = false;
									}
								}
								else
								{
									if (a != allowed.length() - 1 && (ipt[it + 1] == 'a' || ipt[it + 1] == 's'))
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
								cout << "Unknown Character: "<< ipt[it] << endl;
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
					cout << "Speed Formula is now: " << form << " ... Now exiting to ROS Screen..." << endl;
					spForm = form;
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
	bool updateTelemetry(double a, double t, double s, double gx, 
		double gy, double gz, double s1, double s2, double s3, double s4)
	{
		/* -------Telemetry Data------- */
		speed = s;
		gyroX = gx;
		gyroY = gy;
		gyroZ = gz;
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
		updateTelemetry(0, 0, 0, 0, 0, 0, 0, 0, 0, 0);

		if (isKilling) killCommand();

		//Print to the log every 5 seconds
		time_t n = time(0);
		tm *t = localtime(&n);
		if (t->tm_sec != lastSec && t->tm_sec % 5 == 0)
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
		if (!isKilling)
		{
			if (debug) cout << "Gamepad state has changed" << endl;

			// Print out states of left thumbstick
			if (!gamepad.leftStickInDeadzone())
			{
				if (debug) cout << "Left thumbstick x: " << gamepad.leftStick_x() << endl;
				if (debug) cout << "Left thumbstick y: " << gamepad.leftStick_y() << endl;
				fSystemController(0, 0, gamepad.leftStick_x(), gamepad.leftStick_y());
			}
			else
				if (debug) cout << "Left thumbstick in deadzone" << endl;

			// Print out states of right thumbstick
			if (!gamepad.rightStickInDeadzone())
			{
				if (debug) cout << "Right thumbstick x: " << gamepad.rightStick_x() << endl;
				if (debug) cout << "Right thumbstick y: " << gamepad.rightStick_y() << endl;

				if (gamepad.rightStick_y() > 0) { mSystemGimble(1, gamepad.rightStick_y()); cout << "MOVE CAMERA UP" << endl; }
				else if (gamepad.rightStick_y() < 0) { mSystemGimble(0, -1 * gamepad.rightStick_y()); cout << "MOVE CAMERA DOWN" << endl; }
				fSystemController(2, 0, gamepad.rightStick_x(), 0);
			}
			else
				if (debug) cout << "Right thumbstick in deadzone" << endl;

			// Print out states of triggers
			if (debug) cout << "Left trigger: " << gamepad.leftTrigger() << endl;
			if (debug) cout << "Right trigger: " << gamepad.rightTrigger() << endl;
			double intensity = 0 + gamepad.rightTrigger() - gamepad.leftTrigger();

			if (gamepad.leftTrigger() > 0.0 || gamepad.rightTrigger() > 0.0) fSystemController(1, intensity, 0, 0);

			// Print out states of all buttons
			for (int i = 0; i < 14; i++)
			{
				int iValue = (gamepad.getButtonPressed(i) ? 1 : 0);
				if (debug) cout << "Button ID " << i << ": " << iValue << endl;
				if (iValue == 1)
				{
					if (i == 0) { mSystemTalon(0); cout << "CLOSE TALON" << endl; } //a
					else if (i == 1) { killCommand(); cout << "KILL COMMAND" << endl; isKilling = true; }//b
					else if (i == 2) { mSystemTalon(1); cout << "OPEN TALON" << endl; } //x
					else if (i == 3) doNothing(); //y
					else if (i == 4) { mSystemCamera(); cout << "SWITCH CAMERAS" << endl; } //d-pad up
					else if (i == 5) doNothing(); //d-pad down
					else if (i == 6) { cageCaptureAssist(); cout << "CAGE CAPTURE ASSIST" << endl; } //d-pad left
					else if (i == 7) { cageLandAssist(); cout << "CAGE LAND ASSIST" << endl; } //d-pad right
					else if (i == 8) doNothing(); //left bumper
					else if (i == 9) doNothing(); //right bumper
					else if (i == 12) doNothing(); //start
					else if (i == 13) { editTelemetryFormulas(); cout << "OPEN CMD LINE" << endl; } //select
				}
			}
		}
	}
};

int main()
{
	prgm m;
	gui g;

	//Make log directory
	mkdir(".\\log");

	//Make GUI
	g.createGUI();

	// Main loop
	while (true)
	{
		// Wait for gamepad to be connected if not
		while (!gamepad.connected())
		{
			ctr = false;
			g.updateGUI();
			cout << "Gamepad Not Connected..." << endl;
			Sleep(1000);
		}
		if (gamepad.connected() && !ctr)
		{
			if (debug) cout << "Gamepad " << gamepad.getIndex() << " connected" << endl;
			else cout << "Gamepad Connected!" << endl;
			ctr = true;
		}

		// Wait for UAS to be connected if not
		while (false)
		{
			con = false;
			g.updateGUI();
			cout << "UAS Not Connected..." << endl;
			Sleep(1000);
		}
		if (!con)
		{
			cout << "UAS Connected!" << endl;
			con = true;
		}

		// Check Controller status, if used send out commands
		gamepad.update();
		if (gamepad.hasChanged()) m.controllerHandler();

		// Distribute UAS input
		m.uInputHandler();

		g.updateGUI();

		Sleep(100);
	}
}