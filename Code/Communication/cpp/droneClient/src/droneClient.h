#ifndef SOCKETCLASS_H
#define SOCKETCLASS_H

#include "dronePosVec.pb.h"
#include <chrono>
#include <iostream>
#include <string.h>
#include <sys/socket.h>
#include <netdb.h>
#include <thread>
#include <netinet/in.h> //for struct sockaddr_in
#include <arpa/inet.h> //for htons()
#include <cerrno>

//float sleepTimeCalc(int interval,high_resolution_clock::time_point timer);

class IClientSocket {
public:
	virtual ~IClientSocket() = default;
	virtual int dServerConnect() = 0;
	virtual ssize_t initSend(char* msg, size_t msgLen) =0;
	virtual ssize_t sendServer(const char* msg,size_t msglen) const = 0;//might rename
	virtual ssize_t recvServer(char* buffer, size_t buffernLen) = 0;
	virtual void setTimeout() = 0;
	virtual void setTimeout(const long int sec,const long int microSec) = 0;
	virtual int cSyncTime(dronePosVec::dataTransfers* pdroneInfoMsg) = 0;
};

class DroneClient : public IClientSocket
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
		f_socket = socket(AF_INET, SOCK_DGRAM | SOCK_CLOEXEC, IPPROTO_UDP);
		if(f_socket == 1)
		{
			std::cout<<"failed to create udp socket"<<std::endl;
		}
	}
	~DroneClient()
	{
		delete[] genBuffer_;
		//freeaddrinfo(f_addrinfo);
		close(f_socket);
	}

	virtual int dServerConnect() override;
	virtual ssize_t initSend(char* msg, size_t msgLen) override;
	virtual ssize_t sendServer(const char* msg,size_t msglen) const override;
	virtual	ssize_t recvServer(char* buffer,size_t bufferLen) override;
	virtual void setTimeout() override;
	virtual void setTimeout(const long int sec,const long int nanoSec) override;
	virtual int cSyncTime(dronePosVec::dataTransfers* pdroneInfoMsg) override;
	void mainloop(dronePosVec::dataTransfers* pDataMsgs);

	
	std::chrono::nanoseconds getMonoServerTime();
	std::chrono::nanoseconds calcSleepTime(int interval);

private:
	std::chrono::nanoseconds monoTimeNow_();

	int server_port;
	std::string server_addr;
	int f_socket;
	struct sockaddr clientAddr_;
	socklen_t clientsocklen_;
	const size_t bufferLen_ = 1024;
	char* genBuffer_;
	std::chrono::nanoseconds monoServerTime_ = std::chrono::nanoseconds(0);
	std::chrono::nanoseconds timerOffset_ = std::chrono::nanoseconds(0);
};

//-----------------------------------------Thread classes --------------

class Ithreads
{
public:
	virtual ~Ithreads() = default;

	virtual void startThread() = 0;
	virtual void joinThread() = 0;

protected:
	const size_t tBufferLen_ = 1024;
	char* tBuffer_;
	bool threadLoop_ = true;
	std::thread thread_;
	size_t msgbufferSize_ = 0;
};

class TcIMUStream : protected Ithreads
{
public:
	TcIMUStream(int interval, DroneClient* clientClass)
	:interval_(interval)
	,clientClass_(clientClass)
	{
		tBuffer_ = new char[tBufferLen_];
		pIMUmsg_ = std::make_unique<dronePosVec::dronePosition>();
	}
	~TcIMUStream()
	{
		delete[] tBuffer_;
	}
	void tSendIMUStream(std::unique_ptr<dronePosVec::dronePosition> pIMUmsg);
	void startThread()
	{
		thread_ = std::thread([this]()
		{
			tSendIMUStream(std::move(pIMUmsg_));
		});
		thread_.detach();
	}
	
	void joinThread()
	{
		if(thread_.joinable())
		{
			thread_.join();
		}
	}

private:
	std::unique_ptr<dronePosVec::dronePosition> pIMUmsg_;
	const long int interval_; //not sure if it would be nice to be able to update on runtime
	DroneClient* clientClass_;
};


class TcMotorStream : protected Ithreads
{
public:
	TcMotorStream(int interval, DroneClient* clientClass)
	:interval_(interval)
	,clientClass_(clientClass)
	{
		tBuffer_ = new char[tBufferLen_];
		pMotormsg_ = std::make_unique<dronePosVec::droneControl>();
	}
	~TcMotorStream()
	{
		delete[] tBuffer_;
	}

	void tRecvMotorStream(std::unique_ptr<dronePosVec::droneControl> pMotormsg);
	void startThread()
	{
		thread_ = std::thread([this]()
		{
			tRecvMotorStream(std::move(pMotormsg_));
		});
		thread_.detach();
	}
	
	void joinThread()
	{
		if(thread_.joinable())
		{
			thread_.join();
		}
	}

private:
	std::unique_ptr<dronePosVec::droneControl> pMotormsg_;
	const long int interval_; //not sure if it would be nice to be able to update on runtime
	DroneClient* clientClass_;
};

#endif //SOCKETCLASS_H
