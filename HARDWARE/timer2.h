#ifndef __TIMER2_H
#define __TIMER2_H
#include "includes.h"

extern u8 time2_flag_100ms;
extern u8 time2_flag_1000ms;

void TIM2_Int_Init(u16 arr,u16 psc);

#endif
