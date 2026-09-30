#ifndef RTC_H_
#define RTC_H_

#include "STD_TYPES.h"

#define RTC_ADDRESS  0x68

typedef struct
{
	u8 seconds;
	u8 minutes;
	u8 hours;
	u8 day;
	u8 date;
	u8 month;
	u8 year;

} RTC_Time;

void RTC_SetTime(RTC_Time *time);
void RTC_GetTime(RTC_Time *time);

#endif