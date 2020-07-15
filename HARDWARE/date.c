#include <includes.h>

#define  RTCClockSource_LSE


struct rtc_time systmtime;

#define FEBRUARY		2
#define	STARTOFTIME		1970
#define SECDAY			86400L
#define SECYR			(SECDAY * 365)
#define	leapyear(year)		((year) % 4 == 0)
#define	days_in_year(a) 	(leapyear(a) ? 366 : 365)
#define	days_in_month(a) 	(month_days[(a) - 1])

static int month_days[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};


u32 TimeDisplay = 0;

void RTC_NVIC_Config(void)
{
  NVIC_InitTypeDef NVIC_InitStructure;  
  /* Configure one bit for preemption priority */
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);  
  /* Enable the RTC Interrupt */
  NVIC_InitStructure.NVIC_IRQChannel = RTC_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
}


void RTC_IRQHandler(void)
{
  if (RTC_GetITStatus(RTC_IT_SEC) != RESET)
  {    
    RTC_ClearITPendingBit(RTC_IT_SEC);          /* Clear the RTC Second interrupt */        
    TimeDisplay = 1;                            /* Enable time update */        
    RTC_WaitForLastTask();                      /* Wait until last write operation on RTC registers has finished */        
    if (RTC_GetCounter() == 0x00015180)         /* Reset RTC Counter when Time is 23:59:59 */
    {
      RTC_SetCounter(0x0);      
      RTC_WaitForLastTask();                    /* Wait until last write operation on RTC registers has finished */
    }
  }
}


/***********************************************************************
*��������RTC_Configuration
*����������RTC
*���룺��
*�������
*���أ���
************************************************************************/
void RTC_Configuration(void)
{  
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, ENABLE);/*����PWR��BKPʱ��*/    
  PWR_BackupAccessCmd(ENABLE);                                            /*�������BKP��*/    
  BKP_DeInit();                                                           /*��λ������*/  
#ifdef RTCClockSource_LSI    
  RCC_LSICmd(ENABLE);                                                     /*����LSI*/    
  while(RCC_GetFlagStatus(RCC_FLAG_LSIRDY)==RESET);                       /*�ȴ�LSI׼����*/    
  RCC_RTCCLKConfig(RCC_RTCCLKSource_LSI);                                 /*ѡ��LSI��ΪRTCʱ��Դ*/  
#elif defined  RTCClockSource_LSE    
  RCC_LSEConfig(RCC_LSE_ON);                                              /*����LSE*/    
  while(RCC_GetFlagStatus(RCC_FLAG_LSERDY)==RESET);                       /*�ȴ�LSE׼����*/    
  RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);                                 /*ѡ��LSE��ΪRTCʱ��Դ*/
#endif    
  RCC_RTCCLKCmd(ENABLE);                                                  /* Enable RTC Clock */     
  RTC_WaitForSynchro();                                                   /*�ȴ��Ĵ���ͬ��*/    
  RTC_WaitForLastTask();                                                  /*�ȴ��ϴ�RTC�Ĵ���д�������*/    
  RTC_ITConfig(RTC_IT_SEC, ENABLE);                                       /*����RTC���ж�*/    
  RTC_WaitForLastTask();                                                  /*�ȴ��ϴ�RTC�Ĵ���д�������*/  
#ifdef RTCClockSource_LSI
  /*���÷�Ƶϵ��*/
  RTC_SetPrescaler(31999);                                                /*RTC����=RTCCLK/RTC_PR=(32.000kHz/(31999+1))*/  
#elif defined  RTCClockSource_LSE
//  RTC_SetPrescaler(32767);                                                /*RTC����=RTCCLK/RTC_PR=(32.768kHz/(31767+1))*/
/*
  24Сʱ����9��,��ô����ÿ���ӿ��� 9/(24*3600) ��,Լ��:0.1042ms
  �����  1.0001042ms=RTCreal/32768;
  ͨ����ʽ�Ϳ������������RTC����Ƶ��,RTCreal=32771.4Hz
*/
  RTC_SetPrescaler(32772);
