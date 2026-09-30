#include "WHITELIST.h"
#include "EEPROM.h"

#define START 0
#define CARD_SIZE 11

u8 WHITELIST_Check(u8 *uid, u8 size)
{
	u8 i,j;

	for(i=0; i<MAX_CARDS; i++)
	{
		if(EEPROM_ReadByte(START+i*CARD_SIZE)==size)
		{
			for(j=0; j<size; j++)
			{
				if(EEPROM_ReadByte(START+i*CARD_SIZE+1+j)!=uid[j])
				break;
			}

			if(j==size)
			return 1;
		}
	}

	return 0;
}

u8 WHITELIST_Add(u8 *uid, u8 size)
{
	u8 i,j;

	if(WHITELIST_Check(uid,size))
	return 0;

	for(i=0; i<MAX_CARDS; i++)
	{
		if(EEPROM_ReadByte(START+i*CARD_SIZE)==0xFF)
		{
			EEPROM_WriteByte(START+i*CARD_SIZE,size);

			for(j=0; j<size; j++)
			EEPROM_WriteByte(START+i*CARD_SIZE+1+j,uid[j]);

			return 1;
		}
	}

	return 0;
}

u8 WHITELIST_Delete(u8 *uid, u8 size)
{
	u8 i,j;

	for(i=0; i<MAX_CARDS; i++)
	{
		if(EEPROM_ReadByte(START+i*CARD_SIZE)==size)
		{
			for(j=0; j<size; j++)
			{
				if(EEPROM_ReadByte(START+i*CARD_SIZE+1+j)!=uid[j])
				break;
			}

			if(j==size)
			{
				EEPROM_WriteByte(START+i*CARD_SIZE,0xFF);
				return 1;
			}
		}
	}

	return 0;
}