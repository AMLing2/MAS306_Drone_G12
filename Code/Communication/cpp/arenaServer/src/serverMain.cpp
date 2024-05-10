#include "dronePosVec.pb.h"
#include "arenaServer.h"
//#include "programSpecifics.h"
#include <chrono>
#include <iostream>
#include <string.h>
#include <sys/socket.h>
#include <netdb.h>
#include <thread>
#include <queue>
#include <vector> //only used for fancily displaying unconnected clients
#include <arpa/inet.h> //for inet_ntop

int main()
{
    const int queueCount = 10;
    std::queue<std::string> queues[queueCount]; //would prefer to have char* queue type but cant pass length that way

    int unconnected;
    const std::string localAddr = getSelfIP(".uia.no");//"128.39.200.239"; //127.0.0.1 //change to resolve selfhost
    ns_t timenow = std::chrono::duration_cast<ns_t>(std::chrono::steady_clock::now().time_since_epoch()); //get start of global server timer
    ServerMain serverMain(timenow,localAddr,20002);

    std::vector<std::unique_ptr<AbMessenger>> vpMessengers;
    vpMessengers.push_back(std::make_unique<CameraMessenger>(serverMain.getServerTimer(),localAddr,10000000, 10000000,&queues[0])); // Camera
    vpMessengers.push_back(std::make_unique<EstimatorMessenger>(serverMain.getServerTimer(),localAddr,10000000, 10000000,&queues[0],&queues[2])); //Estimator
    vpMessengers.push_back(std::make_unique<DroneMessenger>(serverMain.getServerTimer(),localAddr,10000000, 10000000,&queues[0],&queues[1])); //Drone
    vpMessengers.push_back(std::make_unique<RLMessenger>(serverMain.getServerTimer(),localAddr,10000000, 10000000,&queues[1],&queues[0])); //RL

    serverMain.setSocketList(&vpMessengers);
    bool connectloop = true;
    while (connectloop)
    {
        std::cout<<"unconnected clients: ";
        unconnected = 0;
        for(const std::unique_ptr<AbMessenger>& i: vpMessengers) //range based for loop
        {
            if (i->getConnection() == false)
            {
                unconnected++;
                std::cout<<i->strName<<", ";
            }
        }
        std::cout<<std::endl;

        dronePosVec::progName connectedID = serverMain.mainRecvloop(); //----------------------main socket connection------------
        if (connectedID == dronePosVec::server)
        {
            connectloop = false;
            break;
        }
        std::cout<<"connected ID: " <<connectedID<<std::endl;

        for(const std::unique_ptr<AbMessenger>& i: vpMessengers) //range based for loop
        {
            if (i->getName() == connectedID)
            {
                //std::cout<<i->strName<<std::endl;
                if(i->initRecv() >= 0) //specific client connection
                {
                    i->setConnection(true);
                    i->startThread(i->getThreadStart());
                    break;
                }
            }
        }
        if (unconnected == 0)
        {
            connectloop = false; //would rather have a check that allows clients to be reconnected in case of a connection loss
        }
    }

    std::cout<<"type 'end' or '0' to end program, 'run' to start, 'help' for options"<<std::endl; //TODO: fix this
    bool mainlooping = true;
    int a;
    while(mainlooping)
    {
        
        std::cin>>a;

        switch (a)
        {
        case 1:
            //start
            std::cout<<"starting program"<<std::endl;
            for(const std::unique_ptr<AbMessenger>& i: vpMessengers)
            {
                i->conditionNotify(true);
            }
            break;
        
        default:
            mainlooping = false;
            //finish:
            std::cout<<"ending threads"<<std::endl;
            for(const std::unique_ptr<AbMessenger>& i: vpMessengers) //end all threads (kind of slow because it does it one by one... might take 1000ms)
            {
                i->conditionNotify(false);
                i->joinThread();
            }
            break;
        }
    }
    return 0;
}