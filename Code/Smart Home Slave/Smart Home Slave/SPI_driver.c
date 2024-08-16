/*
 * SPI.c
 *
 * Created: 9/5/2017 8:35:09 PM
 * 
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>
#include "DIO.h"
#include "std_macros.h"
#include "ADC_driver.h"
#include "LED.h"
#define F_CPU 8000000UL
#include <util/delay.h>

void SPI_MasterInit(void)
{
	/*Set SS / MOSI / SCK  as output pins for master*/
	DIO_vsetPINDir('B',4,1);
	DIO_vsetPINDir('B',5,1);
	DIO_vsetPINDir('B',7,1);
	/*Enable Master mode*/
	SET_BIT(SPCR,MSTR);
	/*Set clock to fosc/16*/
	SET_BIT(SPCR,SPR0);
	/*Enable SPI*/
    SET_BIT(SPCR,SPE);
	/*set SS to high */
	DIO_write('B',4,1);
}

void SPI_SlaveInit(void)
{
	/*Enable SPI*/
	SET_BIT(SPCR,SPE);
	/*Set MISO as output*/
	DIO_vsetPINDir('B',6,1);
}

unsigned char SPI_MasterTransmitchar(unsigned char Data)
{
	/*Clear SS to send data to slave*/
	DIO_write('B',4,0);
	/*Put data on SPDR*/
	SPDR=Data;
	/*Wait until the transmission is finished*/
     while(READ_BIT(SPSR,SPIF)==0);	
	 /*read SPDR*/
	 return SPDR ;
}

unsigned char SPI_SlaveReceivechar(unsigned char Data)
{
	/*Put data on SPDR*/
	SPDR=Data;
	READ_BIT(SPSR,WCOL);
	SPDR;
	/*Wait until data is received in SPI register*/
	extern unsigned char aircondon;
	extern unsigned char tempset;
	extern currenttemp;
	extern firstdigit;
	extern seconddigit;
	tempset = (firstdigit-48)*10 + (seconddigit-48);
	do{
		currenttemp = 0.25 * ADC_u16Read();
		if(tempset < currenttemp && aircondon == 1)
		{
			LED_on('C',5);
		}
		else if(tempset >= currenttemp)
		{
			LED_off('C',5);
		}
	}while(READ_BIT(SPSR,SPIF)==0);
	
	return SPDR ;
}

void SPI_MasterTransmitstring(unsigned char *ptr)
{
	while((*ptr)!=0)
	{
		SPI_MasterTransmitchar(*ptr);
		_delay_ms(300);
		ptr++;
	}
}




