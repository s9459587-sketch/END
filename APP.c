#define F_CPU 16000000UL

#include <avr/io.h>
#include <util/delay.h>

#include "STD_TYPES.h"
#include "BIT_MATH.h"
#include "config.h"

#include "APP.h"
#include "WHITELIST.h"
#include "EEPROM.h"
#include "OLED.h"
#include "UART.h"
#include "RFID.h"
#include "RTC.h"
#include "GPIO.h"

#define LOG_START       600
#define LOG_EVENT_SIZE  20

#define LOG_COUNT_ADDR  570
#define LOG_INDEX_ADDR  571

u8 fails = 0;
u8 log_count = 0;
u8 log_index = 0;

static void APP_PrintUID(u8 *uid, u8 size)
{
	u8 i;

	UART_SendString("UID: ");

	for(i=0; i<size; i++)
	{
		UART_SendHex(uid[i]);
		UART_SendChar(' ');
	}

	UART_SendString("\r\n");
}

static void APP_Log(u8 *uid, u8 size, u8 result)
{
	RTC_Time time;
	u16 address;
	u8 i;

	RTC_GetTime(&time);

	address = LOG_START + (log_index * LOG_EVENT_SIZE);

	EEPROM_WriteByte(address, result);
	EEPROM_WriteByte(address + 1, size);

	for(i=0; i<size; i++)
	{
		EEPROM_WriteByte(address + 2 + i, uid[i]);
	}

	EEPROM_WriteByte(address + 12, time.seconds);
	EEPROM_WriteByte(address + 13, time.minutes);
	EEPROM_WriteByte(address + 14, time.hours);
	EEPROM_WriteByte(address + 15, time.day);
	EEPROM_WriteByte(address + 16, time.date);
	EEPROM_WriteByte(address + 17, time.month);
	EEPROM_WriteByte(address + 18, time.year);

	log_index++;

	if(log_index >= LOG_SIZE)
	log_index = 0;

	if(log_count < LOG_SIZE)
	log_count++;

	EEPROM_WriteByte(LOG_COUNT_ADDR, log_count);
	EEPROM_WriteByte(LOG_INDEX_ADDR, log_index);
}

static void APP_Dump(void)
{
	u8 i,j;
	u8 size;
	u8 result;
	u8 uid[10];

	u8 seconds;
	u8 minutes;
	u8 hours;
	u8 date;
	u8 month;
	u8 year;

	u8 start;
	u16 address;

	if(log_count == 0)
	{
		UART_SendString("EMPTY\r\n");
		return;
	}

	if(log_count < LOG_SIZE)
	start = 0;
	else
	start = log_index;

	for(i=0; i<log_count; i++)
	{
		address = LOG_START + (((start + i) % LOG_SIZE) * LOG_EVENT_SIZE);

		result = EEPROM_ReadByte(address);
		size = EEPROM_ReadByte(address + 1);

		for(j=0; j<size; j++)
		{
			uid[j] = EEPROM_ReadByte(address + 2 + j);
		}

		seconds = EEPROM_ReadByte(address + 12);
		minutes = EEPROM_ReadByte(address + 13);
		hours   = EEPROM_ReadByte(address + 14);

		date  = EEPROM_ReadByte(address + 16);
		month = EEPROM_ReadByte(address + 17);
		year  = EEPROM_ReadByte(address + 18);

		if(result == 1)
		UART_SendString("ACCESS GRANTED\r\n");
		else
		UART_SendString("ACCESS DENIED\r\n");

		APP_PrintUID(uid,size);

		UART_SendString("TIME: ");

		UART_SendHex(date);
		UART_SendChar('/');
		UART_SendHex(month);
		UART_SendChar('/');
		UART_SendHex(year);

		UART_SendChar(' ');

		UART_SendHex(hours);
		UART_SendChar(':');
		UART_SendHex(minutes);
		UART_SendChar(':');
		UART_SendHex(seconds);

		UART_SendString("\r\n\r\n");
	}
}

static u8 APP_Hex(char data)
{
	if(data >= '0' && data <= '9')
	return data - '0';

	if(data >= 'A' && data <= 'F')
	return data - 'A' + 10;

	if(data >= 'a' && data <= 'f')
	return data - 'a' + 10;

	return 0xFF;
}

static u8 APP_GetUID(char *command, u8 *uid)
{
	u8 i;
	u8 size = 0;
	u8 high;
	u8 low;

	for(i=0; i<20; i++)
	{
		if(command[i] == '\0')
		break;

		if(command[i] == ' ')
		continue;

		high = APP_Hex(command[i]);
		low = APP_Hex(command[i+1]);

		if(high == 0xFF || low == 0xFF)
		break;

		uid[size] = (high << 4) | low;
		size++;

		i++;
	}

	return size;
}

