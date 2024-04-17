#include "dronePosVec.pb.h"
#include "droneClient.h"
#include <chrono>
#include <iostream>
#include <string.h>
#include <sys/socket.h>
#include <netdb.h>
#include <thread>
#include <netinet/in.h> //for struct sockaddr_in
#include <arpa/inet.h> //for htons()
#include <cerrno>

//#include <sys/time.h> //for timeval struct, not needed? chrono is better

class DroneClient : public ClientSocket
{
public:
	DroneClient(const std::string& serverAddr, int serverPort)
	:server_port(serverPort)
	,server_addr(serverAddr)
	{
		genBuffer_ = new char[bufferLen_];
		char decimal_port[16];
		snprintf(decimal_port, 16, "%d", server_port);
		decimal_port[15] = '\0';
		/*
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
		*/
		f_socket = socket(AF_INET, SOCK_DGRAM | SOCK_CLOEXEC, IPPROTO_UDP);
		if(f_socket == 1)
		{
			std::cout<<"failed to create udp socket"<<std::endl;
		}
		/*
		r = bind(f_socket, f_addrinfo->ai_addr,f_addrinfo->ai_addrlen);
		if(r != 0)
		{
			std::cout<<"failed to bind socket"<<std::endl;
		}
		*/
	}
	~DroneClient()
	{
		delete[] genBuffer_;
		//freeaddrinfo(f_addrinfo);
		close(f_socket);
	}

	virtual int dServerConnect() override;
	virtual ssize_t initSend(char* msg, size_t msgLen) override;
	virtual socklen_t getclient(struct sockaddr* clientAddr) override;
	virtual ssize_t sendServer(const char* msg,size_t msglen) override;
	virtual void setTimeout() override;
	virtual void setTimeout(const long int sec,const long int nanoSec) override;
	int connectInit();
	virtual	ssize_t recvServer(char* buffer,size_t bufferLen) override;
	void mainloop();
	int cSyncTime(dronePosVec::dataTransfers* pdroneInfoMsg);
	
	std::chrono::nanoseconds getMonoServerTime();

private:
	std::chrono::nanoseconds monoTimeNow_();
	std::chrono::nanoseconds calcSleepTime_(int interval);

	int server_port;
	std::string server_addr;
	//struct addrinfo* f_addrinfo;
	int f_socket;
	struct sockaddr clientAddr_;
	socklen_t clientsocklen_;
	const size_t bufferLen_ = 1024;
	char* genBuffer_;
	std::chrono::nanoseconds monoServerTime_ = std::chrono::nanoseconds(0);
	std::chrono::nanoseconds timerOffset_ = std::chrono::nanoseconds(0);
};


//-------------------MAIN-------------------@
int main()
{
	std::string serverAddr = "128.39.200.239";
	int serverPort = 20002;
	DroneClient droneClient(serverAddr,serverPort);
	if (droneClient.dServerConnect() == 0)
	{
		std::cout<<"connection sucess"<<std::endl;
		//checklist before starting streams
		dronePosVec::dataTransfers dataMsg;
		bool checklist = true;
		while(checklist)
		{
			if (droneClient.getMonoServerTime().count() == 0)
			{
				droneClient.cSyncTime(&dataMsg);
			}
			else
			{
				std::cout<<"checklist complete, starting program"<<std::endl;
				checklist = false;
			}
		}
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
void DroneClient::mainloop()
{
	//would be nice to have two sockets, one for udp streaming and one tcp for other communication such as this
	ssize_t msgRecvLen = 0;
	dronePosVec::dataTransfers droneInfoMsg;
	while(true)
	{
		setTimeout(60,0);
		std::cout<<"listening to drone input"<<std::endl;
		msgRecvLen = recvServer(genBuffer_,bufferLen_);
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
					break;
				}
			default:
				{
					break;
				}
		}
	}
}

int DroneClient::dServerConnect()
{
	std::string msg = "hello";
	char* msgBuffer = new char[msg.length()];
	memcpy(msgBuffer, msg.c_str(),msg.length());
	setTimeout(3,0);
	struct sockaddr serverAddress;
	memset(&serverAddress,0,sizeof(serverAddress));
	int r = 0;
	socklen_t serverAddrLen;
	for(int i = 0; i < 10; i++)
	{
		initSend(msgBuffer,msg.length());
		try
		{
			r = recvfrom(f_socket,genBuffer_,bufferLen_,0,&serverAddress,&serverAddrLen);
			if (r >= 0)
			{
				std::cout<<"msg recv: "<<genBuffer_<<std::endl;
			}
			throw(r);
		}
		catch(int rVal)
		{
			if (rVal > 0)
			{
				break;
			}
			else
			{
				std::cout<<"timeout, retrying: " <<i<<", errno = "<<strerror(errno)<<", rVal = "<<rVal<<std::endl;
			}
		}
	}
	delete[] msgBuffer;
	if(r < 0)
	{
		return -1;
	}
	else
	{
		int connstat = connect(f_socket,&serverAddress,serverAddrLen);
		if (connstat == 0)
		{
		
			std::cout<<"sendServer: "<<sendServer(genBuffer_,sizeof(genBuffer_))<<" errno: "<< strerror(errno)<<std::endl;
		}
		return connstat;
	}
}

