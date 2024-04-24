#ifndef SOCKETCLASS_H
#define SOCKETCLASS_H

#include <chrono>
#include "dronePosVec.pb.h"
//#include "programSpecifics.h"
#include <sys/socket.h>
#include <string.h>
#include <netdb.h>
#include <thread>
#include <queue>

using ns_t = std::chrono::nanoseconds;

//forward declarations
class EstimatorMessenger;
class CameraMessenger;


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
		clientAddrLen_ = sizeof(clientAddr_);
	}
	virtual ssize_t initRecv() = 0;
	/*
	more like copy class addr than get class addr...
	*/
	socklen_t getClassAddr(struct addrinfo passAddr,size_t addrinfosize); //localAddr_ needs to be found in constructor with getsockname()
	void genAddrProtoc(dronePosVec::dataTransfers &data);
	int clientConnect(struct sockaddr* clientAddr,socklen_t addrLen);
	ssize_t clientSend(const char* msg, size_t msgSize);
	ssize_t clientRecv(char* buffer, size_t bufferSize);
	void setTimeout();
	void setTimeout(const long int sec,const long int microSec);

protected:
	const ns_t serverTimer_; //needs to be set in constructor
	void sleeptoInterval_(ns_t interval); //might be separated into two functions with get nanoseconds to interval, its nice to have sometimes
	ns_t monoTimeNow_();

	dronePosVec::dataTransfers data_; 
	const std::string addr_; //likely 127.0.0.1 for all implementations
	int socketSetup_(int port); //creates and binds socket, returns 0 if successful, must be called in constructor
	int f_socket_;
	struct addrinfo* plocalAddr_ ; //this is needed because getaddrinfo() wants an addrinfo** type
	//struct addrinfo localAddr_; //breaks everything dont use
	socklen_t localAddrLen_;
	struct sockaddr clientAddr_;
	socklen_t clientAddrLen_;
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
	virtual ssize_t initRecv() override;
	ssize_t serverRecvfrom(char* buffer, size_t bufferSize);
	virtual dronePosVec::progName checklistLoop() = 0;
	virtual dronePosVec::progName mainRecvloop() = 0;

	ns_t calcSleepTime(ns_t interval);
	int socketShutdown();

	ns_t getServerTimer();
	void setSocketList(EstimatorMessenger* estimator,CameraMessenger* camera);//update later


protected:
	dronePosVec::progName clientProgram_;
	//dronePosVec::transferType clientProgram_; //checklist might be based on which ID in enum
	const size_t bufferSize_ = 1024;
	char buffer_[1024];
	const int port_;
	struct 
    {
        EstimatorMessenger* pEstimator;
        CameraMessenger* pCamera;
    } programSockets_;

}; //ServerSocket

class AbMessenger : public SocketMethods {
public:
	AbMessenger(ns_t timer, std::string addr, long int recvInterval, long int sendInterval,std::queue<std::string> sharedQueue)
	:SocketMethods(timer,addr)
	,recvInterval_(ns_t(recvInterval))
	,sendInterval_(ns_t(sendInterval))
	,q(sharedQueue)
	{
		//no code
	}
	virtual ~AbMessenger() = default;
	virtual ssize_t initRecv() override;
	//functions to be overrided:
	virtual void recvThread() = 0;
	virtual void sendThread() = 0;

	int startThread();
	int joinThread();
	ssize_t getThreadList(std::thread* tbuffer[], size_t tbufferlen);

protected:	
	dronePosVec::progName classProgram_;
	clientProgram messageProgram_;
	const size_t bufferSize_ = 1024;
	char recvMsg_[1024]; //fix to bufferSize_
	char sendMsg_[1024]; // do these really need to be that big?
	std::thread tRecv_;
	std::thread tSend_;

	const ns_t recvInterval_;
	const ns_t sendInterval_;
	std::queue<std::string> q;
	volatile bool threadloop_ = true;//unsure if this should be volatile or not
//in private, add protobuf stuff
}; //AbMessenger

class ServerMain : public ServerSocket {
public:
    ServerMain(std::chrono::nanoseconds time,std::string addr,const int port)
    :ServerSocket(time,addr,port)
    {
        socketSetup_(port);
    }
    virtual dronePosVec::progName checklistLoop() override;
	virtual dronePosVec::progName mainRecvloop() override;
	sockaddr* getClientAddr();
	socklen_t getClientAddrSize();
};

class CameraMessenger : public AbMessenger {
public:
    CameraMessenger(ns_t timer, std::string addr, long int recvInterval, long int sendInterval,std::queue<std::string> sharedQueue)
    :AbMessenger(timer,addr,recvInterval,sendInterval,sharedQueue)
    {
		classProgram_ = dronePosVec::camera;
        socketSetup_(0);
    }
    virtual void recvThread() override;
	virtual void sendThread() override; //unused for camera
}; //CameraMessenger

class EstimatorMessenger : public AbMessenger {
public:
    EstimatorMessenger(ns_t timer, std::string addr, long int recvInterval, long int sendInterval,std::queue<std::string> sharedQueue)
    :AbMessenger(timer,addr,recvInterval,sendInterval,sharedQueue)
    {
        socketSetup_(0);
    }
    virtual void recvThread() override;
	virtual void sendThread() override;

private:


}; //CameraMessenger


#endif //SOCKETCLASS_H