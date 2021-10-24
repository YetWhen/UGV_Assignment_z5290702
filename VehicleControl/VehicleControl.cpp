#include"VehicleControl.h"
#include"SMObject.h"
int VC::connect(String^ hostName, int portNumber)
{
	// Pointer to TcpClent type object on managed heap
	TcpClient^ Client;

	// Creat TcpClient object and connect to it, Assignment 1 platform address: "Weeder" 192.168.1.200
	Client = gcnew TcpClient(hostName, portNumber);
	// Configure connection
	Client->NoDelay = true;
	Client->ReceiveTimeout = 500;//ms
	Client->SendTimeout = 500;//ms
	Client->ReceiveBufferSize = 1024;
	Client->SendBufferSize = 1024;
	// Get the network streab object associated with clien so we 
	// can use it to read and write
	Stream = Client->GetStream();
	SendData = gcnew array<unsigned char>(16);
	String^ ResponseData;
	SendData = System::Text::Encoding::ASCII->GetBytes("z5290702\n");
	// Authenticate the user
	Stream->Write(SendData, 0, SendData->Length);
	// Wait for the server to prepare the data, 1 ms would be sufficient, but used 10 ms
	System::Threading::Thread::Sleep(10);
	// Read the incoming data
	Stream->Read(ReadData, 0, ReadData->Length);
	// Convert incoming data from an array of unsigned char bytes to an ASCII string
	ResponseData = System::Text::Encoding::ASCII->GetString(ReadData);
	// Print the received string on the screen
	//Console::WriteLine(ResponseData);
	return 1;
}
int VC::setupSharedMemory()
{
	//PM Shared Memory
	ProcessManagementData = new SMObject;
	ProcessManagementData->SetSize(sizeof(ProcessManagement));
	ProcessManagementData->SetSzname(TEXT("ProcessManagement"));
	//ProcessManagementData->SMCreate();
	ProcessManagementData->SMAccess();
	PMData = (ProcessManagement*)ProcessManagementData->pData;

	SensorData = new SMObject;
	SensorData->SetSize(sizeof(SM_VehicleControl));
	SensorData->SetSzname(TEXT("SM_VehicleControl"));
	SensorData->SMAccess();
	VCData = (SM_VehicleControl*)SensorData->pData;
	return 1;
}
int VC::getData()
{
	//toggle the flag between 1 and 0
	if (flag == 1) {
		flag = 0;
	}
	else {
		flag = 1;
	}

	// Convert string command to an array of unsigned char
	/*sprintf(buffer, "# %f %f %b #", VCData->Steering, VCData->Speed, flag);
	for (int i = 0; i < 16; i++)
	{
		SendData[i] = buffer[i];
	}*/
	String^ buffer = gcnew String("# " + VCData->Steering + " " + VCData->Speed + " " + flag + " #");
	if(VCData->Steering <= 40 && VCData->Steering >= -40 && VCData->Speed<=1 && VCData->Speed>=-1)
	SendData = System::Text::Encoding::ASCII->GetBytes(buffer);
	std::cout << "Steering: " << VCData->Steering << " Speed: " << VCData->Speed << " Flag: " << flag << std::endl;
	Console::WriteLine(SendData);
	Stream->WriteByte(0x02);
	Stream->Write(SendData, 0, SendData->Length);
	Stream->WriteByte(0x03);
	return 1;
}
int VC::checkData()
{
	// YOUR CODE HERE
	return 1;
}
int VC::sendDataToSharedMemory()
{
	// YOUR CODE HERE
	return 1;
}
bool VC::getShutdownFlag()
{
	// YOUR CODE HERE
	if (PMData->Shutdown.Flags.VehicleControl == 1)
		return 1;
	else
		return 0;
}
int VC::setHeartbeat(bool Heartbeat)
{
	// YOUR CODE HERE
	PMData->Heartbeat.Flags.VehicleControl = Heartbeat;
	return 1;
}
bool VC::getHeartbeat()
{
	return PMData->Heartbeat.Flags.VehicleControl;
}
VC::~VC()
{
	Client->Close();
	Stream->Close();
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