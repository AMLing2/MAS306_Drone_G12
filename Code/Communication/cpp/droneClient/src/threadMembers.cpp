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

//--------------thread functions--------------@
/*
IMU
*/
void TcIMUStream::tSendIMUStream(std::unique_ptr<dronePosVec::dronePosition> pIMUmsg)
{
	//std::cout<<"f_socket: "<<clientClass_->f_socket<<std::endl;
	int tempi = 0; //temporary
	const int value_size = 3; //temporary
	float value[value_size] = {1.1,1.2,1.3};
	std::cout<<"IMU thread running"<<std::endl;
	while(threadLoop_)
	{
		pIMUmsg->Clear();
		pIMUmsg->set_devicetype(dronePosVec::IMUonly);
		pIMUmsg->mutable_position()->Add(value[0]);
		msgbufferSize_ = pIMUmsg->ByteSizeLong();
		pIMUmsg->SerializeToArray(tBuffer_,tBufferLen_);
		
		//SEND
		std::this_thread::sleep_for(clientClass_->calcSleepTime(interval_));
		clientClass_->sendServer(tBuffer_,msgbufferSize_);
		tempi++; //run 10 times
		if(tempi >= 10)
		{
			threadLoop_ = false;
		}
	}
    std::cout<<"IMU thread finished"<<std::endl;
}
/*
MOTOR
*/
void TcMotorStream::tRecvMotorStream(std::unique_ptr<dronePosVec::droneControl> pMotormsg)
{
    std::cout<<"Motor thread running"<<std::endl;
    clientClass_->setTimeout(10,0);
    while(threadLoop_)
    {
        std::this_thread::sleep_for(clientClass_->calcSleepTime(interval_));
        //pMotormsg->Clear();
        msgbufferSize_ = clientClass_->recvServer(tBuffer_,tBufferLen_);
        if(msgbufferSize_ < 0)
        {
            threadLoop_ = false;
            break;
        }


    }
    std::cout<<"Motor thread finished"<<std::endl;
}
