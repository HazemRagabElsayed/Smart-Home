/*
 * LED.c
 *
 * Created: 03/03/2024 09:40:19 ص
 *  Author: Hazem Ragab Elsayed
 */ 
#include"DIO.h"

void LED_init(unsigned char portname,unsigned char pinnumber)
{
	DIO_vsetPINDir(portname,pinnumber,1);
}

void LED_on(unsigned char portname,unsigned char pinnumber)
{
	DIO_write(portname,pinnumber,1);
	
}
void LED_off(unsigned char portname,unsigned char pinnumber)
{
	DIO_write(portname,pinnumber,0);
	
}
void LED_toggle(unsigned char portname,unsigned char pinnumber)
{
	DIO_toggle(portname,pinnumber);
}
unsigned char LED_readstatus(unsigned char portname,unsigned char pinnumber)
{
	return DIO_u8read(portname,pinnumber);
}


