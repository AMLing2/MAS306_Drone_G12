#include "dronePosVec.pb.h"
#include "droneClient.h"
#include <chrono>
#include <iostream>
#include <string.h>
#include <sys/socket.h>
#include <netdb.h>
#include <thread>
#include <netinet/in.h> //for struct sockaddr_in
#include <arpa/inet.h> //for htons()
#include <cerrno>

//-------------------MAIN-------------------@
int main()
{
	dronePosVec::dataTransfers dataMsg;
	const std::string serverAddr = "128.39.200.239";
	const int serverPort = 20002;
	DroneClient droneClient(serverAddr,serverPort);
	if (droneClient.dServerConnect() == 0)
	{
		std::cout<<"connection sucess"<<std::endl;
		//checklist before starting streaming
		bool checklist = true;
		while(checklist)
		{
			if (droneClient.getMonoServerTime().count() == 0)
			{
				droneClient.cSyncTime(&dataMsg);
			}
			else
			{
				std::cout<<"checklist complete"<<std::endl;
				checklist = false;
			}
		}
		droneClient.mainloop(&dataMsg);
	}
	else
	{
		std::cout<<"connection error"<<std::endl;
	}
	/*
	std::cout<<"press to exit"<<std::endl;
	int a;
	std::cin>>a;
	*/
	return 0;
}

void DroneClient::mainloop(dronePosVec::dataTransfers* pDataMsgs)
{
	//would be nice to have two sockets, one for udp streaming and one tcp for other communication such as this
	//request for server to start reading streams
	pDataMsgs->set_id(2);
	pDataMsgs->set_type(dronePosVec::stateChange);
	std::string stateMsg = "start";
	pDataMsgs->set_msg(stateMsg);
	pDataMsgs->SerializeToArray(genBuffer_,pDataMsgs->ByteSizeLong());
	sendServer(genBuffer_,pDataMsgs->ByteSizeLong());
	//wait for recv from server
	setTimeout();
	pDataMsgs->Clear();
	ssize_t recvSize = recvServer(genBuffer_,bufferLen_);
	pDataMsgs->ParseFromArray(genBuffer_,recvSize);
	//could probably read what it is but recvining it means its probably fine

	//start threads
	TcIMUStream imuStream(20000000,this);	
	imuStream.startThread();
	//std::cout<<"waiting for imu thread to end"<<std::endl;
	//imuStream.joinThread();
	while (true)
	{
		/* code */
	}
	//end of mainloop marks end of program	
}