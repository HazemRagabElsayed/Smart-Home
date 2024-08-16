/*
 * EEPROM_DRIVER.h
 *
 * Created: 09/03/2024 03:23:27 ص
 *  Author: Hazem Ragab Elsayed
 */ 


#ifndef EEPROM_DRIVER_H_
#define EEPROM_DRIVER_H_

void EEPROM_write(unsigned short address,unsigned char data);
char EEPROM_read(unsigned short address);

#endif /* EEPROM_DRIVER_H_ */