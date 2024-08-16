/*
 * Smart_Home_Slave.c
 *
 * Created: 24/04/2024 09:27:56 ص
 *  Author: FREINDS
 */ 

/*
Macros of 8 bits used by Master:
-to Ask slave what are status of devices and
-to give slave a command to do action to these devices.
*/

#define R1S '0'
#define R2S '1'
#define R3S '2'
#define R4S '3'
#define R1ON '4'
#define R1OFF '5'
#define R2ON '6'
#define R2OFF '7'
#define R3ON '8'
#define R3OFF '9'
#define R4ON 'A'
#define R4OFF 'B'
#define TVS 'C'
#define TVON 'D'
#define TVOFF 'E'
#define AIRCONDS 'F'
#define AIRCONDON 'G'
#define AIRCONDOFF 'J'
#define AIRCONDTSET1 'H'
#define AIRCONDTSET2 'K'
#define F_CPU 8000000UL
#include"DIO.h"
#include"LED.h"
#include"spi_driver.h"
#include"ADC_driver.h"
#include"std_macros.h"

char firstdigit = '1';
char seconddigit = '0';
volatile unsigned char tempset;
volatile unsigned short currenttemp = 1;
unsigned char aircondon = 0;
#include <avr/io.h>
#include<util/delay.h>



int main(void)
{
	unsigned char status_action = 0xe3;
	ADC_vinit();
	SPI_SlaveInit();
	LED_init('C',0);
	LED_init('C',1);
	LED_init('C',2);
	LED_init('C',3);
	LED_init('C',4);
	LED_init('C',5);
	
	
    while(1)
    {
		

		status_action = SPI_SlaveReceivechar(0xff);
		READ_BIT(SPSR,WCOL);
		status_action = SPDR;
		switch(status_action)
		{
			case R1S :
			if(1 == LED_readstatus('C',0))
			{
				SPI_SlaveReceivechar(R1ON);
			}
			else if(0 == LED_readstatus('C',0))
			{
				SPI_SlaveReceivechar(R1OFF);
			}
			break;
			case R2S :
			if(1 == LED_readstatus('C',1))
			{
				SPI_SlaveReceivechar(R2ON);
			}
			else if(0 == LED_readstatus('C',1))
			{
				SPI_SlaveReceivechar(R2OFF);
			}
			break;
			case R3S :
			if(1 == LED_readstatus('C',2))
			{
				SPI_SlaveReceivechar(R3ON);
			}
			else if(0 == LED_readstatus('C',2))
			{
				SPI_SlaveReceivechar(R3OFF);
			}
			break;
			case R4S :
			if(1 == LED_readstatus('C',3))
			{
				SPI_SlaveReceivechar(R4ON);
			}
			else if(0 == LED_readstatus('C',3))
			{
				SPI_SlaveReceivechar(R4OFF);
			}
			break;
			case R1ON :
			LED_on('C',0);
			break;
			case R1OFF :
			LED_off('C',0);
			break;
			case R2ON :
			LED_on('C',1);
			break;
			case R2OFF :
			LED_off('C',1);
			break;
			case R3ON :
			LED_on('C',2);
			break;
			case R3OFF :
			LED_off('C',2);
			break;
			case R4ON :
			LED_on('C',3);
			break;
			case R4OFF :
			LED_off('C',3);
			break;
			case TVS :
			if(1 == LED_readstatus('C',4))
			{
				SPI_SlaveReceivechar(TVON);
			}
			else if (0 == LED_readstatus('C',4))
			{
				SPI_SlaveReceivechar(TVOFF);
			}
			break;
			case TVON :
			LED_on('C',4);
			break;
			case TVOFF :
			LED_off('C',4);
			break;
			case AIRCONDS :
			if(1 == LED_readstatus('C',5))
			{
				SPI_SlaveReceivechar(AIRCONDON);
			}
			else if (0 == LED_readstatus('C',5))
			{
				SPI_SlaveReceivechar(AIRCONDOFF);
			}
			break;
			case AIRCONDON :
			aircondon = 1;
			break;
			case AIRCONDOFF :
			aircondon = 0;
			break;
			case AIRCONDTSET1 :
	        firstdigit = SPI_SlaveReceivechar(0xe1);
			break;
			case AIRCONDTSET2 :
			seconddigit = SPI_SlaveReceivechar(0xe2);
			
			default:
			break;
			
		}
		READ_BIT(SPSR,WCOL);
		SPDR;
		tempset = (firstdigit-48)*10 + (seconddigit-48);
		
	}
		
    }

