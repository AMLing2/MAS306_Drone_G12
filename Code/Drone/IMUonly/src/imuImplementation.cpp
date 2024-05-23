#include "droneIMU.h"
#include <pigpio.h>
#include <iostream>
#include <cmath>
#include <bitset>

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
    int spiHandle = spiOpen(SPI_CHANNEL,SPI_SPEED, SPI_DISABLE_UX | SPI_MODE_0);// using mode 0, disable ux as cs, enable auxilarary spi, make 16 bit word length
    //int spiHandle = spiOpen(SPI_CHANNEL,SPI_SPEED,SPI_AUXILIARY | SPI_DISABLE_UX | SPI_MODE_0);// using mode 0, disable ux as cs, enable auxilarary spi, make 16 bit word length
    if (spiHandle<0)
	{
		std::cout<<"spi open failed";
		return -2;
	}
    //gpioSetMode(LED, PI_OUTPUT);
	gpioSetMode(CS_ACCEL, PI_OUTPUT);
	gpioSetMode(CS_GYRO, PI_OUTPUT);

    gpioWrite(CS_ACCEL,1);
	gpioWrite(CS_GYRO,1);
	//gpioWrite(LED,0); //led for any debugging, TODO: should be removed later though

    std::this_thread::sleep_for(std::chrono::milliseconds(1));//wait 1ms after startup
    

    return spiHandle;
}

void IIMU::numIntergration(outputVals& valsPrev, outputVals& valsOut)
{
    double dt = static_cast<double>((sensorVals.t - valsPrev.t).count()) / 1000000000.0f;

    valsOut.xf += (sensorVals.xf - valsPrev.xf) * dt;
    valsOut.yf += (sensorVals.yf - valsPrev.yf) * dt;
    valsOut.zf += (sensorVals.zf - valsPrev.zf) * dt;
}

void IIMU::dubnumIntergration(outputVals& valsPrev, outputVals& singVals, outputVals& dubvalsOut)
{
    double dt = static_cast<double>((sensorVals.t - valsPrev.t).count()) / 1000000000.0f;

    dubvalsOut.xf += (singVals.xf - valsPrev.xf) * dt;
    dubvalsOut.yf += (singVals.yf - valsPrev.yf) * dt;
    dubvalsOut.zf += (singVals.zf - valsPrev.zf) * dt;
}


ns_t IIMU::monoTimeNow_()
{
    return std::chrono::duration_cast<ns_t>(std::chrono::steady_clock::now().time_since_epoch());
}

void GyroIMU::readData()
{
    txBuffer_[1] = 0xFF;
    txBuffer_[0] = READ_BIT | RATE_X_LSB;

    gpioWrite(CS_GYRO,0);
    spiWrite(spiHandle_,txBuffer_,1);
	//spiXfer(spiHandle_,txBuffer_,rxBuffer_,2);
    //std::cout<<std::bitset<8>(rxBuffer_[1])<<std::endl;
    //rxBuffer_[0] = rxBuffer_[1];
    spiRead(spiHandle_,&rxBuffer_[0],6); //read next 5 registers
    //spiRead(spiHandle_,&rxBuffer_[1],5); //read next 5 registers
	gpioWrite(CS_GYRO,1);
    sensorVals.t = monoTimeNow_();
    
    /*
    std::cout<<"gyro reading: ";
    for (int i = 0; i < 6;i++)
    {
        std::cout<<std::bitset<8>(rxBuffer_[i])<<", ";
    }
    std::cout<<"\r";
    */
    
    //                                LOW                                               HIGH
    sensorVals.xi = (static_cast<int16_t>(rxBuffer_[0]) & 0x00FF ) | (static_cast<int16_t>(rxBuffer_[1])<<8);
    sensorVals.yi = (static_cast<int16_t>(rxBuffer_[2]) & 0x00FF ) | (static_cast<int16_t>(rxBuffer_[3])<<8);
    sensorVals.zi = (static_cast<int16_t>(rxBuffer_[4]) & 0x00FF ) | (static_cast<int16_t>(rxBuffer_[5])<<8);

    sensorVals.xf = (static_cast<double>(sensorVals.xi)/32767.0f * range_);// - offsetVals_.xf; //from datasheet
    sensorVals.yf = (static_cast<double>(sensorVals.yi)/32767.0f * range_);// - offsetVals_.yf;
    sensorVals.zf = (static_cast<double>(sensorVals.zi)/32767.0f * range_);// - offsetVals_.zf;
}

