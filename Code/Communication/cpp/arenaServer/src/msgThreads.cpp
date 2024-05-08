#include "dronePosVec.pb.h"
#include "arenaServer.h"
#include <thread>
#include <queue>
#include <cerrno>

//CAMERA --------------------------------------------------------------------------------
void CameraMessenger::recvThread()
{
    std::cout<<"Camera recvThread up"<<std::endl;
    dronePosVec::dronePosition data; 
    setTimeout(5,0);//too high for 100ms interval
    ssize_t msgsize = 0;
    while(threadloop_)
    {
        data.Clear();
        msgsize = clientRecv(recvMsg_,bufferSize_);
        if (msgsize > 0)
        {
            data.ParseFromArray(recvMsg_,msgsize);
            std::cout<<data.rotmatrix().size()<<std::endl; //TODO: test, remove
            if (data.rotmatrix().size() > 0)
            {
                std::cout<<data.rotmatrix().Get(0)<<std::endl; //TODO: test, remove
            }
            
            //q.push(data.SerializeAsString()); //uncomment
        }
        else
        {
            std::cout<<"errno:"<<strerror(errno)<<": "<<errno<<std::endl;
            threadloop_ = false; //this will end the sending thread too
            break;
        }
        sleeptoInterval_(recvInterval_);
    }
    std::cout<<"Camera recvThread ready to join"<<std::endl;
}

void CameraMessenger::sendThread(){ } 

//ESTIMATOR --------------------------------------------------------------------------------
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

//DRONE --------------------------------------------------------------------------------

void DroneMessenger::recvThread()
{
    std::cout<<"drone recvThread up"<<std::endl;
    dronePosVec::dronePosition data; 
    setTimeout(5,0);//too high for 100ms interval
    ssize_t msgsize = 0;
    while(threadloop_)
    {
        data.Clear();
        msgsize = clientRecv(recvMsg_,bufferSize_);
        if (msgsize > 0)
        {
            data.ParseFromArray(recvMsg_,msgsize);
            //std::cout<<"rotmatrix[1]: "<<data.rotmatrix()<<std::endl;
            std::cout<<data.rotmatrixdot().size()<<std::endl; //TODO: test, remove
            if (data.rotmatrixdot().size() > 0)
            {
                std::cout<<data.rotmatrixdot().Get(0)<<std::endl; //TODO: test, remove
            }
            
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
    std::cout<<"drone recvThread ready to join"<<std::endl;
}

void DroneMessenger::sendThread()
{

}