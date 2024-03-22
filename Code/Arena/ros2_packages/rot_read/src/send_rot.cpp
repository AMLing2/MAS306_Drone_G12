#include <iostream>
//#include <bitset>
#include <pigpio.h>
#include <chrono> //for walltimer
#include <functional> //unsure of usage
#include <memory> //unsure of usage

#include "rot_read.h"
#include "rclcpp/rclcpp.hpp"
#include "interfaces_arena/msg/rot_error.hpp"

#define ENCODERADDR 0x36
#define ENCODERBUS 1

//#define H_ROT_ADDR 0x0E
//#define L_ROT_ADDR 0x0F

using namespace std::chrono_literals;

class AbsoluteEncoder : public I2CDevice
{
public:
	using I2CDevice::I2CDevice; //inherits all constructors
	void errori2cOpen() override
	{
		std::cout<<"error opening i2c port"<<std::endl;
	}
	unsigned int readRegisterByte(const uint8_t& regAddr) override;	
	uint16_t readHLbytes(const uint8_t& regAddrH,const uint8_t& regAddrL);//reads two bytes and combines to  12bit output (12 bit = default)
	//uint16_t readHLbytes(const uint8_t& regAddrH,const uint8_t& regAddrL,uint8_t size);//reads two bytes and combines to [size] bit output > 8
	uint16_t getPos();
	void setPos(uint16_t pos);
private:
	uint16_t pos_;
};

class RotPublisher : public rclcpp::Node
{
public:
	RotPublisher(AbsoluteEncoder* pEncoder)
	:Node("Rotation_Publisher")
	,count_(0) //count neccesary?
	,pEncoder_(pEncoder)
	{
		publisher_ = this->create_publisher<interfaces_arena::msg::RotError>("topic", 10);
		timer_ = this->create_wall_timer(1000ms, std::bind(&RotPublisher::timer_callback, this)); //send every 1000ms
	}
	void setRot_Error(uint16_t error); //i forgor type..
	void setRot_Dir(bool dir);

private:
	void timer_callback()
	{
		dirUpdate_();
		auto message = interfaces_arena::msg::RotError();
		message.rot_error = errorNewest_;
		message.rot_dir = dirNewest_;
		RCLCPP_INFO_STREAM(this->get_logger(), "publishing: '"<< message.rot_error<< "' dir:'"<<message.rot_dir<<"'"<<"count: '"<<count_<<"'");
		publisher_->publish(message);
	}
	void dirUpdate_();
	rclcpp::TimerBase::SharedPtr timer_;
	rclcpp::Publisher<interfaces_arena::msg::RotError>::SharedPtr publisher_;
	size_t count_;
	float errorNewest_;
	bool dirNewest_ = true; //need to update later
	AbsoluteEncoder* const pEncoder_; //declare pointer address as const, not value inside
};

namespace progVars //for general stuff
{
	bool loopEnable = true;
	//const unsigned int encoderAddr = 0x36;//not a big fan of this usage
	//const unsigned int encoderBus = 1; //moved as #define
	const unsigned int addrPosH = 0x0E;  //find
	const unsigned int addrPosL = 0x0F; //find
}

/*
--------------MAIN------------
*/
int main(int argc, char ** argv)
{
	//init (kind of hard to push all to function but should)
  (void) argc;
  (void) argv;

	if(gpioInitialise()<0) //must be initialized before use
	{
		std::cout<<"GPIO initialization failed, exiting"<<std::endl;
		return 0;
	}
	AbsoluteEncoder encoder(ENCODERADDR,ENCODERBUS);
	rclcpp::init(argc,argv);

	std::shared_ptr<RotPublisher> pPublisher = std::make_shared<RotPublisher>(&encoder);//begin ros2 node
	rclcpp::spin(pPublisher);//begin ROS2 node

	shutdownSeq();
  return 0;
}
/*
-----------MAIN END-----------
*/

void shutdownSeq() //call before exiting
{
	rclcpp::shutdown();
	gpioTerminate();
}

/*
Absolute encoder function definitions:
*/
unsigned int AbsoluteEncoder::readRegisterByte(const uint8_t& regAddr)
{
	//reads a single register in the i2c device
	return i2cReadByteData(getHandle(),regAddr);
}

uint16_t AbsoluteEncoder::getPos()
{
	return pos_;
}

void AbsoluteEncoder::setPos(uint16_t pos)
{
	pos_ = pos;
}

uint16_t AbsoluteEncoder::readHLbytes(const uint8_t& regAddrH,const uint8_t& regAddrL)
{
	uint16_t H = static_cast<uint16_t>(readRegisterByte(regAddrH)); //read 11:8
	uint16_t L = static_cast<uint16_t>(readRegisterByte(regAddrL));//read 7:0
	H &= 0xF; //removes all possible 1s to create 4 + 8 bit size
	uint16_t combine = 0 | L | (H<<8);
	return combine;
}
/*
uint16_t AbsoluteEncoder::readHLbytes(const uint8_t& regAddrH,const uint8_t& regAddrL,uint8_t size)
{
	if(size>16 || size<9)
	{
		return 0; //error, cant create larger than 16bit int or 8 bit number
	}
	uint16_t H = static_cast<uint16_t>(readRegisterByte(regAddrH)); //read 11:8
	uint16_t L = static_cast<uint16_t>(readRegisterByte(regAddrL));//read 7:0
	H &= 0xF; //removes all possible 1s to create n bit size
	return 0; //lazy, fix later
}
*/

/*
RotPublisher definitions
*/
void RotPublisher::setRot_Error(uint16_t error)
{
	errorNewest_ = (static_cast<float>(error)/4096.0) * 360.0;//convert to deg
}

void RotPublisher::setRot_Dir(bool dir)
{
	dirNewest_ = dir;
}

void RotPublisher::dirUpdate_()
{
	pEncoder_->setPos(pEncoder_->readHLbytes(progVars::addrPosH,progVars::addrPosL));
	//std::cout<<"count: "<< count_ << " encoder pos: "<< pEncoder_->getPos()<< std::endl;
	this->setRot_Error(pEncoder_->getPos());
}
