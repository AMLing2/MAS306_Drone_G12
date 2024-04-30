#include <iostream>
#include <pigpio.h>
#include <cstring>
#include <bitset>
#include <thread>
#include <chrono>


#define LED 18 //GPIO 18, physical pin 12
#define CS_PIN 25 //GPIO 25, physical pin 22

int main()
{
	if(gpioInitialise()<0) // must be initialized first
	{
		std::cout<<"initialisation failed";
		return -1;
	}
	const size_t buffersize = 8;
	char txBuffer[buffersize];
	char rxBuffer[buffersize];
	memset(rxBuffer,3,buffersize);//set to known value (0000 0011)
	memset(txBuffer,0,buffersize);//set to known value (0000 0011)
	int spiHandle = spiOpen(0,1000000, 0 | (1<<1));// | (1<<1) | (1<<0)); //mode 3 //mode 1 = broken, mode 0,2,3 = somewaht functional
	if (spiHandle<0)
	{
		std::cout<<"spi open failed";
		return -1;
	}
	bool running = true;
	int a = 0;

	gpioSetMode(LED, PI_OUTPUT);
	gpioSetMode(CS_PIN, PI_OUTPUT);
	//------------MAIN----------
	gpioWrite(CS_PIN,1);
	gpioWrite(LED,0);
	//gpioWrite(INT_PIN,1);
	
	//write dummy command
	/*
	gpioWrite(CS_PIN,0);
  char asd = '\x00';
	spiWrite(spiHandle, &asd,1);
	gpioWrite(CS_PIN,1);
	*/

	//----------PWR_MGMT_1 H_RESET -----------
	txBuffer[0] = 0 | 0x6B;
	txBuffer[1] = (0 | (1<<7) | (1<<2));
	std::this_thread::sleep_for(std::chrono::milliseconds(11));
	spiWrite(spiHandle,txBuffer,2);
	//---------disable i2c--------
	txBuffer[0] = 0 | 0x6A;
	txBuffer[1] = (0 | (1<<4));
	std::this_thread::sleep_for(std::chrono::milliseconds(11));
	std::cout<<spiWrite(spiHandle,txBuffer,2)<<std::endl;
	int i = 0;
	while(running)
	{
		/*
		gpioWrite(CS_PIN, 0);
		spiWrite(spiHandle, txBuffer, 1);  // Send command to read from accelerometer starting at register 0x3B
		char data[6];
		spiRead(spiHandle, data, 6);  // Read 6 bytes (x,y,z values for each axis)
		gpioWrite(CS_PIN, 1);

		short x = (data[0] << 8) | data[1];
		short y = (data[2] << 8) | data[3];
		short z = (data[4] << 8) | data[5];

		// Print out the data
		std::cout << "Accelerometer Data: x=" <<std::bitset<16>(x) << ", y=" <<std::bitset<16>(y) << ", z=" <<std::bitset<16>(z) << std::endl;
*/
		//spiXfer(spiHandle,txBuffer,rxBuffer,1);
		//spiRead(spiHandle,&buffer[0],8);
		//buffer[8] = '\0';
		//std::cout<<std::bitset<8>(rxBuffer[0])<<std::endl;
		
		
		//third test: reading WHO_AM_I register
		
		
		for(int i = 0; i < buffersize; i++)
		{
			txBuffer[i] = (1<<7) | 0x3B;
		}
		//txBuffer[1] = ~0;
/*
		std::cout<<"tx: ";
		for(int i = 0; i < buffersize; i++)
		{
			std::cout<<std::bitset<8>(txBuffer[i])<<" ";
		}
		std::cout<<std::endl;
*/
		gpioWrite(CS_PIN,0);
		spiWrite(spiHandle,txBuffer,1);
		//std::cout<<"write: "<< spiWrite(spiHandle,txBuffer,1)<<" msg: "<< std::bitset<8>(txBuffer[0]) <<std::endl;
		spiRead(spiHandle,rxBuffer,buffersize);

		//std::cout<<"xfer: "<< spiXfer(spiHandle,txBuffer,rxBuffer,8) <<std::endl;

		gpioWrite(CS_PIN,1);

		std::cout<<"rx: ";
		for(int i = 0; i < buffersize; i++)
		{
			std::cout<<static_cast<float>(rxBuffer[i])/256.0f<<" ";
		}
		std::cout<<"\r";

		i++;
		std::this_thread::sleep_for(std::chrono::milliseconds(10));
		if (i > 5000)
		{
			std::cout<<std::endl;
			i = 0;
			std::cin>>a;
			if (a == 0)
			{
				running = false;
			}
		}
		
	}

	spiClose(spiHandle);
	gpioTerminate(); // call when done with library
	return 0;
}






