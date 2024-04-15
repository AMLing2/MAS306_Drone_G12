#include "dronePosVec.pb.cc"
#include "droneClient.h"
#include <chrono>
#include <iostream>
#include <string.h>
#include <sys/socket.h>
#include <netdb.h>

class ArenaServer : public ClientSocket
{
public:
	ArenaServer(const std::string& addr, int port)
	:f_port(port)
	,f_addr(addr)
	{
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
		freeaddrinfo(f_addrinfo);
		close(f_socket);
	}

	virtual socklen_t getclient(struct sockaddr* clientAddr) override;
	virtual int sendDrone(const char* msg,size_t msglen) override;
	int connectInit();
	
private:
int f_port;
std::string f_addr;
struct addrinfo* f_addrinfo;
int f_socket;
struct sockaddr clientAddr_;
socklen_t clientsocklen_;
};

//-------------------MAIN-------------------@
int main()
{
	std::string addrS = "128.39.200.239";
	int port = 20002;
	ArenaServer arenaServer(addrS,port);
	//struct sockaddr clientAddr;
	arenaServer.connectInit();
	//arenaServer.getclient(&clientAddr);
	std::cout<<"press to exit"<<std::endl;
	int a;
	std::cin>>a;
	return 0;
}

//--------------class definitions-------------@
socklen_t ArenaServer::getclient(struct sockaddr* clientAddr)
{
	const size_t bufferlen = 1024;
	char* buffer = new char[bufferlen];
	socklen_t addrlen;
	ssize_t msglen = recvfrom(this->f_socket,buffer,bufferlen,0,clientAddr,&addrlen);
	buffer[msglen] = '\0';
	std::cout<<"msg recieved: "<<buffer<<std::endl;
	//msg is ignored for now, only want sending address
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
	}
	return r;
}

int ArenaServer::sendDrone(const char* msg,size_t msglen)
{
	send(f_socket,msg,msglen,0);
	return 0; //should ideally check if length sent = length of msg
}
