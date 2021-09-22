#using<System.dll>
#include<Windows.h>          //for high quality time counter in windows system
#include<conio.h>

using namespace System;
using namespace System::Diagnostics;
using namespace System::Threading;

int main()
{
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
		Console::WriteLine("Vehicle time stamp :{0,12:F3} {1,12:X3}", VehicleTimeStamp, Shutdown); //{0,12:F3} for format manuplation
		Thread::Sleep(25);
		if (Shutdown)
			break;
		if (_kbhit())
			break;

	}
	Console::ReadKey();
	return 0;
}