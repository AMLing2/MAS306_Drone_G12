/*
 * usb_serial_test.cpp
 *
 * Created: 24/01/2024 09:51:57
 * Author : mathi
 */ 

#include <avr/io.h>

void initPorts()
{
	DDRF |= (1 << DDF5); //set pin 5 as output in data-direction of port F
	PORTF &= ~(1<<PINF5); //set pin 5 low as initial value
}

void initRegisters()
{
		
}

int main(void)
{
	initPorts();
	initRegisters();
	
    while (1) 
    {
		PORTF |= (1<<PINF5); //set pin 5 high
    }
}

