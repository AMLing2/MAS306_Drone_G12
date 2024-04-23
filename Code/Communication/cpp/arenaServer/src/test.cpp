#include <iostream>
#include <string>
#include "dronePosVec.pb.h" //use .cc not .h !!!!!!!!!!
const size_t bufferSize = 1024;

int main()
{
	char* buffer = new char[bufferSize];
	dronePosVec::dronePosition dronePos;

	dronePos.set_devicetype(dronePosVec::IMUonly);
	float values[4] = {1.1,1.2,1.3,1.4};
	dronePos.mutable_position()->Add(values[0]);

	size_t msgSize = dronePos.ByteSizeLong();
	dronePos.SerializeToArray(buffer,bufferSize);

	dronePos.Clear();
	dronePos.ParseFromArray(buffer,msgSize);
	
	std::cout<<"values size: "<<dronePos.position_size()<<std::endl;

	dronePosVec::dataTransfers dronemsg;
	//::dataTransfers dronemsg;

	dronemsg.set_id(dronePosVec::drone);
	dronemsg.set_type(dronePosVec::socketInfo);
	std::string msg = "1234567";
	dronemsg.set_msg(msg);
	std::string serialOutput = "aa";
	//dronemsg.SerializeToString(&serialOutput);
	serialOutput = dronemsg.SerializeAsString();
	
	std::cout<<serialOutput<<std::endl;
	std::cout<<serialOutput.length()<<std::endl;

	dronePosVec::dataTransfers newMsg;
	newMsg.ParseFromString(serialOutput);
	std::cout<<newMsg.type()<<std::endl;

	delete[] buffer;
	return 0;
}
