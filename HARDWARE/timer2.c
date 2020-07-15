#include "includes.h"

u8 time2_flag_100ms = 0;
u8 time2_flag_1000ms = 0;
u32 time2_1000ms = 0;
u32 time2_100ms = 0;

//这里时钟选择为APB1的2倍，而APB1为36M
//**--------------函数声明-----------------------------------------------------
/******************************************************************************
**函数名称：TIM2_Int_Init
**输入参数：arr：自动重装值  psc：时钟预分频数
**输出参数：无
**功能描述：定时器初始化
**全局变量：无
**日期：2017-06-22
*******************************************************************************/
void TIM2_Int_Init(u16 arr,u16 psc)
{
  TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
  NVIC_InitTypeDef NVIC_InitStructure;

  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE); //时钟使能
				
  //定时器TIM3初始化
  TIM_TimeBaseStructure.TIM_Period = arr; //设置在下一个更新事件装入活动的自动重装载寄存器周期的值	
  TIM_TimeBaseStructure.TIM_Prescaler =psc; //设置用来作为TIMx时钟频率除数的预分频值
  TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; //设置时钟分割:TDTS = Tck_tim
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM向上计数模式
  TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure); //根据指定的参数初始化TIMx的时间基数单位

  TIM_ITConfig(TIM2,TIM_IT_Update,ENABLE ); //使能指定的TIM3中断,允许更新中断

  //中断优先级NVIC设置
  NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;  //TIM3中断
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;  //先占优先级0级
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;  //从优先级3级
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQ通道被使能
  NVIC_Init(&NVIC_InitStructure);  //初始化NVIC寄存器

  TIM_Cmd(TIM2, ENABLE);  //关闭定时器

}




void TIM2_IRQHandler(void)   //TIM3
{
#ifdef OS_TICKS_PER_SEC	 	//如果时钟节拍数定义了,说明要使用ucosII了.
  OSIntEnter();    
#endif
  if (TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET)  
  {
    TIM_ClearITPendingBit(TIM2, TIM_IT_Update); 

		if(UART1_Rx_time > 0)
		{
			UART1_Rx_time--;
			if(UART1_Rx_time == 0)
			{
				UART1_Rx_flag = 1;
			}	
		}
		
		
		if(UART2_Rx_time > 0)
		{
			UART2_Rx_time--;
			if(UART2_Rx_time == 0)
			{
				UART2_Rx_flag = 1;
			}	
		}
	
	
		if(UART3_Rx_time > 0)
		{
			UART3_Rx_time--;
			if(UART3_Rx_time == 0)
			{
				UART3_Rx_flag = 1;
			}	
		}	
/*
		if(SPI2_Rx_time > 0)
		{
			SPI2_Rx_time--;
		}
		else
		{
			if(SPI2_Rx_len >= 8)
			{
				SPI2_RxDate_Flag =1;
			}
			else
			{
				SPI2_Rx_len = 0;
			}
			
		}
		*/
		if(SPI2_Rx_time > 0)
		{
			SPI2_Rx_time--;
			if(SPI2_Rx_time == 0)
			{
				SPI2_RxDate_Flag =1;
			}
		}		
		
    time2_100ms++;
    if(time2_100ms>=50)
    {
      time2_100ms = 0;
      time2_flag_100ms=1; 
    } 
		
		
    time2_1000ms++;
    if(time2_1000ms>=1000)
    {
      time2_1000ms = 0;
			time2_flag_1000ms = 1;
    }   
  } 
#ifdef OS_TICKS_PER_SEC	 	//如果时钟节拍数定义了,说明要使用ucosII了.
  OSIntExit();  											 
#endif
}


