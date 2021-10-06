
#using<System.dll>
#include<Windows.h>          //for high quality time counter in windows system
#include<conio.h>

#include "smstructs.h"
#include "SMObject.h"
#include "Laser.h"


using namespace System;
using namespace System::Diagnostics;
using namespace System::Threading;
int main()
{

	Laser ^ LaserModule = gcnew Laser;

	LaserModule->setupSharedMemory();

	int PMcounter = 0;


	//main loop, counting time
	while (1)
	{
		if (!LaserModule->getHeartbeat())
		{
			LaserModule->setHeartbeat(1);
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


		Thread::Sleep(25);
		if (LaserModule->getShutdownFlag())   //emergency shutdown controlled by shared memory
			break;
		if (_kbhit())  //regular shutdown
			break;

	}

	LaserModule->~Laser();
	return 0;
}