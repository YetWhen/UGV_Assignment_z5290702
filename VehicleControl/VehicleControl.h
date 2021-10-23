#include <UGV_module.h>
#include <smstructs.h>

#define CRC32_POLYNOMIAL 0xEDB88320L

unsigned long CRC32Value(int i);
unsigned long CalculateBlockCRC32(unsigned long ulCount, unsigned char* ucBuffer);

ref class VC : public UGV_module
{

public:
	int connect(String^ hostName, int portNumber) override;  // Establish TCP connection

	int setupSharedMemory() override;  // Create and access shared memory objects

	int getData() override;    // Get data from shared memory and send to server

	int checkData() override;  // Check Data is correct (eg headers)

	int sendDataToSharedMemory() override;   // Save Data in shared memory structures

	bool getShutdownFlag() override;         // Get Shutdown signal for module, from Process Management SM

	int setHeartbeat(bool Heartbeat) override;  // Update heartbeat signal for module

	bool getHeartbeat() override;

	~VC();



protected:
	// YOUR CODE HERE (ADDITIONAL MEMBER VARIABLES THAT YOU MAY WANT TO ADD)
	ProcessManagement* PMData;
	SM_VehicleControl* VCData;
	array<unsigned char>^ SendData;
	int flag;
};
