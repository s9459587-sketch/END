#ifndef TWI_H_
#define TWI_H_

#include "STD_TYPES.h"

void TWI_InitMaster(void);

void TWI_SendStartCondition(void);
void TWI_SendStopCondition(void);

void TWI_SendSlaveAddressWithWrite(u8 address);
void TWI_SendSlaveAddressWithRead(u8 address);

void TWI_WriteDataByte(u8 data);

u8 TWI_ReadDataByteWithACK(void);
u8 TWI_ReadDataByteWithNACK(void);

#endif