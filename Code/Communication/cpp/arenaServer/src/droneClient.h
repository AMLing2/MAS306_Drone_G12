#ifndef SOCKETCLASS_H
#define SOCKETCLASS_H

#include <chrono>
#include "dronePosVec.pb.h"
#include <sys/socket.h>
#include <string.h>
#include <netdb.h>
//add header protection

//float sleepTimeCalc(int interval,high_resolution_clock::time_point timer);

class ClientSocket {
public:
	virtual ~ClientSocket() = default;
	virtual socklen_t getclient(struct sockaddr* clientAddr) = 0;// ideally should add all these into one init function
	void initSend();
	virtual int sendDrone(const char* msg,size_t msglen) = 0;//might rename
	std::string recvDrone(); //redo
	int dServerConnect();
	int sSyncTimer();
};

#endif //SOCKETCLASS_H
