/*
 * EEPROM_DRIVER.c
 *
 * Created: 09/03/2024 03:23:04 ص
 *  Author: Hazem Ragab Elsayed
 */ 
#include"std_macros.h"
#include<avr/io.h>

void EEPROM_write(unsigned short address,unsigned char data)
{
	EEARL = (char) address;
	EEARH = (char)(address>>8);
	EEDR = data;
	SET_BIT(EECR,EEMWE);
	SET_BIT(EECR,EEWE);
	while(1 == READ_BIT(EECR,EEWE));
	
}
char EEPROM_read(unsigned short address)
{
	EEARL = (char) address;
	EEARH = (char) (address>>8);
	SET_BIT(EECR,EERE);
	return EEDR;
}