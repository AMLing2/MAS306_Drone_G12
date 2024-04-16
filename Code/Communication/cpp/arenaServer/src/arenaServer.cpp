#include "dronePosVec.pb.h"
#include "droneClient.h"
#include <chrono>
#include <iostream>
#include <string.h>
#include <sys/socket.h>
#include <netdb.h>
#include <sys/time.h>

class ArenaServer : public ClientSocket
{
public:
	ArenaServer(const std::string& addr, int port)
	:f_port(port)
	,f_addr(addr)
	{
		genBuffer_ = new char[bufferLen_];
		char decimal_port[16];
		snprintf(decimal_port, 16, "%d", f_port);
		decimal_port[15] = '\0';
		struct addrinfo hints;
		memset(&hints,0,sizeof(hints));
		hints.ai_family = AF_INET;
		hints.ai_socktype = SOCK_DGRAM;
		hints.ai_protocol = IPPROTO_UDP;
		int r = getaddrinfo(f_addr.c_str(), decimal_port, &hints, &f_addrinfo);
		if(r != 0 || f_addrinfo == NULL)
		{
			std::cout<<"invalid addres or port"<<std::endl;
		}
		else
		{
			std::cout<<"ai_addr: "<< f_addrinfo->ai_addr <<std::endl;
		}
		f_socket = socket(f_addrinfo->ai_family, SOCK_DGRAM | SOCK_CLOEXEC, IPPROTO_UDP);
		if(f_socket == 1)
		{
			std::cout<<"failed to create udp socket"<<std::endl;
		}
		r = bind(f_socket, f_addrinfo->ai_addr,f_addrinfo->ai_addrlen);
		if(r != 0)
		{
			std::cout<<"failed to bind socket"<<std::endl;
		}
	}
	~ArenaServer()
	{
		delete[] genBuffer_;
		freeaddrinfo(f_addrinfo);
		close(f_socket);
	}

	virtual socklen_t getclient(struct sockaddr* clientAddr) override;
	virtual int sendDrone(const char* msg,size_t msglen) override;
	virtual void setTimeout() override;
	virtual void setTimeout(const long int sec,const long int microSec) override;
	int connectInit();
	virtual	size_t recvDrone(char* buffer,size_t bufferLen) override;
	void mainloop();
	
private:
int f_port;
std::string f_addr;
struct addrinfo* f_addrinfo;
int f_socket;
struct sockaddr clientAddr_;
socklen_t clientsocklen_;
const size_t bufferLen_ = 1024;
char* genBuffer_;
};

//-------------------MAIN-------------------@
int main()
{
	std::string addrS = "128.39.200.239";
	int port = 20002;
	ArenaServer arenaServer(addrS,port);
	if (arenaServer.connectInit() == 0)
	{
		arenaServer.mainloop();
	}
	else
	{
		std::cout<<"connection error"<<std::endl;
	}
	std::cout<<"press to exit"<<std::endl;
	int a;
	std::cin>>a;
	return 0;
}

//--------------class definitions-------------@
void ArenaServer::mainloop()
{
	//would be nice to have two sockets, one for udp streaming and one tcp for other communication such as this
	setTimeout();
	size_t msgRecvLen = 0;
	dronePosVec::dataTransfers droneInfoMsg;
	while(true)
	{
		msgRecvLen = recvDrone(genBuffer_,bufferLen_);
		droneInfoMsg.ParseFromArray(genBuffer_,msgRecvLen);
		switch(droneInfoMsg.type())
		{
			case dronePosVec::timeSync:
				{

					break;
				}
			case dronePosVec::socketInfo:
				{

					break;
				}
			case dronePosVec::stateChange:
				{

					break;
				}
			default:
				{
					break;
				}
		}


	}
}

socklen_t ArenaServer::getclient(struct sockaddr* clientAddr)
{
	const size_t bufferlen = 1024;
	char* buffer = new char[bufferlen];
	socklen_t addrlen;
	setTimeout(30,0);
	ssize_t msglen = 0;
	msglen = recvfrom(this->f_socket,buffer,bufferlen,0,clientAddr,&addrlen);
	buffer[msglen] = '\0';
	if (msglen > 0)
	{std::cout<<"msg recieved: "<<buffer<<std::endl;}
	//msg is ignored for now, only want address of sender
	delete[] buffer;
	return addrlen;
}

int ArenaServer::connectInit()
{
	clientsocklen_ = getclient(&clientAddr_);
	int r = connect(f_socket,&clientAddr_,clientsocklen_);
	std::cout<<r<<std::endl;
	if(r == 0)
	{
		std::string msgInit = "message sent";
		std::cout<<msgInit.length()<<std::endl;
		sendDrone(msgInit.c_str(),msgInit.length());
		setTimeout(3,0);
		recvDrone(genBuffer_,bufferLen_); //just read to clear buffer
	}
	return r;
}

int ArenaServer::sendDrone(const char* msg,size_t msglen)
{
	send(f_socket,msg,msglen,0);
	return 0; //should ideally check if length sent = length of msg
}

size_t ArenaServer::recvDrone(char* buffer,size_t bufferLen)
{
	size_t recvLen = recv(f_socket, buffer, bufferLen,0);
	buffer[recvLen] = '\0';
	return recvLen;
}

void ArenaServer::setTimeout()
{
	struct timeval noTimeout;
	noTimeout.tv_sec = 0;
	noTimeout.tv_usec = 0;
	setsockopt(f_socket,SOL_SOCKET,SO_RCVTIMEO,&noTimeout,sizeof(noTimeout));
}

void ArenaServer::setTimeout(const long int sec,const long int microSec)
{
	struct timeval timeoutTime;
	timeoutTime.tv_sec = sec;
	timeoutTime.tv_usec = microSec;
	setsockopt(f_socket,SOL_SOCKET,SO_RCVTIMEO,&timeoutTime,sizeof(timeoutTime));
}
