#include "udpServerClient.h"
#include <string.h>
#include <unistd.h>
#include <iostream>

namespace udp_client_server
{	//constructor: same as server
	udp_server::udp_server(const std::string& addr, int port)
		: f_port(port)
		, f_addr(addr)
	{
	    char decimal_port[16];
	    snprintf(decimal_port, sizeof(decimal_port), "%d", f_port);
	    decimal_port[sizeof(decimal_port) / sizeof(decimal_port[0]) - 1] = '\0';
	    struct addrinfo hints;
	    memset(&hints, 0, sizeof(hints));
	    hints.ai_family = AF_UNSPEC;
	    hints.ai_socktype = SOCK_DGRAM;
	    hints.ai_protocol = IPPROTO_UDP;
	    int r(getaddrinfo(addr.c_str(), decimal_port, &hints, &f_addrinfo));

	    if(r != 0 || f_addrinfo == NULL)
	    {
		throw udp_client_server_runtime_error(("invalid address or port for UDP socket: \"" + addr + ":" + decimal_port + "\"").c_str());
	    }
	    f_socket = socket(f_addrinfo->ai_family, SOCK_DGRAM | SOCK_CLOEXEC, IPPROTO_UDP);
	    if(f_socket == -1)
	    {
		freeaddrinfo(f_addrinfo);
		throw udp_client_server_runtime_error(("could not create UDP socket for: \"" + addr + ":" + decimal_port + "\"").c_str());
	    }
	    r = bind(f_socket, f_addrinfo->ai_addr, f_addrinfo->ai_addrlen);
	    if(r != 0)
	    {
		freeaddrinfo(f_addrinfo);
		close(f_socket);
		throw udp_client_server_runtime_error(("could not bind UDP socket with: \"" + addr + ":" + decimal_port + "\"").c_str());
	    }
	}


	udp_server::~udp_server()
	{
		freeaddrinfo(f_addrinfo);
		close(f_socket);
	}
//getters
	int udp_server::get_socket() const
	{
		return f_socket;
	}
	int udp_server::get_port() const
	{
	    return f_port;
	}

	std::string udp_server::get_addr() const
	{
	    return f_addr;
	}
	//wait on message:
	int udp_server::recv(char *msg, size_t max_size)
	{
		return ::recv(f_socket, msg, max_size, 0);
	}
	//wait for data to come in:
	int udp_server::timed_recv(char *msg, size_t max_size, int max_wait_ms)
	{
		fd_set s;
		FD_ZERO(&s);
		FD_SET(f_socket, &s);
		struct timeval timeout;
		timeout.tv_sec = max_wait_ms / 1000;
		timeout.tv_usec = (max_wait_ms % 1000) * 1000;
		int retval = select(f_socket + 1, &s, &s, &s, &timeout);
		if(retval ==  -1)
		{
			//select() set errno accordingly
			return -1;
		}
		if(retval > 0)
		{
			//our socket has data
			return ::recv(f_socket, msg, max_size, 0);
		}

		//our socket has no data
		errno = EAGAIN;
		return -1;
	}
}; //namespace

int main()
{
	const int port = 54586;
	const std::string addrServer = "127.0.0.1";
	char * buffer;
	size_t maxsize = 16;
	int msgLen;

	udp_client_server::udp_server testServer(addrServer, port);
	std::cout<<"socket: "<< testServer.get_socket()<< " port: " << testServer.get_port() << " address: "<<testServer.get_addr()<<std::endl;

	std::cout<<"echoing"<<std::endl;

	while(true)
	{
		msgLen = testServer.recv(buffer,maxsize);
		buffer[msgLen] = '\0';
		std::cout<<"msg length recieved: "<< msgLen << " message: " << buffer <<std::endl;
	}
	
	return 0;
}

