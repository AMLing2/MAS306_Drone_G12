#include "arenaHeader.h"
#include "dronePosVec.pb.h"
#include <arpa/inet.h> //for inet_ntop
#include <thread>
#include <cerrno>

int SocketMethods::socketSetup_(int port)
{
    struct addrinfo hints;
    memset(&hints,0,sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_DGRAM;
    hints.ai_protocol = IPPROTO_UDP;
    int r = -1;
    if (port == 0)
    {
        r = getaddrinfo(addr_.c_str(), 0, &hints, &plocalAddr_); //port as 0 gets automatic unused port
    }
    else
    {
        char decimal_port[16];
		snprintf(decimal_port, 16, "%d", port);
		decimal_port[15] = '\0';
        r = getaddrinfo(addr_.c_str(), decimal_port, &hints, &plocalAddr_); //connect with port
    }
    clientAddrLen_ = plocalAddr_->ai_addrlen;
    if(r != 0 || plocalAddr_ == NULL)
    {
        std::cout<<"invalid address or port"<<std::endl;
        return r;
    }
    f_socket_ = socket(plocalAddr_->ai_family, SOCK_DGRAM | SOCK_CLOEXEC, IPPROTO_UDP);
    if(f_socket_ == 1)
    {
        std::cout<<"failed to create udp socket"<<std::endl;
    }
    int reusenum = 1;
    setsockopt(f_socket_,SOL_SOCKET,0,&reusenum,1); //allow reuse of local address, primarily for main server class
    r = bind(f_socket_, plocalAddr_->ai_addr,plocalAddr_->ai_addrlen);
    if(r != 0)
    {
        std::cout<<"failed to bind socket"<<std::endl;
    }
    localAddrLen_ = sizeof(plocalAddr_);
    return r;
}

void SocketMethods::genAddrProtoc(dronePosVec::dataTransfers &data)
{
    char strIP[INET_ADDRSTRLEN];
    getsockname(f_socket_,plocalAddr_->ai_addr,&plocalAddr_->ai_addrlen);

    inet_ntop(AF_INET,&(((struct sockaddr_in*)plocalAddr_->ai_addr)->sin_addr),&strIP[0],INET_ADDRSTRLEN);
    uint32_t port = ntohs(((struct sockaddr_in*)plocalAddr_->ai_addr)->sin_port);
    std::cout<<"ip address base: " <<strIP<<":"<<port<<std::endl;

    data.Clear();
    data.set_sa_family(((struct sockaddr_in*)plocalAddr_)->sin_family);
    data.set_sockaddr(plocalAddr_->ai_addr->sa_data); //for sending to c++ programs
    data.set_sockaddrlen(plocalAddr_->ai_addrlen);
    data.set_ip(strIP); //for sending to python and matlab programs
    data.set_port(port);//ntohs() converts from network byte order, (sockaddr_in*)... converts sockaddr to human readable form
    data.set_id(clientName_);
}

void SocketMethods::sleeptoInterval_(ns_t interval)
{
    std::this_thread::sleep_for(interval -((monoTimeNow_() - serverTimer_) % interval.count()));
}

ns_t SocketMethods::monoTimeNow_()
{
    return std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::steady_clock::now().time_since_epoch());
}

ssize_t SocketMethods::clientSend(const char* msg, size_t msgSize)
{
    return send(f_socket_,msg,msgSize,0);
}

ssize_t SocketMethods::clientRecv(char* buffer, size_t bufferSize)
{
    ssize_t recvLen = recv(f_socket_, buffer, bufferSize,0);
	buffer[recvLen] = '\0';
	return recvLen;
}

int SocketMethods::clientConnect(struct sockaddr* clientAddr,socklen_t addrLen)
{
    int r = connect(f_socket_,clientAddr,addrLen);
    if (r == 0) //for connecting in the future, useless for initial loop
    {
        memcpy(&clientAddr_,clientAddr,addrLen);
        clientAddrLen_ = addrLen;
    }
    else
    {
        std::cout<<"connection failure"<<std::endl;
        std::cout<<"errno:"<<strerror(errno)<<", "<<errno<<std::endl;
    }
    return r;
}

void SocketMethods::setTimeout()
{
	struct timeval noTimeout;
	noTimeout.tv_sec = 0;
	noTimeout.tv_usec = 0;
	setsockopt(f_socket_,SOL_SOCKET,SO_RCVTIMEO,&noTimeout,sizeof(noTimeout));
}

void SocketMethods::setTimeout(const long int sec,const long int microSec)
{
	struct timeval timeoutTime;
	timeoutTime.tv_sec = sec;
	timeoutTime.tv_usec = microSec;
	setsockopt(f_socket_,SOL_SOCKET,SO_RCVTIMEO,&timeoutTime,sizeof(timeoutTime));
}

int SocketMethods::socketShutdown()
{
    shutdown(f_socket_,SHUT_RDWR);
    return close(f_socket_);
}
/*
---------------------------------------------CLIENTCLASS-------------------------------------------------------------------
*/
int ClientClass::connectServer()
{
    struct sockaddr serverAddress;
    socklen_t serverAddrLen = sizeof(serverAddress);

    data_.Clear();
    data_.set_id(clientName_);
    data_.set_msg("hi");
    data_.SerializeToArray(sendMsg_,bufferSize_);

    setTimeout(3,0);
    int r = 0;
	for(int i = 0; i < 10; i++)
	{
		initSend(sendMsg_,data_.ByteSizeLong());
		r = recvfrom(f_socket_,recvMsg_,bufferSize_,0,&serverAddress,&serverAddrLen);
		if (r > 0)
		{
			std::cout<<"msg recv: "<<recvMsg_<<std::endl;
			break;
		}
		else
		{
			std::cout<<"timeout, retrying: " <<i<<", errno = "<<strerror(errno)<<", rVal = "<<r<<std::endl;
		}
	}
    if(r < 0)
	{
		return -1;
	}
	else
	{
		int connstat = clientConnect(&serverAddress,serverAddrLen);
		return connstat;
	}
}

