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

//--------------class definitions-------------@

int DroneClient::dServerConnect()
{
	std::string msg = "hello";
	char* msgBuffer = new char[msg.length()];
	memcpy(msgBuffer, msg.c_str(),msg.length());
	setTimeout(3,0);
	struct sockaddr serverAddress;
	socklen_t serverAddrLen = sizeof(serverAddress);
	//memset(&serverAddress,0,sizeof(serverAddress)); //dont do this, because i can make connect() attempt to connect to ip "0.0.0.0:0" which will return 0....
	
	int r = 0;
	for(int i = 0; i < 10; i++)
	{
		initSend(msgBuffer,msg.length());
		r = recvfrom(f_socket,genBuffer_,bufferLen_,0,&serverAddress,&serverAddrLen);
		if (r > 0)
		{
			std::cout<<"msg recv: "<<genBuffer_<<std::endl;
			break;
		}
		else
		{
			std::cout<<"timeout, retrying: " <<i<<", errno = "<<strerror(errno)<<", rVal = "<<r<<std::endl;
		}
	}
	delete[] msgBuffer;
	//std::cout<<"r: "<<r<<" serverAddrLen: "<<serverAddrLen<<" serverAddress: "<<serverAddress.sa_data<<" recv family: "<<serverAddress.sa_family<<std::endl;
	if(r < 0)
	{
		return -1;
	}
	else
	{
		int connstat = connect(f_socket,&serverAddress,serverAddrLen);
		if (connstat == 0)
		{
			sendServer(genBuffer_,r); //echo
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

	std::this_thread::sleep_for(calcSleepTime(intervalTime));
	sendServer(genBuffer_,byteSize);

	msgLen = recvServer(genBuffer_,bufferLen_);
	pdroneInfoMsg->ParseFromArray(genBuffer_,msgLen);
	timerOffset_ = std::chrono::nanoseconds(pdroneInfoMsg->timesync_ns());

	monoServerTime_ = monoServerTime_ - timerOffset_;
	std::cout<<"offset: "<< timerOffset_.count()<<std::endl;
	return 0;
}

std::chrono::nanoseconds DroneClient::calcSleepTime(int interval)
{
	return std::chrono::nanoseconds(interval) -((monoTimeNow_() - monoServerTime_) % interval);
}

ssize_t DroneClient::sendServer(const char* msg,const size_t msglen) const
{
	return send(f_socket,msg,msglen,0);
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
