#ifndef OLED_H_
#define OLED_H_

#include "STD_TYPES.h"

#define OLED_ADDRESS    0x3C

void OLED_Init(void);
void OLED_Clear(void);
void OLED_SetCursor(u8 row, u8 column);
void OLED_WriteChar(u8 data);
void OLED_WriteString(char *str);

#endif