#ifndef EEPROM_H_
#define EEPROM_H_

#include "STD_TYPES.h"


#define EEPROM_ADDRESS   0x50

void EEPROM_WriteByte(u16 address, u8 data);
u8 EEPROM_ReadByte(u16 address);

#endif





// whitelist , log 