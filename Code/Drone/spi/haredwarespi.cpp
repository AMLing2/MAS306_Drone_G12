#include <iostream>
#include <pigpio.h>
#include <cstring>
#include <bitset>
#include <thread>
#include <chrono>
#include <hardware/spi.h>

#define LED 18 //GPIO 18, pin 12
#define CS_PIN 24//
#define INT_PIN 22

#define PIN_MISO 21
#define PIN_MOSI 19
#define PIN_SCLK 23

#define SPI_PORT spi0


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
	/*
	int spiHandle = spiOpen(0,1000000,3);
	if (spiHandle<0)
	{
		std::cout<<"spi open failed";
		return -1;
	}
	*/
	bool running = true;
	int a = 0;

	gpioSetMode(INT_PIN, PI_OUTPUT);
	gpioSetMode(CS_PIN, PI_OUTPUT);
	//------------MAIN----------
	gpioWrite(CS_PIN,1);
	//gpioWrite(INT_PIN,1);
	
	//write dummy command
	//gpioWrite(CS_PIN,0);
  //char asd = '\x00';
	//spiWrite(spiHandle, &asd,1);
	//gpioWrite(CS_PIN,1);
	
	txBuffer[0] = 0 | 0x6A;
	memcpy(&txBuffer,&txBuffer,1);
	//disable i2c
	//std::this_thread::sleep_for(std::chrono::milliseconds(11));
	//std::cout<<spiWrite(spiHandle,txBuffer,1)<<std::endl;

	//txBuffer[0] = (1<<7) | 0x3B;
	txBuffer[0] = 0x80 + 0x3B;
	memcpy(&txBuffer,&txBuffer,1);
	while(running)
	{
		rxBuffer[0] = 3;//test to see if worked
		txBuffer[0] = 0x80 + 0x75;
		spi_write_blocking(SPI_PORT,txBuffer,1);
		std::cout<<"tx: "<<std::bitset<8>(txBuffer[0])<<std::endl;
		std::this_thread::sleep_for(std::chrono::milliseconds(10));
		spi_read_blocking(SPI_PORT,txBuffer,rxBuffer,buffersize);
		//spiRead(spiHandle,rxBuffer,1);
		gpioWrite(CS_PIN,1);

		std::cout<<"rx: ";
		for(int i = 0; i < buffersize; i++)
		{
			std::cout<<std::bitset<8>(rxBuffer[i])<<" ";
		}
		std::cout<<std::endl;

		std::cin>>a;
		if (a == 0)
		{
			running = false;
		}
	}

	spiClose(spiHandle);
	gpioTerminate(); // call when done with library
	return 0;
}


int main3()
{
	
	if(gpioInitialise()<0) // must be initialized first
	{
		std::cout<<"initialisation failed";
		return 0;
	}
	gpioSetMode(LED,PI_OUTPUT);
	int cinput;
	bool loop = true;
	bool ledstat = 1;
	while(loop)
	{
		gpioWrite(LED,ledstat);
		std::cout<<"write input:";
		std::cin >> cinput;
		std::cout<<" input: "<<cinput<<std::endl;
		if (cinput == 0)
		{
			loop = false;
			std::cout<<"exiting"<<std::endl;
			gpioWrite(LED,0);
		}
		else
		{
			ledstat = !ledstat;
			std::cout<<"switcing LED" <<std::endl;
		}
	}
	gpioTerminate(); // call when done with library
	return 0;
}




