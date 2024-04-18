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
	:server_port_(serverPort)
	,server_addr_(serverAddr)
	{
		genBuffer_ = new char[bufferLen_];
		char decimal_port[16];
		snprintf(decimal_port, 16, "%d", server_port_);
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
		if(f_socket == -1)
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
	virtual ssize_t sendServer(const char* msg,size_t msglen) const override;
	virtual void setTimeout() override;
	virtual void setTimeout(const long int sec,const long int nanoSec) override;
	int connectInit();
	virtual	ssize_t recvServer(char* buffer,size_t bufferLen) override;
	void mainloop(dronePosVec::dataTransfers* pDataMsgs);
	int cSyncTime(dronePosVec::dataTransfers* pdroneInfoMsg);
	
	std::chrono::nanoseconds getMonoServerTime();
	std::chrono::nanoseconds calcSleepTime(int interval);



private:
	std::chrono::nanoseconds monoTimeNow_();
	int f_socket;
	int server_port_;
	std::string server_addr_;
	//struct addrinfo* f_addrinfo;
	struct sockaddr clientAddr_;
	socklen_t clientsocklen_;
	const size_t bufferLen_ = 1024;
	char* genBuffer_;
	std::chrono::nanoseconds monoServerTime_ = std::chrono::nanoseconds(0);
	std::chrono::nanoseconds timerOffset_ = std::chrono::nanoseconds(0);
};

class TcIMUStream
{
public:
	TcIMUStream(int interval, DroneClient* clientClass)
	:interval_(interval)
	,clientClass_(clientClass)
	{
		imuBuffer_ = new char[imuBufferLen_];
		pIMUmsg_ = std::make_unique<dronePosVec::dronePosition>();
	}
	~TcIMUStream()
	{
		delete[] imuBuffer_;
	}
	void tSendIMUStream(std::unique_ptr<dronePosVec::dronePosition> pIMUmsg);
	void tstartIMUthread()
	{
		thread_imu_ = std::thread([this]()
		{
			tSendIMUStream(std::move(pIMUmsg_));
		});
		thread_imu_.detach();
	}
	
	void joinThread()
	{
		
		if(thread_imu_.joinable())
		{
			thread_imu_.join();
		}
	}

private:
	const size_t imuBufferLen_ = 1024;
	char* imuBuffer_;
	//std::string strBuffer_;
	const int interval_; //not sure if it would be nice to be able to update on runtime
	DroneClient* clientClass_; //pointer is const, cant be used to write to class, preventing read and write at the same time
	bool threadLoop_ = true;
	std::thread thread_imu_;
	size_t msgbufferSize_ = 0;
	std::unique_ptr<dronePosVec::dronePosition> pIMUmsg_;
	//dronePosVec::dronePosition imuProtoc_;

};

/*
class TcMotorStream
{
public:
	TcMotorStream(int interval, DroneClient* clientClass)
	:interval_(interval)
	,clientClass_(clientClass)
	{
		motorBuffer_ = new char[motorBufferLen_];
	}
	~TcMotorStream()
	{
		delete[] motorBuffer_;
	}
	void tRecvMotorStream();

private:
	const size_t motorBufferLen_ = 1024;
	char* motorBuffer_;
	const int interval_; //not sure if it would be nice to be able to update on runtime
	DroneClient* clientClass_; //pointer is const, cant be used to write to class, preventing read and write at the same time
};
*/

//-------------------MAIN-------------------@
int main()
{
	dronePosVec::dataTransfers dataMsg;
	const std::string serverAddr = "128.39.200.239";
	const int serverPort = 20002;
	DroneClient droneClient(serverAddr,serverPort);
	if (droneClient.dServerConnect() == 0)
	{
		std::cout<<"connection sucess"<<std::endl;
		//checklist before starting streaming
		bool checklist = true;
		while(checklist)
		{
			if (droneClient.getMonoServerTime().count() == 0)
			{
				droneClient.cSyncTime(&dataMsg);
			}
			else
			{
				std::cout<<"checklist complete"<<std::endl;
				checklist = false;
			}
		}
		droneClient.mainloop(&dataMsg);
	}
	else
	{
		std::cout<<"connection error"<<std::endl;
	}
	/*
	std::cout<<"press to exit"<<std::endl;
	int a;
	std::cin>>a;
	*/
	return 0;
}

