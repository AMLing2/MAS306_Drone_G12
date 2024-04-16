#include <iostream>
#include <libusbp.hpp>

int main()
{
	std::cout<<"Listing Devices:" <<std::endl;
	libusbp_device*** devicesNull;
	size_t* devicesNumber;
	if(libusbp_list_connected_devices(devicesNull,devicesNumber) == NULL)
	{
		std::cout<<"no error"<<std::endl;
	}

	return 0;
}
