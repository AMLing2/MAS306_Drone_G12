#include <iostream>
#include <string>
#include "dronePosVec.pb.cc" //use .cc not .h !!!!!!!!!!


int main()
{
	dronePosVec::dataTransfers dronemsg;
	//::dataTransfers dronemsg;

	dronemsg.set_id(2);
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
	return 0;
}
