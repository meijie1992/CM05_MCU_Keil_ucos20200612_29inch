#ifndef __USART2_H
#define __USART2_H
#include "includes.h"	


extern u8 UART2_Rx_Buf[];
extern u8 UART2_Rx_len;		
extern u8 UART2_Rx_time;	
extern u8 UART2_Rx_flag;

extern u8 heartbeat_start;
extern u8 power_restart_cnt;

void uart2_init(u32 bound);
void USART2_Send_Data(u8 Data);
void USART2_Send_nData(u8 *data,u32 size);
void USART2_Send_String(char *str);

void USART2_Printf(char *fmt, ...);


void handle_UART2(void);
#endif


