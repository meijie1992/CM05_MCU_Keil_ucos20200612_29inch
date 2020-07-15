#ifndef __USART3_H
#define __USART3_H
#include "includes.h"	

#define RS485_TX PBout(1)

extern u8 UART3_Rx_Buf[];
extern u8 UART3_Rx_len;	
extern u8 UART3_Rx_begin;	
extern u8 UART3_Rx_time;	
extern u8 UART3_Rx_flag;

void uart3_init(u32 bound);
void USART3_Send_Data(u8 Data);
void USART3_Send_nData(u8 *data,u32 size);
void USART3_Send_String(char *str);

void USART3_Printf(char *fmt, ...);


void handle_UART3(void);
#endif


