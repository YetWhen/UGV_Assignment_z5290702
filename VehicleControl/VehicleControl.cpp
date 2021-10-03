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
	// -------------------------------------------------------------------------
		//declare Shared memory
	SMObject PMObj(TEXT("ProcessManagement"), sizeof(ProcessManagement));

	PMObj.SMAccess();
	ProcessManagement* PMData = (ProcessManagement*)PMObj.pData;

	
	//--------------------------------------------------
	//Declaration
	double VehicleTimeStamp;
	__int64 Frequency, Counter;//the counter in windows is quite fast, set 64bit to prevent overflow
	int Shutdown = 0x00;
	QueryPerformanceFrequency((LARGE_INTEGER*)&Frequency);//this function's input: pointer to LARGE_INTEGER type, write in frequency, returns a bool

	//main loop, counting time
	while (1)
	{
		QueryPerformanceCounter((LARGE_INTEGER*)&Counter);//input a pointer/address pointing to Counter, and do the counting
		VehicleTimeStamp = (double)Counter / (double)Frequency * 1000; //do calculation in double form, 1*1000 ms = 1s
		Console::WriteLine("Vehicle time stamp :{0,12:F3} {1,12:X2}", VehicleTimeStamp, Shutdown); //{0,12:F3} for format manuplation
		
		//------------------------PM
		if (PMData->Heartbeat.Flags.VehicleControl == 0)
		{
			PMData->Heartbeat.Flags.VehicleControl = 1;
			std::cout << "turn up heartbeat: " << PMData->Heartbeat.Status << std::endl;
		}
		else if (PMData->PMTimeStamp > PMData->PMLimit)
			PMData->Shutdown.Status = 0xFF;

		Thread::Sleep(25);
		if (PMData->Shutdown.Flags.VehicleControl)   //emergency shutdown controlled by shared memory
			break;
		//-----------------------------

		if (_kbhit())
			break;

	}
	Console::ReadKey();
	return 0;
}