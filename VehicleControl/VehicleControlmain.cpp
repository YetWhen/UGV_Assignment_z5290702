
#using<System.dll>
#include<Windows.h>          //for high quality time counter in windows system
#include<conio.h>

#include "smstructs.h"
#include "SMObject.h"
#include "VehicleControl.h"


using namespace System;
using namespace System::Diagnostics;
using namespace System::Threading;
int main()
{

	VC^ VCModule = gcnew VC;

	VCModule->setupSharedMemory();
	VCModule->connect("192.168.1.200", 25000);
	int PMcounter = 0;


	//main loop, counting time
	while (1)
	{
		if (!VCModule->getHeartbeat())
		{
			VCModule->setHeartbeat(1);
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
		VCModule->getData();

		Thread::Sleep(25);
		if (VCModule->getShutdownFlag())   //emergency shutdown controlled by shared memory
			break;
		if (_kbhit())  //regular shutdown
			break;

	}


	return 0;
}