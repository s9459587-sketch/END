#include <avr/io.h>

#include "STD_TYPES.h"
#include "BIT_MATH.h"
#include "GPIO.h"

void GPIO_Init(void)
{
	DDRC = 0xFF;
}

void GPIO_SetPinOutput(u8 pin)
{
	SET_BIT(DDRC, pin);
}

void GPIO_SetPinInput(u8 pin)
{
	CLR_BIT(DDRC, pin);
}

void GPIO_SetPinHigh(u8 pin)
{
	SET_BIT(PORTC, pin);
}

void GPIO_SetPinLow(u8 pin)
{
	CLR_BIT(PORTC, pin);
}