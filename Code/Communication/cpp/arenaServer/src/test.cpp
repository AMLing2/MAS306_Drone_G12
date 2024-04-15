#include <iostream>
#include <string>
#include "dronePosVec.pb.cc" //use .cc not .h !!!!!!!!!!


int main()
{
	dronePosVec::dataTransfers dronemsg;
	//::dataTransfers dronemsg;

	dronemsg.set_id(1);
	//dronemsg.set_type();
	//std::string msg = "123456";
	//dronemsg.set_msg(msg);
	std::string serialOutput = "aa";
	dronemsg.SerializeToString(&serialOutput);
	//serialOutput = dronemsg.SerializeAsString();
	
	std::cout<<serialOutput<<std::endl;

	return 0;
}
