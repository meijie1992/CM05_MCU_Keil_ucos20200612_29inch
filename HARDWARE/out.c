#include "includes.h"
u8 out_timer_cnt = 0;
//��ʼ��PB5��PE5Ϊ�����.��ʹ���������ڵ�ʱ��		    
//LED IO��ʼ��
void OUT_Init(void)
{
 
 GPIO_InitTypeDef  GPIO_InitStructure;
 	
 RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB|RCC_APB2Periph_AFIO, ENABLE);	 //ʹ��PB,PE�˿�ʱ��
 GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable,ENABLE) ;
	
 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3 | GPIO_Pin_4;				 //LED0-->PB.5 �˿�����
 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //�������
 GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO���ٶ�Ϊ50MHz
 GPIO_Init(GPIOB, &GPIO_InitStructure);					 //�����趨������ʼ��GPIOB.5

	OUT1 = 0;
	OUT2 = 0;
}
 
