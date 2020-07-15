#include "includes.h"					//ucos 使用	  

u8 UART1_Rx_buf[200] = {0x00};
u8 UART1_Rx_len = 0x00;	
u8 UART1_Rx_time = 0x00;	
u8 UART1_Rx_flag = 0x00;

//////////////////////////////////////////////////////////////////
//加入以下代码,支持printf函数,而不需要选择use MicroLIB	  
#if 1
#pragma import(__use_no_semihosting)             
//标准库需要的支持函数                 
struct __FILE 
{ 
	int handle; 

}; 

FILE __stdout;       
//定义_sys_exit()以避免使用半主机模式    
_sys_exit(int x) 
{ 
	x = x; 
} 
//重定义fputc函数 
int fputc(int ch, FILE *f)
{      
	while((USART1->SR&0X40)==0);//循环发送,直到发送完毕   
    USART1->DR = (u8) ch;      
	return ch;
}
#endif 

/*使用microLib的方法*/
 /* 
int fputc(int ch, FILE *f)
{
	USART_SendData(USART1, (uint8_t) ch);

	while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET) {}	
   
    return ch;
}
int GetKey (void)  { 

    while (!(USART1->SR & USART_FLAG_RXNE));

    return ((int)(USART1->DR & 0x1FF));
}
*/

  
void uart1_init(u32 bound)
{

  GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	 
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1|RCC_APB2Periph_GPIOA, ENABLE);	//使能USART1，GPIOA时钟以及复用功能时钟

	//GPIO端口设置
	//USART1_TX   PA.9
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9; //PA.9
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//复用推挽输出
  GPIO_Init(GPIOA, &GPIO_InitStructure);
   
  //USART1_RX	  PA.10
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//浮空输入
  GPIO_Init(GPIOA, &GPIO_InitStructure);  

  //Usart1 NVIC 配置
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3 ;//抢占优先级3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;		//子优先级3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器
  
  //USART 初始化设置
	USART_InitStructure.USART_BaudRate = bound;//一般设置为9600;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//字长为8位数据格式
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//一个停止位
	USART_InitStructure.USART_Parity = USART_Parity_No;//无奇偶校验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//收发模式
  USART_Init(USART1, &USART_InitStructure); //初始化串口
  
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);//开启中断
  USART_Cmd(USART1, ENABLE);                    //使能串口 
	
}




void USART1_IRQHandler(void)                	//串口1中断服务程序
{
#ifdef OS_TICKS_PER_SEC	 	//如果时钟节拍数定义了,说明要使用ucosII了.
	OSIntEnter();    
#endif
	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)  //接收中断(接收到的数据必须是0x0d 0x0a结尾)
		{
			UART1_Rx_time = 8;
			UART1_Rx_buf[UART1_Rx_len++] =USART_ReceiveData(USART1);//(USART1->DR);	//
			if(UART1_Rx_len>=40)
			{
				UART1_Rx_len = 0x00;
			}
			
			USART_ClearITPendingBit(USART1 , USART_IT_RXNE);
			
     } 
#ifdef OS_TICKS_PER_SEC	 	//如果时钟节拍数定义了,说明要使用ucosII了.
	OSIntExit();  											 
#endif
} 



void USART1_Send_Data(u8 Data)
{
  while(USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET);	  
  USART_SendData(USART1,Data);  
  while(USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET);  	
} 


void USART1_Send_nData(u8 *data,u32 size)
{
  while (size)
  {
    USART1_Send_Data(*data);
    data++;
    size--;
  }
}



void handle_UART1(void) 
{
			//USART1_Send_nData(UART1_Rx_buf,UART1_Rx_len);
				
			if(UART1_Rx_buf[0] == 0x55 && UART1_Rx_buf[7] == 0x55) // ???
			{
				printf("%s",SoftwareVersion);

			}        
      else if(UART1_Rx_buf[0] == 0x66 && UART1_Rx_buf[7] == 0x55) // ???
      {
        POWER = 1;        
      } 
      else if(UART1_Rx_buf[0] == 0x77 && UART1_Rx_buf[7] == 0x55) // ???
      {
        POWER = 0;       
      } 			
 
      else if(UART1_Rx_buf[0] == 0xAA && UART1_Rx_buf[7] == 0x55) // ???
      {
        USART1_Send_nData(bib_info.g_xSystemParameter_OnChip.u8FlashBuffer,sizeof(bib_info.g_xSystemParameter_OnChip));        
      } 
      else if(UART1_Rx_buf[0] == 0x55 && UART1_Rx_buf[7] == 0xBB) // ???????? 0x55 0x14 0x02 0x12 0x14 0x47 0x02 0xBB   014-02-12-14:47:00 ???
      {
        systmtime.tm_year = 2000 + hex2bcd(UART1_Rx_buf[1]);//(u16)(2000 + Input_Temp[7] * 10+ Input_Temp[6]);          
        systmtime.tm_mon  = hex2bcd(UART1_Rx_buf[2]);//(u8)(Input_Temp[4] * 10 + Input_Temp[3]);
        systmtime.tm_mday = hex2bcd(UART1_Rx_buf[3]);//(u8)(Input_Temp[1] * 10 + Input_Temp[0]);
        systmtime.tm_hour = hex2bcd(UART1_Rx_buf[4]);//(u8)(Input_Temp[7] * 10+ Input_Temp[6]);          
        systmtime.tm_min  = hex2bcd(UART1_Rx_buf[5]);//(u8)(Input_Temp[4] * 10 + Input_Temp[3]);
        systmtime.tm_sec  = 00;//(u8)(Input_Temp[1] * 10 + Input_Temp[0]);
        RTC_Configuration();
        RTC_WaitForLastTask();
        GregorianDay(&systmtime); 
        RTC_SetCounter(mktimev(&systmtime));
        RTC_WaitForLastTask();
        BKP_WriteBackupRegister(BKP_DR1, 0x5A5A);
        RTC_Init(); 

      }
      else if(UART1_Rx_buf[0] == 0xBB && UART1_Rx_buf[7] == 0x55) // ????????
      {
				USART1_Send_Data(0xBB);
				USART1_Send_Data(0x06);
        to_tm(RTC_GetCounter(), &systmtime);
        USART1_Send_Data(bcd2hex((u8)(systmtime.tm_year-2000)));
        USART1_Send_Data(bcd2hex(systmtime.tm_mon));
        USART1_Send_Data(bcd2hex(systmtime.tm_mday));
        USART1_Send_Data(bcd2hex(systmtime.tm_hour));
        USART1_Send_Data(bcd2hex(systmtime.tm_min));
        USART1_Send_Data(bcd2hex(systmtime.tm_sec));
        USART1_Send_Data(bcd2hex(systmtime.tm_wday));
				USART1_Send_Data(0x55);

      }

}
