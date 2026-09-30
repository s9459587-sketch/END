#ifndef RFID_H_
#define RFID_H_


#include "STD_TYPES.h"

#define RFID_MAX_UID_SIZE 10

typedef struct
{
	u8 uid[RFID_MAX_UID_SIZE];
	u8 size;

} RFID_Card;

void RFID_Init(void);

u8 RFID_IsCardPresent(void);

u8 RFID_ReadUID(RFID_Card *card);

void RFID_Halt(void);

#endif