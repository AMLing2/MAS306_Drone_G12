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
#include <atomic>
#include <condition_variable>

using ns_t = std::chrono::nanoseconds;

//forward declarations
std::string getSelfIP(const char nameserver[]);
class EstimatorMessenger;
class CameraMessenger;
class DroneMessenger;
class AbMessenger;

enum threadStartType
{
	recvOnly,
	sendOnly,
	sendRecv
};

class ClientSocket {
public:
	virtual ~ClientSocket() = default;
	virtual socklen_t getclient(struct sockaddr* clientAddr) = 0;// ideally should add all these into one init function
	void initSend();
	virtual int sendDrone(const char* msg,size_t msglen) = 0;//might rename
	virtual ssize_t recvDrone(char* buffer, size_t bufferLen) = 0;
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
	int getIPfromName(std::string hostname,int port);
	virtual ssize_t initRecv() = 0;
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
	std::string addr_; //likely 127.0.0.1 for all implementations
	int socketSetup_(int port); //creates and binds socket, returns 0 if successful, must be called in constructor
	int f_socket_;
	struct addrinfo* plocalAddr_ ; //this is needed because getaddrinfo() wants an addrinfo** type
	//struct addrinfo localAddr_; //breaks everything dont use
	socklen_t localAddrLen_;
	struct sockaddr clientAddr_;
	socklen_t clientAddrLen_;
}; //SocketMethods

class ServerSocket : public SocketMethods { //TODO: public -> protected
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
	void setSocketList(std::vector<std::unique_ptr<AbMessenger>>* socketClasses);


protected:
	dronePosVec::progName clientProgram_;
	//dronePosVec::transferType clientProgram_; //checklist might be based on which ID in enum
	const size_t bufferSize_ = 1024;
	char buffer_[1024];
	const int port_;
	std::vector<std::unique_ptr<AbMessenger>>* vpSocketClasses_;

}; //ServerSocket

class AbMessenger : public SocketMethods {
public:
	AbMessenger(ns_t timer, std::string addr, long int recvInterval, long int sendInterval,std::queue<std::string>* sharedQueue,const dronePosVec::progName progType, threadStartType threadfuncs,std::string stringName)
	:SocketMethods(timer,addr)
	,strName(stringName)
	,recvInterval_(ns_t(recvInterval))
	,sendInterval_(ns_t(sendInterval))
	,pq1(sharedQueue)
	,pq2(sharedQueue) //TODO: have to do this but q2 shouldnt be used
	,classProgram_(progType)
	,threadFuncs_(threadfuncs)
	{
		//no code
	}
	AbMessenger(ns_t timer, std::string addr, long int recvInterval, long int sendInterval,std::queue<std::string>* sharedQueue1,std::queue<std::string>* sharedQueue2,const dronePosVec::progName progType, threadStartType threadfuncs,std::string stringName)
	:SocketMethods(timer,addr)
	,strName(stringName)
	,recvInterval_(ns_t(recvInterval))
	,sendInterval_(ns_t(sendInterval))
	,pq1(sharedQueue1)
	,pq2(sharedQueue2)
	,classProgram_(progType)
	,threadFuncs_(threadfuncs)
	{
		//no code
	}
	virtual ~AbMessenger() = default;
	virtual ssize_t initRecv() override;
	//functions to be overrided:
	virtual void recvThread(); //generic: recv and add to q
	virtual void sendThread(); //generic: send from q2

	int startThread(threadStartType startType);
	int joinThread();
	ssize_t getThreadList(std::thread* tbuffer[], size_t tbufferlen); //currently unused 
	dronePosVec::progName getName();
	bool getConnection();
	void setConnection(bool status);
	const std::string strName;
	threadStartType getThreadStart();
	/*starts this programs threads and also sends a message to the client to start*/
	void conditionNotify(bool startProg);

protected:
	int waitforProgStart_();
	void appendToQueue_(std::queue<std::string>& queueNum,const std::string& msg);
	int blockingGetQueue_(std::queue<std::string>& queueNum,std::string& msg,int timeout);
	ns_t recvInterval_; //not const because might  change during runtime
	ns_t sendInterval_;
	std::queue<std::string>* const pq1;
	std::queue<std::string>* const pq2;
	volatile std::atomic_bool threadloop_ = true;

