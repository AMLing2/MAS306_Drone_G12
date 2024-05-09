#include "dronePosVec.pb.h"
#include "arenaServer.h"
#include <chrono>
#include <iostream>
#include <string.h>
#include <sys/socket.h>
#include <netdb.h>
#include <thread>
//#include <sys/time.h> //for timeval struct, not needed? chrono is better

/*
OUTDATED OUTDATED OUTDATED OUTDATED OUTDATED OUTDATED OUTDATED OUTDATED OUTDATED OUTDATED OUTDATED 
OUTDATED OUTDATED OUTDATED OUTDATED OUTDATED OUTDATED OUTDATED OUTDATED OUTDATED OUTDATED OUTDATED 
OUTDATED OUTDATED OUTDATED OUTDATED OUTDATED OUTDATED OUTDATED OUTDATED OUTDATED OUTDATED OUTDATED 
OUTDATED OUTDATED OUTDATED OUTDATED OUTDATED OUTDATED OUTDATED OUTDATED OUTDATED OUTDATED OUTDATED 
*/




/* shove all this stuff below somewhere else*/

class ArenaServer : public ClientSocket
{
public:
	ArenaServer(const std::string& addr, int port)
	:f_port(port)
	,f_addr(addr)
	,monoTimeStart_(
			std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::steady_clock::now().time_since_epoch()))
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
	virtual void setTimeout(const long int sec,const long int nanoSec) override;
	int connectInit();
	virtual	ssize_t recvDrone(char* buffer,size_t bufferLen) override;
	void mainloop();
	int cSyncTime(dronePosVec::dataTransfers* pdroneInfoMsg);
	void tempReader();
	
private:
	std::chrono::nanoseconds monoTimeNow_();
	std::chrono::nanoseconds calcSleepTime_(int interval);

	int f_port;
	std::string f_addr;
	struct addrinfo* f_addrinfo;
	int f_socket;
	struct sockaddr clientAddr_;
	socklen_t clientsocklen_;
	const size_t bufferLen_ = 1024;
	char* genBuffer_;
	const std::chrono::nanoseconds monoTimeStart_;
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
	ssize_t msgRecvLen = 0;
	dronePosVec::dataTransfers droneInfoMsg;
	while(true)
	{
		setTimeout(30,0);
		std::cout<<"listening to drone input"<<std::endl;
		msgRecvLen = recvDrone(genBuffer_,bufferLen_);
		if(msgRecvLen == -1)
		{
			break;
		}
		droneInfoMsg.ParseFromArray(genBuffer_,msgRecvLen);
		std::cout<<"msg recv: "<<droneInfoMsg.msg()<<std::endl;
		switch(droneInfoMsg.type())
		{
			case dronePosVec::timeSync:
				{
					std::cout<<"timesync req"<<std::endl;
					cSyncTime(&droneInfoMsg);
					break;
				}
			case dronePosVec::socketInfo:
				{

					break;
				}
			case dronePosVec::stateChange:
				{
					std::cout<<"statechange req"<<std::endl;
					if(true) //should reallly change this to an enum
					{
						tempReader();
					}
					else
					{
						std::cout<<"unexpected request"<<std::endl;
					}
					break;
				}
			default:
				{
					break;
				}
		}
	}
}

int ArenaServer::cSyncTime(dronePosVec::dataTransfers* pdroneInfoMsg) //sync client's time to server
{
	long int intervalTime = 100000000; //100ms
	pdroneInfoMsg->Clear();
	pdroneInfoMsg->set_id(1);
	pdroneInfoMsg->set_msg("timesyncReq");
	pdroneInfoMsg->set_type(dronePosVec::timeSync);
	pdroneInfoMsg->set_timesync_ns(monoTimeStart_.count()); //this is pointless
	pdroneInfoMsg->SerializeToArray(genBuffer_,bufferLen_);
	setTimeout(1,0);

	std::chrono::nanoseconds sendTime = calcSleepTime_(intervalTime); //send next relaive 100ms
	std::this_thread::sleep_for(sendTime);
	sendDrone(genBuffer_,pdroneInfoMsg->ByteSizeLong());
	sendTime = monoTimeNow_();
	
	ssize_t msgLen = recvDrone(genBuffer_,bufferLen_); //read
	if (msgLen > 0)
	{
		pdroneInfoMsg->Clear();
		pdroneInfoMsg->ParseFromArray(genBuffer_,msgLen);
		std::chrono::nanoseconds recvTime = monoTimeNow_();
		long int timeDiff = (recvTime - sendTime).count() - intervalTime;
		std::cout<<"time difference = "<<timeDiff<<std::endl;
		pdroneInfoMsg->set_id(1);
		pdroneInfoMsg->set_timesync_ns(timeDiff);
		pdroneInfoMsg->SerializeToArray(genBuffer_,bufferLen_);
		sendDrone(genBuffer_,pdroneInfoMsg->ByteSizeLong());//adding an offset might be wierd, 0.2-0.7ms delay without it, but more consistent(?) and less delay with
	}
	return 0;
}

std::chrono::nanoseconds ArenaServer::calcSleepTime_(int interval)
{
	return std::chrono::nanoseconds(interval) -((monoTimeNow_() - monoTimeStart_) % interval);
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
	std::cout<<"connection r = "<<r<<std::endl;
	if(r == 0)
	{
		std::string msgInit = "message sent";
		sendDrone(msgInit.c_str(),msgInit.length());
		std::cout<<"msg to drone sent: "<<msgInit<<std::endl;
		setTimeout(30,0);
		recvDrone(genBuffer_,bufferLen_); //just read to clear buffer
		std::cout<<"msg recv:"<<genBuffer_<<std::endl;
	}
	return r;
}

int ArenaServer::sendDrone(const char* msg,size_t msglen)
{
	send(f_socket,msg,msglen,0);
	return 0; //should ideally check if length sent = length of msg
}

ssize_t ArenaServer::recvDrone(char* buffer,size_t bufferLen)
{
	ssize_t recvLen = recv(f_socket, buffer, bufferLen,0);
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

std::chrono::nanoseconds ArenaServer::monoTimeNow_() //gives time of start of monotonic clock, NOT unix time (ie. 1970)
{
	return std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::steady_clock::now().time_since_epoch());
}

void ArenaServer::tempReader()
{
	int interval = 20000000;
	long recvTime = 0;
	long expTime = 0;

	setTimeout(1,0);
	std::cout<<"starting reading"<<std::endl;
	std::string msgtemp = "aa";
	sendDrone(msgtemp.c_str(),msgtemp.length());
	for(int i = 0;i<11;i++)
	{
		expTime = (monoTimeNow_() + calcSleepTime_(interval)).count();
		std::cout<<"expected recv time: "<<expTime<<std::endl;
		recvDrone(genBuffer_,bufferLen_);
		recvTime = monoTimeNow_().count();
		std::cout<<"actual recv time: "<<recvTime<<" difference = "<<recvTime - expTime <<std::endl;
	}
	std::cout<<"reading finished"<<std::endl;
}
