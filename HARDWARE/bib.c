#include <includes.h>

#ifdef BIB_INFO_EN
tbl_info  bib_info;

int WriteBIB(tbl_info * info)
{
  
#if OS_CRITICAL_METHOD == 3  
    OS_CPU_SR  cpu_sr;
#endif
    
  u32 writeAddr;
  FLASH_Status  s;
  int i;
  u8 *ch1;

  writeAddr = BIB_ADDRESS;
  ch1 =(u8*)info;
  OS_ENTER_CRITICAL();
  //NVIC_SETPRIMASK();
  FLASH_Unlock();
  /* Clear All pending flags */
  FLASH_ClearFlag(FLASH_FLAG_BSY | FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPRTERR);         
  s = FLASH_ErasePage(BIB_ADDRESS);
  for(i=0;i<sizeof(tbl_info)/4;i++)
  {
    s = FLASH_ProgramWord(writeAddr,*((u32*)ch1)); 
    ch1 += 4;
    writeAddr += 4;
  }
  FLASH_Lock();
  OS_EXIT_CRITICAL(); 
  //NVIC_RESETPRIMASK();

  return s;
                      
}


int ChksumBib(tbl_info * info)
{
  u32 chksum;
  u32 *p32;
  int i;

  p32 = (u32 *)info;
  chksum = 0;
  for(i=0;i<(sizeof(tbl_info)/4);i++)
  {  
    chksum += *p32;
    p32++;
  }  
  if( chksum ==0) 
    return 1;
  else
    return -1;
  
}


void SaveSystemStatus(void)
{
  u32 chksum;
  u32 *p32;
  int i;
#ifdef OS_TICKS_PER_SEC	 	//如果时钟节拍数定义了,说明要使用ucosII了.
  OSIntEnter();    
#endif  
  //bib_info.g_xSystemParameter_OnChip.xValue = g_xSystemParameter.xValue;
  
  bib_info.length = sizeof(tbl_info);  
  bib_info.type   = SYSTEM_INFO;  
  bib_info.chksum = 0;
  
  p32 = (u32 *)&bib_info;
  chksum = 0;
  for(i=0;i<(sizeof(tbl_info)/4);i++)
  {  
    chksum += *p32;
    p32++;
  }  
  chksum = (~chksum) + 1;
  bib_info.chksum = chksum;
  WriteBIB(&bib_info);
#ifdef OS_TICKS_PER_SEC	 	//如果时钟节拍数定义了,说明要使用ucosII了.
  OSIntExit();  											 
#endif
}


#endif