int GyroIMU::sensorInit_(int spiHandle, char* buffer)
{
    //enter normal mode
    buffer[0] = 0 | (GYRO_RANGE);
	buffer[1] = (0x01);//write full scale, +-1000 deg/s
    range_ = 1000.0f;
	//std::cout<<"bytes sent: "<<std::bitset<8>(buffer[0])<<" "<<std::bitset<8>(buffer[1])<<std::endl;// " 1: "<<std::bitset<8>(rxBuffer[1])<<std::endl;
	gpioWrite(CS_GYRO,0);
	spiWrite(spiHandle,buffer,2);
	gpioWrite(CS_GYRO,1);

    //set up low pass filter
    buffer[0] = 0 | (GYRO_BANDWIDTH);
	buffer[1] = (0x02);//ODR: 116
    //buffer[0] = (0x07);//ODR: 100, BW: 32
	gpioWrite(CS_GYRO,0);
	spiWrite(spiHandle,buffer,2);
	gpioWrite(CS_GYRO,1);

    //read id
    txBuffer_[0] = (1<<7) | (0x00);
    txBuffer_[1] = (0xFF);
    gpioWrite(CS_GYRO,0);
    spiXfer(spiHandle,txBuffer_,rxBuffer_,2);
    std::cout<<"Gyro ID:  "<<std::bitset<8>(rxBuffer_[0])<<" "<<std::bitset<8>(rxBuffer_[1])<<std::endl;//first byte is ONLY ignored on ACCEL, NOT GYRO
    gpioWrite(CS_GYRO,1);

    //self test
    txBuffer_[0] = 0 | (0x3C);
    txBuffer_[1] = (0x01);
    gpioWrite(CS_GYRO,0);
    spiWrite(spiHandle,txBuffer_,2);
    gpioWrite(CS_GYRO,1);

    std::this_thread::sleep_for(std::chrono::microseconds(450));//specified in datasheet

    txBuffer_[0] = READ_BIT | (0x3C);
    txBuffer_[1] = (0xFF);
    gpioWrite(CS_GYRO,0);
    spiXfer(spiHandle,txBuffer_,rxBuffer_,2);
    std::cout<<"Gyro self test:  "<<std::bitset<8>(rxBuffer_[0])<<" "<<std::bitset<8>(rxBuffer_[1])<<std::endl;//first byte is ONLY ignored on ACCEL, NOT GYRO
    gpioWrite(CS_GYRO,1);

    txBuffer_[0] = 0 | (0x3C);
    txBuffer_[1] = (0x00);
    gpioWrite(CS_GYRO,0);
    spiWrite(spiHandle,txBuffer_,2);
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
        calibX += sensorVals.xf;
        calibY += sensorVals.yf;
        calibZ += sensorVals.zf;
    }
    offsetVals_.xf = static_cast<double>(calibX / static_cast<double>(calibSize));
    offsetVals_.yf = static_cast<double>(calibY / static_cast<double>(calibSize));
    offsetVals_.zf = static_cast<double>(calibZ / static_cast<double>(calibSize));
}

int GyroIMU::writeSingleReg()
{
    return 0;
}
/*
void GyroIMU::populateProtobuf(dronePosVec::dronePosition& dp)
{
    dp.clear_rotation();
    dp.add_rotation(sensorVals.xf);
    dp.add_rotation(sensorVals.yf);
    dp.add_rotation(sensorVals.zf);
    dp.set_timestamp_ns(sensorVals.t.count());
}
*/
//ACCEL---------------------------------------------------------------------------------------------------------------

