#include "arenaHeader.h"
#include "droneIMU.h"
#include "dronePosVec.pb.h"
#include <iostream>

volatile bool mainExit = false;
void sig_handler(int signum);

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
    dronePosVec::dronePosition dp;

    //initialize dp
    dp.set_devicetype(dronePosVec::dataDevices::IMUonly);
    dp.add_rotshape(1); //1x3 vector
    dp.add_rotshape(3);
    dp.add_posshape(1); //1x3 vector
    dp.add_posshape(3);

    //ClientClass droneClass("127.0.0.1",dronePosVec::drone,"128.39.200.239",20002,threadStartType::sendOnly);
    ClientClass droneClass("dronearena.uia.no",dronePosVec::drone,"b12.uia.no",20002,threadStartType::sendRecv); //temp sendRecv, normally sendOnly for IMU
    if (droneClass.connectServer() == 0)
    {
        if (droneClass.checkList() < 0)
        {
            std::cout<<"failure during checklist"<<std::endl;
            return -1;
        }
    }
    else
    {
        std::cout<<"failed to connect"<<std::endl;
        return -1;
    }
    droneClass.startThread(droneClass.threadFuncs);
    signal(SIGINT, &sig_handler);
    while(droneClass.threadloop) //TODO: make some way of exiting safely
    {
        accel.readData();
        gyro.readData();
        accel.populateProtobuf(dp);
        gyro.populateProtobuf(dp); //timestamp is only set here
        if ((droneClass.sendQueue.size() > 0) & (droneClass.readingQueue == false))
        {
            try
            {
                droneClass.sendQueue = {}; //clear queue
            }
            catch(...)
            {
                //do nothing, .pop will throw an exception rarely when thread calls .pop right before this .pop is called but not before .size returns 0, not a dangerous error
            }
        }
        droneClass.sendQueue.push(dp.SerializeAsString());
        droneClass.conditionNotify(); //tell thread that its safe to read queue (............... this is basically just the same as using an atomic_string at this point)

        //std::this_thread::sleep_for(std::chrono::microseconds(50));
        if (mainExit)
        {
            droneClass.threadloop = false;
        }
    }
    droneClass.joinThread();
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
        signal(signum, SIG_DFL);
    }
}