int DroneClient::cSyncTime(dronePosVec::dataTransfers* pdroneInfoMsg) //sync client's time to server
{
	long int intervalTime = 100000000; //100ms
	std::cout<<"timer missing, getting"<<std::endl;
	pdroneInfoMsg->Clear();
	pdroneInfoMsg->set_id(2);//2 for drone probably
	pdroneInfoMsg->set_type(dronePosVec::timeSync);
	std::string msg = "syncReq";
	pdroneInfoMsg->set_msg(msg);
	pdroneInfoMsg->SerializeToArray(genBuffer_,bufferLen_);
	
	setTimeout();
	sendServer(genBuffer_,pdroneInfoMsg->ByteSizeLong());
	pdroneInfoMsg->Clear();
	ssize_t msgLen = recvServer(genBuffer_,bufferLen_);
	pdroneInfoMsg->ParseFromArray(genBuffer_,msgLen);
	monoServerTime_ = monoTimeNow_();

	pdroneInfoMsg->set_id(2);
	pdroneInfoMsg->SerializeToArray(genBuffer_,bufferLen_);
	size_t byteSize = pdroneInfoMsg->ByteSizeLong();

	std::this_thread::sleep_for(calcSleepTime_(intervalTime));
	sendServer(genBuffer_,byteSize);

	msgLen = recvServer(genBuffer_,bufferLen_);
	pdroneInfoMsg->ParseFromArray(genBuffer_,msgLen);
	timerOffset_ = std::chrono::nanoseconds(pdroneInfoMsg->timesync_ns());

	monoServerTime_ = monoServerTime_ - timerOffset_;
	std::cout<<"offset: "<< timerOffset_.count()<<std::endl;
	return 0;
}

/*
int DroneClient::cSyncTime(dronePosVec::dataTransfers* pdroneInfoMsg) //sync client's time to server
{
	long int intervalTime = 100000000; //100ms
	pdroneInfoMsg->Clear();
	pdroneInfoMsg->set_id(1);
	pdroneInfoMsg->set_type(dronePosVec::timeSync);
	pdroneInfoMsg->set_timesync_ns(monoServerTime_.count()); //this is pointless
	pdroneInfoMsg->SerializeToArray(genBuffer_,bufferLen_);
	setTimeout(1,0);

	std::chrono::nanoseconds sendTime = calcSleepTime_(intervalTime); //send next relaive 100ms
	std::this_thread::sleep_for(sendTime);
	sendServer(genBuffer_,pdroneInfoMsg->ByteSizeLong());
	sendTime = monoTimeNow_();
	
	ssize_t msgLen = recvServer(genBuffer_,bufferLen_); //read
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
		sendServer(genBuffer_,pdroneInfoMsg->ByteSizeLong());//adding an offset might be wierd, 0.2-0.7ms delay without it
	}
	return 0;
}
*/
std::chrono::nanoseconds DroneClient::calcSleepTime_(int interval)
{
	return std::chrono::nanoseconds(interval) -((monoTimeNow_() - monoServerTime_) % interval);
}

socklen_t DroneClient::getclient(struct sockaddr* clientAddr)
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

int DroneClient::connectInit()
{
	clientsocklen_ = getclient(&clientAddr_);
	int r = connect(f_socket,&clientAddr_,clientsocklen_);
	std::cout<<r<<std::endl;
	if(r == 0)
	{
		std::string msgInit = "message sent";
		std::cout<<msgInit.length()<<std::endl;
		sendServer(msgInit.c_str(),msgInit.length());
		setTimeout(3,0);
		recvServer(genBuffer_,bufferLen_); //just read to clear buffer
	}
	return r;
}

ssize_t DroneClient::sendServer(const char* msg,size_t msglen)
{
	return send(f_socket,msg,msglen,0);//should ideally check if length sent = length of msg
}

ssize_t DroneClient::initSend(char* msg, size_t msgLen)
{
	struct sockaddr_in arenaServerAddr;
	memset(&arenaServerAddr,0,sizeof(arenaServerAddr)); //probably pointless
	arenaServerAddr.sin_family = AF_INET;
	arenaServerAddr.sin_port = htons(server_port); //converts to network byte order
	inet_pton(arenaServerAddr.sin_family, server_addr.c_str(), &(arenaServerAddr.sin_addr));
	memset(&arenaServerAddr.sin_zero,0,sizeof(arenaServerAddr.sin_zero));
	struct sockaddr* sockaddrServer = reinterpret_cast<sockaddr*>(&arenaServerAddr);
	return sendto(f_socket,msg,msgLen,0,sockaddrServer,sizeof(arenaServerAddr));
}

ssize_t DroneClient::recvServer(char* buffer,size_t bufferLen)
{
	ssize_t recvLen = recv(f_socket, buffer, bufferLen,0);
	buffer[recvLen] = '\0';
	return recvLen;
}

void DroneClient::setTimeout()
{
	struct timeval noTimeout;
	noTimeout.tv_sec = 0;
	noTimeout.tv_usec = 0;
	setsockopt(f_socket,SOL_SOCKET,SO_RCVTIMEO,&noTimeout,sizeof(noTimeout));
}

void DroneClient::setTimeout(const long int sec,const long int microSec)
{
	struct timeval timeoutTime;
	timeoutTime.tv_sec = sec;
	timeoutTime.tv_usec = microSec;
	setsockopt(f_socket,SOL_SOCKET,SO_RCVTIMEO,&timeoutTime,sizeof(timeoutTime));
}

std::chrono::nanoseconds DroneClient::monoTimeNow_() //gives time of start of monotonic clock, NOT unix time (ie. 1970)
{
	return std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::steady_clock::now().time_since_epoch());
}

std::chrono::nanoseconds DroneClient::getMonoServerTime()
{
	return monoServerTime_;
}