#endif   
  RTC_WaitForLastTask();                                                  /*�ȴ��ϴ�RTC�Ĵ���д�������*/  
}


void RTC_Init(void)
{	  
  RTC_NVIC_Config();                                /* NVIC configuration */  
  if (BKP_ReadBackupRegister(BKP_DR1) != 0x5A5A)    /*������ʱ��鱸�ݼĴ���BKP_DR1��������ݲ���0xA5A5,������������ʱ�䲢ѯ���û�����ʱ��*/
  {       
    RTC_Configuration();                            /* RTC Configuration */     
    Time_Adjust();                                  /* Adjust time by users typed on the hyperterminal */    
    BKP_WriteBackupRegister(BKP_DR1, 0x5A5A);
  }
  else                                              /*��������������ʱ��*/
  {               
    RTC_WaitForSynchro();                           /*�ȴ��Ĵ���ͬ��*/        
    RTC_ITConfig(RTC_IT_SEC, ENABLE);               /*����RTC���ж�*/        
    RTC_WaitForLastTask();                          /*�ȴ��ϴ�RTC�Ĵ���д�������*/
  }    
  RCC_ClearFlag();                                  /* Clear reset flags */  
}



/*******************************************************************************
* Function Name  : Time_Regulate
* Description    : Returns the time entered by user, using Hyperterminal.
* Input          : None
* Output         : None
* Return         : Current time RTC counter value
*******************************************************************************/
void Time_Regulate(struct rtc_time *tm)
{
  u32 Tmp_YY = 18, Tmp_MM = 3, Tmp_DD = 29, Tmp_HH = 0, Tmp_MI = 0, Tmp_SS = 0;
  
  tm->tm_year = Tmp_YY+2000; 
  tm->tm_mon= Tmp_MM;
  tm->tm_mday= Tmp_DD;
  tm->tm_hour= Tmp_HH;
  tm->tm_min= Tmp_MI;
  tm->tm_sec= Tmp_SS;
}


/*******************************************************************************
* Function Name  : Time_Adjust
* Description    : Adjusts time.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void Time_Adjust(void)
{  
  RTC_WaitForLastTask();                    /* Wait until last write operation on RTC registers has finished */    
  Time_Regulate(&systmtime);                /* Get time entred by the user on the hyperterminal */    
  GregorianDay(&systmtime);                 /* Get wday */    
  RTC_SetCounter(mktimev(&systmtime));      /* �޸ĵ�ǰRTC�����Ĵ������� */    
  RTC_WaitForLastTask();                    /* Wait until last write operation on RTC registers has finished */
}


/*******************************************************************************
* Function Name  : Time_Display
* Description    : Displays the current time.
* Input          : - TimeVar: RTC counter value.
* Output         : None
* Return         : None
*******************************************************************************/
void Time_Display(u32 TimeVar)
{
  u8 str[15]; // �ַ����ݴ�
  to_tm(TimeVar, &systmtime);

  
  

      
    //if((!systmtime.tm_hour && !systmtime.tm_min && !systmtime.tm_sec))
    {    
      GetChinaCalendar((u16)systmtime.tm_year, (u8)systmtime.tm_mon, (u8)systmtime.tm_mday, str);    
      printf("\r\n  ����ũ����%0.2d%0.2d-%0.2d-%0.2d", str[0], str[1], str[2],  str[3]);    
      GetChinaCalendarStr((u16)systmtime.tm_year,(u8)systmtime.tm_mon,(u8)systmtime.tm_mday,str);
      printf("  %s", str);    
      if(GetJieQiStr((u16)systmtime.tm_year, (u8)systmtime.tm_mon, (u8)systmtime.tm_mday, str))
        printf("  %s", str);    
    }      



}



/*******************************************************************************
* Function Name  : Time_Show
* Description    : Shows the current time (HH:MM:SS) on the Hyperterminal.
* Input          : None
* Output         : None
* Return         : None
******************************************************************************/
void Time_Show(void)
{    
    if (TimeDisplay == 1)               /* ÿ��1s */
    {     
      Time_Display(RTC_GetCounter());   /* Display current time */
      TimeDisplay = 0;
    }  
}



