#include "includes.h"					//ucos 使用	  


u8 UART2_Rx_Buf[200] = {0x00};
u8 UART2_Rx_len = 0x00;	
u8 UART2_Rx_time = 0x00;	
u8 UART2_Rx_flag = 0x00;
 

u8 heartbeat_start = 0x00;
u8 power_restart_cnt = 0;

void uart2_init(u32 bound){
  //GPIO
  GPIO_InitTypeDef GPIO_InitStructure;
  USART_InitTypeDef USART_InitStructure;
  NVIC_InitTypeDef NVIC_InitStructure;
  
  //RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_AFIO, ENABLE);	//
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);	
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
  //GPIO_PinRemapConfig(GPIO_Remap_USART2, ENABLE);

	
  //USART2_TX   PD.5
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2; //PD.5
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//
  GPIO_Init(GPIOA, &GPIO_InitStructure);
  
  //USART2_RX	  PD.6
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;//
  GPIO_Init(GPIOA, &GPIO_InitStructure);  
  
  //Usart1 NVIC 
  
  NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3 ;//
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;		//
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//
  NVIC_Init(&NVIC_InitStructure);	//
  
  //USART 
  
  USART_InitStructure.USART_BaudRate = bound;//
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;//
  USART_InitStructure.USART_StopBits = USART_StopBits_1;//
  USART_InitStructure.USART_Parity = USART_Parity_No;//
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//
  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//
  
  USART_Init(USART2, &USART_InitStructure); //
  USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);//
  USART_Cmd(USART2, ENABLE);                    // 

}

void USART2_IRQHandler(void)                	//
{
#ifdef OS_TICKS_PER_SEC	 	//
  OSIntEnter();    
#endif
  if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)  //
  {
		
		if(UART2_Rx_flag == 0x00)
		{
		UART2_Rx_time = 8;
    UART2_Rx_Buf[UART2_Rx_len++] =USART_ReceiveData(USART2);//(USART1->DR);	//
		}
    USART_ClearITPendingBit(USART2 , USART_IT_RXNE);
  } 
#ifdef OS_TICKS_PER_SEC	 	//
  OSIntExit();  											 
#endif
} 





void USART2_Send_Data(u8 Data)
{
  while(USART_GetFlagStatus(USART2, USART_FLAG_TC) == RESET);	  
  USART_SendData(USART2,Data);  
  while(USART_GetFlagStatus(USART2, USART_FLAG_TC) == RESET);   
} 


void USART2_Send_nData(u8 *data,u32 size)
{
  while (size)
  {
    USART2_Send_Data(*data);
    data++;
    size--;
  }
}



void USART2_Send_String(char *str)
{
  while (*str!='\0')
  {
    while(USART_GetFlagStatus(USART2, USART_FLAG_TC) == RESET);    
    if(*str == '\n')
    {      
      USART_SendData(USART2,'\r');      
    }
    USART_SendData(USART2,*str++);    
  }
  while(USART_GetFlagStatus(USART2, USART_FLAG_TC) == RESET);		
 
}


void USART2_Printf(char *fmt, ...)
{     
  char tmpBuf[200];              
  va_list arg_ptr; 
  
  va_start(arg_ptr, fmt);
  vsprintf(tmpBuf, fmt, arg_ptr);
  va_end(arg_ptr);
 
  USART2_Send_String(tmpBuf);  
}
  


void handle_UART2(void)
{
	//41 06 02 01 09 4A   开门命令
	//41 06 02 02 0A 4A   开门命令
	if((UART2_Rx_Buf[0] == 0x41) && (UART2_Rx_Buf[UART2_Rx_len - 1] == 0x4A) && 
		((unsigned char)check_sum(&UART2_Rx_Buf[1],UART2_Rx_Buf[1] - 0x03) == UART2_Rx_Buf[UART2_Rx_Buf[1] - 0x02]))
	{
		if(UART2_Rx_Buf[2] == 0x04)//警铃
		{
			OUT1 = 1;
			OUT2 = 1;
			out_timer_cnt = 2;
		}
		else if(UART2_Rx_Buf[2] == 0x05)//心跳
		{
			LED_RUN = !LED_RUN;
			heartbeat_start = 100;
		}
		else
		{
			USART3_Send_nData(UART2_Rx_Buf,UART2_Rx_len); //透传到电梯
		}
	}
}

