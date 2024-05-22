#include "droneIMU.h"
#include <iostream>
#include <csignal>

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

    std::signal(SIGINT, &sig_handler);
    while(running) //TODO: make some way of exiting safely
    {
        accel.readData();
        gyro.readData();

        //std::this_thread::sleep_for(std::chrono::microseconds(50));
        if (mainExit)
        {
            running = false;
        }
    }
    gpioEnd(spiHandle); //needs to be called before end of program
    return 0;
}

void sig_handler(int signum)
{
    if (signum == SIGINT)
    {
        mainExit = true;
        std::cerr << "Interrupt signal (" << signum << ") received.\n";
    }
    else
    {
        //restore default signal handler
        std::signal(signum, SIG_DFL);
    }
}
