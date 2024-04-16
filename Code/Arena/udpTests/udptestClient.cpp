#include "udpServerClient.h"
#include <string.h>
#include <unistd.h>
#include <iostream>
#include <cstring>

namespace udp_client_server
{
	udp_client::udp_client(const std::string& addr, int port)
		:f_port(port)
		,f_addr(addr)
	{
		char decimal_port[16];
		snprintf(decimal_port, sizeof(decimal_port), "%d", f_port);
		decimal_port[sizeof(decimal_port) / sizeof(decimal_port[0]) - 1] = '\0';
		struct addrinfo hints;
		memset(&hints, 0, sizeof(hints)); //replace all memory of addrinfo struct hints with 0
		hints.ai_family = AF_UNSPEC;
		hints.ai_socktype = SOCK_DGRAM;
		hints.ai_protocol = IPPROTO_UDP;

		int r(getaddrinfo(addr.c_str(), decimal_port, &hints, &f_addrinfo)); //f_addrinfo is gotten here by a pass by reference
		if(r != 0 || f_addrinfo == NULL)
		{
			throw udp_client_server_runtime_error(("invalid address or port: \""+ addr + ":" + decimal_port + "\"").c_str());
		}
		f_socket = socket(f_addrinfo->ai_family, SOCK_DGRAM | SOCK_CLOEXEC, IPPROTO_UDP);
		if(f_socket == -1)
		{
			freeaddrinfo(f_addrinfo);
			throw udp_client_server_runtime_error(("could not create socket for: \"" + addr + ":" + decimal_port + "\"").c_str());
		}
	}

	udp_client::~udp_client()
	{
		freeaddrinfo(f_addrinfo);
		close(f_socket);
	}
//getters
	int udp_client::get_socket() const
	{
		return f_socket;
	}
	int udp_client::get_port() const
	{
		return f_port;
	}

	std::string udp_client::get_addr() const
	{
    		return f_addr;
	}

	int udp_client::send(const char *msg, size_t size)
	{
		return sendto(f_socket, msg, size, 0, f_addrinfo->ai_addr, f_addrinfo->ai_addrlen);
	}

};//namespace


int main()
{
	const int port = 54586;
	const std::string addrServer = "128.39.202.170";
	std::string msgString = "starting";

	udp_client_server::udp_client testClient(addrServer, port);
	std::cout<<"socket: "<< testClient.get_socket() << " port: "<< testClient.get_port() << " address: " << testClient.get_addr() <<std::endl;

	bool a = true;
	int input;

	while(a)
	{
		char *msg = new char[msgString.size() + 1]; //sizeof(msg) = 8 since size of pointer
		msg[msgString.size()] = '\0';
		//std::cout<<"size of string: "<<msgString.size() + 1<<" size of message: " << sizeof(msg)<<std::endl;
		std::strcpy(msg,msgString.c_str() );//copy contents of string to msg

		std::cin>>input;
		std::cout<<std::endl;
		if(input == 0)
		{
			a = false;
			std::cout<<"exiting"<<std::endl;
		}
		else
		{
			//char *newmsg = input;
			std::cout<<"msg size: "<<msgString.size()<<" sent bytes: "<< testClient.send(msg, msgString.size()) <<std::endl;
			std::cout<<"write new message: " <<std::endl;
			std::cin>>msgString;
			std::cout<<"new message: "<<msgString<<std::endl;
		}
		delete[] msg;
	}

	return 0;
}
