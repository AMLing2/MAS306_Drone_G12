#include <iostream>
#include <pigpio.h>
#include <cstring>
#include <bitset>
#include <thread>
#include <chrono>
#include <bit>
#include <cmath>
#include <fstream>//for writing to file

//PUT THESE DEFINES IN A .h FILE LATER:
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

//function declarations
std::chrono::nanoseconds monoTimeNow_();
void calib_gyro(int spiHandle,char* txBuffer,char* rxBuffer);
void calib_accel(int spiHandle,char* txBuffer,char* rxBuffer);
int spiInit(char* txbuffer,char* rxbuffer,const size_t bufferlen);
void accel_init(int spiHandle, char* buffer,const size_t bufferlen);
void gyro_init(int spiHandle, char* buffer,const size_t bufferlen);
void gpioEnd(int spiHandle);
void accel_readData(int spiHandle,char* txBuffer,char* rxBuffer, struct AccelVals& accel);
void gyro_readData(int spiHandle,char* txBuffer,char* rxBuffer, struct GyroVals& gyro);

struct AccelVals
{
    int16_t xi,yi,zi;
    float xf,yf,zf = 0;
    std::chrono::nanoseconds t;
};

struct GyroVals
{
    int16_t xi,yi,zi;
    float xf,yf,zf = 0;
    std::chrono::nanoseconds t;
};

namespace imuVals //move to private of a class
{
    float acc_range,gyro_range;
    struct AccelVals accelOffset;
    struct GyroVals gyroOffset;
}

int main()
{
    //PUT IN CLASS:
    AccelVals accelo;
    GyroVals gyrom;
    const size_t buffersize = 16;
    char txBuffer[buffersize];
	char rxBuffer[buffersize];
	memset(rxBuffer,3,buffersize);//set to known value (0000 0011)
	memset(txBuffer,0,buffersize);

    //CSV:
    std::ofstream csvOut;
    csvOut.open("AccelGyro.csv");
    csvOut << "Gyro t,Gyro X, Gyro Y, Gyro Z,Accel t, Accel X, Accel Y, Accel Z \n";

    int spihandle = spiInit(txBuffer,rxBuffer,buffersize);
    if (spihandle < 0 )
    {
        return -1;
    }
    bool running =1;
    int i = 0;
    int a;
    while(running)
    {
        accel_readData(spihandle,txBuffer,rxBuffer,accelo);
        //std::cout<<"accl: X: "<<accelo.xf<<" Y: "<<accelo.yf<<" Z: "<<accelo.zf<<std::endl;
        gyro_readData(spihandle,txBuffer,rxBuffer,gyrom);
        //std::cout<<"gyro: X: "<<gyrom.xf<<" Y: "<<gyrom.yf<<" Z: "<<gyrom.zf<<std::endl;
        csvOut <<gyrom.t.count()<<","<<gyrom.xf<<","<<gyrom.yf<<","<<gyrom.zf<<","<<accelo.t.count()<<","<<accelo.xf<<","<<accelo.yf<<","<<accelo.zf<<"\n";

        //std::this_thread::sleep_for(std::chrono::microseconds(50));
        i++;
        if (i > 50000)
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
   /*
    accel_readData(spihandle,txBuffer,rxBuffer,accelo);
    std::cout<<"accl: X: "<<accelo.xf<<" Y: "<<accelo.yf<<" Z: "<<accelo.zf<<std::endl;
    gyro_readData(spihandle,txBuffer,rxBuffer,gyrom);
    std::cout<<"gyro: X: "<<gyrom.xf<<" Y: "<<gyrom.yf<<" Z: "<<gyrom.zf<<std::endl;
    */
    csvOut.close();
    gpioEnd(spihandle);
    return 0;
}

int spiInit(char* txbuffer,char* rxbuffer,const size_t bufferlen)
{
    if (bufferlen<8)
    {
        std::cout<<"buffer too short";
        return -1;
    }
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
    accel_init(spiHandle,txbuffer,bufferlen); //run first before gyro
    gyro_init(spiHandle,txbuffer,bufferlen);

    //calibrate
    calib_accel(spiHandle,txbuffer,rxbuffer);
    calib_gyro(spiHandle,txbuffer,rxbuffer);
    

    return spiHandle;
}

void accel_init(int spiHandle, char* buffer,const size_t bufferlen)
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
    imuVals::acc_range = static_cast<float>(buffer[0]);
	gpioWrite(CS_ACCEL,0);
	spiWrite(spiHandle,buffer,2);
	gpioWrite(CS_ACCEL,1);

    //set up low pass filter
    buffer[1] = 0 | (ACC_CONF);
	buffer[0] = (0x05 | (0x08<<4));//12.5
	gpioWrite(CS_ACCEL,0);
	spiWrite(spiHandle,buffer,2);
	gpioWrite(CS_ACCEL,1);
}

void gyro_init(int spiHandle, char* buffer,const size_t bufferlen)
{
    //enter normal mode
    buffer[1] = 0 | (GYRO_RANGE);
	buffer[0] = (0x00);//write full scale, +-2000 deg/s
    imuVals::gyro_range = 2000.0f;
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
}

//------------------------------------------ADD TO ACCEL CLASS ---------------------------------------------------
int accel_writeSingleReg() 
{
    return 0;
}

void gpioEnd(int spiHandle)
{
    spiClose(spiHandle);
	gpioTerminate(); // call when done with library
}

