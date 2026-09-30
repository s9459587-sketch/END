#include <avr/io.h>

#include "STD_TYPES.h"
#include "BIT_MATH.h"
#include "TWI.h"

void TWI_InitMaster(void)
{
    /* SCL = 100 kHz
       F_CPU = 16 MHz
       Prescaler = 1
    */

    TWSR = 0x00;

    TWBR = 72;

    TWCR = (1 << TWEN);
}

void TWI_SendStartCondition(void)
{
    TWCR = (1 << TWINT) | (1 << TWSTA) | (1 << TWEN);

    while(GET_BIT(TWCR, TWINT) == 0);
}

void TWI_SendStopCondition(void)
{
    TWCR = (1 << TWINT) | (1 << TWSTO) | (1 << TWEN);
}

void TWI_SendSlaveAddressWithWrite(u8 address)
{
    TWDR = (address << 1);

    TWCR = (1 << TWINT) | (1 << TWEN);

    while(GET_BIT(TWCR, TWINT) == 0);
}

void TWI_SendSlaveAddressWithRead(u8 address)
{
    TWDR = (address << 1) | 1;

    TWCR = (1 << TWINT) | (1 << TWEN);

    while(GET_BIT(TWCR, TWINT) == 0);
}

void TWI_WriteDataByte(u8 data)
{
    TWDR = data;

    TWCR = (1 << TWINT) | (1 << TWEN);

    while(GET_BIT(TWCR, TWINT) == 0);
}

u8 TWI_ReadDataByteWithACK(void)
{
    TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWEA);

    while(GET_BIT(TWCR, TWINT) == 0);

    return TWDR;
}

u8 TWI_ReadDataByteWithNACK(void)
{
    TWCR = (1 << TWINT) | (1 << TWEN);

    while(GET_BIT(TWCR, TWINT) == 0);

    return TWDR;
}