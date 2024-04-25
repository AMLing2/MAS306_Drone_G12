#include "dronePosVec.pb.h"
#include "arenaServer.h"
//#include "programSpecifics.h"
#include <sys/socket.h>
#include <chrono>
#include <netdb.h> //for getaddrinfo
#include <arpa/inet.h> //for inet_ntop
//rename this file 
#include <cerrno>

//std::cout<<"errno:"<<strerror(errno)<<", "<<errno<<std::endl;

/* Generic socket methods*/
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
    setsockopt(f_socket_,SOL_SOCKET,SO_REUSEADDR,&reusenum,1); //allow reuse of local address, primarily for main server class
    r = bind(f_socket_, plocalAddr_->ai_addr,plocalAddr_->ai_addrlen);
    if(r != 0)
    {
        std::cout<<"failed to bind socket"<<std::endl;
    }
    localAddrLen_ = sizeof(plocalAddr_);
    static_cast<const int>(f_socket_); //cast to const so nothing writes it while threads read
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
    data.set_id(dronePosVec::server);
}


socklen_t SocketMethods::getClassAddr(struct addrinfo passAddr,size_t addrinfosize) //depracated? was replaced wtih genAddrProtoc but should return to using this
{
    char strIP[INET_ADDRSTRLEN];
    inet_ntop(AF_INET,&(((struct sockaddr_in*)plocalAddr_->ai_addr)->sin_addr),&strIP[0],INET_ADDRSTRLEN);
    std::cout<<"ip address base: " <<strIP<<std::endl;
    memcpy(&passAddr,plocalAddr_,sizeof(plocalAddr_));
    //passAddr = plocalAddr_; //bad very bad
    return localAddrLen_;
}

void SocketMethods::sleeptoInterval_(std::chrono::nanoseconds interval)
{
    std::this_thread::sleep_for(interval -((monoTimeNow_() - serverTimer_) % interval.count()));
}

std::chrono::nanoseconds SocketMethods::monoTimeNow_()
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

ns_t ServerSocket::getServerTimer()
{
    return serverTimer_;
}
/* socket related functions for AbMessenger */
/* other */

/* REMOVE --------------------------------------------------------------------------------------------------------------------------------------------
ssize_t AbMessenger::getThreadList(std::thread* tbuffer[], size_t tbufferlen)
{
    if (tbufferlen < numOfThreads_)
    {
        return -1;
    }
    else
    {
        for(size_t i = 0; i < numOfThreads_; i++)
        {
            tbuffer[i] = threads_[i];
        }
        return numOfThreads_;
    }
}
*/
/* Server functions */
ssize_t ServerSocket::initRecv()
{
    setTimeout();
    ssize_t msgSize;
    msgSize = serverRecvfrom(&buffer_[0],bufferSize_);
    try
    {
        data_.ParseFromArray(buffer_,msgSize);
        //if no exception:
        std::cout<<"msg recivied: '" << data_.msg()<< "' with ID: "<<data_.id() <<std::endl;
        clientProgram_ = data_.id();
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
        std::cout<<"invalid message"<<std::endl;
        return -1;
    }
    return msgSize;
}

int ServerSocket::socketShutdown()
{
    shutdown(f_socket_,SHUT_RDWR);
    return close(f_socket_);
}

ssize_t ServerSocket::serverRecvfrom(char* buffer,const size_t bufferSize)
{
    ssize_t r = recvfrom(f_socket_, buffer, bufferSize,0,&clientAddr_,&clientAddrLen_);
    return r;
}

ns_t ServerSocket::calcSleepTime(ns_t interval)
{
    return (interval -((monoTimeNow_() - serverTimer_) % interval.count()));
}

dronePosVec::progName ServerMain::mainRecvloop()
{
    char tempmsg[6] = {'h','e','l','l','o','\0'}; //make not temporary >:(
    if (initRecv() > 0) //wierd check
    {
        memcpy(buffer_,&tempmsg,6);
        //serverSendto(buffer_,5);//temporary msg
        if ((clientConnect(&clientAddr_,clientAddrLen_)) >= 0)
        {
            clientSend(tempmsg,6);//temporary msg

            checklistLoop();
            std::cout<<"checklist done"<<std::endl;
            //restart mainloop
            socketShutdown();
            socketSetup_(port_);
            return clientProgram_;
        }
        else
        {
            std::cout<<"failed to connect"<<std::endl;
        }
    }
    return dronePosVec::server; //default
}

dronePosVec::progName ServerMain::checklistLoop()
{
    setTimeout();
    bool looping = true;
    while(looping)
    {
        clientRecv(buffer_,bufferSize_);
        data_.ParseFromArray(buffer_,bufferSize_);
        switch(data_.type())
            {
                case dronePosVec::timeSync:
                    {
                        std::cout<<"timesync request"<<std::endl;
                        syncTimer();
                        break;
                    }
                case dronePosVec::socketInfo:
                    {
                        std::cout<<"request for new socket info"<<std::endl;
                        sendSocketInfo();
                        break;
                    }
                case dronePosVec::stateChange:
                    {
                        //TODO: CONTINUE HERE
                        //start threads
                        std::cout<<"statechange req"<<std::endl;
                        //stateChange_();
                        //tempReader();

                        looping = false;
                        break;
                    }
                default:
                    {
                        looping = false;
                        break;
                    }
            }
    }
    return data_.id();//TODO: pointless, repalce with void?
}