static u8 APP_GetNumber(char *command, u8 start)
{
	u8 number = 0;
	u8 i;

	for(i=start; command[i]!='\0'; i++)
	{
		if(command[i] >= '0' && command[i] <= '9')
		{
			number = (number * 10) + (command[i] - '0');
		}
		else
		{
			break;
		}
	}

	return number;
}

static void APP_SetTime(char *command)
{
	RTC_Time time;

	time.year    = (APP_GetNumber(command, 6) * 10) + APP_GetNumber(command, 7);
	time.month   = (APP_GetNumber(command, 11) * 10) + APP_GetNumber(command, 12);
	time.date    = (APP_GetNumber(command, 14) * 10) + APP_GetNumber(command, 15);

	time.hours   = (APP_GetNumber(command, 17) * 10) + APP_GetNumber(command, 18);
	time.minutes = (APP_GetNumber(command, 20) * 10) + APP_GetNumber(command, 21);
	time.seconds = (APP_GetNumber(command, 23) * 10) + APP_GetNumber(command, 24);

	time.day = 1;

	RTC_SetTime(&time);

	UART_SendString("OK\r\n");
}

static void APP_Command(void)
{
	char command[30];
	u8 i;
	u8 uid[10];
	u8 size;

	if(GET_BIT(UCSR0A,RXC0) == 0)
	return;

	for(i=0; i<29; i++)
	{
		command[i] = UART_ReceiveChar();

		if(command[i] == '\r' || command[i] == '\n')
		{
			command[i] = '\0';
			break;
		}
	}

	command[29] = '\0';

	if(command[0]=='A' && command[1]=='D' && command[2]=='D')
	{
		size = APP_GetUID(command + 4,uid);

		if(WHITELIST_Add(uid,size))
		UART_SendString("OK\r\n");
		else
		UART_SendString("ERR FULL\r\n");
	}

	else if(command[0]=='D' && command[1]=='E' && command[2]=='L')
	{
		size = APP_GetUID(command + 4,uid);

		if(WHITELIST_Delete(uid,size))
		UART_SendString("OK\r\n");
		else
		UART_SendString("ERR\r\n");
	}

	else if(command[0]=='D' && command[1]=='U' && command[2]=='M' && command[3]=='P')
	{
		APP_Dump();
	}

	else if(command[0]=='S' && command[1]=='E' && command[2]=='T')
	{
		APP_SetTime(command);
	}
}

void APP_Init(void)
{
	fails = 0;

	log_count = EEPROM_ReadByte(LOG_COUNT_ADDR);
	log_index = EEPROM_ReadByte(LOG_INDEX_ADDR);

	if(log_count > LOG_SIZE || log_index >= LOG_SIZE)
	{
		log_count = 0;
		log_index = 0;

		EEPROM_WriteByte(LOG_COUNT_ADDR,0);
		EEPROM_WriteByte(LOG_INDEX_ADDR,0);
	}
}

u8 APP_CheckCard(u8 *uid, u8 size)
{
	RTC_Time time;

	if(fails >= MAX_FAILS)
	{
		OLED_Clear();
		OLED_SetCursor(0,0);
		OLED_WriteString("LOCKED");

		GPIO_SetPinHigh(3);

		_delay_ms(LOCKOUT_TIME_S * 1000);

		GPIO_SetPinLow(3);

		fails = 0;
	}

	RTC_GetTime(&time);

	if(time.hours < STUDENT_START_H || time.hours >= STUDENT_END_H)
	{
		APP_Log(uid,size,0);

		OLED_Clear();
		OLED_SetCursor(0,0);
		OLED_WriteString("ACCESS DENIED");

		UART_SendString("ACCESS DENIED\r\n");
		APP_PrintUID(uid,size);

		return 0;
	}

	if(WHITELIST_Check(uid,size))
	{
		fails = 0;

		APP_Log(uid,size,1);

		OLED_Clear();
		OLED_SetCursor(0,0);
		OLED_WriteString("ACCESS GRANTED");

		GPIO_SetPinHigh(0);

		UART_SendString("ACCESS GRANTED\r\n");
		APP_PrintUID(uid,size);

		_delay_ms(500);

		GPIO_SetPinLow(0);

		return 1;
	}

	fails++;

	APP_Log(uid,size,0);

	OLED_Clear();
	OLED_SetCursor(0,0);
	OLED_WriteString("ACCESS DENIED");

	GPIO_SetPinHigh(3);

	UART_SendString("ACCESS DENIED\r\n");
	APP_PrintUID(uid,size);

	_delay_ms(500);

	GPIO_SetPinLow(3);

	return 0;
}

void APP_Run(void)
{
	RFID_Card card;

	APP_Command();

	if(RFID_IsCardPresent())
	{
		if(RFID_ReadUID(&card))
		{
			APP_CheckCard(card.uid,card.size);

			RFID_Halt();

			_delay_ms(1000);
		}
	}
}