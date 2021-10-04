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
	int PMCounter = 0;
	//declare shared memory
	SMObject PMObj(_TEXT("ProcessManagement"), sizeof(ProcessManagement));

	//access shared memory
	PMObj.SMAccess();
	ProcessManagement* PMData = (ProcessManagement*)PMObj.pData;


	//main loop, counting time
	while (1)
	{
		//------------------------PM
		if (PMData->Heartbeat.Flags.VehicleControl == 0)
		{

			PMData->Heartbeat.Flags.VehicleControl = 1;
			std::cout << "turn on heartbeat " << (int)PMData->Heartbeat.Flags.VehicleControl << " PMCounter: " << PMCounter << std::endl;
			PMCounter = 0;
		}
		else
		{
			PMCounter++;
			if (PMCounter > 40)
			{
				PMData->Shutdown.Status = 0xFF;
				Console::WriteLine("No response from PM, exit");
			}
		}
		/*if (TSCounter < 100)
			TSValues[TSCounter++] = TimeGap;*/

		Thread::Sleep(25);
		if (PMData->Shutdown.Flags.VehicleControl)   //emergency shutdown controlled by shared memory
			break;
		if (_kbhit())  //regular shutdown
			break;
	}
}