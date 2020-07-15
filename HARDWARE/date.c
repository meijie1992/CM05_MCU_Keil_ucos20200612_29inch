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
*函数名：RTC_Configuration
*描述：配置RTC
*输入：无
*输出：无
*返回：无
************************************************************************/
void RTC_Configuration(void)
{  
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, ENABLE);/*允许PWR和BKP时钟*/    
  PWR_BackupAccessCmd(ENABLE);                                            /*允许访问BKP域*/    
  BKP_DeInit();                                                           /*复位备份域*/  
#ifdef RTCClockSource_LSI    
  RCC_LSICmd(ENABLE);                                                     /*允许LSI*/    
  while(RCC_GetFlagStatus(RCC_FLAG_LSIRDY)==RESET);                       /*等待LSI准备好*/    
  RCC_RTCCLKConfig(RCC_RTCCLKSource_LSI);                                 /*选择LSI作为RTC时钟源*/  
#elif defined  RTCClockSource_LSE    
  RCC_LSEConfig(RCC_LSE_ON);                                              /*允许LSE*/    
  while(RCC_GetFlagStatus(RCC_FLAG_LSERDY)==RESET);                       /*等待LSE准备好*/    
  RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);                                 /*选择LSE作为RTC时钟源*/
#endif    
  RCC_RTCCLKCmd(ENABLE);                                                  /* Enable RTC Clock */     
  RTC_WaitForSynchro();                                                   /*等待寄存器同步*/    
  RTC_WaitForLastTask();                                                  /*等待上次RTC寄存器写操作完成*/    
  RTC_ITConfig(RTC_IT_SEC, ENABLE);                                       /*允许RTC秒中断*/    
  RTC_WaitForLastTask();                                                  /*等待上次RTC寄存器写操作完成*/  
#ifdef RTCClockSource_LSI
  /*设置分频系数*/
  RTC_SetPrescaler(31999);                                                /*RTC周期=RTCCLK/RTC_PR=(32.000kHz/(31999+1))*/  
#elif defined  RTCClockSource_LSE
//  RTC_SetPrescaler(32767);                                                /*RTC周期=RTCCLK/RTC_PR=(32.768kHz/(31767+1))*/
/*
  24小时快了9秒,那么就是每秒钟快了 9/(24*3600) 秒,约合:0.1042ms
  因此有  1.0001042ms=RTCreal/32768;
  通过上式就可以求出真正的RTC晶振频率,RTCreal=32771.4Hz
*/
  RTC_SetPrescaler(32772);
#endif   
  RTC_WaitForLastTask();                                                  /*等待上次RTC寄存器写操作完成*/  
}


void RTC_Init(void)
{	  
  RTC_NVIC_Config();                                /* NVIC configuration */  
  if (BKP_ReadBackupRegister(BKP_DR1) != 0x5A5A)    /*在启动时检查备份寄存器BKP_DR1，如果内容不是0xA5A5,则需重新配置时间并询问用户调整时间*/
  {       
    RTC_Configuration();                            /* RTC Configuration */     
    Time_Adjust();                                  /* Adjust time by users typed on the hyperterminal */    
    BKP_WriteBackupRegister(BKP_DR1, 0x5A5A);
  }
  else                                              /*启动无需设置新时钟*/
  {               
    RTC_WaitForSynchro();                           /*等待寄存器同步*/        
    RTC_ITConfig(RTC_IT_SEC, ENABLE);               /*允许RTC秒中断*/        
    RTC_WaitForLastTask();                          /*等待上次RTC寄存器写操作完成*/
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
  RTC_SetCounter(mktimev(&systmtime));      /* 修改当前RTC计数寄存器内容 */    
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
  u8 str[15]; // 字符串暂存
  to_tm(TimeVar, &systmtime);

  
  

      
    //if((!systmtime.tm_hour && !systmtime.tm_min && !systmtime.tm_sec))
    {    
      GetChinaCalendar((u16)systmtime.tm_year, (u8)systmtime.tm_mon, (u8)systmtime.tm_mday, str);    
      printf("\r\n  今天农历：%0.2d%0.2d-%0.2d-%0.2d", str[0], str[1], str[2],  str[3]);    
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
    if (TimeDisplay == 1)               /* 每过1s */
    {     
      Time_Display(RTC_GetCounter());   /* Display current time */
      TimeDisplay = 0;
    }  
}



/************************************************************************
* This only works for the Gregorian calendar - i.e. after 1752 (in the UK)
*************************************************************************/
/*计算公历*/
void GregorianDay(struct rtc_time * tm)
{
  int leapsToDate;
  int lastYear;
  int day;
  int MonthOffset[] = { 0,31,59,90,120,151,181,212,243,273,304,334 };
  
  lastYear=tm->tm_year-1;
  
  /*计算从公元元年到计数的前一年之中一共经历了多少个闰年*/
  leapsToDate = lastYear/4 - lastYear/100 + lastYear/400;      
  
  /*如若计数的这一年为闰年，且计数的月份在2月之后，则日数加1，否则不加1*/
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
  
  day += lastYear*365 + leapsToDate + MonthOffset[tm->tm_mon-1] + tm->tm_mday; /*计算从公元元年元旦到计数日期一共有多少天*/
  
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
  
  /* Number of years in days */ /*算出当前年份，起始的计数年份为1970年*/
  for (i = STARTOFTIME; day >= days_in_year(i); i++) {
    day -= days_in_year(i);
  }
  tm->tm_year = i;
  
  /* Number of months in days left */ /*计算当前的月份*/
  if (leapyear(tm->tm_year)) {
    days_in_month(FEBRUARY) = 29;
  }
  for (i = 1; day >= days_in_month(i); i++) {
    day -= days_in_month(i);
  }
  days_in_month(FEBRUARY) = 28;
  tm->tm_mon = i;
  
  /* Days are what is left over (+1) from all that. *//*计算当前日期*/
  tm->tm_mday = day + 1;
  
  /*
  * Determine the day of week
  */
  GregorianDay(tm);
  
}
