#ifndef GPIO_H_
#define GPIO_H_

#include "STD_TYPES.h"

void GPIO_Init(void);

void GPIO_SetPinOutput(u8 pin);
void GPIO_SetPinInput(u8 pin);

void GPIO_SetPinHigh(u8 pin);
void GPIO_SetPinLow(u8 pin);

#endif