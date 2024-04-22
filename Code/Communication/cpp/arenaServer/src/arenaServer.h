#ifndef SOCKETCLASS_H
#define SOCKETCLASS_H

#include <chrono>
#include "dronePosVec.pb.h"
#include <sys/socket.h>
#include <string.h>
#include <netdb.h>
#include <thread>

using ns_t = std::chrono::nanoseconds;

enum clientProgram
{
	serverProg,
	droneProg,
	cameraprog,
	estimatorProg,
	RLProg,
	ArenaProg
};

class ClientSocket {
public:
	virtual ~ClientSocket() = default;
	virtual socklen_t getclient(struct sockaddr* clientAddr) = 0;// ideally should add all these into one init function
	void initSend();
	virtual int sendDrone(const char* msg,size_t msglen) = 0;//might rename
	virtual ssize_t recvDrone(char* buffer, size_t buffernLen) = 0;
	virtual void setTimeout() = 0;
	virtual void setTimeout(const long int sec,const long int microSec) = 0;
	int dServerConnect();
	int sSyncTimer();
};

class SocketMethods{
public:
	SocketMethods(ns_t timer,std::string addr)
	:serverTimer_(timer)
	,addr_(addr)
	{
		//no code
	}
	socklen_t getClassAddr(struct addrinfo* localAddr); //localAddr_ needs to be found in constructor with getsockname()
	int clientConnect(struct addrinfo* clientAddr,size_t clientAddrSize);
	ssize_t clientSend(char* msg, size_t msgSize);
	ssize_t clientRecv(char* buffer, size_t bufferSize);
	void setTimeout();
	void setTimeout(const long int sec,const long int microSec);

protected:
	const std::string addr_; //likely 127.0.0.1 for all implementations
	int socketSetup_(int port); //creates and binds socket, returns 0 if successful, must be called in constructor
	int f_socket_;
	struct addrinfo* localAddr_;
	socklen_t localAddrLen_;
	struct addrinfo* clientAddr_;
	socklen_t clientAddrLen_;
	
	void sleeptoInterval_(ns_t interval); //might be separated into two functions with get nanoseconds to interval, its nice to have sometimes
	ns_t monoTimeNow_();
	const ns_t serverTimer_; //needs to be set in constructor

}; //SocketMethods

class ServerSocket : public SocketMethods {
public:
	ServerSocket(ns_t timer,std::string addr, const int port)
	:SocketMethods(timer,addr)
	,port_(port)
	{
		//no code
	}
	virtual ~ServerSocket() = default;
	int syncTimer();
	
	int sendSocketInfo();
	ssize_t initRecv();
	ssize_t serverRecvfrom(char* buffer, size_t bufferSize);
	virtual int checklistLoop() = 0;
	virtual int mainRecvloop() = 0;

	ns_t calcSleepTime(ns_t interval);
	int socketShutdown();
	int passSocketInfo();

	ns_t getServerTimer();
	void setSocketList(EstimatorMessenger* estimator,CameraMessenger* camera);//update later


protected:
	int clientProgram_;
	//dronePosVec::transferType clientProgram_; //checklist might be based on which ID in enum
	const size_t bufferSize_ = 1024;
	char buffer_[1024];
	dronePosVec::dataTransfers data_; 
	const int port_;
	struct 
    {
        EstimatorMessenger* pEstimator;
        CameraMessenger* pCamera;
    } programSockets_;

}; //ServerSocket

class AbMessenger : public SocketMethods {
public:
	AbMessenger(ns_t timer, std::string addr, long int recvInterval, long int sendInterval)
	:SocketMethods(timer,addr)
	,recvInterval_(ns_t(recvInterval))
	,sendInterval_(ns_t(sendInterval))
	{
		//no code
	}
	virtual ~AbMessenger() = default;
	//functions to be overrided:
	virtual void recvThread() = 0;
	virtual void sendThread() = 0;

	int startThread();
	int joinThread();
	ssize_t getThreadList(std::thread* tbuffer[], size_t tbufferlen);

protected:	
	dronePosVec::transferType classProgram_;
	clientProgram messageProgram_;
	const size_t bufferSize_ = 1024;
	char recvMsg_[1024]; //fix to bufferSize_
	char sendMsg_[1024]; // do these really need to be that big?
	const size_t numOfThreads_ = 2;
	std::thread* threads_[2]; //make to numOfThreads_

	const ns_t recvInterval_;
	const ns_t sendInterval_;
//in private, add protobuf stuff
}; //AbMessenger

#endif //SOCKETCLASS_H