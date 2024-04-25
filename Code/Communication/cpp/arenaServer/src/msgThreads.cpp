#include "dronePosVec.pb.h"
#include "arenaServer.h"
#include <thread>
#include <queue>
#include <cerrno>

void CameraMessenger::recvThread()
{
    std::cout<<"Camera recvThread up"<<std::endl;
    dronePosVec::dataTransfers data; 
    setTimeout(1,0);//too high for 100ms
    ssize_t msgsize = 0;
    while(threadloop_)
    {
        data.Clear();
        msgsize = clientRecv(recvMsg_,bufferSize_);
        if (msgsize > 0)
        {
            data.ParseFromArray(recvMsg_,msgsize);
            std::cout<<data.msg()<<std::endl; //TODO: test, remove
            //q.push(data.SerializeAsString()); //uncomment
        }
        else
        {
            std::cout<<"errno:"<<strerror(errno)<<": "<<errno<<std::endl;
            threadloop_ = false;
            break;
        }
        sleeptoInterval_(recvInterval_);
    }
    std::cout<<"Camera recvThread ready to join"<<std::endl;
}

void CameraMessenger::sendThread(){ } 

void EstimatorMessenger::recvThread(){ }

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