#include"Laser.h"
#include"SMObject.h"
#include<cmath>
int Laser::connect(String^ hostName, int portNumber) 
{

	// Pointer to TcpClent type object on managed heap
	TcpClient^ Client;

	// String command to ask for Channel 1 analogue voltage from the PLC
	// These command are available on Galil RIO47122 command reference manual
	// available online
	String^ AskScan = gcnew String("sRN LMDscandata");

	// Creat TcpClient object and connect to it, Assignment 1 platform address: "Weeder" 192.168.1.200
	Client = gcnew TcpClient(hostName, portNumber);
	// Configure connection
	Client->NoDelay = true;
	Client->ReceiveTimeout = 500;//ms
	Client->SendTimeout = 500;//ms
	Client->ReceiveBufferSize = 1024;
	Client->SendBufferSize = 1024;

	// unsigned char arrays of 16 bytes each are created on managed heap
	SendData = gcnew array<unsigned char>(16);
	ReadData = gcnew array<unsigned char>(2500);


	// Get the network streab object associated with clien so we 
	// can use it to read and write
	Stream = Client->GetStream();
	/*----------------------------------------------------------------*/
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
	/*----------------------------------------------------------------*/

	// Convert string command to an array of unsigned char
	SendData = System::Text::Encoding::ASCII->GetBytes(AskScan);
	return 1;
}
int Laser::setupSharedMemory()
{
	//PM Shared Memory
	ProcessManagementData = new SMObject;
	ProcessManagementData->SetSize(sizeof(ProcessManagement));
	ProcessManagementData->SetSzname(TEXT("ProcessManagement"));
	//ProcessManagementData->SMCreate();
	ProcessManagementData->SMAccess();
	PMData = (ProcessManagement*) ProcessManagementData->pData;
	SensorData = new SMObject;
	SensorData->SetSize(sizeof(SM_Laser));
	SensorData->SetSzname(TEXT("SM_Laser"));
	SensorData->SMCreate();
	SensorData->SMAccess();
	LaserData = (SM_Laser*) SensorData->pData;
	return 1;
}
//receive binary data from server and transform it into string
int Laser::getData()
{
/*----------------------Laser data interaction---------------------------------*/

	// Write command asking for data
	Stream->WriteByte(0x02);
	Stream->Write(SendData, 0, SendData->Length);
	Stream->WriteByte(0x03);
	// Wait for the server to prepare the data, 1 ms would be sufficient, but used 10 ms
	System::Threading::Thread::Sleep(20);
	// Read the incoming data
	Stream->Read(ReadData, 0, ReadData->Length);
	// Convert incoming data from an array of unsigned char bytes to an ASCII string
	ResponseData = System::Text::Encoding::ASCII->GetString(ReadData);

	/*-------------------------end of Laser data interaction-----------------------*/
	return 1;
}
//check if data is in desired format
int Laser::checkData()
{
	array<wchar_t>^ Space = { ' ' };
	array<String^>^ StringArray = ResponseData->Split(Space);
	if (StringArray[1] == "LMDscandata" && (StringArray->Length > (25 + 361)) && (StringArray[25] == "169"))
	{
		try
		{
			double StartAngle = System::Convert::ToInt32(StringArray[23], 16);
		}
		catch (FormatException^)
		{
			Console::WriteLine("Bad String  " + StringArray[23]);
		}
		try
		{
			double Resolution = System::Convert::ToInt32(StringArray[24], 16) / 10000.0;
		}
		catch (FormatException^)
		{
			Console::WriteLine("Bad String  " + StringArray[24]);
		}
		try
		{
			int NumRanges = System::Convert::ToInt32(StringArray[25], 16);
		}
		catch (FormatException^)
		{
			Console::WriteLine("Bad String  " + StringArray[25]);
		}
		return 1;
	}
	else
		return 0;
}
//convert data points to x y coordinates and push into shared memory
int Laser::sendDataToSharedMemory()
{	
	array<wchar_t>^ Space = {' '};
	array<String^>^ StringArray = ResponseData->Split(Space);
	//get basic parameters from data
	double StartAngle = System::Convert::ToInt32(StringArray[23], 16);
	double Resolution = System::Convert::ToInt32(StringArray[24], 16) / 10000.0;
	int NumRanges = System::Convert::ToInt32(StringArray[25], 16);
	array<double>^ Range = gcnew array<double>(NumRanges);
	for (int i = 0; i < NumRanges; i++)
	{
		try
		{
			Console::WriteLine(StringArray[i + 26]);
			Range[i] = System::Convert::ToInt32(StringArray[26 + i], 16);
		}
		catch (FormatException^)
		{
			Console::WriteLine("Bad String  " + StringArray[26+i]);
		}
		catch (ArgumentOutOfRangeException^)
		{
			Console::WriteLine("Index out of range, Bad String element");
			Console::WriteLine("StringArray["+i+"+26]: "+StringArray[26 + i]+"|end of this element|");
			Console::WriteLine("Next element: " + StringArray[27 + i] + "|end of this element|");

			Console::WriteLine("Size of StringArray[i+26]: " + sizeof(StringArray[26 + i]));
			continue;
			//Console::ReadKey();
		}
		//Range[i] = System::Convert::ToInt32(StringArray[26 + i], 16);
		LaserData->x[i] = Range[i] * sin(i * Resolution/360*2*(2*acos(0.0)));
		LaserData->y[i] = Range[i] * cos(i * Resolution / 360 * 2 * (2 * acos(0.0)));
		Console::WriteLine("X-coordinate: {0,6:F2} , Y-Coordinate: {1,6:F2}", LaserData->x[i], LaserData->y[i]);
	}
	return 1;
}
bool Laser::getShutdownFlag()
{
	// YOUR CODE HERE
	if (PMData->Shutdown.Flags.Laser == 1)
		return 1;
	else
		return 0;
}
int Laser::setHeartbeat(bool Heartbeat)
{
	// YOUR CODE HERE
	PMData->Heartbeat.Flags.Laser = Heartbeat;
	return 1;
}
bool Laser::getHeartbeat()
{   
	return PMData->Heartbeat.Flags.Laser;
}
Laser::~Laser()
{
	// YOUR CODE HERE
	Stream->Close();
	Client->Close();
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