#include "includes.h"					//ucos 使用	  


u8 UART3_Rx_Buf[200] = {0x00};
u8 UART3_Rx_len = 0x00;	
u8 UART3_Rx_begin = 0x00;	
u8 UART3_Rx_time = 0x00;	
u8 UART3_Rx_flag = 0x00;
 



void uart3_init(u32 bound){
  //GPIO
  GPIO_InitTypeDef GPIO_InitStructure;
  USART_InitTypeDef USART_InitStructure;
  NVIC_InitTypeDef NVIC_InitStructure;
  
  //RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_AFIO, ENABLE);	//
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);	
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);
  //GPIO_PinRemapConfig(GPIO_Remap_USART3, ENABLE);

	//GPIO端口设置
	//RS485_EN   PB.1
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1; //PB.1
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;	//复用推挽输出
  GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	
  //USART3_TX   PD.5
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10; //PD.5
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//
  GPIO_Init(GPIOB, &GPIO_InitStructure);
  
  //USART3_RX	  PD.6
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;//
  GPIO_Init(GPIOB, &GPIO_InitStructure);  
  
  //Usart1 NVIC 
  
  NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;
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
  
  USART_Init(USART3, &USART_InitStructure); //
  USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);//
  USART_Cmd(USART3, ENABLE);                    // 

	RS485_TX = 0;
}

void USART3_IRQHandler(void)                	//
{
#ifdef OS_TICKS_PER_SEC	 	//
  OSIntEnter();    
#endif
  if(USART_GetITStatus(USART3, USART_IT_RXNE) != RESET)  //
  {
		
		if(UART3_Rx_flag == 0x00)
		{
		UART3_Rx_begin = 1;
		UART3_Rx_time = 8;
    UART3_Rx_Buf[UART3_Rx_len++] =USART_ReceiveData(USART3);//(USART1->DR);	//
		}
    USART_ClearITPendingBit(USART3 , USART_IT_RXNE);
  } 
#ifdef OS_TICKS_PER_SEC	 	//
  OSIntExit();  											 
#endif
} 





void USART3_Send_Data(u8 Data)
{
	RS485_TX = 1;
  while(USART_GetFlagStatus(USART3, USART_FLAG_TC) == RESET);	  
  USART_SendData(USART3,Data);  
  while(USART_GetFlagStatus(USART3, USART_FLAG_TC) == RESET);   
	RS485_TX = 0;
} 

void USART3_Send_nData(u8 *data,u32 size)
{
  while (size)
  {
    USART3_Send_Data(*data);
    data++;
    size--;
  }
}

void USART3_Send_String(char *str)
{
  while (*str!='\0')
  {
    while(USART_GetFlagStatus(USART3, USART_FLAG_TC) == RESET);    
    if(*str == '\n')
    {      
      USART_SendData(USART3,'\r');      
    }
    USART_SendData(USART3,*str++);    
  }
  while(USART_GetFlagStatus(USART3, USART_FLAG_TC) == RESET);		
 
}


void USART3_Printf(char *fmt, ...)
{     
  char tmpBuf[200];              
  va_list arg_ptr; 
  
  va_start(arg_ptr, fmt);
  vsprintf(tmpBuf, fmt, arg_ptr);
  va_end(arg_ptr);
 
  USART3_Send_String(tmpBuf);  
}
  


void handle_UART3(void)
{
	USART2_Send_nData(UART3_Rx_Buf,UART3_Rx_len); //透传到CM05
	//USART3_Send_nData(UART3_Rx_Buf,UART3_Rx_len); //
}

