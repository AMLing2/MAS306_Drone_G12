#include "droneIMU.h"
#include <iostream>
#include <thread>
#include <chrono>

#define MHZ_1 1000000 

int main()
{
    IMUDevice imu(MHZ_1);

    int n = 0;
    int a;
    bool running = true;
    while(running)
    {
        imu.readIMU(imu.txBuffer_,imu.rxBuffer_,imu.bufferSize_);
        imu.combineHL(imu.rxBuffer_,imu.bufferSize_);
        std::cout<<"gyro: X: "<<imu.gyroscope_[0]<<" Y: "<<imu.gyroscope_[1]<<" Z: "<<imu.gyroscope_[2]<<std::endl;
        std::cout<<"Accel: X: "<<imu.accelerometer_[0]<<" Y: "<<imu.accelerometer_[1]<<" Z: "<<imu.accelerometer_[2]<<std::endl;
        n++;
        std::this_thread::sleep_for(std::chrono::milliseconds(2));
        if (n >= 50)
        {
            std::cout<<"press 0 to exit"<<std::endl;
            std::cin>>a;
            if (a == 0)
            {
                running = false;
            }
            else
            {
                n = 0;
            }
        }
    }
    
    return 0;
}