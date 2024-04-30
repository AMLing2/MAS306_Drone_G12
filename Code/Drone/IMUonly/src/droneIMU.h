#ifndef DRONEIMUHEADER_H
#define DRONEIMUHEADER_H

#include "dronePosVec.pb.h"
#include <pigpio.h>

#define LED 18 //GPIO 18, physical pin 12 //remove !!!!!!!!!!!!!!!!
#define CS_PIN 25 //GPIO 25, physical pin 22

#define G_SI 9.80665
#define PI  3.14159

class IMUDevice
{
public:
    IMUDevice(int spiRate)
    :spiRate_(spiRate)
    {
        spiHandle_ = initIMU_();
        if (spiHandle_ < 0)
        {
            std::cout<<"failed to setup gpio or spi"<<std::endl;
            exit(-1); //terminate
        }
        imuCalibrate_(txBuffer_,bufferSize_);
        accelerometer_[3] = 0;
        gyroscope_[3] = 0;
    }
    //buffersizes have to be the same.
    int readIMU(char* txBuffer,char* rxBuffer, size_t bufferSize);
    //takes the exact rx output from readIMU()?
    int combineHL(char* imuBuffer, size_t bufferSize);
    void populateProtoc();
    int calculateIMU();

//temporarily moving to public:
    //bufferSize cannot be smaller than 2
    const size_t bufferSize_ = 15; //max needed + 1 for null terminator
    char txBuffer_[15];
    char rxBuffer_[15];

    const size_t dataArraySize = 4;
    float accelerometer_[4];
    float gyroscope_[4];
private:
    //turns on IMU as well as pigpio and GPIO pins
    int initIMU_();
    //call after initIMU_()
    int imuCalibrate_(char* buffer,size_t bufferSize);
    int calibGyro_(char* buffer,size_t bufferSize);
    int calibAccel_(char* buffer,size_t bufferSize);
    int regWriteSingle_(uint8_t addr,uint8_t data);
    

    const int spiRate_;
    int spiHandle_;
};

#endif //DRONEIMUHEADER_H