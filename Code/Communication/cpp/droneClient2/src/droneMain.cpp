#include "arenaHeader.h"
#include <iostream>

int main()
{
    ClientClass droneClass("127.0.0.1",dronePosVec::drone,"128.39.200.239",20002);
    if (droneClass.connectServer() == 0)
    {
        droneClass.checkList();
    }
    else
    {
        std::cout<<"failed to connect"<<std::endl;
    }
    return 0;
}