#define F_CPU 16000000UL

#include "STD_TYPES.h"
#include "TWI.h"
#include "SPI.h"
#include "UART.h"
#include "GPIO.h"
#include "RFID.h"
#include "OLED.h"
#include "APP.h"

int main(void)
{
	TWI_InitMaster();
	SPI_Init();
	UART_Init(9600);
	GPIO_Init();
		GPIO_SetPinHigh(0);
		GPIO_SetPinHigh(3);
	RFID_Init();
	OLED_Init();
	APP_Init();

	while (1)
	{
		APP_Run();
	}
}