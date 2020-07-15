#include "includes.h"

u8 time2_flag_100ms = 0;
u8 time2_flag_1000ms = 0;
u32 time2_1000ms = 0;
u32 time2_100ms = 0;

//����ʱ��ѡ��ΪAPB1��2������APB1Ϊ36M
//**--------------��������-----------------------------------------------------
/******************************************************************************
**�������ƣ�TIM2_Int_Init
**���������arr���Զ���װֵ  psc��ʱ��Ԥ��Ƶ��
**�����������
**������������ʱ����ʼ��
**ȫ�ֱ�������
**���ڣ�2017-06-22
*******************************************************************************/
void TIM2_Int_Init(u16 arr,u16 psc)
{
  TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
  NVIC_InitTypeDef NVIC_InitStructure;

  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE); //ʱ��ʹ��
				
  //��ʱ��TIM3��ʼ��
  TIM_TimeBaseStructure.TIM_Period = arr; //��������һ�������¼�װ�����Զ���װ�ؼĴ������ڵ�ֵ	
  TIM_TimeBaseStructure.TIM_Prescaler =psc; //����������ΪTIMxʱ��Ƶ�ʳ�����Ԥ��Ƶֵ
  TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; //����ʱ�ӷָ�:TDTS = Tck_tim
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM���ϼ���ģʽ
  TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure); //����ָ���Ĳ�����ʼ��TIMx��ʱ�������λ

  TIM_ITConfig(TIM2,TIM_IT_Update,ENABLE ); //ʹ��ָ����TIM3�ж�,��������ж�

  //�ж����ȼ�NVIC����
  NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;  //TIM3�ж�
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;  //��ռ���ȼ�0��
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;  //�����ȼ�3��
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQͨ����ʹ��
  NVIC_Init(&NVIC_InitStructure);  //��ʼ��NVIC�Ĵ���

  TIM_Cmd(TIM2, ENABLE);  //�رն�ʱ��

}




void TIM2_IRQHandler(void)   //TIM3
{
#ifdef OS_TICKS_PER_SEC	 	//���ʱ�ӽ�����������,˵��Ҫʹ��ucosII��.
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
#ifdef OS_TICKS_PER_SEC	 	//���ʱ�ӽ�����������,˵��Ҫʹ��ucosII��.
  OSIntExit();  											 
#endif
}


