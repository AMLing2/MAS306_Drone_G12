#include "droneIMU.h"
#include <iostream>
#include <csignal>
#include <fstream>
#include <cstring>

volatile bool mainExit = false;
void sig_handler(int signum);
volatile bool running = true;

int main()
{
    int spiHandle = spiInit();
    if (spiHandle < 0)
    {
        std::cout<<"\n pigpio failed to initialize"<<std::endl;
        return -1;
    }
    
    GyroIMU gyro(spiHandle);
    AccelIMU accel(spiHandle);

    outputVals accelprev;
    outputVals gyroprev;
    outputVals accelint;
    outputVals gyroint;

    outputVals acceldubint;

    bool firstrun = false;

    std::ofstream csvWrite;
    csvWrite.open("numint.csv");
    csvWrite << "time,accelX,accelY,accelZ,gyroX,gyroY,gyroZ"<<std::endl;
    signal(SIGINT, &sig_handler);
    std::cout<<"starting read"<<std::endl;
    while(running) //TODO: make some way of exiting safely
    {
        if (firstrun)
        {
            accelprev = accel.sensorVals;
            gyroprev = gyro.sensorVals;
        }
        gyro.readData();
        accel.readData();
        if (firstrun)
        {
            accel.numIntergration(accelprev,accelint);
            gyro.numIntergration(gyroprev,gyroint);
            //accel.dubnumIntergration(accelprev,accelint,acceldubint);
            csvWrite << gyro.sensorVals.t.count() <<","<< accel.sensorVals.xf <<","<< accel.sensorVals.yf <<","<< accel.sensorVals.zf <<","<< gyro.sensorVals.xf <<","<< gyro.sensorVals.yf <<","<< gyro.sensorVals.zf<<std::endl;
            //csvWrite << gyro.sensorVals.t.count() <<","<< acceldubint.xf <<","<< acceldubint.yf <<","<< acceldubint.zf <<","<< gyroint.xf <<","<< gyroint.yf <<","<< gyroint.zf<<std::endl;
            
        }
        std::this_thread::sleep_for(std::chrono::microseconds(10));
        firstrun = true;
        if (mainExit)
        {
            running = false;
        }
    }
    csvWrite.close();
    gpioEnd(spiHandle); //needs to be called before end of program
    return 0;
}

void sig_handler(int signum)
{
    if (signum == SIGINT)
    {
        mainExit = true;
        std::cerr << "\nInterrupt signal (" << signum << ") received.\n";
    }
    else
    {
        //restore default signal handler
        signal(signum, SIG_DFL);
    }
}
