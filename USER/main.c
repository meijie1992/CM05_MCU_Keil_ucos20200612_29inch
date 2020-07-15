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
 
/////////////////////////UCOSII任务堆栈设置///////////////////////////////////
//START 任务
//设置任务优先级
#define START_TASK_PRIO      			10 //开始任务的优先级设置为最低
//设置任务堆栈大小
#define START_STK_SIZE  				64
//创建任务堆栈空间	
OS_STK START_TASK_STK[START_STK_SIZE];
//任务函数接口
void start_task(void *pdata);	
 			   
//LED1任务
//设置任务优先级
#define PWM_TASK_PRIO       			7 
//设置任务堆栈大小
#define PWM_STK_SIZE  		    		64
//创建任务堆栈空间	
OS_STK PWM_TASK_STK[PWM_STK_SIZE];
//任务函数接口
void pwm_task(void *pdata);

//LED1任务
//设置任务优先级
#define USART_TASK_PRIO       			9 
//设置任务堆栈大小
#define USART_STK_SIZE  		    		64
//创建任务堆栈空间	
OS_STK USART_TASK_STK[USART_STK_SIZE];
//任务函数接口
void usart_task(void *pdata);






	
 int main(void)
 {
	
	//TIM4_GPIO_Init();
    PWM_Init(360,280);
	uart_init(9600);
    delay_init();	     //延时初始化	  
	DS18B20_Init() ;
	Adc_Init();
	NVIC_Configuration(); 	 //设置NVIC中断分组2:2位抢占优先级，2位响应优先级
//	LED_Init();		  	 //初始化与LED连接的硬件接口
	OSInit();   
 	OSTaskCreate(start_task,(void *)0,(OS_STK *)&START_TASK_STK[START_STK_SIZE-1],START_TASK_PRIO );//创建起始任务
	OSStart();
 }

 
	  
//开始任务
void start_task(void *pdata)
{
    OS_CPU_SR cpu_sr=0;
	pdata = pdata; 
	OSStatInit();					//初始化统计任务.这里会延时1秒钟左右	
 	OS_ENTER_CRITICAL();			//进入临界区(无法被中断打断)    
 //	OSTaskCreate(pwm_task,(void *)0,(OS_STK*)&PWM_TASK_STK[PWM_STK_SIZE-1],PWM_TASK_PRIO);						   
 	OSTaskCreate(usart_task,(void *)0,(OS_STK*)&USART_TASK_STK[USART_STK_SIZE-1],USART_TASK_PRIO);	 				   
	OSTaskSuspend(START_TASK_PRIO);	//挂起起始任务.
	OS_EXIT_CRITICAL();				//退出临界区(可以被中断打断)
}

//LED1任务
//void pwm_task(void *pdata)
//{	 	
//	while(1)
//	{
//	TIM_CtrlPWMOutputs(TIM4,ENABLE);
//
//	};
//}

//LED1任务
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
