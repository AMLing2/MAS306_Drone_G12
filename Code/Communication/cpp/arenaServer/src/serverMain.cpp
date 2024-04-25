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

    std::vector<std::string> connectedClients({"Drone","Camera","RL","Estimator","Arena"});
    int unconnected = 5; //useless
    std::string localAddr = "128.39.200.239"; //127.0.0.1
    ns_t timenow = std::chrono::duration_cast<ns_t>(std::chrono::steady_clock::now().time_since_epoch());
    ServerMain serverMain(timenow,localAddr,20002);
    CameraMessenger cameraMessenger(serverMain.getServerTimer(),localAddr,10000000,10000000,queues[0]); // do this for all 5 programs...?
    EstimatorMessenger estimatorMessenger(serverMain.getServerTimer(),localAddr,10000000,10000000,queues[0]);

    serverMain.setSocketList(&estimatorMessenger,&cameraMessenger);
    bool connectloop = true;
    while (connectloop)
    {
        std::cout<<"unconnected clients: "; //finish
        for(const std::string& i: connectedClients) //range based for loop
        {
            std::cout<<i<<", ";
        }
        std::cout<<std::endl;

        dronePosVec::progName connectedID = serverMain.mainRecvloop(); 
        std::cout<<"connected ID: " <<connectedID<<std::endl;
        unconnected--;
        switch (connectedID)
        {
            case dronePosVec::drone: //DRONE
            {
                connectedClients.erase(std::find(connectedClients.begin(),connectedClients.end(),"Drone"));

                break;
            }
            case dronePosVec::estimator: //ESTIMATOR
            {
                connectedClients.erase(std::find(connectedClients.begin(),connectedClients.end(),"Estimator"));
                //estimatorMessenger;
                break;
            }
            case dronePosVec::arena: //ARENA
            {
                connectedClients.erase(std::find(connectedClients.begin(),connectedClients.end(),"Arena"));
                break;
            }
            case dronePosVec::camera: //CAMERA = 4?
            {
                connectedClients.erase(std::find(connectedClients.begin(),connectedClients.end(),"Camera"));
                if (cameraMessenger.initRecv() >= 0){
                    //cameraMessenger.clientConnect(serverMain.getClientAddr(),serverMain.getClientAddrSize());
                    cameraMessenger.startThread();
                }
                break;
            }
            case dronePosVec::rl: //RL
            {
                connectedClients.erase(std::find(connectedClients.begin(),connectedClients.end(),"RL"));
                break;
            }
            default:
            {
                connectloop = false; //incase enum server is returned
                break;
            }
        }
        if (unconnected == 0)
        {
            connectloop = false;
        }
        
    }
    
    return 0;
}

/*
void connectThreads(int ID)
{
    
}
*/