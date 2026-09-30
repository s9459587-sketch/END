#ifndef APP_H_
#define APP_H_

#include "STD_TYPES.h"

void APP_Init(void);
u8 APP_CheckCard(u8 *uid, u8 size);
void APP_Run(void);

#endif