#using<System.dll>
#include<Windows.h>          //for high quality time counter in windows system
#include<conio.h>

#include "smstructs.h"
#include "SMObject.h"
using namespace System;
using namespace System::Diagnostics;
using namespace System::Threading;

int main()
{	
	//declaration
	__int64 Frequency, Counter;//the counter in windows is quite fast, set 64bit to prevent overflow
	double GPSTimeStamp;
	//declare shared memory
	SMObject PMObj(_TEXT("ProcessManagement"), sizeof(ProcessManagement));

	//access shared memory
	PMObj.SMAccess();
	ProcessManagement* PMData = (ProcessManagement*)PMObj.pData;
	//get frequency
	//this function's input: pointer to LARGE_INTEGER type, write in frequency, returns a bool
	QueryPerformanceFrequency((LARGE_INTEGER*)&Frequency);

	//main loop, counting time
	while (1)
	{
		//input a pointer/address pointing to Counter, and do the counting
		QueryPerformanceCounter((LARGE_INTEGER*)&Counter);
		//calculate the time, in double form, 1*1000 ms = 1s
		GPSTimeStamp = (double)Counter / (double)Frequency * 1000;
		//Output timestamp, {0,12:F3} for format manuplation
		Console::WriteLine("GPS time stamp :{0,12:F3} {1,12:X2}", GPSTimeStamp, PMData->Heartbeat.Status);
		//------------------------PM
		if (PMData->Heartbeat.Flags.GPS == 0)
		{
			PMData->Heartbeat.Flags.GPS = 1;
			std::cout << "turn up heartbeat: " << PMData->Heartbeat.Status << std::endl;
		}
		else if (PMData->PMTimeStamp > PMData->PMLimit)
			PMData->Shutdown.Status = 0xFF;

		Thread::Sleep(25);
		if (PMData->Shutdown.Flags.GPS)   //emergency shutdown controlled by shared memory
			break;
		//-----------------------------

		if (_kbhit())
			break;

	}
	Console::ReadKey();
	return 0;
}