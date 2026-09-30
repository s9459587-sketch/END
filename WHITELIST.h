#ifndef WHITELIST_H_
#define WHITELIST_H_

#include "STD_TYPES.h"

#define MAX_CARDS 50

u8 WHITELIST_Add(u8 *uid, u8 size);
u8 WHITELIST_Delete(u8 *uid, u8 size);
u8 WHITELIST_Check(u8 *uid, u8 size);

#endif