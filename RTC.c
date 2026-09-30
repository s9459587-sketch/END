#include "RTC.h"
#include "TWI.h"

static u8 RTC_DecToBCD(u8 data)
{
	return ((data / 10) << 4) | (data % 10);
}

static u8 RTC_BCDToDec(u8 data)
{
	return ((data >> 4) * 10) + (data & 0x0F);
}

void RTC_SetTime(RTC_Time *time)
{
	TWI_SendStartCondition();

	TWI_SendSlaveAddressWithWrite(RTC_ADDRESS);

	/* Start from Seconds register */
	TWI_WriteDataByte(0x00);

	TWI_WriteDataByte(RTC_DecToBCD(time->seconds));
	TWI_WriteDataByte(RTC_DecToBCD(time->minutes));
	TWI_WriteDataByte(RTC_DecToBCD(time->hours));

	TWI_WriteDataByte(RTC_DecToBCD(time->day));
	TWI_WriteDataByte(RTC_DecToBCD(time->date));
	TWI_WriteDataByte(RTC_DecToBCD(time->month));
	TWI_WriteDataByte(RTC_DecToBCD(time->year));

	TWI_SendStopCondition();
}

void RTC_GetTime(RTC_Time *time)
{
	TWI_SendStartCondition();

	TWI_SendSlaveAddressWithWrite(RTC_ADDRESS);

	/* Start reading from Seconds register */
	TWI_WriteDataByte(0x00);

	TWI_SendStartCondition();

	TWI_SendSlaveAddressWithRead(RTC_ADDRESS);

	time->seconds = RTC_BCDToDec(TWI_ReadDataByteWithACK());
	time->minutes = RTC_BCDToDec(TWI_ReadDataByteWithACK());
	time->hours   = RTC_BCDToDec(TWI_ReadDataByteWithACK());

	time->day   = RTC_BCDToDec(TWI_ReadDataByteWithACK());
	time->date  = RTC_BCDToDec(TWI_ReadDataByteWithACK());
	time->month = RTC_BCDToDec(TWI_ReadDataByteWithACK());

	time->year = RTC_BCDToDec(TWI_ReadDataByteWithNACK());

	TWI_SendStopCondition();
}