ssize_t ClientClass::initSend(char* msg, size_t msgLen)
{
	struct sockaddr_in arenaServerAddr;
	memset(&arenaServerAddr,0,sizeof(arenaServerAddr)); //probably pointless
	arenaServerAddr.sin_family = AF_INET;
	arenaServerAddr.sin_port = htons(serverPort_); //converts to network byte order
	inet_pton(arenaServerAddr.sin_family, serverAddr_.c_str(), &(arenaServerAddr.sin_addr));
	memset(&arenaServerAddr.sin_zero,0,sizeof(arenaServerAddr.sin_zero));
	struct sockaddr* sockaddrServer = reinterpret_cast<sockaddr*>(&arenaServerAddr);
	return sendto(f_socket_,msg,msgLen,0,sockaddrServer,sizeof(arenaServerAddr));
}

int ClientClass::checkList() //checklist before starting streaming
{
    std::cout<<"connection sucess"<<std::endl;
    bool checklistLoop = true;
    while(checklistLoop)
    {
        if (serverTimer_.count() == 0)
        {
            std::cout<<"timer missing, getting"<<std::endl;
            data_.Clear();
            data_.set_id(clientName_);
            data_.set_type(dronePosVec::timeSync);
            data_.set_msg("syncReq");
            data_.SerializeToArray(sendMsg_,bufferSize_);
            clientSend(sendMsg_,data_.ByteSizeLong());
            cSyncTime_();
        }
        if (nextAddrLen_ == 0)
        {
            std::cout<<"next address missing, getting"<<std::endl;
            getNextAddr_();
        }
        else
        {
            std::cout<<"checklist complete, requesting state change"<<std::endl;
            if (statechange_() > 0)
            {
                std::cout<<"connection wtih new socket success"<<std::endl;
            }
            checklistLoop = false;
            return 0;
        }
    }
    return -1;
    //droneClient.mainloop(&dataMsg);
}

int ClientClass::cSyncTime_()
{
    ns_t intervalTime = ns_t(100000000);

	setTimeout();
	data_.Clear();
	ssize_t msgLen = clientRecv(recvMsg_,bufferSize_);
	data_.ParseFromArray(recvMsg_,msgLen);
	serverTimer_ = monoTimeNow_();

	data_.set_id(clientName_);
	data_.SerializeToArray(sendMsg_,bufferSize_);
	size_t byteSize = data_.ByteSizeLong();

	sleeptoInterval_(intervalTime);
	clientSend(sendMsg_,byteSize);

	msgLen = clientRecv(recvMsg_,bufferSize_);
	data_.ParseFromArray(recvMsg_,msgLen);
	timerOffset_ = std::chrono::nanoseconds(data_.timesync_ns());

	serverTimer_ = serverTimer_ - timerOffset_;
	//std::cout<<"offset: "<< timerOffset_.count()<<std::endl;
	return 0;
}

void ClientClass::getNextAddr_() //TODO: CONTINUE AND FIX HERE
{
    data_.Clear();
    data_.set_id(clientName_);
    data_.set_type(dronePosVec::socketInfo);
    data_.set_msg("socketReq");
    data_.SerializeToArray(sendMsg_,bufferSize_);
    clientSend(sendMsg_,data_.ByteSizeLong());

    setTimeout();
    int msgLen = clientRecv(recvMsg_,bufferSize_);
    data_.Clear();
    data_.ParseFromArray(recvMsg_,msgLen);
    std::cout<<"next address: "<<data_.ip()<<":"<<data_.port()<<std::endl;

    memcpy(&nextAddress_.sa_data,data_.sockaddr().c_str(),data_.sockaddrlen());
    nextAddress_.sa_family = data_.sa_family();
    nextAddrLen_ = data_.sockaddrlen();
}

int ClientClass::statechange_() //TODO: fix
{
    data_.Clear();
    data_.set_id(clientName_);
    data_.set_type(dronePosVec::stateChange);
    data_.set_msg("stateChangeReq");
    data_.SerializeToArray(sendMsg_,bufferSize_);
    clientSend(sendMsg_,data_.ByteSizeLong());

    setTimeout();
    //int msgLen = clientRecv(recvMsg_,bufferSize_);
    socketShutdown();
    //generate new socket and connect to next socket
    int r = socketSetup_(0);
    if (r != 0)
    {
        std::cout<<"failure to set up socket"<<std::endl;
        return -1;
    }
    clientConnect(&nextAddress_,nextAddrLen_);

    data_.Clear();
    data_.set_id(clientName_);
    data_.set_msg("hi");
    data_.SerializeToArray(sendMsg_,bufferSize_);
    return clientSend(sendMsg_,data_.ByteSizeLong());
}

int ClientClass::startThread(threadStartType startType)
{
    if (startType == threadStartType::recvOnly)
    {
        tRecv_ = std::thread([this]()
		{
            recvThread();
        });
        
        //std::thread(&AbMessenger::recvThread, this);
        tRecv_.detach();
    }
    else if (startType == threadStartType::sendOnly)
    {
        tRecv_ = std::thread([this]()
		{
            recvThread();
        });

        tSend_.detach();
    }
    else
    {
        tRecv_ = std::thread([this]()
		{
            recvThread();
        });
        tSend_ = std::thread([this]()
		{
            sendThread();
        });

        tRecv_.detach();
        tSend_.detach();
    }
    return 0;
}