	bool connected_ = false;
	const dronePosVec::progName classProgram_;

	const threadStartType threadFuncs_;
	const size_t bufferSize_ = 1024;
	char recvMsg_[1024]; //fix to bufferSize_
	char sendMsg_[1024];
	std::thread tRecv_;
	std::thread tSend_;
	std::mutex m_;
	std::condition_variable cv_;
//in private, add protobuf stuff
}; //AbMessenger

class ServerMain : public ServerSocket {
public:
    ServerMain(ns_t time,std::string addr,const int port)
    :ServerSocket(time,addr,port)
    {
        socketSetup_(port);
    }
    virtual dronePosVec::progName checklistLoop() override;
	virtual dronePosVec::progName mainRecvloop() override;
	struct sockaddr* getClientAddr();
	socklen_t getClientAddrSize();

private:
	void stateChange_();
};

class CameraMessenger : public AbMessenger {
public:
    CameraMessenger(ns_t timer, std::string addr, long int recvInterval, long int sendInterval,std::queue<std::string>* sharedQueue)
    :AbMessenger(timer,addr,recvInterval,sendInterval,sharedQueue,dronePosVec::camera,threadStartType::recvOnly,std::string("Camera"))
    {
        socketSetup_(0);
    }
	~CameraMessenger()
	{

	}
    //virtual void recvThread() override;
	//virtual void sendThread() override; //unused for camera
}; //CameraMessenger

class EstimatorMessenger : public AbMessenger {
public:
    EstimatorMessenger(ns_t timer, std::string addr, long int recvInterval, long int sendInterval,std::queue<std::string>* sharedQueue1,std::queue<std::string>* sharedQueue2)
    :AbMessenger(timer,addr,recvInterval,sendInterval,sharedQueue1,sharedQueue2,dronePosVec::estimator,threadStartType::sendRecv,std::string("Estimator"))
    {
        socketSetup_(0);
    }
    //virtual void recvThread() override;
	//virtual void sendThread() override;
}; //EstimatorMessenger

class DroneMessenger : public AbMessenger {
public:
    DroneMessenger(ns_t timer, std::string addr, long int recvInterval, long int sendInterval,std::queue<std::string>* sharedQueue1,std::queue<std::string>* sharedQueue2)
    :AbMessenger(timer,addr,recvInterval,sendInterval,sharedQueue1,sharedQueue2,dronePosVec::drone,threadStartType::sendRecv,std::string("Drone"))
    {
        socketSetup_(0);
    }
    //virtual void recvThread() override;
	//virtual void sendThread() override; //unsure if i will separate these
}; //DroneMessenger

class RLMessenger : public AbMessenger {
public:
    RLMessenger(ns_t timer, std::string addr, long int recvInterval, long int sendInterval,std::queue<std::string>* sharedQueue1,std::queue<std::string>* sharedQueue2)
    :AbMessenger(timer,addr,recvInterval,sendInterval,sharedQueue1,sharedQueue2,dronePosVec::rl,threadStartType::sendRecv,std::string("RL"))
    {
        socketSetup_(0);
    }
    //virtual void recvThread() override;
	//virtual void sendThread() override;
}; //DroneMessenger

class ArenaMessenger : public AbMessenger {
public:
    ArenaMessenger(ns_t timer, std::string addr, long int recvInterval, long int sendInterval,std::queue<std::string>* sharedQueue1,std::queue<std::string>* sharedQueue2)
    :AbMessenger(timer,addr,recvInterval,sendInterval,sharedQueue1,sharedQueue2,dronePosVec::arena,threadStartType::sendRecv,std::string("Arena"))
    {
        socketSetup_(0);
    }
    //virtual void recvThread() override;
	//virtual void sendThread() override;
}; //DroneMessenger

#endif //SOCKETCLASS_H