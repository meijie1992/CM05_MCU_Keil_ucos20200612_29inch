#ifndef __DATE_H
#define __DATE_H
#include <includes.h>

typedef struct rtc_time {
	s8  tm_sec;
	s8  tm_min;
	s8  tm_hour;
	s8  tm_mday;
	s8  tm_mon;
	s16 tm_year;
	s8  tm_wday;
}S_Time;
extern  struct rtc_time         systmtime;

void RTC_Configuration(void);
void RTC_Init(void);
void Time_Regulate(struct rtc_time *tm);
void Time_Adjust(void);
void Time_Display(u32 TimeVar);
void Time_Show(void);
u16 USART_Scanf(u32 value);
void GregorianDay(struct rtc_time * tm);
u32 mktimev(struct rtc_time *tm);
void to_tm(uint32_t tim, struct rtc_time * tm);
void Update_Time(void);
#endif
