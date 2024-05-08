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

std::string getSelfIP(const char nameserver[])
{
    struct addrinfo hints;
    struct addrinfo* ret;
    memset(&hints,0,sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_DGRAM;
    hints.ai_protocol = IPPROTO_UDP;
    hints.ai_protocol = IPPROTO_UDP;
    hints.ai_flags = AI_CANONNAME;
    //const char uiasearch[] = ".uia.no";
    const size_t hostnamesize = 64;
    char hostName[hostnamesize];
    gethostname(hostName,hostnamesize);
    for (size_t i = 0; i< hostnamesize;i++) //append ".uia.no"
    {
        if (hostName[i] == '\0')
        {
            if ((hostnamesize - i) < sizeof(&nameserver))
            {
                std::cout<<"hostname too long"<<std::endl;
                exit(-1);
            }
            else
            {
                memcpy(&hostName[i],nameserver,sizeof(&nameserver));
                break;
            }
            
        }
    }
    int r = getaddrinfo(hostName, NULL, &hints,&ret);
    if (r < 0)
    {
        std::cout<<"error when getting hostname or ip, check internet connection"<<std::endl;
        std::cout<<"errno:"<<strerror(errno)<<", "<<errno<<std::endl;
        exit(r);
    }
    //convert to human readable IP
    char strIP[INET_ADDRSTRLEN];

    inet_ntop(AF_INET,&(((struct sockaddr_in*)ret->ai_addr)->sin_addr),&strIP[0],INET_ADDRSTRLEN);
    std::cout<<"hostname: "<<std::string(ret->ai_canonname)<<", ip: "<<std::string(strIP)<<std::endl;
    freeaddrinfo(&hints);
    freeaddrinfo(ret);
    return std::string(strIP);
}

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
    return r;
}

int SocketMethods::getIPfromName(std::string hostname,int port) //alternative method for future connectsocket for the future when not using static ip
{
    std::cout<<"getting ip of: "<<hostname<<std::endl;
    struct addrinfo hints;
    memset(&hints,0,sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_DGRAM;
    hints.ai_protocol = IPPROTO_UDP;
    char decimal_port[16];
    snprintf(decimal_port, 16, "%d", port);
    decimal_port[15] = '\0';
    int r = getaddrinfo(hostname.c_str(), decimal_port, &hints, &plocalAddr_);
    if(r != 0 || plocalAddr_ == NULL)
    {
        std::cout<<"invalid address or port"<<std::endl;
        return -1;
    }
    //convert to human readable IP
    char strIP[INET_ADDRSTRLEN];

    inet_ntop(AF_INET,&(((struct sockaddr_in*)plocalAddr_->ai_addr)->sin_addr),&strIP[0],INET_ADDRSTRLEN);
    uint32_t port2 = ntohs(((struct sockaddr_in*)plocalAddr_->ai_addr)->sin_port);
    std::cout<<"ip address server: " <<strIP<<":"<<port2<<std::endl;
    addr_ = strIP;
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
    data.set_sa_family(plocalAddr_->ai_addr->sa_family); //for sending to c++ programs
    data.set_sockaddr(plocalAddr_->ai_addr->sa_data); 
    data.set_sockaddrlen(plocalAddr_->ai_addrlen);

    data.set_ip(strIP); //for sending to python and matlab programs
    data.set_port(port);//ntohs() converts from network byte order, (sockaddr_in*)... converts sockaddr to human readable form
    data.set_id(dronePosVec::server);
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
ssize_t ServerSocket::initRecv()//TODO: fix
{
    setTimeout(60,0);
    ssize_t msgSize;
    msgSize = serverRecvfrom(&buffer_[0],bufferSize_);
    if (msgSize < 0)
    {
        std::cout<<"timeout"<<std::endl;
        return -1;
    }
    std::cout<<"msg recv"<<std::endl; //somewhat temp
    try
    {
        data_.ParseFromArray(buffer_,msgSize);
        //if no exception:
        std::cout<<"msg recivied: '" << data_.msg()<< "' with ID: "<<data_.id() <<std::endl;
        clientProgram_ = data_.id();
        if (clientProgram_ == dronePosVec::server)
        {
            std::cout<<"ERROR: ID not specified in message"<<std::endl;
            return -2;
        }
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
        std::cout<<"invalid message"<<std::endl;
        return -3;
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
    clientProgram_ = dronePosVec::server; //default;
    char tempmsg[6] = {'h','e','l','l','o','\0'}; //TODO: make not temporary >:(
    bool recvLooping = true;
    int r = 0;
    while(recvLooping) //restart recv in case of an invalid message such as from port scanning, but end if timeout
    {
        r = initRecv();//wait for first recvTo
        if (r > 0)
        {
            memcpy(buffer_,&tempmsg,6); //temporary
            if ((clientConnect(&clientAddr_,clientAddrLen_)) >= 0)
            {
                clientSend(tempmsg,6);//temporary msg

                checklistLoop(); //checklist 
                std::cout<<"checklist done"<<std::endl;
                //restart mainloop
                socketShutdown();
                socketSetup_(port_);
                recvLooping = false;
                return clientProgram_;
            }
            else
            {
                std::cout<<"failed to connect"<<std::endl;
            }
        }
        else if (r == -1)
        {
            recvLooping = false;
            break;
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
        setTimeout(5,0);
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
    data_.set_msg("GlobalTimer");
	data_.set_type(dronePosVec::timeSync);
	data_.SerializeToArray(buffer_,bufferSize_);
	setTimeout(10,0);

	ns_t sendTime = calcSleepTime(ns_t(intervalTime)); //send next relative 100ms
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
        data_.set_msg("offset");
		data_.set_timesync_ns(timeDiff);
		data_.SerializeToArray(buffer_,bufferSize_);
		clientSend(buffer_,data_.ByteSizeLong());//adding an offset might be wierd, 0.2-0.7ms delay without it
        return 0;
	}
	return -1; //failed to recv msg
}

int ServerSocket::sendSocketInfo()
{
    for(const std::unique_ptr<AbMessenger>& i: *vpSocketClasses_)
    {
        if (i->getName() == clientProgram_)
        {
            i->genAddrProtoc(data_);
            break;
        }
    }
    data_.SerializeToArray(buffer_,bufferSize_);
    return clientSend(buffer_,data_.ByteSizeLong());
}

void ServerSocket::setSocketList(std::vector<std::unique_ptr<AbMessenger>>* socketClasses) // is supposed to be type: std::vector<std::unique_prt<AbMessenger>>* 
{
    vpSocketClasses_ = socketClasses;
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
    std::cout<<"msg recieved: '" << data_.msg()<< "' with ID: "<<data_.id() <<std::endl;
    return clientConnect(&clientAddr_,sizeof(clientAddr_));
}

dronePosVec::progName AbMessenger::getName()
{
    return classProgram_;
}
bool AbMessenger::getConnection()
{
    return connected_;
}

int AbMessenger::joinThread()
{
    std::cout<<"joining thrread of: "<<strName<<std::endl;
    threadloop_ = false;
    if(tRecv_.joinable())
    {
        tRecv_.join();
    }
    if(tSend_.joinable())
    {
        tSend_.join();
    }
    return 0;
}

int AbMessenger::startThread(threadStartType startType)
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

threadStartType AbMessenger::getThreadStart()
{
    return threadFuncs_;
}

void AbMessenger::setConnection(bool status)
{
    connected_ = status;
}