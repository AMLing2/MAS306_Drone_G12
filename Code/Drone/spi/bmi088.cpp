#include <iostream>
#include <pigpio.h>
#include <cstring>
#include <bitset>
#include <thread>
#include <chrono>
#include <bit>

#define LED 18 //GPIO 18, physical pin 12
#define CS_ACCEL 12 //GPIO 12, physical pin 22
#define CS_GYRO 13
#define SPI_CHANNEL 1

#define ACC_PWR_CTRL 0x7D

#define READ_BIT 0x01

uint8_t rev(uint8_t v) //not sure if addresses should be reversed or not
{	
	v = (v & 0xF0) >> 4 | (v & 0x0F) << 4;
	v = (v & 0xCC) >> 2 | (v & 0x33) << 2;
	v = (v & 0xAA) >> 1 | (v & 0x55) << 1;
	return v;
}

int main()
{
	if(gpioInitialise()<0) // must be initialized first
	{
		std::cout<<"initialisation failed";
		return -1;
	}
	const size_t buffersize = 16;
	char txBuffer[buffersize];
	char rxBuffer[buffersize];
	memset(rxBuffer,3,buffersize);//set to known value (0000 0011)
	memset(txBuffer,0,buffersize);//set to known value (0000 0011)
	int spiHandle = spiOpen(SPI_CHANNEL,1000000,(0x0<<15)|(0x0<<14)|(0x10<<16)| (0x1<<8) | (0x1<<5) | (0x1<<6)| (0x1<<7) | (0x0));// using mode 0, disable ux as cs, enable auxilarary spi, make 16 bit word length
	std::cout<<"spiHandle:" <<spiHandle<<std::endl;
	if (spiHandle<0)
	{
		std::cout<<"spi open failed";
		return -1;
	}
	bool running = true;
	int a = 0;

	gpioSetMode(LED, PI_OUTPUT);
	gpioSetMode(CS_ACCEL, PI_OUTPUT);
	gpioSetMode(CS_GYRO, PI_OUTPUT);
	//gpioSetMode(CS_PIN, PI_OUTPUT);
	//------------MAIN----------
	std::this_thread::sleep_for(std::chrono::milliseconds(1));//wait 1ms after startup
	//gpioWrite(CS_PIN,1);//to disable previous IMU
	gpioWrite(CS_ACCEL,1);
	gpioWrite(CS_GYRO,1);
	gpioWrite(LED,0);
	//gpioWrite(INT_PIN,1);
	
	//send dummy byte to wake up device (bad) TODO: consider remove
	//bit 0: R(1)/W(0),bit 1-7 = ADDR, bit 8-15= data if in Write
	
	
	
	/*
	txBuffer[0] = (0x01<<0) | 0x00; //0x00 = ACC_CHIP_ID
	txBuffer[1] = 0xFF; //data SDO bit?
	gpioWrite(CS_ACCEL,0);
	spiXfer(spiHandle,txBuffer,rxBuffer,2);
	std::cout<<"rx: "<<std::bitset<8>(rxBuffer[0])<<" "<<std::bitset<8>(rxBuffer[1])<<std::endl;
	gpioWrite(CS_ACCEL,1);
	*/

	//enter normal mode	
	txBuffer[1] = 0 | (ACC_PWR_CTRL);
	txBuffer[0] = (0x04);//enter normal mode by writing 4
	std::cout<<"bytes sent: "<<std::bitset<8>(txBuffer[0])<<" "<<std::bitset<8>(txBuffer[1])<<std::endl;// " 1: "<<std::bitset<8>(rxBuffer[1])<<std::endl;
	gpioWrite(LED,1);
	gpioWrite(CS_ACCEL,0);
	spiWrite(spiHandle,txBuffer,2);
	gpioWrite(CS_ACCEL,1);
	

	std::this_thread::sleep_for(std::chrono::milliseconds(1));
	//read ACC_PWR_CTRL to confirm
	txBuffer[1] = (1<<7) | (ACC_PWR_CTRL);
	txBuffer[0] = (0xFF);
	std::cout<<"bytes read: "<<std::bitset<8>(txBuffer[0])<<" "<<std::bitset<8>(txBuffer[1])<<std::endl;// " 1: "<<std::bitset<8>(rxBuffer[1])<<std::endl;
	gpioWrite(CS_ACCEL,0);
	spiWrite(spiHandle,txBuffer,2);
	spiRead(spiHandle,rxBuffer,2);
	std::cout<<"ACC_PWR_CTRL: "<<std::bitset<8>(rxBuffer[1])<<std::endl;// " 1: "<<std::bitset<8>(rxBuffer[1])<<std::endl;
	gpioWrite(CS_ACCEL,1);

	//read gyro
	txBuffer[1] = (1<<7) | (0x00);
	txBuffer[0] = (0xFF);
	gpioWrite(CS_GYRO,0);
	spiXfer(spiHandle,txBuffer,rxBuffer,2);
	std::cout<<"Gyro ID:  "<<std::bitset<8>(rxBuffer[0])<<" "<<std::bitset<8>(rxBuffer[1])<<std::endl;//first byte is ONLY ignored on ACCEL, NOT GYRO
	gpioWrite(CS_GYRO,1);

	gpioWrite(CS_ACCEL,0);
	spiWrite(spiHandle,txBuffer,2);
	spiRead(spiHandle,rxBuffer,2);
	std::cout<<"Accel ID: "<<std::bitset<8>(rxBuffer[0])<<" "<<std::bitset<8>(rxBuffer[1])<<std::endl;// " 1: "<<std::bitset<8>(rxBuffer[1])<<std::endl;
	gpioWrite(CS_ACCEL,1);
	gpioWrite(LED,0);
/*
	//read TEMP
	gpioWrite(CS_ACCEL,0);
	txBuffer[0] = (0x01) | (rev(0x23)<<1);
	txBuffer[1] = (0xFF);
	spiWrite(spiHandle,txBuffer,2);
	spiRead(spiHandle,rxBuffer,2);
	uint8_t temp_lsb = rxBuffer[1] & (0xE0);
	txBuffer[0] = (0x01) | (rev(0x22)<<1);
	spiWrite(spiHandle,txBuffer,2);
	spiRead(spiHandle,rxBuffer,2);
	uint8_t temp_msb = rxBuffer[1];
	std::cout<<"acc_temp: "<<((float( 0x0 | (temp_msb<<3) | (temp_lsb >> 5)) / 2048.0f)*0.125f)+23.0f<<" bin: "<<std::bitset<8>(temp_msb)<<" "<< std::bitset<8>(temp_lsb) <<std::endl;// " 1: "<<std::bitset<8>(rxBuffer[1])<<std::endl;	
	gpioWrite(CS_ACCEL,1);
	//from datasheet:
	uint16_t bit11 = (uint16_t(temp_msb) * 8) + uint16_t(temp_lsb)/32;
	int16_t temp = 0;
	if (bit11 > 1023)
	{
		temp = bit11 - 2048;
	}
	else
	{
		temp = bit11;
	}
	float tempf = (float(temp) *0.125f) + 23.0f;
	std::cout<<"datasheet temp: "<<tempf<<" tempint: "<<temp<<std::endl;
*/
	/*
	gpioWrite(CS_PIN,0);
  char asd = '\x00';
	spiWrite(spiHandle, &asd,1);
	gpioWrite(CS_PIN,1);
	*/
	//ACCEL FIFO_CONFIG_1
/*
	gpioWrite(CS_ACCEL,0);
	txBuffer[0] = (0x01) | (rev(0x49)<<1);
	txBuffer[1] = (0xFF);
	spiWrite(spiHandle,txBuffer,2);
	spiRead(spiHandle,rxBuffer,2);
	gpioWrite(CS_ACCEL,1);
	std::cout<<"ACCEL_FIFO_CONFIG_1: "<<std::bitset<8>(rxBuffer[1] & 0b01011100)<<std::endl;

	//GYRO GYRO_BANDWIDTH
	gpioWrite(CS_GYRO,0);
	txBuffer[0] = (0x01) | (rev(0x10)<<1);
	txBuffer[1] = (0xFF);
	spiWrite(spiHandle,txBuffer,2);
	spiRead(spiHandle,rxBuffer,2);
	gpioWrite(CS_GYRO,1);
	std::cout<<"GYRO GYRO_BANDWIDTH: "<<std::bitset<8>(rxBuffer[0] & 0xFF)<<std::endl;

	int i = 0;
	//txBuffer[0] = (0x01<<0) | (0x02<<1);
	//txBuffer[1] = (0xFF);
	std::this_thread::sleep_for(std::chrono::milliseconds(500));
	while(running)
	{
		/*
		gpioWrite(CS_GYRO,0);
		spiWrite(spiHandle,&txBuffer[0],2);
		//std::cout<<"write: "<< spiWrite(spiHandle,txBuffer,1)<<" msg: "<< std::bitset<8>(txBuffer[0]) <<std::endl;
		spiRead(spiHandle,rxBuffer,8);

		//std::cout<<"xfer: "<< spiXfer(spiHandle,txBuffer,rxBuffer,8) <<std::endl;

		gpioWrite(CS_GYRO,1);

		std::cout<<"gyro rx: ";
		for(int i = 0; i < 6; i++)
		{
			std::cout<<static_cast<float>(rxBuffer[i+1])/256.0f<<" "; //ignore [0]
		}
		std::cout<<"\r";

		*/
		//read TEMP
/*
		gpioWrite(CS_ACCEL,0);
		txBuffer[0] = (0x01) | (rev(0x23)<<1); //read lsb
		txBuffer[1] = (0xFF);
		spiWrite(spiHandle,txBuffer,2);
		spiRead(spiHandle,rxBuffer,2);
		temp_lsb = rxBuffer[1] & (0xE0);
		txBuffer[0] = (0x01) | (rev(0x22)<<1);
		spiWrite(spiHandle,txBuffer,2); //read msb
		spiRead(spiHandle,rxBuffer,2);
		temp_msb = rxBuffer[1];
		//std::cout<<"acc_temp: "<<((float( 0x0 | (temp_msb<<3) | (temp_lsb >> 5)) / 2048.0f)*0.125f)+23.0f<<" bin: "<<std::bitset<8>(temp_msb)<<" "<< std::bitset<8>(temp_lsb) <<std::endl;// " 1: "<<std::bitset<8>(rxBuffer[1])<<std::endl;	
		gpioWrite(CS_ACCEL,1);
		//from datasheet:
		bit11 = (uint16_t(temp_msb) * 8) + uint16_t(temp_lsb)/32;
		temp = 0;
		if (bit11 > 1023)
		{
			temp = bit11 - 2048;
		}
		else
		{
			temp = bit11;
		}
		tempf = (float(temp) *0.125f) + 23.0f;
		std::cout<<"datasheet temp: "<<tempf<<" tempint: "<<temp;
		std::cout<<"\r";

		i++;
		std::this_thread::sleep_for(std::chrono::milliseconds(1));
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
*/
	spiClose(spiHandle);
	gpioTerminate(); // call when done with library
	return 0;
}






