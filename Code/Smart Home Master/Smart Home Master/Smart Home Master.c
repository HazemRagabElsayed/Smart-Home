/*
 * Smart_Home_Master.c
 *
 * Created: 18/04/2024 12:03:36 م
 *  Author: Hazem Ragab Elsayed
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

#include"LCD.h"
#include"keypad_driver.h"
#include"ADC_driver.h"
#include"EEPROM_DRIVER.h"
#include"spi_driver.h"
#include"timer.h"


#define ADMIN_AND_GUEST_SET 0x20
#define NOTSET 0xff
#define ADMIN_PASSWORD_OFFSET_Location 0x00
#define Guest_PASSWORD_OFFSET_Location 0x04
#define TRIES_ALLOWED 3
#define BLOCK_ENABLE 0x10
#define SESSION_TIME_OUTX100 5000  // 5000 means 50 seconds
#define F_CPU 8000000UL
#include<util/delay.h>

#include<avr/interrupt.h>
#include <avr/io.h>
unsigned char passwordindex = 0;
char selection = NOTPRESSED;
char firstdigit = NOTPRESSED;
char seconddigit = NOTPRESSED;

unsigned char adminflag = 1;
unsigned char guestflag = 1;
unsigned char modeentered = 0;
unsigned char triesleft = TRIES_ALLOWED;
volatile unsigned short counter;
char password[4] = {NOTPRESSED,NOTPRESSED,NOTPRESSED,NOTPRESSED};
int main(void)
{
	char status = '1';
	//Admin mode LED,Guest mode LED,Block mode LED (LED is active
	LED_init('C',0);
	LED_init('C',1);
	LED_init('C',2);
	LCD_vInit();
	keypad_vInit();
	ADC_vinit();
	SPI_MasterInit();
	timer_CTC_init_interrupt();
	if(EEPROM_read(BLOCK_ENABLE) == 0x00)
	{
		LED_on('C',2);
		LCD_vSend_string("login blocked");
		LCD_movecursor(2,1);
		LCD_vSend_string("wait 20 seconds");
		_delay_ms(20000);
		LCD_clearscreen();
		EEPROM_write(BLOCK_ENABLE,0xFF);
		LED_off('C',2);
	}
	LCD_vSend_string("Welcome to Smart");
	LCD_movecursor(2,1);
	LCD_vSend_string("home system");
	_delay_ms(700);
	start:
	modeentered = 0;
	counter=1;
	LCD_clearscreen();
	_delay_ms(300);

    while(1)
    {
		if(NOTSET == EEPROM_read(ADMIN_AND_GUEST_SET))
		{
			LCD_vSend_string("Login for");
			LCD_movecursor(2,1);
			LCD_vSend_string("first time");
			_delay_ms(700);
			LCD_clearscreen();
			LCD_vSend_string("Set Admin pass");
			LCD_movecursor(2,1);
			LCD_vSend_string("Admin pass:");
			for(passwordindex = 0;passwordindex<4;passwordindex++)
			{
				do{
					password[passwordindex] = keypad_u8check_press();
				}while(NOTPRESSED == password[passwordindex]);
				_delay_ms(280);
				LCD_vSend_char(password[passwordindex]);
				_delay_ms(400);
				LCD_movecursor(2,12+passwordindex);
				LCD_vSend_char('*');
				EEPROM_write(ADMIN_PASSWORD_OFFSET_Location + passwordindex,password[passwordindex]);
			}
			LCD_clearscreen();
			LCD_vSend_string("Pass Saved");
			_delay_ms(200);
			LCD_clearscreen();
			LCD_vSend_string("Set Guest pass");
			LCD_movecursor(2,1);
			LCD_vSend_string("Guest pass:");
			for(passwordindex = 0;passwordindex<4;passwordindex++)
			{
				
				do{
					password[passwordindex] = keypad_u8check_press();
				}while(NOTPRESSED == password[passwordindex]);
				_delay_ms(280);
				LCD_vSend_char(password[passwordindex]);
				_delay_ms(400);
				LCD_movecursor(2,12+passwordindex);
				LCD_vSend_char('*');
				EEPROM_write(Guest_PASSWORD_OFFSET_Location + passwordindex,password[passwordindex]);
			}
			EEPROM_write(ADMIN_AND_GUEST_SET,0x00);
			LCD_clearscreen();
			LCD_vSend_string("Pass Saved");
			_delay_ms(200);
			LCD_clearscreen();
		}
		else
		{
			LCD_vSend_string("Select mode :");
			LCD_movecursor(2,1);
			LCD_vSend_string("0:Admin 1:Guest");
			do{
				selection = keypad_u8check_press();
			}while(NOTPRESSED == selection);
			_delay_ms(250);
			LCD_clearscreen();
			if('0'== selection)
			{
				LCD_vSend_string("Admin mode");
				LCD_movecursor(2,1);
				LCD_vSend_string("Enter pass:");
				for(passwordindex = 0;passwordindex<4;passwordindex++)
				{
					do{
						password[passwordindex] = keypad_u8check_press();
					}while(NOTPRESSED == password[passwordindex]);
					_delay_ms(280);
					LCD_vSend_char(password[passwordindex]);
					_delay_ms(100);
					LCD_movecursor(2,12+passwordindex);
					LCD_vSend_char('*');
					if(EEPROM_read(ADMIN_PASSWORD_OFFSET_Location + passwordindex) != password[passwordindex])
					{
						adminflag = 0;
					}
				}
				
				if(1 == adminflag)
				{
					LED_on('C',0);
					TCNT0 = 0;
					modeentered = 1;
					LCD_clearscreen();
					LCD_vSend_string("Right pass");
					LCD_movecursor(2,1);
					LCD_vSend_string("Admin mode");
					_delay_ms(500);
					LCD_clearscreen();
					LCD_vSend_string("1:Room1 2:Room2");
					LCD_movecursor(2,1);
					LCD_vSend_string("3:Room3 4:More");
					while(1)
					{
						do{
							selection = keypad_u8check_press();
							if(counter >= SESSION_TIME_OUTX100)
							{
								goto start;
							}
						}while(NOTPRESSED == selection);
						_delay_ms(280);
						LCD_clearscreen();
						switch(selection)
						{
							case '1':
							SPI_MasterTransmitchar(R1S);
							_delay_ms(1);
							status = SPI_MasterTransmitchar(0xff);
							LCD_vSend_string("Room1 S:");
							
							if(R1ON == status)
							{
								LCD_vSend_string("ON");
							}
							else if(R1OFF == status)
							{
								LCD_vSend_string("OFF");
							}
							LCD_movecursor(2,1);
							LCD_vSend_string("1-on 2-off 0-RET");
							do{
								selection = keypad_u8check_press();
								if(counter >= SESSION_TIME_OUTX100)
								{
									goto start;
								}
							}while(NOTPRESSED == selection);
							_delay_ms(280);
							switch(selection)
							{
								case '1':
								SPI_MasterTransmitchar(R1ON);
								break;
								case '2':
								SPI_MasterTransmitchar(R1OFF);
								break;
								case '0':
								break;
								default:
								break;
							}
							break;
							case '2':
							SPI_MasterTransmitchar(R2S);
							_delay_ms(1);
							status = SPI_MasterTransmitchar(0xff);
							LCD_vSend_string("Room2 S:");
							if(R2ON == status)
							{
								LCD_vSend_string("ON");
							}
							else if(R2OFF == status)
							{
								LCD_vSend_string("OFF");
							}
							LCD_movecursor(2,1);
							LCD_vSend_string("1-on 2-off 0-RET");
							do{
								selection = keypad_u8check_press();
								if(counter >= SESSION_TIME_OUTX100)
								{
									goto start;
								}
							}while(NOTPRESSED == selection);
							_delay_ms(280);
							switch(selection)
							{
								case '1':
								SPI_MasterTransmitchar(R2ON);
								break;
								case '2':
								SPI_MasterTransmitchar(R2OFF);
								break;
								case '0':
								break;
								default:
								break;
							}
							break;
							case '3':
							SPI_MasterTransmitchar(R3S);
							_delay_ms(1);
							status = SPI_MasterTransmitchar(0xff);
							LCD_vSend_string("Room3 S:");
							if(R3ON == status)
							{
								LCD_vSend_string("ON");
							}
							else if(R3OFF == status)
							{
								LCD_vSend_string("OFF");
							}
							LCD_movecursor(2,1);
							LCD_vSend_string("1-on 2-off 0-RET");
							do{
								selection = keypad_u8check_press();
								if(counter >= SESSION_TIME_OUTX100)
								{
									goto start;
								}
							}while(NOTPRESSED == selection);
							_delay_ms(280);
							switch(selection)
							{
								case '1':
								SPI_MasterTransmitchar(R3ON);
								break;
								case '2':
								SPI_MasterTransmitchar(R3OFF);
								break;
								case '0':
								break;
								default:
								break;
							}
							break;
							case '4':
							RETMENU:
							LCD_clearscreen();
							LCD_vSend_string("1:Room4");
							LCD_movecursor(1,12);
							LCD_vSend_string("2:TV");
							LCD_movecursor(2,1);
							LCD_vSend_string("3:Air Cond.4:RET");
							do{
								selection = keypad_u8check_press();
								if(counter >= SESSION_TIME_OUTX100)
								{
									goto start;
								}
							}while(NOTPRESSED == selection);
							_delay_ms(280);
							switch(selection)
							{
								case '1':
								LCD_clearscreen();
								SPI_MasterTransmitchar(R4S);
								_delay_ms(1);
								status = SPI_MasterTransmitchar(0xff);
								LCD_vSend_string("Room4 S:");
								if(R4ON == status)
								{
									LCD_vSend_string("ON");
								}
								else if(R4OFF == status)
								{
									LCD_vSend_string("OFF");
								}
								LCD_movecursor(2,1);
								LCD_vSend_string("1-on 2-off 0-RET");
								do{
									selection = keypad_u8check_press();
									if(counter >= SESSION_TIME_OUTX100)
									{
										goto start;
									}
								}while(NOTPRESSED == selection);
								_delay_ms(280);
								switch(selection)
								{
									case '1':
									SPI_MasterTransmitchar(R4ON);
									break;
									case '2':
									SPI_MasterTransmitchar(R4OFF);
									break;
									case '0':
									goto RETMENU;
									break;
									default:
									break;
								}
								break;
								case '2':
								LCD_clearscreen();
								SPI_MasterTransmitchar(TVS);
								_delay_ms(1);
								status = SPI_MasterTransmitchar(0xff);
								LCD_vSend_string("TV S:");
								if(TVON == status)
								{
									LCD_vSend_string("ON");
								}
								else if(TVOFF == status)
								{
									LCD_vSend_string("OFF");
								}
								LCD_movecursor(2,1);
								LCD_vSend_string("1-on 2-off 0-RET");
								do{
									selection = keypad_u8check_press();
									if(counter >= SESSION_TIME_OUTX100)
									{
										goto start;
									}
								}while(NOTPRESSED == selection);
								_delay_ms(280);
								switch(selection)
								{
									case '1':
									SPI_MasterTransmitchar(TVON);
									break;
									case '2':
									SPI_MasterTransmitchar(TVOFF);
									break;
									case '0':
									goto RETMENU;
									break;
									default:
									break;
								}
								break;
								case '3':
								RETAIRCond:
								LCD_clearscreen();
								LCD_vSend_string("1:Set Temp");
								LCD_movecursor(2,1);
								LCD_vSend_string("2:Control");
								LCD_movecursor(2,12);
								LCD_vSend_string("0:RET");
								do{
									selection = keypad_u8check_press();
									if(counter >= SESSION_TIME_OUTX100)
									{
										goto start;
									}
								}while(NOTPRESSED == selection);
								_delay_ms(280);
								LCD_clearscreen();
								switch (selection)
								{
									case '1' :  
									LCD_vSend_string("Set temp.:--");
									LCD_vSend_char(' ');
									//LCD Send Degree character
									LCD_vSend_char(0xdf);
									LCD_vSend_char('C');
									LCD_movecursor(1,11);
									_delay_ms(1);
									do{
										firstdigit = keypad_u8check_press();
										if(counter >= SESSION_TIME_OUTX100)
										{
											goto start;
										}
									}while(NOTPRESSED == firstdigit);
									_delay_ms(280);
									LCD_vSend_char(firstdigit);
									SPI_MasterTransmitchar(AIRCONDTSET1);
									_delay_ms(1);
									SPI_MasterTransmitchar(firstdigit);
									_delay_ms(1);
									
									do{
										seconddigit = keypad_u8check_press();
										if(counter >= SESSION_TIME_OUTX100)
										{
											goto start;
										}
									}while(NOTPRESSED == seconddigit);
									_delay_ms(281);
									LCD_vSend_char(seconddigit);
									SPI_MasterTransmitchar(AIRCONDTSET2);
									_delay_ms(1);
									SPI_MasterTransmitchar(seconddigit);
									LCD_clearscreen();
									_delay_ms(700);
									LCD_vSend_string("Temperature Sent");
									goto RETAIRCond;
									break;
									case '2' :
									SPI_MasterTransmitchar(AIRCONDS);
									_delay_ms(1);
									status = SPI_MasterTransmitchar(0xff);
									LCD_vSend_string("Air Cond. S:");
									if(AIRCONDON == status)
									{
										LCD_vSend_string("ON");
									}
									else if(AIRCONDOFF == status)
									{
										LCD_vSend_string("OFF");
									}
									LCD_movecursor(2,1);
									LCD_vSend_string("1-On 2-Off 0-RET");
									do{
										selection = keypad_u8check_press();
										if(counter >= SESSION_TIME_OUTX100)
										{
											goto start;
										}
									}while(NOTPRESSED == selection);
									_delay_ms(280);
									switch(selection)
									{
										case '1':
										SPI_MasterTransmitchar(AIRCONDON);
										break;
										case '2':
										SPI_MasterTransmitchar(AIRCONDOFF);
										break;
										case '0':
										goto RETAIRCond;
										break;
										default:
										break;
									}
									
									
									break;
									case '0' :
									goto RETMENU;
									break;
									default:
									
									break;
								}
								break;
								case '4':
								break;
								default:
								break;
							}								
							break;
							default:
							LCD_vSend_string("wrong input");
							_delay_ms(1000);
							LCD_clearscreen();
						}
						LCD_clearscreen();
						LCD_vSend_string("1:Room1 2:Room2");
						LCD_movecursor(2,1);
						LCD_vSend_string("3:Room3 4:More");
						
					}
					
				}
			}
			else if('1' == selection)
			{
				LCD_vSend_string("Guest mode");
				LCD_movecursor(2,1);
				LCD_vSend_string("Enter pass:");
				for(passwordindex = 0;passwordindex<4;passwordindex++)
				{
					do{
						password[passwordindex] = keypad_u8check_press();
					}while(NOTPRESSED == password[passwordindex]);
					_delay_ms(280);
					LCD_vSend_char(password[passwordindex]);
					_delay_ms(100);
					LCD_movecursor(2,12+passwordindex);
					LCD_vSend_char('*');
					if(EEPROM_read(Guest_PASSWORD_OFFSET_Location + passwordindex) != password[passwordindex])
					{
						guestflag = 0;
					}
				}
				if(1 == guestflag)
				{
					TCNT0 = 0;
					LED_on('C',1);
					modeentered = 1;
					LCD_clearscreen();
					LCD_vSend_string("Right pass");
					LCD_movecursor(2,1);
					LCD_vSend_string("Guest mode");
					_delay_ms(300);
					LCD_clearscreen();
					LCD_vSend_string("1:Room1 2:Room2");
					LCD_movecursor(2,1);
					LCD_vSend_string("3:Room3 4:Room4");
					while(1)
					{
						do{
							selection = keypad_u8check_press();
							if(counter >= SESSION_TIME_OUTX100)
							{
								goto start;
							}
						}while(NOTPRESSED == selection);
						_delay_ms(280);
						LCD_clearscreen();
						switch(selection)
						{
							case '1':
							SPI_MasterTransmitchar(R1S);
							_delay_ms(1);
							status = SPI_MasterTransmitchar(0xff);
							LCD_vSend_string("Room1 S:");
							if(R1ON == status)
							{
								LCD_vSend_string("ON");
							}
							else if(R1OFF == status)
							{
								LCD_vSend_string("OFF");
							}
							LCD_movecursor(2,1);
							LCD_vSend_string("1-on 2-off 0-RET");
							do{
								selection = keypad_u8check_press();
								if(counter >= SESSION_TIME_OUTX100)
								{
									goto start;
								}
							}while(NOTPRESSED == selection);
							_delay_ms(280);
							switch(selection)
							{
								case '1':
								SPI_MasterTransmitchar(R1ON);
								break;
								case '2':
								SPI_MasterTransmitchar(R1OFF);
								break;
								case '0':
								break;
								default:
								break;
							}
							break;
							case '2':
							SPI_MasterTransmitchar(R2S);
							_delay_ms(1);
							status = SPI_MasterTransmitchar(0xff);
							LCD_vSend_string("Room2 S:");
							if(R2ON == status)
							{
								LCD_vSend_string("ON");
							}
							else if(R2OFF == status)
							{
								LCD_vSend_string("OFF");
							}
							LCD_movecursor(2,1);
							LCD_vSend_string("1-on 2-off 0-RET");
							do{
								selection = keypad_u8check_press();
								if(counter >= SESSION_TIME_OUTX100)
								{
									goto start;
								}
							}while(NOTPRESSED == selection);
							_delay_ms(280);
							switch(selection)
							{
								case '1':
								SPI_MasterTransmitchar(R2ON);
								break;
								case '2':
								SPI_MasterTransmitchar(R2OFF);
								break;
								case '0':
								break;
								default:
								break;
							}
							break;
							case '3':
							SPI_MasterTransmitchar(R3S);
							_delay_ms(1);
							status = SPI_MasterTransmitchar(0xff);
							LCD_vSend_string("Room3 S:");
							if(R3ON == status)
							{
								LCD_vSend_string("ON");
							}
							else if(R3OFF == status)
							{
								LCD_vSend_string("OFF");
							}
							LCD_movecursor(2,1);
							LCD_vSend_string("1-on 2-off 0-RET");
							do{
								selection = keypad_u8check_press();
								if(counter >= SESSION_TIME_OUTX100)
								{
									goto start;
								}
							}while(NOTPRESSED == selection);
							_delay_ms(280);
							switch(selection)
							{
								case '1':
								SPI_MasterTransmitchar(R3ON);
								break;
								case '2':
								SPI_MasterTransmitchar(R3OFF);
								break;
								case '0':
								break;
								default:
								break;
							}
							break;
							case '4':
							SPI_MasterTransmitchar(R4S);
							_delay_ms(1);
							status = SPI_MasterTransmitchar(0xff);
							LCD_vSend_string("Room4 S:");
							if(R4ON == status)
							{
								LCD_vSend_string("ON");
							}
							else if(R4OFF == status)
							{
								LCD_vSend_string("OFF");
							}
							LCD_movecursor(2,1);
							LCD_vSend_string("1-on 2-off 0-RET");
							do{
								selection = keypad_u8check_press();
								if(counter >= SESSION_TIME_OUTX100)
								{
									goto start;
								}
							}while(NOTPRESSED == selection);
							_delay_ms(280);
							switch(selection)
							{
								case '1':
								SPI_MasterTransmitchar(R4ON);
								break;
								case '2':
								SPI_MasterTransmitchar(R4OFF);
								break;
								case '0':
								break;
								default:
								break;
							}
							break;
							default:
							LCD_vSend_string("wrong input");
							_delay_ms(1000);
							LCD_clearscreen();
						}
						LCD_clearscreen();
						LCD_vSend_string("1:Room1 2:Room2");
						LCD_movecursor(2,1);
						LCD_vSend_string("3:Room3 4:Room4");
						
					}
					
				}
				
				}
				if(0 == guestflag | 0 == adminflag)
				{
					triesleft--;
					LCD_clearscreen();
					LCD_vSend_string("Wrong pass");
					LCD_movecursor(2,1);
					LCD_vSend_string("Tries left:");
					LCD_vSend_char(triesleft+48);
					_delay_ms(500);
					LCD_clearscreen();
					guestflag = adminflag = 1;
					if(0 == triesleft)
					{
						EEPROM_write(BLOCK_ENABLE,0x00);
						LED_on('C',2);
						LCD_vSend_string("login blocked");
						LCD_movecursor(2,1);
						LCD_vSend_string("wait 20 seconds");
						_delay_ms(20000);
						LCD_clearscreen();
						triesleft = TRIES_ALLOWED;
						LED_off('C',2);
					}
				else
				{
					
				}
			}
			else
			{
				
			}
			
		} 
    }
}

ISR(TIMER0_COMP_vect)
{
	if(1 == modeentered)
	{
		counter++;
		if(counter >= SESSION_TIME_OUTX100)
		{
			LCD_clearscreen();
			LED_off('C',0);
			LED_off('C',1);
			LCD_vSend_string("Session Timeout");
			modeentered = 0;
			_delay_ms(700);
			
		}
	}

	
}