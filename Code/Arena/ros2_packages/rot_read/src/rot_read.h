#include <pigpio.h>

void shutdownSeq();

class IErrorHandle
{
public:
	virtual void errori2cOpen() = 0;
	virtual void errorGpioInitialize() = 0;
};

class I2CDevice : public IErrorHandle //abstract for general i2c
{
public:
	I2CDevice(unsigned int addr, unsigned int bus)
	:addr_(addr)
	,bus_(bus)
	{
		i2cInit();
	}
	virtual ~I2CDevice() = default;

	int getHandle(){return handle_;}
	virtual unsigned int readRegisterByte(const uint8_t& regAddr) = 0;
	
	void errorGpioInitialize() override{} //unused
protected:
	const unsigned int addr_;
	const unsigned int bus_;
	int handle_;
	void i2cInit()
	{
		handle_ = i2cOpen(bus_,addr_,0);
		if(handle_<0)
		{
			errori2cOpen();
		}
	}
};
