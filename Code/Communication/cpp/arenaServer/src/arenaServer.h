#ifndef SOCKETCLASS_H
#define SOCKETCLASS_H

#include <chrono>
#include "dronePosVec.pb.h"
#include <sys/socket.h>
#include <string.h>
#include <netdb.h>
#include <chrono> //for nanoseconds type

//float sleepTimeCalc(int interval,high_resolution_clock::time_point timer);

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

class AbMessager{
public:
	//functions to be overrided:
	//constructor should use port 0 to automatically define an unused port
	virtual ~AbMessager() = default;
	virtual void recvThread() = 0;
	virtual void sendThread() = 0;

	ssize_t recvClient();
	ssize_t sendClient();

	int startThread();
	int joinThread();
	socklen_t getLocalAddr(struct addrinfo* localAddr); //localAddr_ needs to be found in constructor with getsockname()

protected:
	int socketSetup_();
	int connectClient_();
	int f_socket_;
	struct addrinfo* localAddr_;
	socklen_t localAddrLen_;
	const size_t bufferSize_ = 1024;
	char buffer[1024];//fix to bufferSize_
	const size_t numOfThreads_ = 2;
	std::thread* threads_[2]; //make to numOfThreads_;

	std::chrono::nanoseconds serverTimer_;
	std::chrono::nanoseconds recvInterval_;
	std::chrono::nanoseconds sendInterval_;

};

#endif //SOCKETCLASS_H
