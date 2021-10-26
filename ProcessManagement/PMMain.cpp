#using <System.dll>
#include <Windows.h>
#include <tchar.h>
#include <TlHelp32.h>
#include <stdio.h>
#include <iostream>
#include <conio.h>

#include "smstructs.h"
#include "SMObject.h"

using namespace System;
using namespace System::Net::Sockets;
using namespace System::Net;
using namespace System::Text;
using namespace System::Threading;

#define NUM_UNITS  5                          //how many modules you set up

bool IsProcessRunning(const char* processName);
void StartProcesses();
void Restart1Process(int i);

//defining start up sequence
TCHAR Units[10][20] =                           //array of .exe file names for main loop to find
{
	TEXT("Laser2.exe"),
	TEXT("Display.exe"),
	TEXT("VehicleControl.exe"),
	TEXT("GPS_z5290702.exe"),
	TEXT("Camera.exe")
};
struct GPS                                   //latitude and longitude, SM_GPS has nothing easting height
{
	double Lat;
	double Long;
	char Header[4];
};
int main()
{
	//declare shared memory
	SMObject PMObj(_TEXT("ProcessManagement"), sizeof(ProcessManagement));
	SMObject* PMPtr = new SMObject;
	*PMPtr = PMObj;
	/*-------------------------------------------------------------------*/
	//Teleoperation
	// 
	//create an array of module names and a critical list in its corresponding order.
	//array<String^>^ ModuleList = gcnew array<String^>{"Laser", "Display", "VechicleControl", "GPS", "Camera"};
	array<int>^ Critical = gcnew array<int>(NUM_UNITS) { 1, 1, 1, 0, 1 }; 
	array<long int>^ TimeLimit = gcnew array<long int>(NUM_UNITS) { 6, 10, 5, 20, 10 };

	unsigned char shutdown = 0;


	/*-------------------------------------------------------------------*/

	PMPtr->SMCreate();
	PMPtr->SMAccess();

	ProcessManagement* PMData = new ProcessManagement;
	PMData = (ProcessManagement*)PMPtr->pData;
	PMData->Shutdown.Status = 0x00;
	//start all 5 modules, replacing the lecture Process handle operation codes
	StartProcesses();
	while (!_kbhit())
	{
		//check for heartbeats
			//iterate through all processes
			//if true, turn the bit of process[i] off
			//if false, increase heartbeat lost counter (life counter)
				//check if counter passes limit for process[i]                 have a limit array corresponding to i th process
				//if passed, is process critical                               have a critical array to compare
					//true, shutdown all                                       shutdown=0xFF
					//false, has process exited?                               use modulelist to check
						//true, start()                                        use modulelist and Process
						//false, kill() then start().
				//if didn't pass limit, increase the counter for process[i]

		for (int i = 0; i < NUM_UNITS; i++)
		{
			//PMData->Heartbeat.Status & (1<<i)     
			//by shifting 1 bit by bit (00000001, 00000010,...) use AND & operation to compare with Status byte bit by bit.
			//eg. status & 00000001 = first(LSB, 0th) bit of status
			if (PMData->Heartbeat.Status & (1 << i))
			{
				std::cout << 1 << " ";
				PMData->Heartbeat.Status = PMData->Heartbeat.Status & (~(1 << i));  //turn i th bit to 0, turning off heartbeat
				PMData->LifeCounter[i] = 0;


			}
			else
			{
				PMData->LifeCounter[i]++;
				std::cout << 0 << " ";
				if (PMData->LifeCounter[i] > TimeLimit[i])
				{
					if (Critical[i])
					{
						PMData->Shutdown.Status = 0xFF;
						std::cout << "Critical process " << Units[i] << " failed, exit." << PMData->LifeCounter[i] << " " << (int)PMData->Heartbeat.Flags.Display << std::endl;
						shutdown = 1;
						getch();
						break;
					}
					else if (IsProcessRunning(Units[i]))
					{
						PMData->Shutdown.Status = PMData->Heartbeat.Status | (1 << i); //turn i th bit to 1
						Restart1Process(i);
					}
					else
					{
						Restart1Process(i);
					}
				}
			}
			

		}
		std::cout << std::endl;
		Thread::Sleep(125);
		if (shutdown) {
			break;
		}

	}

	//routine shutdown

	PMData->Shutdown.Status = 0xFF;
	for (int i = 0; i < NUM_UNITS; i++)
	{
		while (IsProcessRunning(Units[i]))
		{
			Thread::Sleep(25);
		}
	}

	delete PMPtr;
	return 0;
}


//Is process running function
bool IsProcessRunning(const char* processName)
{
	bool exists = false;
	PROCESSENTRY32 entry;
	entry.dwSize = sizeof(PROCESSENTRY32);

	HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);

	if (Process32First(snapshot, &entry))
		while (Process32Next(snapshot, &entry))
			if (!_stricmp((const char*)entry.szExeFile, processName))
				exists = true;

	CloseHandle(snapshot);
	return exists;
}


void StartProcesses()
{
	STARTUPINFO s[10];
	PROCESS_INFORMATION p[10];

	for (int i = 0; i < NUM_UNITS; i++)               //loop NUM_UNIT times to check if correct number of modules are running
	{
		if (!IsProcessRunning((const char*)Units[i]))
		{
			ZeroMemory(&s[i], sizeof(s[i]));
			s[i].cb = sizeof(s[i]);
			ZeroMemory(&p[i], sizeof(p[i]));

			if (!CreateProcess(NULL, Units[i], NULL, NULL, FALSE, CREATE_NEW_CONSOLE, NULL, NULL, &s[i], &p[i]))
			{
				printf("%s failed (%d).\n", Units[i], GetLastError());
				_getch();
			}
			std::cout << "Started: " << Units[i] << std::endl;
			Sleep(100);
		}
	}
}

//single process opening function, 1-D version of StartProcesses
void Restart1Process(int i)
{
	// additional information
	STARTUPINFO si;
	PROCESS_INFORMATION pi;

	ZeroMemory(&si, sizeof(si));
	si.cb = sizeof(si);
	ZeroMemory(&pi, sizeof(pi));

	// start the program up
	if (!CreateProcess(NULL,   // the path
		Units[i],        // Command line
		NULL,           // Process handle not inheritable
		NULL,           // Thread handle not inheritable
		FALSE,          // Set handle inheritance to FALSE
		CREATE_NEW_CONSOLE,
		NULL,           // Use parent's environment block
		NULL,           // Use parent's starting directory 
		&si,            // Pointer to STARTUPINFO structure
		&pi             // Pointer to PROCESS_INFORMATION structure (removed extra parentheses)
	)) {
		printf("%s failed (%d).\n", Units[i], GetLastError());
		_getch();
	}
	std::cout << "Restarted: " << Units[i] << std::endl;   //this function is only used in restarting process
	Sleep(250);
	/*	// Close process and thread handles.
		CloseHandle(pi.hProcess);
		CloseHandle(pi.hThread); */
}