//--------------thread functions--------------@

void TcIMUStream::tSendIMUStream(std::unique_ptr<dronePosVec::dronePosition> pIMUmsg)
{
	//std::cout<<"f_socket: "<<clientClass_->f_socket<<std::endl;
	int tempi = 0; //temporary
	const int value_size = 3;
	float value[value_size] = {1.1,1.2,1.3};
	//google::protobuf::RepeatedField<float>* mpPos = imuProtoc_.mutable_position();
	std::cout<<"IMUthread running"<<std::endl;
	while(threadLoop_)
	{
		pIMUmsg->Clear();
		pIMUmsg->set_devicetype(dronePosVec::IMUonly);
		std::cout<<"adding to repeated pos"<<std::endl;
		std::cout<<"str print: "<<pIMUmsg->devicetype()<<std::endl;
		pIMUmsg->mutable_position()->Add(value[0]);
		msgbufferSize_ = pIMUmsg->ByteSizeLong();
		std::cout<<"serializing"<<std::endl;
		std::cout<<"pos size: "<<pIMUmsg->position_size()<<std::endl;
		pIMUmsg->SerializeToArray(imuBuffer_,imuBufferLen_);
		
		//SEND
		std::this_thread::sleep_for(clientClass_->calcSleepTime(interval_));
		std::cout<<"sending"<<std::endl;
		clientClass_->sendServer(imuBuffer_,msgbufferSize_);
		std::cout<<"msg sent"<<std::endl;
		std::cout<<tempi<<std::endl;
		tempi++; //run 10 times
		if(tempi >= 10)
		{
			threadLoop_ = false;
		}
	}
}
/*
void TcMotorStream::tRecvMotorStream()
{

}
*/
//--------------class definitions-------------@
void DroneClient::mainloop(dronePosVec::dataTransfers* pDataMsgs) //update for drone, change to reference
{
	//would be nice to have two sockets, one for udp streaming and one tcp for other communication such as this
	//request for server to start reading streams
	pDataMsgs->set_id(2);
	pDataMsgs->set_type(dronePosVec::stateChange);
	std::string stateMsg = "start";
	pDataMsgs->set_msg(stateMsg);
	pDataMsgs->SerializeToArray(genBuffer_,pDataMsgs->ByteSizeLong());
	sendServer(genBuffer_,pDataMsgs->ByteSizeLong());
	//wait for recv from server
	setTimeout();
	pDataMsgs->Clear();
	ssize_t recvSize = recvServer(genBuffer_,bufferLen_);
	pDataMsgs->ParseFromArray(genBuffer_,recvSize);
	//could probably read what it is but recvining it means its probably fine

	//start threads
	TcIMUStream imuStream(20000000,this);	
	imuStream.tstartIMUthread();
	std::cout<<"waiting for imu thread to end"<<std::endl;
	//imuStream.joinThread();
	while (true)
	{
	}
	
}

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

	monoServerTime_ = monoServerTime_ - timerOffset_; //wierd
	std::cout<<"offset: "<< timerOffset_.count()<<std::endl;
	return 0;
}

std::chrono::nanoseconds DroneClient::calcSleepTime(int interval)
{
	return std::chrono::nanoseconds(interval) -((monoTimeNow_() - monoServerTime_) % interval);
}

socklen_t DroneClient::getclient(struct sockaddr* clientAddr)//unused
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

int DroneClient::connectInit() //unused
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

ssize_t DroneClient::sendServer(const char* msg,const size_t msglen) const
{
	return send(f_socket,msg,msglen,0);
}

ssize_t DroneClient::initSend(char* msg, size_t msgLen)
{
	struct sockaddr_in arenaServerAddr;
	memset(&arenaServerAddr,0,sizeof(arenaServerAddr)); //probably pointless
	arenaServerAddr.sin_family = AF_INET;
	arenaServerAddr.sin_port = htons(server_port_); //converts to network byte order
	inet_pton(arenaServerAddr.sin_family, server_addr_.c_str(), &(arenaServerAddr.sin_addr));
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
