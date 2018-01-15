// winutk.cpp : Defines the entry point for the console application.

/*
Dan Witt
 5/2/2008
 winutk.cpp
 Tracks the idle time of a user.
*/ 

#include "stdafx.h"
#include <iostream>
#include <fstream>
#include <time.h>
using namespace std;
//#define _WIN32_WINNT 0x0500
//#include <windows.h>
#include <shellapi.h>

/*
Todo:
 #command line switch for 'idle time' defaults to ten minutes
 #command line switch for log path defaults to C:\windows\system32\logfiles\wintk
 #help info
 #idle time starts after the idle timeout period, needs to start when user is idle (subtract off 'maxIdleTime')
 #Help needs to open in a window.
 CLI is making this thing a memeory hog (7MB)!

Possible enhancements:
 Screen shot every hour
 Get list of applictions running
*/

//int main( int argc, char* argv[] )
int APIENTRY _tWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, char* lpCmdLine, int nCmdShow)
{
	//int maxIdleTime = 5000; //For testing. 5 seconds.
	int maxIdleTime = 600000; //Time is in milliseconds, 10 minute default idle time.
	wchar_t* logLocation = L"C:\\windows\\system32\\logfiles\\winutk";
	wchar_t* logFileName = L"\\winutk_log.txt";

	int argc;
	wchar_t** argv = CommandLineToArgvW(GetCommandLineW(), &argc);

	if (argc > 1) {
		for(int i = 0; i < argc; i++) {
			if (wcscmp(argv[i], L"-t") == 0) {
				i++;
				maxIdleTime = _wtoi(argv[i])*60000;
				/*
				istringstream i((string)argv[i]);
				if(!(i >> maxIdleTime)) {
					cout << "Please enter a number in minutes" << endl;
					return 0;
				}
				maxIdleTime *= 60000;
				*/
			}
			if (wcscmp(argv[i], L"-l") == 0) {
				i++;
				logLocation = argv[i];
			}
			if (wcscmp(argv[i], L"--help") == 0) {
				::MessageBox(NULL, TEXT("Usage:\n winutk [OPTION]\n\nOptions:\n -t	Minutes until idle time is tracked (default: 10 minutes)\n -l	Location of log file, the directory must already exist (default: C:\\windows\\system32\\logfiles\\winutk)"), TEXT("Ilde Time Tracker Help"), MB_OK);
				return 0;
			}
		}
	}

	//Check if the directory exists, if so, concatenate the directory path to the log file name.
	if(GetFileAttributesW(logLocation) == 0xFFFFFFFF) {
		wchar_t message[1000];
		wcscpy(message, logLocation);
		wcscat(message, L" does not exist, please create the directory.\n\nwinutk --help for more options.");
		::MessageBox(NULL, message, TEXT("Error!"), MB_OK);
		cout << logLocation << " does not exist, please create the directory." << endl;
		cout << "winutk --help for more options." << endl;
		return 0;
	}
	//Getting warnings here, maybe try _tcscpy_s
	wchar_t logLoct[1000];
	wcscpy(logLoct, logLocation);
	wcscat(logLoct, logFileName);
	logLocation = logLoct;

	LocalFree(argv);

	int isIdle = 0;

	LASTINPUTINFO lii;
	memset(&lii, 0, sizeof(lii));

	lii.cbSize = sizeof(lii);
	for (;;) {
		::GetLastInputInfo(&lii);

		time_t idlTime, actTime;
		double difTime;

		//long currTicks = System::Environment::TickCount;
		long currTicks = GetTickCount();
		//const DWORD tcount = GetTickCount();
		long lastInputTicks = lii.dwTime;
		long idleTicks = currTicks - lastInputTicks;

		if (idleTicks >= maxIdleTime) {
			if(isIdle == 0) {
				ofstream logFile;
				logFile.open(logLocation, ios::app);
				//logFile.open(L"C:\\windows\\system32\\logfiles\\winutk\\pisd.txt", ios::app);
				time(&idlTime);
				idlTime -= maxIdleTime/1000; //Subtract off the idle time specified to get accurate idle start time.
				logFile << "Idle start: " << ctime(&idlTime);
				logFile.close();
			}
			isIdle = 1;
		} else {
			if(isIdle == 1) {
				ofstream logFile;
				logFile.open(logLocation, ios::app);
				//logFile.open(L"C:\\windows\\system32\\logfiles\\winutk\\pisd.txt", ios::app);
				time(&actTime);
				logFile << "Idle End: " << ctime(&actTime);
				difTime = difftime(actTime, idlTime);
				//difTime = 175964; //48hrs 52mins 44secs
				int time, hour, min, sec;
				time = (int) difTime;
				hour = time/3600;
				time = time%3600;
				min = time/60;
				sec = time%60;
				logFile << "Total Idle Time: " << hour << "::" << min << "::" << sec << "\n\n";
				logFile.close();
			}
			isIdle = 0;
		}
		//Required so the process doesn't take up 100% of the CPU.
		//System::Threading::Thread::Sleep(1000);
		Sleep(1000);
	}

	return 0;
}