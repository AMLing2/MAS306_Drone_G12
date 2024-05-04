#include "arenaHeader.h"
#include <iostream>

int main()
{
    ClientClass droneClass("127.0.0.1",dronePosVec::drone,"128.39.200.239",20002,threadStartType::sendOnly);
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
    //start thread here:

    std::cout<<"press enter to exit"<<std::endl;
    int a;
    std::cin>>a;
    return 0;
}