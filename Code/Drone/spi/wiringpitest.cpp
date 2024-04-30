#include <iostream>
#include <wiringPi.h>
#include <wiringPiSPI.h>
#include <cstring>
#include <bitset>
#include <thread>
#include <chrono>

#define SPI_CHAN 0
#define LED 1 //wiringpi 1, pin 12
#define CS_PIN 6//wiring pi 6, pin 22

int main()
{
	int myFd;
	wiringPiSetup();
	if(myFd = wiringPiSPISetup(SPI_CHAN, 1000000)<0) // must be initialized first
	{
		std::cout<<"initialisation failed";
		return -1;
	}
	const size_t buffersize = 8;
	unsigned char txBuffer[buffersize];
	//char txBuffer[buffersize];
	//memset(txBuffer,3,buffersize);//set to known value (0000 0011)
	memset(txBuffer,0,buffersize);//set to known value (0000 0011)

	bool running = true;
	int a = 0;

	pinMode(LED, OUTPUT);
	pinMode(CS_PIN, OUTPUT);
	//------------MAIN----------
	digitalWrite(CS_PIN,1);
	digitalWrite(LED,0);

	//----------PWR_MGMT_1 H_RESET -----------
	txBuffer[0] = 0 | 0x6B;
	txBuffer[1] = (0 | (1<<7) | (1<<2));
	if(wiringPiSPIDataRW(SPI_CHAN,txBuffer,2) == -1)
	{
		std::cout<<"failed to write to spi"<<std::endl;
		return -1;
	}
	//---------disable i2c--------
	txBuffer[0] = 0 | 0x6A;
	txBuffer[1] = (0 | (1<<4));
	std::this_thread::sleep_for(std::chrono::milliseconds(11));
	std::cout<<wiringPiSPIDataRW(SPI_CHAN,txBuffer,2)<<std::endl;

	while(running)
	{
		//test: reading WHO_AM_I register
		
		
		for(int i = 0; i < buffersize; i++)
		{
			txBuffer[i] = (1<<7) | 0x71;
		}
		txBuffer[1] = ~0;

		std::cout<<"tx: ";
		for(int i = 0; i < buffersize; i++)
		{
			std::cout<<std::bitset<8>(txBuffer[i])<<" ";
		}
		std::cout<<std::endl;

		digitalWrite(CS_PIN,0);

		//std::cout<<"write: "<< spiWrite(spiHandle,txBuffer,1)<<" msg: "<< std::bitset<8>(txBuffer[0]) <<std::endl;
		//spiRead(spiHandle,txBuffer,buffersize);

		std::cout<<"xfer: "<< wiringPiSPIDataRW(SPI_CHAN,txBuffer,8) <<std::endl;

		digitalWrite(CS_PIN,1);

		std::cout<<"rx: ";
		for(int i = 0; i < buffersize; i++)
		{
			std::cout<<std::bitset<8>(txBuffer[i])<<" ";
		}
		std::cout<<std::endl;

		std::cin>>a;
		if (a == 0)
		{
			running = false;
		}
	}

	return 0;
}






