#include <chrono>
#include "dronePosVec.pb.h"
#include <sys/socket.h>
#include <string.h>
//add header protection

float sleepTimeCalc(int interval,high_resolution_clock::time_point timer);

class ClientSocket {
public:
	ClientSocket(const std::string& serverAddr, int port);
	void connect();
	int initRecv();// ideally should add all these into one init function
	void initSend();
	void send();
	std::string recv(); //
	int dServerConnect();
	int sSyncTimer();

protected:
	high_resolution_clock::time_point globalClock_;
	int bufferSize_ = 1024;
	const uint16_t serverPort;
	sockaddr 
}