void ServerMain::stateChange_() //REMOVE, THIS IS IN MAIN FUNCTION
{
    switch (clientProgram_)
    {
        case dronePosVec::drone: //DRONE
        {
            break;
        }
        case dronePosVec::estimator: //ESTIMATOR
        {
            break;
        }
        case dronePosVec::arena: //ARENA
        {
            break;
        }
        case dronePosVec::camera: //CAMERA
        {
            break;
        }
        case dronePosVec::rl: //RL
        {
            break;
        }
        default:
        {
            break;
        }
    }
       
}

int ServerSocket::syncTimer()
{
    long int intervalTime = 100000000; //100ms
	data_.Clear();
	data_.set_id(dronePosVec::server);
	data_.set_type(dronePosVec::timeSync);
	data_.SerializeToArray(buffer_,bufferSize_);
	setTimeout(1,0);

	ns_t sendTime = calcSleepTime(ns_t(intervalTime)); //send next relaive 100ms
	std::this_thread::sleep_for(sendTime);
	clientSend(buffer_,data_.ByteSizeLong());
	sendTime = monoTimeNow_();
	
	ssize_t msgLen = clientRecv(buffer_,bufferSize_); //read
	if (msgLen > 0)
	{
		data_.Clear();
		data_.ParseFromArray(buffer_,msgLen);
		std::chrono::nanoseconds recvTime = monoTimeNow_();
		long int timeDiff = (recvTime - sendTime).count() - intervalTime;
		std::cout<<"time difference = "<<timeDiff<<std::endl;
		data_.set_id(dronePosVec::server);
		data_.set_timesync_ns(timeDiff);
		data_.SerializeToArray(buffer_,bufferSize_);
		clientSend(buffer_,data_.ByteSizeLong());//adding an offset might be wierd, 0.2-0.7ms delay without it
	}
	return 0;
}

int ServerSocket::sendSocketInfo()
{
    std::string addr;
    socklen_t addrLen;
    //struct addrinfo sendingAddr;
    //memset(sendingAddr,0,sizeof(sendingAddr));
    switch (clientProgram_)
    {
        case dronePosVec::drone: //DRONE
        {
            break;
        }
        case dronePosVec::estimator: //ESTIMATOR
        {
            //addrLen = programSockets_.pEstimator->getClassAddr(sendingAddr,sizeof(sendingAddr));
            break;
        }
        case dronePosVec::arena: //ARENA
        {
            break;
        }
        case dronePosVec::camera: //CAMERA
        {
            programSockets_.pCamera->genAddrProtoc(data_);
            //addrLen = programSockets_.pCamera->getClassAddr(sendingAddr,sizeof(sendingAddr));
            break;
        }
        case dronePosVec::rl: //RL
        {
            break;
        }
        default:
        {
            break;
        }
    }
    /*
    char strIP[INET_ADDRSTRLEN];
    inet_ntop(AF_INET,&(((struct sockaddr_in*)sendingAddr.ai_addr)->sin_addr),&strIP[0],INET_ADDRSTRLEN); //segfault here
    std::cout<<"ip address: "<<strIP<<std::endl;
    data_.Clear();
    data_.set_sa_family(sendingAddr.ai_addr->sa_family);
    data_.set_sockaddr(sendingAddr.ai_addr->sa_data); //for sending to c++ programs
    data_.set_sockaddrlen(sendingAddr.ai_addrlen);
    data_.set_ip(strIP); //for sending to python and matlab programs
    data_.set_port(ntohs(((struct sockaddr_in*)sendingAddr.ai_addr)->sin_port));//ntohs() converts from network byte order, (sockaddr_in*)... converts sockaddr to human readable form
    data_.set_id(dronePosVec::server);
    */
    data_.SerializeToArray(buffer_,bufferSize_);
    return clientSend(buffer_,data_.ByteSizeLong());
}

void ServerSocket::setSocketList(EstimatorMessenger* estimator,CameraMessenger* camera)
{
    programSockets_.pEstimator = estimator;
    programSockets_.pCamera = camera;
}

sockaddr* ServerMain::getClientAddr()
{
    return &clientAddr_;
}
socklen_t ServerMain::getClientAddrSize()
{
    return clientAddrLen_;
}

ssize_t AbMessenger::initRecv()
{
    setTimeout();
    ssize_t msgSize;
    msgSize = recvfrom(f_socket_,recvMsg_,bufferSize_,0,&clientAddr_,&clientAddrLen_);
    try
    {
        data_.ParseFromArray(recvMsg_,msgSize);
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
        std::cout<<"invalid message"<<std::endl;
        return -1;
    }
    std::cout<<"msg recivied: '" << data_.msg()<< "' with ID: "<<data_.id() <<std::endl;
    clientConnect(&clientAddr_,sizeof(clientAddr_));
    return msgSize;
}

int AbMessenger::startThread()
{
    if (classProgram_ == dronePosVec::camera) //this class does not have a send thread
    {
        tRecv_ = std::thread([this]()
		{
            recvThread();
        });
        
        //std::thread(&AbMessenger::recvThread, this);
        tRecv_.detach();
    }
    else
    {
        tRecv_ = std::thread([this]()
		{
            recvThread();
        });
        tSend_ = std::thread([this]()
		{
            recvThread();
        });

        tRecv_.detach();
        tSend_.detach();
    }
    return 0;
}