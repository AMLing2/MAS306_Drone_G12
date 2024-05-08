#include "droneIMU.h"
#include <pigpio.h>
#include <iostream>
#include <cmath>

void gpioEnd(int spiHandle)
{
    spiClose(spiHandle);
	gpioTerminate(); // call when done with library
}

int spiInit()
{
    if(gpioInitialise()<0) // must be initialized first
	{
		std::cout<<"initialisation failed";
		return -1;
	}
    int spiHandle = spiOpen(SPI_CHANNEL,SPI_SPEED,SPI_WORDLEN_16 | SPI_AUXILIARY | SPI_DISABLE_UX | SPI_MODE_0);// using mode 0, disable ux as cs, enable auxilarary spi, make 16 bit word length
    //int spiHandle = spiOpen(SPI_CHANNEL,SPI_SPEED,SPI_AUXILIARY | SPI_DISABLE_UX | SPI_MODE_0);// using mode 0, disable ux as cs, enable auxilarary spi, make 16 bit word length
    if (spiHandle<0)
	{
		std::cout<<"spi open failed";
		return -2;
	}
    gpioSetMode(LED, PI_OUTPUT);
	gpioSetMode(CS_ACCEL, PI_OUTPUT);
	gpioSetMode(CS_GYRO, PI_OUTPUT);

    gpioWrite(CS_ACCEL,1);
	gpioWrite(CS_GYRO,1);
	gpioWrite(LED,0); //led for any debugging, TODO: should be removed later though

    std::this_thread::sleep_for(std::chrono::milliseconds(1));//wait 1ms after startup
    return spiHandle;
}

ns_t IIMU::monoTimeNow_()
{
    return std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::steady_clock::now().time_since_epoch());
}

void GyroIMU::readData()
{
    txBuffer_[0] = 0x00;
    txBuffer_[1] = READ_BIT | RATE_X_LSB;

    gpioWrite(CS_GYRO,0);
	spiXfer(spiHandle_,txBuffer_,&rxBuffer_[0],2);
    spiRead(spiHandle_,&rxBuffer_[1],6); //read next 5 registers after xfer reads 1st, xfer order is reversed for some reason
	gpioWrite(CS_GYRO,1);
    sensorVals_.t = monoTimeNow_();
    /*
    std::cout<<"gyro reading: ";
    for (int i = 0; i < 6;i++)
    {
        std::cout<<std::bitset<8>(rxBuffer[i])<<", ";
    }
    std::cout<<std::endl;
    */
    //                                LOW                                               HIGH
    sensorVals_.xi = (static_cast<int16_t>(rxBuffer_[0]) & 0x00FF ) | (static_cast<int16_t>(rxBuffer_[2])<<8);
    sensorVals_.yi = (static_cast<int16_t>(rxBuffer_[1]) & 0x00FF ) | (static_cast<int16_t>(rxBuffer_[4])<<8);
    sensorVals_.zi = (static_cast<int16_t>(rxBuffer_[3]) & 0x00FF ) | (static_cast<int16_t>(rxBuffer_[6])<<8); //awful, switch to 8 bit word length plzzzzzz

    sensorVals_.xf = (static_cast<float>(sensorVals_.xi)/32767.0f * range_) - offsetVals_.xf; //from datasheet
    sensorVals_.yf = (static_cast<float>(sensorVals_.yi)/32767.0f * range_) - offsetVals_.yf;
    sensorVals_.zf = (static_cast<float>(sensorVals_.zi)/32767.0f * range_) - offsetVals_.zf;
}

int GyroIMU::sensorInit_(int spiHandle, char* buffer)
{
    //enter normal mode
    buffer[1] = 0 | (GYRO_RANGE);
	buffer[0] = (0x00);//write full scale, +-2000 deg/s
    range_ = 2000.0f;
	//std::cout<<"bytes sent: "<<std::bitset<8>(buffer[0])<<" "<<std::bitset<8>(buffer[1])<<std::endl;// " 1: "<<std::bitset<8>(rxBuffer[1])<<std::endl;
	gpioWrite(CS_GYRO,0);
	spiWrite(spiHandle,buffer,2);
	gpioWrite(CS_GYRO,1);

    //set up low pass filter
    buffer[1] = 0 | (GYRO_BANDWIDTH);
	buffer[0] = (0x07);//ODR: 100, BW: 32
    //buffer[0] = (0x07);//ODR: 100, BW: 32
	gpioWrite(CS_GYRO,0);
	spiWrite(spiHandle,buffer,2);
	gpioWrite(CS_GYRO,1);
    return 0;
}

void GyroIMU::calib_()
{
    const size_t calibSize = 1000;
    double calibX,calibY,calibZ = 0;

    for(size_t i = 0; i < calibSize;i++)
    {
        readData();
        calibX += sensorVals_.xf;
        calibY += sensorVals_.yf;
        calibZ += sensorVals_.zf;
    }
    offsetVals_.xf = static_cast<float>(calibX / static_cast<double>(calibSize));
    offsetVals_.yf = static_cast<float>(calibY / static_cast<double>(calibSize));
    offsetVals_.zf = static_cast<float>(calibZ / static_cast<double>(calibSize));
}

