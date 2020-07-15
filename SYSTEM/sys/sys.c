#include "includes.h"

char *SoftwareVersion = __DATE__ __TIME__;          // ?????
//********************************************************************************  
void NVIC_Configuration(void)
{

    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);	//设置NVIC中断分组2:2位抢占优先级，2位响应优先级

}


//hex?dec 0x99 --> 99
unsigned char hex2bcd(unsigned char hex_data)
{
	unsigned char dec_data = 0x00;
	dec_data = (hex_data >> 4) * 10;
	dec_data += (hex_data & 0x0F);
	return dec_data;
}


//hex?dec 0x11 --> 0x17
unsigned char bcd2hex(unsigned char hex_data)
{
	unsigned char dec_data = 0x00;
        dec_data = (hex_data/10) *16 ;
        dec_data += (hex_data%10);
	return dec_data;
}


void vRestore_Factory_Set(void)
{
  memset((u8 *)(&bib_info),0x00,sizeof(tbl_info));
}


unsigned char do_sum_check(unsigned char *buf)
{
	unsigned char sum = 0x00;
	unsigned char len = 0x00;
	unsigned char i = 0x00;
	
	len = buf[1]; // ????????
	for (i = 0x00; i < len - 0x02; i++)
	{
		sum += buf[i];
	}
	sum += 0xAA;
	if (buf[len - 0x02] == sum) // ????OK
	{
		return 0x01;
	}
	else // ??????
	{
		return 0x00;	
	}
}



unsigned long check_sum(unsigned char *a,unsigned long lenth)
{
  unsigned long i;
  unsigned long sum=0;
  
  for(i=0;i<lenth;i++)
  {
    sum	+=	*(a+i);
  }
  return sum;	
}
