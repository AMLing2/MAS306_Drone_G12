#include "arenaHeader.h"
#include "droneIMU.h"
#include "dronePosVec.pb.h"
#include <iostream>

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
    dp.add_matrixsize(1); //1x3 vector
    dp.add_matrixsize(3);

    //ClientClass droneClass("127.0.0.1",dronePosVec::drone,"128.39.200.239",20002,threadStartType::sendOnly);
    ClientClass droneClass("dronearena.uia.no",dronePosVec::drone,"b12.uia.no",20002,threadStartType::sendOnly);
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

    while(droneClass.threadloop) //TODO: make some way of exiting safely
    {
        accel.readData();
        gyro.readData();
        accel.populateProtobuf(dp);
        gyro.populateProtobuf(dp);
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

        //std::this_thread::sleep_for(std::chrono::microseconds(50));
    }
    droneClass.joinThread();
    gpioEnd(spiHandle); //needs to be called before end of program
    std::cout<<"press enter to exit"<<std::endl;
    int a;
    std::cin>>a;
    return 0;
}