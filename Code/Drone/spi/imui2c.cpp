#include <iostream>
#include <bitset>
#include <pigpio.h>

#define LED 18 //GPIO 18, pin 12

//#define SCL 3 //pin 3
//#define SDA 2 //pin 5
const unsigned int addr = 0x68;
const unsigned int bus = 1;
int handle;

void i2cinit()
{
	handle = i2cOpen(bus, addr, 0);
	if(handle<0)
	{
		std::cout<<"opening i2c failed"<<std::endl;
	}
	else
	{
		std::cout<<"handle: " <<handle<<std::endl;
	}
}

unsigned int readRegisterByte(const uint8_t& regAddrH,const uint8_t& regAddrL)
{

	unsigned int H = static_cast<unsigned int>(i2cReadByteData(handle,regAddrH));//reads H byte (11:8)
	H &= 0x000F;//HL = 12 bit value, removes all possible 1s in H before last 4 bits
	unsigned int L = static_cast<unsigned int>(i2cReadByteData(handle,regAddrL));//reads L byte (7:0)
	unsigned int combine = 0 | L | (H<<8);
	return combine;
}

unsigned int readRegisterByte(const uint8_t& regAddr)
{
	return i2cReadByteData(handle,regAddr);//reads a single register in the i2c device
}

int main()
{
	
	if(gpioInitialise()<0) // must be initialized first
	{
		std::cout<<"initialisation failed";
		return 0;
	}

	i2cinit();

	int cinput;
	bool loop = true;
	while(loop)
	{
		std::cout<<"write input:";
		std::cin >> cinput;
		std::cout<<" input: "<<cinput<<std::endl;
		if (cinput == 0)
		{
			loop = false;
			std::cout<<"exiting"<<std::endl;
			gpioWrite(LED,0);
		}
		else if(cinput == 1)
		{
			std::cout<<"reading status register: " <<std::bitset<8>(readRegisterByte(0x75))<<std::endl;
		}
		else if(cinput == 2)
		{
			std::cout<<"reading H and L: "<<readRegisterByte(0x0E,0x0F)<<std::endl;
		}
		else if (cinput == 3)
		{
			std::cout<<"mass reading:"<<std::endl;
			unsigned int reading;
			for(int i = 0; i <= 50000; i++)
			{
				reading = readRegisterByte(0x43,0x44);
				std::cout<<std::bitset<16>(reading);
				std::cout<<" gyro X: " << (static_cast<float>(reading)/4096.0)<<"\r";
				//std::cout<<" rotation: " << (static_cast<float>(reading)/4096.0) * 360.0<<"\r";
			}
			std::cout<<std::endl;

		}
		else
		{
			//std::cout<<"reading i2c: " << readRegisterByte()<<std::endl;
		}
	}

	gpioTerminate(); // call when done with library
	return 0;
}

