#ifndef DRONEIMU_H
#define DRONEIMU_H

#include "dronePosVec.pb.h"
#include <thread>
#include <chrono>

//pin defs
#define LED 18 //GPIO 18, physical pin 12
#define CS_ACCEL 12 //GPIO 12, physical pin 22
#define CS_GYRO 13
//spi flags:
#define SPI_CHANNEL 1
#define SPI_SPEED 1000000
#define SPI_MODE_0 0x0
#define SPI_WORDLEN_16 (0x10<<16)
#define SPI_DISABLE_UX ((0x1<<5) | (0x1<<6)| (0x1<<7))
#define SPI_AUXILIARY (0x1<<8)

//registers:
#define READ_BIT (1<<7)
#define ACC_PWR_CTRL 0x7D
#define ACC_X_LSB 0x12
#define ACC_RANGE 0x41
#define GYRO_RANGE 0x0F
#define RATE_X_LSB 0x02
#define GYRO_BANDWIDTH 0x10
#define ACC_CONF 0x40

void gpioEnd(int spiHandle);
int spiInit();//push to class but leave for now

using ns_t = std::chrono::nanoseconds;

class IIMU
{
public:
    IIMU(int handle)
    :spiHandle_(handle)
    {
        
    }
    //virtual void readData(int spiHandle,char* txBuffer,char* rxBuffer, struct outputVals& vals) = 0 ;
    virtual int writeSingleReg() = 0;
    virtual void populateProtobuf(dronePosVec::dronePosition& dp) = 0;

protected:
    virtual int sensorInit_(int spiHandle, char* buffer) = 0;
    ns_t monoTimeNow_();
    virtual void calib_() = 0;
    
    const int spiHandle_;
    struct outputVals
    {
        float xf,yf,zf = 0;
        int16_t xi,yi,zi = 0;
        ns_t t;
    } offsetVals_,sensorVals_;
    float range_;
    const size_t bufferSize = 16;
    char txBuffer_[16];
	char rxBuffer_[16];

}; //IIMU

class GyroIMU : protected IIMU
{
public:
    GyroIMU(int handle)
    :IIMU(handle)
    {
        sensorInit_(spiHandle_,txBuffer_);
        calib_();
    }
    void readData();//should be override but compiler dosent like it because ??????????
    virtual int writeSingleReg() override;
    virtual void populateProtobuf(dronePosVec::dronePosition& dp) override;

protected:
    virtual int sensorInit_(int spiHandle, char* buffer) override;
    virtual void calib_() override;

}; //GyroIMU

class AccelIMU : protected IIMU
{
public:
    AccelIMU(int handle)
    :IIMU(handle)
    {

    }
    void readData();
    virtual int writeSingleReg() override;
    virtual void populateProtobuf(dronePosVec::dronePosition& dp) override;

protected:
    virtual int sensorInit_(int spiHandle,char* buffer) override;
    virtual void calib_() override;

}; //AccelIMU

#endif //DRONEIMU_H