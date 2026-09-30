#include "EEPROM.h"
#include "TWI.h"
#include <util/delay.h>

void EEPROM_WriteByte(u16 address, u8 data)
{
	TWI_SendStartCondition();

	TWI_SendSlaveAddressWithWrite(EEPROM_ADDRESS);

	TWI_WriteDataByte((u8)(address >> 8));
	TWI_WriteDataByte((u8)address);

	TWI_WriteDataByte(data);

    TWI_SendStopCondition();

	_delay_ms(5);
}

u8 EEPROM_ReadByte(u16 address)
{
	u8 data;

	TWI_SendStartCondition();

	TWI_SendSlaveAddressWithWrite(EEPROM_ADDRESS);

	TWI_WriteDataByte((u8)(address >> 8));
	TWI_WriteDataByte((u8)address);

	TWI_SendStartCondition();

	TWI_SendSlaveAddressWithRead(EEPROM_ADDRESS);

	data = TWI_ReadDataByteWithNACK();
	TWI_SendStopCondition();


	return data;
}