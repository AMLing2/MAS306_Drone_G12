#include "dronePosVec.pb.h"
#include "droneClient.h"
#include <chrono>
#include <iostream>
#include <string.h>
#include <sys/socket.h>

class droneClient : protected clientSocket
{
public:
	droneClient(const std::string& serverAddr, int port)
	:f_port = port
	,serverAddr = serverAddr
	{
		r = socket(family, SOCK_DGRAM, IPPROTO_UDP);
	}
}
int main()
{
	return 0;
}
