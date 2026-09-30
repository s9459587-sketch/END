#include <avr/io.h>

#include "STD_TYPES.h"
#include "BIT_MATH.h"
#include "SPI.h"

void SPI_Init(void)
{
	SET_BIT(DDRB, 3);   // MOSI
	CLR_BIT(DDRB, 4);   // MISO
	SET_BIT(DDRB, 5);   // SCK
	SET_BIT(DDRB, 2);   // SS

	SET_BIT(PORTB, 2);  // SS High

	SET_BIT(SPCR, SPE);   // Enable SPI
	SET_BIT(SPCR, MSTR);  // Master

	CLR_BIT(SPCR, SPR1);
	SET_BIT(SPCR, SPR0);  // F_CPU / 16
}

u8 SPI_Transceive(u8 data)
{
	SPDR = data;

	while(GET_BIT(SPSR, SPIF) == 0);

	return SPDR;
}