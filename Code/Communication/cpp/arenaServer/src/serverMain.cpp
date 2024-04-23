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

int main()
{
    const int queueCount = 10;
    std::queue<std::string> queues[queueCount]; //would prefer to have char* queue type but cant pass length that way :/

    std::string connectedClients[6] = {"Drone","Camera","RL","Estimator","Arena"};//unused
    std::string localAddr = "127.0.0.1";
    ns_t timenow = std::chrono::duration_cast<ns_t>(std::chrono::steady_clock::now().time_since_epoch());
    ServerMain serverMain(timenow,localAddr,20002);
    CameraMessenger cameraMessenger(serverMain.getServerTimer(),localAddr,10000000,10000000,queues[0]); // do this for all 5 programs...?
    EstimatorMessenger estimatorMessenger(serverMain.getServerTimer(),localAddr,10000000,10000000,queues[0]);

    serverMain.setSocketList(&estimatorMessenger,&cameraMessenger);
    while (true)
    {
        std::cout<<"unconnected clients: "; //finish
        for(int i = 0; i < 5; i++)
        {
            std::cout<<connectedClients[i]<<", ";
        }
        std::cout<<std::endl;

        dronePosVec::progName connectedID = serverMain.mainRecvloop(); 
        switch (connectedID)
        {
            case dronePosVec::drone: //DRONE
            {

                break;
            }
            case dronePosVec::estimator: //ESTIMATOR
            {
                //estimatorMessenger;
                break;
            }
            case dronePosVec::arena: //ARENA
            {
                break;
            }
            case dronePosVec::camera: //CAMERA
            {
                if (cameraMessenger.initRecv() >= 0){
                    cameraMessenger.clientConnect(serverMain.getClientAddr(),serverMain.getClientAddrSize());
                    cameraMessenger.startThread();
                }
                break;
            }
            case dronePosVec::rl: //RL
            {
                break;
            }
            default:
            {
                break;
            }
        }
    }
    
    return 0;
}

/*
void connectThreads(int ID)
{
    
}
*/