int GyroIMU::writeSingleReg()
{
    return 0;
}

void GyroIMU::populateProtobuf(dronePosVec::dronePosition& dp)
{
    dp.clear_rotmatrixdot();
    dp.add_rotmatrixdot(sensorVals_.xf);
    dp.add_rotmatrixdot(sensorVals_.yf);
    dp.add_rotmatrixdot(sensorVals_.zf);
}

//ACCEL---------------------------------------------------------------------------------------------------------------

void AccelIMU::readData()
{
    txBuffer_[0] = 0xFF;
    txBuffer_[1] = READ_BIT | ACC_X_LSB;

    gpioWrite(CS_ACCEL,0);
	spiWrite(spiHandle_,txBuffer_,2);
	spiRead(spiHandle_,rxBuffer_,6); //1st is ignored in 2nd sent byte of write
	gpioWrite(CS_ACCEL,1);
    sensorVals_.t = monoTimeNow_();
    /*
    std::cout<<"accel reading: ";
    for (int i = 0; i < 6;i++)
    {
        std::cout<<std::bitset<8>(rxBuffer[i])<<", ";
    }
    std::cout<<std::endl;
    */
    //                                LOW                                               HIGH
    sensorVals_.xi = (static_cast<int16_t>(rxBuffer_[1]) & 0x00FF ) | (static_cast<int16_t>(rxBuffer_[0])<<8);
    sensorVals_.yi = (static_cast<int16_t>(rxBuffer_[3]) & 0x00FF ) | (static_cast<int16_t>(rxBuffer_[2])<<8);
    sensorVals_.zi = (static_cast<int16_t>(rxBuffer_[5]) & 0x00FF ) | (static_cast<int16_t>(rxBuffer_[4])<<8);

    sensorVals_.xf = (static_cast<float>(sensorVals_.xi)/32768.0f * 1000.0f * std::pow(2.0,static_cast<float>(range_+1)) * 1.5) - offsetVals_.xf; //from datasheet
    sensorVals_.yf = (static_cast<float>(sensorVals_.yi)/32768.0f * 1000.0f * std::pow(2.0,static_cast<float>(range_+1)) * 1.5) - offsetVals_.yf;
    sensorVals_.zf = (static_cast<float>(sensorVals_.zi)/32768.0f * 1000.0f * std::pow(2.0,static_cast<float>(range_+1)) * 1.5) - offsetVals_.zf;
}

int AccelIMU::sensorInit_(int spiHandle, char* buffer)
{
    //enter normal mode
    buffer[1] = 0 | (ACC_PWR_CTRL);
	buffer[0] = (0x04);//enter normal mode by writing 4
	//std::cout<<"bytes sent: "<<std::bitset<8>(buffer[0])<<" "<<std::bitset<8>(buffer[1])<<std::endl;// " 1: "<<std::bitset<8>(rxBuffer[1])<<std::endl;
	gpioWrite(CS_ACCEL,0);
	spiWrite(spiHandle,buffer,2);
	gpioWrite(CS_ACCEL,1);

    std::this_thread::sleep_for(std::chrono::microseconds(450));//specified in datasheet

    //write to accelerometer range register
    buffer[1] = 0 | (ACC_RANGE);
	buffer[0] = (0x03);//+- 12g
    range_ = static_cast<float>(buffer[0]);
	gpioWrite(CS_ACCEL,0);
	spiWrite(spiHandle,buffer,2);
	gpioWrite(CS_ACCEL,1);

    //set up low pass filter
    buffer[1] = 0 | (ACC_CONF);
	buffer[0] = (0x05 | (0x08<<4));//12.5
	gpioWrite(CS_ACCEL,0);
	spiWrite(spiHandle,buffer,2);
	gpioWrite(CS_ACCEL,1);
    return 0;
}

void AccelIMU::calib_()
{
    const size_t calibSize = 1000;
    double calibX = 0;
    double calibY = 0;
    double calibZ = 0;

    for(size_t i = 0; i < calibSize;i++)
    {
        readData();
        calibX += sensorVals_.xf;
        calibY += sensorVals_.yf;
        calibZ += sensorVals_.zf;
    }
    offsetVals_.xf = static_cast<float>(calibX / static_cast<double>(calibSize));
    offsetVals_.yf = static_cast<float>(calibY / static_cast<double>(calibSize));
    offsetVals_.zf = static_cast<float>(calibZ / static_cast<double>(calibSize));
}

int AccelIMU::writeSingleReg()
{
    return 0;
}

void AccelIMU::populateProtobuf(dronePosVec::dronePosition& dp)
{
    dp.clear_positiondot();
    //dp.mutable_positiondot()->Add(sensorVals_.xf); //alt method if needed, add_xxx should work fine however
    dp.add_positiondot(sensorVals_.xf);
    dp.add_positiondot(sensorVals_.yf);
    dp.add_positiondot(sensorVals_.zf);
}