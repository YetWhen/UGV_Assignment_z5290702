
#using<System.dll>
#include<Windows.h>          //for high quality time counter in windows system
#include<conio.h>

#include "smstructs.h"
#include "SMObject.h"
#include "GPS.h"


using namespace System;
using namespace System::Diagnostics;
using namespace System::Threading;
int main()
{

	GPS^ GPSModule = gcnew GPS;

	GPSModule->setupSharedMemory();
	GPSModule->connect("192.168.1.200", 24000);
	int PMcounter = 0;


	//main loop, counting time
	while (1)
	{
		if (!GPSModule->getHeartbeat())
		{
			GPSModule->setHeartbeat(1);
			std::cout << "Turn on heartbeat, PMCounter: " << PMcounter << std::endl;
			PMcounter = 0;
		}
		else
		{
			PMcounter++;
			if (PMcounter > 60)
			{
				Console::WriteLine("No response from ProcessManagement, exit");
				break;
			}
		}
		GPSModule->getData();
		if (GPSModule->checkData())
		{
			GPSModule->sendDataToSharedMemory();
		}
		Thread::Sleep(25);
		if (GPSModule->getShutdownFlag())   //emergency shutdown controlled by shared memory
			break;
		if (_kbhit())  //regular shutdown
			break;

	}

	GPSModule->~GPS();

	return 0;
}