void AccelIMU::readData()
{
    txBuffer_[1] = 0xFF;
    txBuffer_[0] = READ_BIT | ACC_X_LSB;

    gpioWrite(CS_ACCEL,0);
	spiWrite(spiHandle_,txBuffer_,2);
	spiRead(spiHandle_,rxBuffer_,6); //1st is ignored in 2nd sent byte of write
	gpioWrite(CS_ACCEL,1);
    sensorVals.t = monoTimeNow_();
    
    /*
    std::cout<<"accel reading: ";
    for (int i = 0; i < 6;i++)
    {
        std::cout<<std::bitset<8>(rxBuffer_[i])<<", ";
    }
    std::cout<<std::endl;
    */
    
    //                                LOW                                               HIGH
    sensorVals.xi = (static_cast<int16_t>(rxBuffer_[0]) & 0x00FF ) | (static_cast<int16_t>(rxBuffer_[1])<<8);
    sensorVals.yi = (static_cast<int16_t>(rxBuffer_[2]) & 0x00FF ) | (static_cast<int16_t>(rxBuffer_[3])<<8);
    sensorVals.zi = (static_cast<int16_t>(rxBuffer_[4]) & 0x00FF ) | (static_cast<int16_t>(rxBuffer_[5])<<8);

    sensorVals.xf = (static_cast<double>(sensorVals.xi)/32768.0f * 1000.0f * std::pow(2.0,static_cast<double>(range_+1)) * 1.5) - offsetVals_.xf; //from datasheet
    sensorVals.yf = (static_cast<double>(sensorVals.yi)/32768.0f * 1000.0f * std::pow(2.0,static_cast<double>(range_+1)) * 1.5) - offsetVals_.yf;
    sensorVals.zf = (static_cast<double>(sensorVals.zi)/32768.0f * 1000.0f * std::pow(2.0,static_cast<double>(range_+1)) * 1.5) - offsetVals_.zf;
}

int AccelIMU::sensorInit_(int spiHandle, char* buffer)
{
    //enter normal mode
    buffer[0] = 0 | (ACC_PWR_CTRL);
	buffer[1] = (0x04);//enter normal mode by writing 4
	//std::cout<<"bytes sent: "<<std::bitset<8>(buffer[0])<<" "<<std::bitset<8>(buffer[1])<<std::endl;// " 1: "<<std::bitset<8>(rxBuffer[1])<<std::endl;
	gpioWrite(CS_ACCEL,0);
	spiWrite(spiHandle,buffer,2);
	gpioWrite(CS_ACCEL,1);

    std::this_thread::sleep_for(std::chrono::microseconds(450));//specified in datasheet

    //write to accelerometer range register
    buffer[0] = 0 | (ACC_RANGE);
	buffer[1] = (0x03);//+- 12g
    range_ = static_cast<double>(buffer[1]);
	gpioWrite(CS_ACCEL,0);
	spiWrite(spiHandle,buffer,2);
	gpioWrite(CS_ACCEL,1);

    //set up low pass filter
    buffer[0] = 0 | (ACC_CONF);
	buffer[1] = (0x08 | (0x0A<<4));//100, normal
	gpioWrite(CS_ACCEL,0);
	spiWrite(spiHandle,buffer,2);
	gpioWrite(CS_ACCEL,1);

    //read accel ID
    txBuffer_[0] = (1<<7) | (0x00);
    txBuffer_[1] = (0xFF);

    gpioWrite(CS_ACCEL,0);
    spiWrite(spiHandle,txBuffer_,2);
    spiRead(spiHandle,rxBuffer_,2);
    std::cout<<"Accel ID: "<<std::bitset<8>(rxBuffer_[0])<<" "<<std::bitset<8>(rxBuffer_[1])<<std::endl;
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
        calibX += sensorVals.xf;
        calibY += sensorVals.yf;
        calibZ += sensorVals.zf;
    }
    offsetVals_.xf = static_cast<double>(calibX / static_cast<double>(calibSize));
    offsetVals_.yf = static_cast<double>(calibY / static_cast<double>(calibSize));
    offsetVals_.zf = static_cast<double>(calibZ / static_cast<double>(calibSize));
}

int AccelIMU::writeSingleReg()
{
    return 0;
}
/*
void AccelIMU::populateProtobuf(dronePosVec::dronePosition& dp)
{
    dp.clear_position();
    //dp.mutable_positiondot()->Add(sensorVals.xf); //alt method if needed, add_xxx should work fine however
    dp.add_position(sensorVals.xf);
    dp.add_position(sensorVals.yf);
    dp.add_position(sensorVals.zf);
}
*/