#include "dronePosVec.pb.h"
#include "arenaServer.h"
#include <thread>
#include <queue>

void CameraMessenger::recvThread()
{
    std::cout<<"Camera recvThread up"<<std::endl;
    dronePosVec::dataTransfers data; 
    setTimeout(1,0);//too high for 100ms
    ssize_t msgsize;
    while(threadloop_)
    {
        data.Clear();
        msgsize = (recvMsg_,bufferSize_);
        data.ParseFromArray(recvMsg_,msgsize);
        q.push(data.SerializeAsString());

        sleeptoInterval_(recvInterval_);
    }
    std::cout<<"Camera recvThread ready to join"<<std::endl;
}

void EstimatorMessenger::sendThread()
{
    std::cout<<"estimator sendThread up"<<std::endl;
    //dronePosVec::dataTransfers data; 
    //setTimeout(1,0);//too high for 100ms
    std::string msg;
    while(threadloop_)
    {
        sleeptoInterval_(sendInterval_);
        if(q.empty() != true)
        {
            msg = q.front();
            clientSend(msg.c_str(),msg.length());
            q.pop();
        }
    }
    std::cout<<"estimator sendThread ready to join"<<std::endl;
}