#ifndef PROGSPECIFICS_H
#define PROGSPECIFICS_H

#include <chrono>
#include "arenaServer.h"
#include "dronePosVec.pb.h"
#include <sys/socket.h>
#include <string.h>
#include <netdb.h>
#include <thread>

class ServerMain : public ServerSocket {
public:
    ServerMain(std::chrono::nanoseconds time,std::string addr,const int port)
    :ServerSocket(time,addr,port)
    {
        socketSetup_(port);
    }
    virtual int checklistLoop() override;
	virtual int mainRecvloop() override;
};

class CameraMessenger : public AbMessenger {
public:
    CameraMessenger(ns_t timer, std::string addr, long int recvInterval, long int sendInterval)
    :AbMessenger(timer,addr,recvInterval,sendInterval)
    {
        socketSetup_(0);
    }
    virtual void recvThread() override;
	virtual void sendThread() override;
}; //CameraMessenger

class EstimatorMessenger : public AbMessenger {
public:
    EstimatorMessenger(ns_t timer, std::string addr, long int recvInterval, long int sendInterval)
    :AbMessenger(timer,addr,recvInterval,sendInterval)
    {
        socketSetup_(0);
    }
    virtual void recvThread() override;
	virtual void sendThread() override;

private:


}; //CameraMessenger

#endif //PROGSPECIFICS_H