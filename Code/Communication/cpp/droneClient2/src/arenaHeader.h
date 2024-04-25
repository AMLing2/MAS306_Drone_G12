#ifndef ARENAHEADER_H
#define ARENAHEADER_H

#include <chrono>
#include "dronePosVec.pb.h"
#include <sys/socket.h>
#include <string.h>
#include <netdb.h>
#include <thread>
#include <atomic>

using ns_t = std::chrono::nanoseconds;

class SocketMethods{
public:
	SocketMethods(std::string addr,dronePosVec::progName clientName)
	:addr_(addr)
    ,clientName_()
	{
		clientAddrLen_ = sizeof(clientAddr_);
	}
	//virtual ssize_t initRecv() = 0;
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
    int socketShutdown();


protected:
	ns_t serverTimer_ = ns_t(0);
    ns_t timerOffset_;
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
    const dronePosVec::progName clientName_;
}; //SocketMethods

class ClientClass : protected SocketMethods
{
public:
    ClientClass(std::string addr,dronePosVec::progName clientName,std::string serverAddr,int serverPort)
    :SocketMethods(addr, clientName)
    ,serverAddr_(serverAddr)
    ,serverPort_(serverPort)
    {
        socketSetup_(0);
    }
    int connectServer();
    int checkList();

private:
    //int connectNewServer_();
    int statechange_();
    struct sockaddr nextAddress_;
    socklen_t nextAddrLen_ = 0;
    const int serverPort_;

    void getNextAddr_();
    int cSyncTime_();
    const std::string serverAddr_;

    ssize_t initSend(char* msg, size_t msgLen);
    const size_t bufferSize_ = 1024;
    char recvMsg_[1024]; //fix to bufferSize_
	char sendMsg_[1024]; // do these really need to be that big?

    std::thread tRecv_;
	std::thread tSend_;
	std::atomic_bool threadloop_ = true;//this needs to be atomic as it may be chagned and read at the same time at program end

}; //ClientClass

#endif //ARENAHEADER_H