void accel_readData(int spiHandle,char* txBuffer,char* rxBuffer, struct AccelVals& accel) 
{
    txBuffer[0] = 0xFF;
    txBuffer[1] = READ_BIT | ACC_X_LSB;

    gpioWrite(CS_ACCEL,0);
	spiWrite(spiHandle,txBuffer,2);
	spiRead(spiHandle,rxBuffer,6); //1st is ignored in 2nd sent byte of write
	gpioWrite(CS_ACCEL,1);
    accel.t = monoTimeNow_();
    /*
    std::cout<<"accel reading: ";
    for (int i = 0; i < 6;i++)
    {
        std::cout<<std::bitset<8>(rxBuffer[i])<<", ";
    }
    std::cout<<std::endl;
    */
    //                                LOW                                               HIGH
    accel.xi = (static_cast<int16_t>(rxBuffer[1]) & 0x00FF ) | (static_cast<int16_t>(rxBuffer[0])<<8);
    accel.yi = (static_cast<int16_t>(rxBuffer[3]) & 0x00FF ) | (static_cast<int16_t>(rxBuffer[2])<<8);
    accel.zi = (static_cast<int16_t>(rxBuffer[5]) & 0x00FF ) | (static_cast<int16_t>(rxBuffer[4])<<8);

    accel.xf = (static_cast<float>(accel.xi)/32768.0f * 1000.0f * std::pow(2.0,static_cast<float>(imuVals::acc_range+1)) * 1.5) - imuVals::accelOffset.xf; //from datasheet
    accel.yf = (static_cast<float>(accel.yi)/32768.0f * 1000.0f * std::pow(2.0,static_cast<float>(imuVals::acc_range+1)) * 1.5) - imuVals::accelOffset.yf;
    accel.zf = (static_cast<float>(accel.zi)/32768.0f * 1000.0f * std::pow(2.0,static_cast<float>(imuVals::acc_range+1)) * 1.5) - imuVals::accelOffset.zf;
}//32768 -> 32767?

void calib_accel(int spiHandle,char* txBuffer,char* rxBuffer)
{
    const size_t calibSize = 1000;
    double calibX = 0;
    double calibY = 0;
    double calibZ = 0;

    struct AccelVals accelCalib;
    for(int i = 0; i < calibSize;i++)
    {
        accel_readData(spiHandle,txBuffer,rxBuffer,accelCalib);
        calibX += accelCalib.xf;
        calibY += accelCalib.yf;
        calibZ += accelCalib.zf;
    }
    imuVals::accelOffset.xf = static_cast<float>(calibX / static_cast<double>(calibSize));
    imuVals::accelOffset.yf = static_cast<float>(calibY / static_cast<double>(calibSize));
    imuVals::accelOffset.zf = static_cast<float>(calibZ / static_cast<double>(calibSize));
}

//----------------------------------------------GYRO---------------------------------------------------------------

void gyro_readData(int spiHandle,char* txBuffer,char* rxBuffer, struct GyroVals& gyro) 
{
    txBuffer[0] = 0x00;
    txBuffer[1] = READ_BIT | RATE_X_LSB;

    gpioWrite(CS_GYRO,0);
	spiXfer(spiHandle,txBuffer,&rxBuffer[0],2);
    spiRead(spiHandle,&rxBuffer[1],6); //read next 5 registers after xfer reads 1st, xfer order is reversed for some reason
	gpioWrite(CS_GYRO,1);
    gyro.t = monoTimeNow_();
    /*
    std::cout<<"gyro reading: ";
    for (int i = 0; i < 6;i++)
    {
        std::cout<<std::bitset<8>(rxBuffer[i])<<", ";
    }
    std::cout<<std::endl;
    */
    //                                LOW                                               HIGH
    gyro.xi = (static_cast<int16_t>(rxBuffer[0]) & 0x00FF ) | (static_cast<int16_t>(rxBuffer[2])<<8);
    gyro.yi = (static_cast<int16_t>(rxBuffer[1]) & 0x00FF ) | (static_cast<int16_t>(rxBuffer[4])<<8);
    gyro.zi = (static_cast<int16_t>(rxBuffer[3]) & 0x00FF ) | (static_cast<int16_t>(rxBuffer[6])<<8); //awful, switch to 8 bit word length plzzzzzz

    gyro.xf = (static_cast<float>(gyro.xi)/32767.0f * imuVals::gyro_range) - imuVals::gyroOffset.xf; //from datasheet
    gyro.yf = (static_cast<float>(gyro.yi)/32767.0f * imuVals::gyro_range) - imuVals::gyroOffset.yf;
    gyro.zf = (static_cast<float>(gyro.zi)/32767.0f * imuVals::gyro_range) - imuVals::gyroOffset.zf;
}

void calib_gyro(int spiHandle,char* txBuffer,char* rxBuffer)
{
    const size_t calibSize = 1000;
    double calibX = 0;
    double calibY = 0;
    double calibZ = 0;

    struct GyroVals GyroCalib;
    for(int i = 0; i < calibSize;i++)
    {
        gyro_readData(spiHandle,txBuffer,rxBuffer,GyroCalib);
        calibX += GyroCalib.xf;
        calibY += GyroCalib.yf;
        calibZ += GyroCalib.zf;
    }
    imuVals::gyroOffset.xf = static_cast<float>(calibX / static_cast<double>(calibSize));
    imuVals::gyroOffset.yf = static_cast<float>(calibY / static_cast<double>(calibSize));
    imuVals::gyroOffset.zf = static_cast<float>(calibZ / static_cast<double>(calibSize));
}

//----------------------------------CSV STUFF---------------------------------------------------------------------------
void csv_writeline()
{
    std::ofstream csvOut;
    csvOut.open("AccelGyro.csv");
}

std::chrono::nanoseconds monoTimeNow_()
{
    return std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::steady_clock::now().time_since_epoch());
}