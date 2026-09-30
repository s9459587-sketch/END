#include <avr/io.h>

#include "STD_TYPES.h"
#include "BIT_MATH.h"
#include "UART.h"

void UART_Init(u16 baud_rate)
{
	u16 ubrr;

	ubrr = (16000000UL / (16UL * baud_rate)) - 1;

	UBRR0H = (u8)(ubrr >> 8);
	UBRR0L = (u8)ubrr;

	SET_BIT(UCSR0B, TXEN0);
	SET_BIT(UCSR0B, RXEN0);

	UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);
}

void UART_SendChar(u8 data)
{
	while(GET_BIT(UCSR0A, UDRE0) == 0);

	UDR0 = data;
}

u8 UART_ReceiveChar(void)
{
	while(GET_BIT(UCSR0A, RXC0) == 0);

	return UDR0;
}

void UART_SendString(char *str)
{
	while(*str != '\0')
	{
		UART_SendChar(*str);
		str++;
	}
}

void UART_SendHex(u8 data)
{
	u8 high;
	u8 low;

	high = (data >> 4) & 0x0F;
	low  = data & 0x0F;

	if(high < 10)
	UART_SendChar(high + '0');
	else
	UART_SendChar(high - 10 + 'A');

	if(low < 10)
	UART_SendChar(low + '0');
	else
	UART_SendChar(low - 10 + 'A');
}