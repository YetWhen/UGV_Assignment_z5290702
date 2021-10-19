#include"GPS.h"
#include"SMObject.h"
int GPS::connect(String^ hostName, int portNumber)
{
	// Pointer to TcpClent type object on managed heap
	TcpClient^ Client;

	// Creat TcpClient object and connect to it, Assignment 1 platform address: "Weeder" 192.168.1.200
	Client = gcnew TcpClient(hostName, portNumber);
	// Configure connection
	Client->NoDelay = true;
	Client->ReceiveTimeout = 500;//ms
	Client->SendTimeout = 500;//ms
	Client->ReceiveBufferSize = 224;
	Client->SendBufferSize = 1024;

	ReadData = gcnew array<unsigned char>(224);
	DataFitter = new GPSData;

	// Get the network streab object associated with clien so we 
	// can use it to read and write
	Stream = Client->GetStream();


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

	//GPS Shared Memory
	SensorData = new SMObject;
	SensorData->SetSize(sizeof(SM_GPS));
	SensorData->SetSzname(TEXT("SM_GPS"));
	SensorData->SMAccess();
	SM_GPSData = (SM_GPS*)SensorData->pData;
	return 1;
}
int GPS::getData()
{
	unsigned int Header = 0;
	unsigned char Data;
	unsigned char* BytePtr = nullptr;
	BytePtr = (unsigned char*) DataFitter;
	int i = 0;
	/*----------------------------Get Binary stream-------------------------------*/
	// Wait for the server to prepare the data, 1 ms would be sufficient, but used 10 ms
	System::Threading::Thread::Sleep(1000);
	// Read the incoming data
	Stream->Read(ReadData, 0, ReadData->Length);
	/*----------------------------------------------------------------------------*/
	/*-----------------------------Find Header------------------------------------*/
	do
	{
		Data = ReadData[i++];
		Header = ((Header << 8) | Data);
	} 
	while (Header != 0xaa44121c);
	Start = i - 4;
	/*----------------------------------------------------------------------------*/
	/*-----------------------------Filling Struct---------------------------------*/
	for (i = Start; i < Start + sizeof(GPSData); i++)
	{
		*(BytePtr++) = ReadData[i];
	}
	std::cout << "CRC: " << DataFitter->CRC << std::endl;
	/*----------------------------------------------------------------------------*/
	return 1;
}
int GPS::checkData()
{
	unsigned char Buffer[sizeof(GPSData)];
	for (int i = Start; i < Start + sizeof(GPSData); i++)
	{
		Buffer[i] = ReadData[i];
	}
	int CalculatedCRC = CalculateBlockCRC32(108, Buffer);
	std::cout << "CalculatedCRC: " << CalculatedCRC << std::endl;
	return (CalculatedCRC == DataFitter->CRC);
}
int GPS::sendDataToSharedMemory()
{
	SM_GPSData->easting = DataFitter->Easting;
	SM_GPSData->northing = DataFitter->Northing;
	SM_GPSData->height = DataFitter->Height;
	std::cout << "Header: " << std::hex << DataFitter->Header << std::endl;
	std::cout << "Northing: " << DataFitter->Northing << std::endl;
	std::cout << "Easting: " << DataFitter->Easting << std::endl;
	std::cout << "Height: " << DataFitter->Height << std::endl<< std::endl;

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
	delete DataFitter;

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
