#ifndef SOCKETCLASS_H
#define SOCKETCLASS_H

#include <chrono>
#include "dronePosVec.pb.h"
#include <sys/socket.h>
#include <string.h>
#include <netdb.h>

//float sleepTimeCalc(int interval,high_resolution_clock::time_point timer);

class ClientSocket {
public:
	virtual ~ClientSocket() = default;
	virtual socklen_t getclient(struct sockaddr* clientAddr) = 0;// ideally should add all these into one init function
	virtual int dServerConnect() = 0;
	virtual ssize_t initSend(char* msg, size_t msgLen) =0;
	virtual ssize_t sendServer(const char* msg,size_t msglen) const = 0;//might rename
	virtual ssize_t recvServer(char* buffer, size_t buffernLen) = 0;
	virtual void setTimeout() = 0;
	virtual void setTimeout(const long int sec,const long int microSec) = 0;
	int sSyncTimer();
};

#endif //SOCKETCLASS_H
