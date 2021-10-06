#include"GPS.h"
#include"SMObject.h"
int GPS::connect(String^ hostName, int portNumber)
{
	// YOUR CODE HERE
	return 1;
}
int GPS::setupSharedMemory()
{
	//PM Shared Memory
	ProcessManagementData = new SMObject;
	ProcessManagementData->SetSize(sizeof(ProcessManagement));
	ProcessManagementData->SetSzname(TEXT("ProcessManagement"));
	//ProcessManagementData->SMCreate();
	ProcessManagementData->SMAccess();
	PMData = (ProcessManagement*)ProcessManagementData->pData;
	return 1;
}
int GPS::getData()
{
	// YOUR CODE HERE
	return 1;
}
int GPS::checkData()
{
	// YOUR CODE HERE
	return 1;
}
int GPS::sendDataToSharedMemory()
{
	// YOUR CODE HERE
	return 1;
}
bool GPS::getShutdownFlag()
{
	// YOUR CODE HERE
	if (PMData->Shutdown.Flags.GPS == 1)
		return 1;
	else
		return 0;
}
int GPS::setHeartbeat(bool Heartbeat)
{
	// YOUR CODE HERE
	PMData->Heartbeat.Flags.GPS = Heartbeat;
	return 1;
}
bool GPS::getHeartbeat()
{
	return PMData->Heartbeat.Flags.GPS;
}
GPS::~GPS()
{
	// YOUR CODE HERE
	delete ProcessManagementData;
	delete SensorData;

}

unsigned long CRC32Value(int i)
{
	int j;
	unsigned long ulCRC;
	ulCRC = i;
	for (j = 8; j > 0; j--)
	{
		if (ulCRC & 1)
			ulCRC = (ulCRC >> 1) ^ CRC32_POLYNOMIAL;
		else
			ulCRC >>= 1;
	}
	return ulCRC;
}

unsigned long CalculateBlockCRC32(unsigned long ulCount, /* Number of bytes in the data block */
	unsigned char* ucBuffer) /* Data block */
{
	unsigned long ulTemp1;
	unsigned long ulTemp2;
	unsigned long ulCRC = 0;
	while (ulCount-- != 0)
	{
		ulTemp1 = (ulCRC >> 8) & 0x00FFFFFFL;
		ulTemp2 = CRC32Value(((int)ulCRC ^ *ucBuffer++) & 0xff);
		ulCRC = ulTemp1 ^ ulTemp2;
	}
	return(ulCRC);
}