#include "includes.h"

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


//主任务

//设置任务优先级
#define MAIN_TASK_PRIO       			4 
//设置任务堆栈大小
#define MAIN_STK_SIZE  					512
//创建任务堆栈空间	
OS_STK MAIN_TASK_STK[MAIN_STK_SIZE];
//任务函数接口
void main_task(void *pdata);


int main(void)
 {
	NVIC_Configuration(); 	 //设置NVIC中断分组2:2位抢占优先级，2位响应优先级
	delay_init();	     //延时初始化	
	IWDG_Init(4,2500); 
	TIM2_Int_Init(100,720);
	
	uart1_init(9600);
	uart2_init(9600);
  uart3_init(9600);
	
	LED_Init();		  	 //初始化与LED连接的硬件接口
	OUT_Init();
	POWER_Init();
	SPI2_Init();	 
	RTC_Init();
	 
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
	OSTaskCreate(main_task,(void *)0,(OS_STK*)&MAIN_TASK_STK[MAIN_STK_SIZE-1],MAIN_TASK_PRIO);
	OSTaskSuspend(START_TASK_PRIO);	//挂起起始任务.
	OS_EXIT_CRITICAL();				//退出临界区(可以被中断打断)
}




/*-----------------------------------------------------------------------
	   
-------------------------------------------------------------------------*/

void main_task(void *pdata)
{
	u8  j =0;
	
#ifdef BIB_INFO_EN
  memcpy((u8 *)(&bib_info),(u8 *)BIB_ADDRESS,sizeof(tbl_info));
#endif 
	
  while(1)
	{
		IWDG_Feed();
		
		if(time2_flag_100ms)
		{
			time2_flag_100ms = 0;		
		}
		
		
		if(time2_flag_1000ms)
		{
			time2_flag_1000ms = 0;			
			if(heartbeat_start > 0)
			{
				heartbeat_start --;
				if(heartbeat_start == 0)
				{
					POWER = 0x00;//????
					power_restart_cnt = 5;//??????
				}						
			}
			
			if(power_restart_cnt > 0)
			{
				power_restart_cnt --;
				if(power_restart_cnt == 0)
				{
					POWER = 0x01;//????
				}		
			}	

			if(out_timer_cnt > 0)
			{
				out_timer_cnt--;
				if(out_timer_cnt == 0)
				{
					OUT1 = 0;
					OUT2 = 0;
				}
			}
		}		
		

	
		if(UART1_Rx_flag)//配置口
		{
			handle_UART1();
			memset(UART1_Rx_buf,0x00,UART1_Rx_len);
			UART1_Rx_flag = 0;
			UART1_Rx_len = 0;
		}		
		
		//41 06 02 01 09 4A   开门命令
		if(UART2_Rx_flag)//CM05通讯口
		{
			handle_UART2();
      memset(UART2_Rx_Buf,0x00,UART2_Rx_len);
			UART2_Rx_flag = 0;
			UART2_Rx_len = 0;
		}
		
		if(UART3_Rx_flag)//RS485
		{
			handle_UART3();
      memset(UART3_Rx_Buf,0x00,UART3_Rx_len);
			UART3_Rx_flag = 0;
			UART3_Rx_len = 0;
		}	


    if(SPI2_CS == 0x01)
    //if(SPI2_RxDate_Flag == 0x01)
    {
			
      SPI2_RxDate_Flag = 0x00;
			if(SPI2_Rx_len >= 8)
			{
				/*
				for(j = 0;j<32;j++)
				{
					if(SPI2_Rx_Buf[0] == 0x81)
					{
						break;
					}
					left_shift(SPI2_Rx_Buf,SPI2_Rx_len);
				}
				*/
				
				
				handle_SPI2(SPI2_Rx_Buf,SPI2_Rx_len);
			}

      memset(SPI2_Rx_Buf,0x00,SPI2_Rx_len);      
      SPI2_Rx_len = 0x00;
    }		

	}
}







