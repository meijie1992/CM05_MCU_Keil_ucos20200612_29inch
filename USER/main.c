#include "led.h"
#include "delay.h"
#include "sys.h"
#include "usart.h"	
#include "includes.h"
#include "stm32f10x.h"
#include "pwm.h"
#include "stm32f10x_tim.h"
#include "18b20.h"
#include "adc.h"
 
/////////////////////////UCOSII�����ջ����///////////////////////////////////
//START ����
//�����������ȼ�
#define START_TASK_PRIO      			10 //��ʼ��������ȼ�����Ϊ���
//���������ջ��С
#define START_STK_SIZE  				64
//���������ջ�ռ�	
OS_STK START_TASK_STK[START_STK_SIZE];
//�������ӿ�
void start_task(void *pdata);	
 			   
//LED1����
//�����������ȼ�
#define PWM_TASK_PRIO       			7 
//���������ջ��С
#define PWM_STK_SIZE  		    		64
//���������ջ�ռ�	
OS_STK PWM_TASK_STK[PWM_STK_SIZE];
//�������ӿ�
void pwm_task(void *pdata);

//LED1����
//�����������ȼ�
#define USART_TASK_PRIO       			9 
//���������ջ��С
#define USART_STK_SIZE  		    		64
//���������ջ�ռ�	
OS_STK USART_TASK_STK[USART_STK_SIZE];
//�������ӿ�
void usart_task(void *pdata);






	
 int main(void)
 {
	
	//TIM4_GPIO_Init();
    PWM_Init(360,280);
	uart_init(9600);
    delay_init();	     //��ʱ��ʼ��	  
	DS18B20_Init() ;
	Adc_Init();
	NVIC_Configuration(); 	 //����NVIC�жϷ���2:2λ��ռ���ȼ���2λ��Ӧ���ȼ�
//	LED_Init();		  	 //��ʼ����LED���ӵ�Ӳ���ӿ�
	OSInit();   
 	OSTaskCreate(start_task,(void *)0,(OS_STK *)&START_TASK_STK[START_STK_SIZE-1],START_TASK_PRIO );//������ʼ����
	OSStart();
 }

 
	  
//��ʼ����
void start_task(void *pdata)
{
    OS_CPU_SR cpu_sr=0;
	pdata = pdata; 
	OSStatInit();					//��ʼ��ͳ������.�������ʱ1��������	
 	OS_ENTER_CRITICAL();			//�����ٽ���(�޷����жϴ��)    
 //	OSTaskCreate(pwm_task,(void *)0,(OS_STK*)&PWM_TASK_STK[PWM_STK_SIZE-1],PWM_TASK_PRIO);						   
 	OSTaskCreate(usart_task,(void *)0,(OS_STK*)&USART_TASK_STK[USART_STK_SIZE-1],USART_TASK_PRIO);	 				   
	OSTaskSuspend(START_TASK_PRIO);	//������ʼ����.
	OS_EXIT_CRITICAL();				//�˳��ٽ���(���Ա��жϴ��)
}

//LED1����
//void pwm_task(void *pdata)
//{	 	
//	while(1)
//	{
//	TIM_CtrlPWMOutputs(TIM4,ENABLE);
//
//	};
//}

//LED1����
//void usart_task(void *pdata)
//{
//    short temperature;	 	
//	while(DS18B20_Init())
//	{
//	delay_ms(500);
//
//
//
////	temperature=DS18B20_Get_Temp();
//	temperature=56;
//	printf("%d\n",temperature);
//	};
//}


void usart_task(void *pdata)
{
    u16 adclx;
	u16 temp;	 	
	while(DS18B20_Init())
	{
	delay_ms(500);
   	adclx= Get_Adc(1);
	
    temp=(float)adclx*(3.3/4096);
//	temp=56	;
	printf("%d V\n",temp);


//	temperature=DS18B20_Get_Temp();
//	temperature=56;
//	printf("%d\n",temperature);
	};
}



//void usart_task(void *pdata)
//{
////    float temperature;
//	u16 adcx;
//	float temp;		 	
//	while(1)
//	{
////	delay_ms(500);
////	temperature= Get_Adc(1);
////
////	printf("%f\n",temperature);
//	
//    delay_ms(500);
//	adcx= Get_Adc(1);
//
//    temp=(float)adcx*(3.3/4096);
//	printf("%f V\n",temp);
//	
//	};
//}
