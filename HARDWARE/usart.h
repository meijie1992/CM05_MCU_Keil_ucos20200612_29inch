#ifndef __USART1_H
#define __USART1_H
#include "includes.h"	


extern u8 UART1_Rx_buf[];
extern u8 UART1_Rx_len;	
extern u8 UART1_Rx_time;	
extern u8 UART1_Rx_flag;



void uart1_init(u32 bound);
void USART1_Send_Data(u8 Data);
void USART1_Send_nData(u8 *data,u32 size);
void handle_UART1(void) ;

#endif