/************************************************************************
* This only works for the Gregorian calendar - i.e. after 1752 (in the UK)
*************************************************************************/
/*���㹫��*/
void GregorianDay(struct rtc_time * tm)
{
  int leapsToDate;
  int lastYear;
  int day;
  int MonthOffset[] = { 0,31,59,90,120,151,181,212,243,273,304,334 };
  
  lastYear=tm->tm_year-1;
  
  /*����ӹ�ԪԪ�굽������ǰһ��֮��һ�������˶��ٸ�����*/
  leapsToDate = lastYear/4 - lastYear/100 + lastYear/400;      
  
  /*������������һ��Ϊ���꣬�Ҽ������·���2��֮����������1�����򲻼�1*/
  if((tm->tm_year%4==0) &&((tm->tm_year%100!=0) || (tm->tm_year%400==0)) &&(tm->tm_mon>2))
  {
    /*
    * We are past Feb. 29 in a leap year
    */
    day=1;
  } 
  else 
  {
    day=0;
  }
  
  day += lastYear*365 + leapsToDate + MonthOffset[tm->tm_mon-1] + tm->tm_mday; /*����ӹ�ԪԪ��Ԫ������������һ���ж�����*/
  
  tm->tm_wday=day%7;
}

/* Converts Gregorian date to seconds since 1970-01-01 00:00:00.
* Assumes input in normal date format, i.e. 1980-12-31 23:59:59
* => year=1980, mon=12, day=31, hour=23, min=59, sec=59.
*
* [For the Julian calendar (which was used in Russia before 1917,
* Britain & colonies before 1752, anywhere else before 1582,
* and is still in use by some communities) leave out the
* -year/100+year/400 terms, and add 10.]
*
* This algorithm was first published by Gauss (I think).
*
* WARNING: this function will overflow on 2106-02-07 06:28:16 on
* machines were long is 32-bit! (However, as time_t is signed, we
* will already get problems at other places on 2038-01-19 03:14:08)
*/
u32 mktimev(struct rtc_time *tm)
{
  
  if (0 >= (int) (tm->tm_mon -= 2))  
  {	/* 1..12 -> 11,12,1..10 */
    tm->tm_mon += 12;		/* Puts Feb last since it has leap day */
    tm->tm_year -= 1;
  }
  
  return (((
            (u32) (tm->tm_year/4 - tm->tm_year/100 + tm->tm_year/400 + 367*tm->tm_mon/12 + tm->tm_mday) +
              tm->tm_year*365 - 719499
                )*24 + tm->tm_hour /* now have hours */
           )*60 + tm->tm_min /* now have minutes */
          )*60 + tm->tm_sec; /* finally seconds */
}

void to_tm(u32 tim, struct rtc_time * tm)
{
  register u32    i;
  register long   hms, day;
  
  day = tim / SECDAY;
  hms = tim % SECDAY;
  
  /* Hours, minutes, seconds are easy */
  tm->tm_hour = hms / 3600;
  tm->tm_min = (hms % 3600) / 60;
  tm->tm_sec = (hms % 3600) % 60;
  
  /* Number of years in days */ /*�����ǰ��ݣ���ʼ�ļ������Ϊ1970��*/
  for (i = STARTOFTIME; day >= days_in_year(i); i++) {
    day -= days_in_year(i);
  }
  tm->tm_year = i;
  
  /* Number of months in days left */ /*���㵱ǰ���·�*/
  if (leapyear(tm->tm_year)) {
    days_in_month(FEBRUARY) = 29;
  }
  for (i = 1; day >= days_in_month(i); i++) {
    day -= days_in_month(i);
  }
  days_in_month(FEBRUARY) = 28;
  tm->tm_mon = i;
  
  /* Days are what is left over (+1) from all that. *//*���㵱ǰ����*/
  tm->tm_mday = day + 1;
  
  /*
  * Determine the day of week
  */
  GregorianDay(tm);
  
}
