#ifndef UART_H_
#define UART_H_

#include "STD_TYPES.h"

void UART_Init(u16 baud_rate);

void UART_SendChar(u8 data);
u8 UART_ReceiveChar(void);

void UART_SendString(char *str);
void UART_SendHex(u8 data);

#endif