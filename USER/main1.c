#include "includes.h"

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


//������

//�����������ȼ�
#define MAIN_TASK_PRIO       			4 
//���������ջ��С
#define MAIN_STK_SIZE  					512
//���������ջ�ռ�	
OS_STK MAIN_TASK_STK[MAIN_STK_SIZE];
//�������ӿ�
void main_task(void *pdata);


int main(void)
 {
	NVIC_Configuration(); 	 //����NVIC�жϷ���2:2λ��ռ���ȼ���2λ��Ӧ���ȼ�
	delay_init();	     //��ʱ��ʼ��	
	IWDG_Init(4,2500); 
	TIM2_Int_Init(100,720);
	
	uart1_init(9600);
	uart2_init(9600);
  uart3_init(9600);
	
	LED_Init();		  	 //��ʼ����LED���ӵ�Ӳ���ӿ�
	OUT_Init();
	POWER_Init();
	SPI2_Init();	 
	RTC_Init();
	 
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
	OSTaskCreate(main_task,(void *)0,(OS_STK*)&MAIN_TASK_STK[MAIN_STK_SIZE-1],MAIN_TASK_PRIO);
	OSTaskSuspend(START_TASK_PRIO);	//������ʼ����.
	OS_EXIT_CRITICAL();				//�˳��ٽ���(���Ա��жϴ��)
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
		

	
		if(UART1_Rx_flag)//���ÿ�
		{
			handle_UART1();
			memset(UART1_Rx_buf,0x00,UART1_Rx_len);
			UART1_Rx_flag = 0;
			UART1_Rx_len = 0;
		}		
		
		//41 06 02 01 09 4A   ��������
		if(UART2_Rx_flag)//CM05ͨѶ��
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







