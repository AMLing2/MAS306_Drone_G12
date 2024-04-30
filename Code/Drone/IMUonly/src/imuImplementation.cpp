#include "droneIMU.h"
#include <thread>
#include <limits>
#include <iostream>

int IMUDevice::readIMU(char* txBuffer,char* rxBuffer, size_t bufferSize)
{
    //THIS SHOULD BE DONE ONLY ONCE, BIG WASTE OF CPU 
    for(int i = 0; i < bufferSize-1; i++)
    {
        txBuffer[i] = (1<<7) | (0x3B + i);
    }
    txBuffer[bufferSize-1] = '\0';

    gpioWrite(CS_PIN,0);
	spiXfer(spiHandle_,txBuffer,rxBuffer,bufferSize-1);
    gpioWrite(CS_PIN,1);
    return 0; //maybe make void or return Xfer value.
}

//imuBuffer comes directly from rxBuffer from readIMU()
int IMUDevice::combineHL(char* imuBuffer, size_t bufferSize)
{
    if(bufferSize < 14)
    {
        std::cout<<"combine error, incorrect buffer size"<<std::endl;
        return -1;
    }

    for(int i = 0; i < (dataArraySize - 1); i++)
    {
        gyroscope_[i] = (static_cast<float>(0 | (static_cast<int16_t>(imuBuffer[i*2])<<8) | (static_cast<int16_t>(imuBuffer[(i*2) + 1]))) * (PI/180.0f))  / 16.4f;
        accelerometer_[i] = (static_cast<float>(0 | (static_cast<int16_t>(imuBuffer[(i*2) + 8])<<8) | (static_cast<int16_t>(imuBuffer[(i*2) + 9]))) * G_SI) / static_cast<float>(INT16_MAX);
    }


    /*
    if(bufferSize < 14)
    {
        std::cout<<"combine error, incorrect buffer size"<<std::endl;
        return -1;
    }

    accelerometer_.x =  static_cast<float>(0 | (static_cast<uint16_t>(imuBuffer[1])<<8) | (static_cast<uint16_t>(imuBuffer[0]))) / max_size();
    accelerometer_.y =  0 | (static_cast<uint16_t>(imuBuffer[2])<<8) | (static_cast<uint16_t>(imuBuffer[3]));
    accelerometer_.z =  0 | (static_cast<uint16_t>(imuBuffer[4])<<8) | (static_cast<uint16_t>(imuBuffer[5]));

    gyroscope_.x =  0 | (static_cast<uint16_t>(imuBuffer[8])<<8) | (static_cast<uint16_t>(imuBuffer[9]));
    gyroscope_.y =  0 | (static_cast<uint16_t>(imuBuffer[10])<<8) | (static_cast<uint16_t>(imuBuffer[11]));
    gyroscope_.z =  0 | (static_cast<uint16_t>(imuBuffer[12])<<8) | (static_cast<uint16_t>(imuBuffer[13]));
    */

    /*
    if(((outBufferSize * 2) + 9) > inBufferSize )
    {
        std::cout<<"combine error, incorrect buffer sizes"<<std::endl;
        return -1;
    }
    for(int i = 0; i < outBufferSize;i++)
    {
        //pattern: H -> L, H -> L...
        gyroOut[i] = 0 | (static_cast<uint16_t>(imuBuffer[i*2])<<7) | (static_cast<uint16_t>(imuBuffer[(i*2) + 1]));
        accelOut[i] = 0 | (static_cast<uint16_t>(imuBuffer[(i*2) + 8])<<7) | (static_cast<uint16_t>(imuBuffer[(i*2) + 9]));
    }
    */
    return 0;
}


int IMUDevice::initIMU_()
{
    if(gpioInitialise()<0) //pigpio lib must be initialized first
	{
		std::cout<<"initialisation failed";
		return -1;
	}
    memset(rxBuffer_,3,bufferSize_);//set to known value (0000 0011) for debugging
	memset(txBuffer_,0,bufferSize_);//set to known value (0000 0000)
    int spiHandle = spiOpen(0,spiRate_, 0 | (1<<1));// | (1<<1) | (1<<0)); //mode 3 //mode 1 = broken, mode 0,2,3 = somewaht functional?
	if (spiHandle<0)
	{
		std::cout<<"spi open failed";
		return -1;
	}
    gpioSetMode(LED, PI_OUTPUT);
	gpioSetMode(CS_PIN, PI_OUTPUT);

    gpioWrite(CS_PIN,1);//turn to high to disable spi device
	gpioWrite(LED,0);//turn off LED

    //----------PWR_MGMT_1 H_RESET -----------
	txBuffer_[0] = 0 | 0x6B;
	txBuffer_[1] = (0 | (1<<7) | (1<<2)); //less magic numbers plzzz
	spiWrite(spiHandle,txBuffer_,2);
	//---------disable i2c--------
	txBuffer_[0] = 0 | 0x6A;
	txBuffer_[1] = (0 | (1<<4)); //writing is prefaced by 0 in bit 7
	std::this_thread::sleep_for(std::chrono::milliseconds(11)); // wait this time after startup, from datasheet
    spiWrite(spiHandle,txBuffer_,2);

    return spiHandle;
}

int IMUDevice::imuCalibrate_(char* buffer,size_t bufferSize)
{
    //GYRO and ACCEL CONFIG
    buffer[0] =  (0 | 0x1B);
    buffer[1] = 0 | (1<<4) | (1<<3); //set bits 4 and 3 to enable max dps for gyro
    gpioWrite(CS_PIN,0);
    spiWrite(spiHandle_,buffer,2);
    buffer[0] =  (0 | 0x1B);
    buffer[1] = 0 | (1<<3); //set bit 3 to enable +-4g for accel
    spiWrite(spiHandle_,buffer,2);
    gpioWrite(CS_PIN,1);
}

int IMUDevice::calibGyro_(char* buffer,size_t bufferSize)
{

}

int IMUDevice::calibAccel_(char* buffer,size_t bufferSize)
{
    
}

int IMUDevice::regWriteSingle_(uint8_t addr,uint8_t data)
{
    txBuffer_[0] = 0 | addr; //could also do (0x7F & addr) mabe, (0 | addr) does literally nothing
    txBuffer_[1] = 0 | data;

    gpioWrite(CS_PIN,0);
    spiWrite(spiHandle_,txBuffer_,2);
    gpioWrite(CS_PIN,1);
}