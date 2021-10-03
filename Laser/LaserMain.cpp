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
	//Declaration
	SMObject PMObj(TEXT("ProcessManagement"), sizeof(ProcessManagement));

	//timegap for this module tested is arround 31ms, 100ms limit would fit
	/*array<double>^ TSValues = gcnew array<double>(100);
	int TSCounter = 0;  //used as TSValues[TSCounter]
	double TimeGap*/


	double LaserTimeStamp;
	__int64 Frequency, Counter, OldCounter;//the counter in windows is quite fast, set 64bit to prevent overflow
	QueryPerformanceFrequency((LARGE_INTEGER*)&Frequency);//this function's input: pointer to LARGE_INTEGER type, write in frequency, returns a bool
	QueryPerformanceCounter((LARGE_INTEGER*)&OldCounter);
	/*PMObj.SMCreate(); */
	PMObj.SMAccess();
	ProcessManagement* PMData = (ProcessManagement*)PMObj.pData;

	//main loop, counting time
	while (1)
	{
		QueryPerformanceCounter((LARGE_INTEGER*)&Counter);//input a pointer/address pointing to Counter, and do the counting

		//TimeGap = (double)(Counter - OldCounter) / (double)Frequency * 1000; 
		//OldCounter = Counter;
		LaserTimeStamp = (double)Counter / (double)Frequency * 1000;//do calculation in double form (in sec), *1000 to read ms.
		Console::WriteLine("Laser time stamp :{0,12:F3} {1,12:X2}", LaserTimeStamp, PMData->Heartbeat.Status); //{0,12:F3} for format manuplation

		if (PMData->Heartbeat.Flags.Laser == 0)
		{
			PMData->Heartbeat.Flags.Laser = 1;
		}
		else if (PMData->PMTimeStamp > PMData->PMLimit)
			PMData->Shutdown.Status = 0xFF;
		/*if (TSCounter < 100)
			TSValues[TSCounter++] = TimeGap;*/

		Thread::Sleep(25);
		if (PMData->Shutdown.Flags.Laser)   //emergency shutdown controlled by shared memory
			break;
		if (_kbhit())  //regular shutdown
			break;

	}
/*	for (int i = 0; i < 100; i++)
		Console::WriteLine("{0,12:F3}", TSValues[i]);
		*/
	getch();
	getch();

	return 0;
}