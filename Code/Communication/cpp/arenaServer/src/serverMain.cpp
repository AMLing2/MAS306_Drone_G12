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

int main()
{
    const int queueCount = 10;
    std::queue<std::string> queues[queueCount]; //would prefer to have char* queue type but cant pass length that way :/

    int unconnected;
    std::string localAddr = "128.39.200.239"; //127.0.0.1
    ns_t timenow = std::chrono::duration_cast<ns_t>(std::chrono::steady_clock::now().time_since_epoch());
    ServerMain serverMain(timenow,localAddr,20002);

    std::vector<std::unique_ptr<AbMessenger>> vpMessengers;
    vpMessengers.push_back(std::make_unique<CameraMessenger>(serverMain.getServerTimer(),localAddr,10000000, 10000000,queues[0]));
    vpMessengers.push_back(std::make_unique<EstimatorMessenger>(serverMain.getServerTimer(),localAddr,10000000, 10000000,queues[0]));
    vpMessengers.push_back(std::make_unique<DroneMessenger>(serverMain.getServerTimer(),localAddr,10000000, 10000000,queues[0]));

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

        dronePosVec::progName connectedID = serverMain.mainRecvloop(); 
        std::cout<<"connected ID: " <<connectedID<<std::endl;

        for(const std::unique_ptr<AbMessenger>& i: vpMessengers) //range based for loop
        {
            if (i->getName() == connectedID)
            {
                //std::cout<<i->strName<<std::endl;
                if(i->initRecv() >= 0)
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

    std::cout<<"press enter to exit"<<std::endl;
    int a;
    std::cin>>a;

    for(const std::unique_ptr<AbMessenger>& i: vpMessengers) //end all threads (kind of slow because it does it one by one... might take 1000ms)
    {
        i->joinThread();
    }

    return 0;
}

/*
void connectThreads(int ID)
{
    
}
*/