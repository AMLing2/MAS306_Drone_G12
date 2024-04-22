#include "dronePosVec.pb.h"
#include "arenaServer.h"
#include "programSpecifics.h"
#include <chrono>
#include <iostream>
#include <string.h>
#include <sys/socket.h>
#include <netdb.h>
#include <thread>

int main()
{
    std::string connectedClients[5] = {"Drone","Camera","RL","Estimator","Arena"};
    std::string localAddr = "127.0.0.1";
    ns_t timenow = std::chrono::duration_cast<ns_t>(std::chrono::steady_clock::now().time_since_epoch());
    ServerMain serverMain(timenow,localAddr,20002);

    CameraMessenger cameraMessenger(serverMain.getServerTimer(),localAddr,10000000,10000000); // do this for all 5 programs...
    EstimatorMessenger estimatorMessenger(serverMain.getServerTimer(),localAddr,10000000,10000000);

    serverMain.setSocketList(&estimatorMessenger,&cameraMessenger);

    return 0;
}