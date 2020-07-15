#include <includes.h>

u8 SPI2_RxData;
u8 SPI2_Rx_Buf[256];
u8 SPI2_Rx_len;
u8 SPI2_Rx_time;
u8 SPI2_RxDate_Flag;



void SPI2_Init(void)
{
  
  GPIO_InitTypeDef GPIO_InitStructure;
  SPI_InitTypeDef  SPI_InitStructure;
  NVIC_InitTypeDef NVIC_InitStructure;
  
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB|RCC_APB2Periph_AFIO, ENABLE );//PORTB???? 
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2,  ENABLE );//SPI2???? 
  
  
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13 | GPIO_Pin_15;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;  //PB13/14/15?????? 
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOB, &GPIO_InitStructure);//???GPIOB
	
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;  //PB13/14/15?????? 
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOB, &GPIO_InitStructure);//???GPIOB

  NVIC_InitStructure.NVIC_IRQChannel = SPI2_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=2 ;//?????3
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;		//????3
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ????
  NVIC_Init(&NVIC_InitStructure);	//??????????VIC???
  
	SPI_Cmd(SPI2, DISABLE); //??SPI??
  SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_RxOnly;  //??SPI???????????:SPI??????????
  SPI_InitStructure.SPI_Mode = SPI_Mode_Slave;		//??SPI????:????SPI
  SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;		//??SPI?????:SPI????8????
  SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;		//???????????????
  SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;	//?????????????(?????)?????
  SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;		//NSS?????(NSS??)????(??SSI?)??:??NSS???SSI???
  SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_2;//SPI_BaudRatePrescaler_4;		//??????????:????????256
  SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;	//???????MSB???LSB???:?????MSB???
  SPI_InitStructure.SPI_CRCPolynomial = 7;	//CRC???????
  SPI_Init(SPI2, &SPI_InitStructure);  //??SPI_InitStruct???????????SPIx???
  

  SPI_I2S_ITConfig(SPI2, SPI_I2S_IT_RXNE, ENABLE);//????
  SPI_Cmd(SPI2, ENABLE); //??SPI??

}



void SPI2_IRQHandler(void)
{
#ifdef OS_TICKS_PER_SEC	 	//??????????,?????ucosII?.
  OSIntEnter();    
#endif
  if(SPI_I2S_GetITStatus(SPI2, SPI_I2S_IT_RXNE) != RESET)  
  {     
		//while(SPI_I2S_GetITStatus(SPI2, SPI_I2S_FLAG_RXNE) == RESET)
    SPI2_RxData = SPI_I2S_ReceiveData(SPI2);

		//if(SPI2_RxDate_Flag == 0)
		{
			//if(SPI2_RxData != 0)
			{
				SPI2_Rx_Buf[SPI2_Rx_len++] = SPI2_RxData;
				//SPI2_Rx_time = 0x10;
				
			}
			/*
			else
			{
				if(SPI2_Rx_len >= 8)
				{
					SPI2_RxDate_Flag = 0x01;
				}
				else
				{
					SPI2_Rx_len = 0;
				}
				
				
			}
			*/
		}
		
		
		
    SPI_I2S_ClearITPendingBit(SPI2 , SPI_I2S_IT_RXNE);  

  } 
#ifdef OS_TICKS_PER_SEC	 	//??????????,?????ucosII?.
  OSIntExit();  											 
#endif 
  
  
}


/*-----------------------------------------------------------------------	   
??:??SPI
-------------------------------------------------------------------------*/
u8 SPI2_SendByte(u8 byte)
{
  
  while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE) == RESET);
  SPI_I2S_SendData(SPI2, byte);

  while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_RXNE) == RESET);
  return SPI_I2S_ReceiveData(SPI2);
}

/*-----------------------------------------------------------------------	   
-------------------------------------------------------------------------*/
u8 SPI2_ReadByte(void)
{
  return (SPI2_SendByte(Dummy2_Byte));
}







struct elevator_state e_s = {0x20, 0x31, 0x33, 0xFF}; // ???????????  ??????1?
u8 spi_data_change = 0x00; // SPI????????
u8 spi_data_copy[7]; // ????????SPI??
u8 spi_data[7]; // ????SPI????

u8 floor_3 = 0x00; // 

u8 spi_err_count = 0x00;



void left_shift(unsigned char *str, int len)
{
  int i;
  
  for(i = 1; i <= len; i++)
  {
    str[i-1] = str[i-1] << 1;
    
    if(i < len && str[i] & 0x80)
    {
      str[i-1] = str[i-1] | 0x01;    
    }    
  }
}


u8 switch_floor_data(u8 data_in) // ?????
{
  u8 data_out;
  
  switch (data_in)
  {
  case 0x3F: data_out = '0'; break;
  case 0x06: data_out = '1'; break;
  case 0x5B: data_out = '2'; break;
  case 0x4F: data_out = '3'; break;
  case 0x66: data_out = '4'; break;
  case 0x6D: data_out = '5'; break;
  case 0x7D: data_out = '6'; break;
  case 0x07: data_out = '7'; break;
  case 0x7F: data_out = '8'; break;
  case 0x6F: data_out = '9'; break;
  case 0x00: data_out = ' '; break;
  
  case 0x77: data_out = 'A'; break;
  case 0x91: data_out = 'B'; break;
  case 0x39: data_out = 'C'; break;
  case 0x92: data_out = 'D'; break;
  case 0x79: data_out = 'E'; break;
  case 0x71: data_out = 'F'; break;
  case 0x93: data_out = 'G'; break;
  case 0x76: data_out = 'H'; break;
  case 0x94: data_out = 'I'; break;
  case 0x1E: data_out = 'J'; break;
  case 0x95: data_out = 'K'; break;
  case 0x38: data_out = 'L'; break;
  case 0x96: data_out = 'M'; break;
  case 0x97: data_out = 'N'; break;
  case 0x98: data_out = 'O'; break;
  case 0x73: data_out = 'P'; break;
  case 0x99: data_out = 'Q'; break;
  case 0x9A: data_out = 'R'; break;
  case 0x9B: data_out = 'S'; break;
  case 0x9C: data_out = 'T'; break;
  case 0x3E: data_out = 'U'; break;
  case 0x9D: data_out = 'V'; break;
  case 0x9E: data_out = 'W'; break;
  case 0x9F: data_out = 'X'; break;
  case 0xA0: data_out = 'Y'; break;
  case 0xA1: data_out = 'Z'; break;
  case 0x40: data_out = '-'; break;
  
  default: data_out = '?'; break;
  }
  
  return data_out;
}


/******************************************************************************
** ???? : send_lift_state
** ???? : ?
** ???? : ?
** ???? : ????????????rs232?????????
** ???? : ?

*******************************************************************************/
void send_lift_state(void)
{
	u8 send_buf[10] = {0x00};
	send_buf[0] = 0x41;
	send_buf[1] = 0x0A;
	send_buf[2] = 0x03;
	send_buf[3] = floor_3;
	send_buf[4] = e_s.floor_h;
	send_buf[5] = e_s.floor_l;
	send_buf[6] = e_s.run_direction;
	send_buf[7] = e_s.state;
	send_buf[8] = (unsigned char)check_sum(&send_buf[1],7);
	send_buf[9] = 0x4A;
  USART2_Send_nData(send_buf,10);
/*
  {
    USART2_Send_Data(e_s.floor_h);
    USART2_Send_Data(e_s.floor_l);
    USART2_Send_Data(e_s.run_direction);
    if (floor_3 != 0x20) // 
    {
      USART2_Send_Data(floor_3);
    }
    else
    {
      USART2_Send_Data(0x46);
    }
    USART2_Send_Data(e_s.state);
    USART2_Send_Data('\n');
  } 
*/	
}




void handle_SPI2(u8 * cmd_spi,u8 len)
{
 u8 i;
  static u8 state = 0xFF;
  
  //USART3_Send_nData(cmd_spi,len); 
  /*
  for(i = 0;i<8;i++)
  {
    cmd_spi[i] = cmd_spi[i];
    
  }
  if((cmd_spi[0] != 0x81)||(len != 8))
  {
    SPI_I2S_DeInit(SPI2);
    SPI_I2S_ITConfig(SPI2, SPI_I2S_IT_RXNE, DISABLE);
    SPI_Cmd(SPI2, DISABLE);
    __NOP();__NOP();__NOP();
    __NOP();__NOP();__NOP();
    __NOP();__NOP();__NOP();
    __NOP();__NOP();__NOP();
    SPI2_Init();
    //SPI_I2S_ITConfig(SPI2, SPI_I2S_IT_RXNE, ENABLE);
    //SPI_Cmd(SPI2, ENABLE);
    return;
  }
  */
  
  
    
  
  for (i = 0x00; i < 0x07; i++)
  {
    spi_data[i] = cmd_spi[len - 0x07 + i]; // ??????,??6????????
  }
  
  
  if(memcmp(spi_data,spi_data_copy,7) == 0)
  {    
    return;
  }
  

    floor_3 = switch_floor_data(spi_data[0]);
    e_s.floor_h = switch_floor_data(spi_data[4]);
    e_s.floor_l = switch_floor_data(spi_data[5]); 
 
  if ((e_s.floor_h == '?') || (e_s.floor_l == '?') || (floor_3 == '?'))
  {
    SPI_I2S_DeInit(SPI2);
    SPI_I2S_ITConfig(SPI2, SPI_I2S_IT_RXNE, DISABLE);
    SPI_Cmd(SPI2, DISABLE);
    __NOP();__NOP();__NOP();
    __NOP();__NOP();__NOP();
    __NOP();__NOP();__NOP();
    __NOP();__NOP();__NOP();
    SPI2_Init();
    //SPI_I2S_ITConfig(SPI2, SPI_I2S_IT_RXNE, ENABLE);
    //SPI_Cmd(SPI2, ENABLE);	
    return;
  }        
  LED_TK = !LED_TK;
	
  if (!(spi_data[3] & 0x80)) // bit7 = 0 COB ; bit7 = 1 FCB
  {

    if (spi_data[6] & 0x10) // bit4 UP ????
    {
      e_s.run_direction = 0x32; 
    }
    else if (spi_data[6] & 0x20) // bit5 DOWN ????
    {
      e_s.run_direction = 0x31;
    }
    else
    {
      e_s.run_direction = 0x33; // ????
    }
    
    
    if (spi_data[6] & 0x01) // bit0 FRD ??
    {
      e_s.state = 0x81;
    }
    else if (spi_data[6] & 0x02) // bit1 HEL
    {
      e_s.state = 0x85;
    }
    else if (spi_data[6] & 0x40) // bit6 OLF ??
    {
      e_s.state = 0x82;
    }
    else if (spi_data[6] & 0x80) // bit7 OSS C*I ????
    {
      e_s.state = 0x87;
    }
    else if (spi_data[3] & 0x01) // bit0 OSI ????
    {
      e_s.state = 0x88;
    }
    else if (spi_data[3] & 0x02) // bit1 ATS ????
    {
      e_s.state = 0x89;
    }
    else if (spi_data[3] & 0x04) // bit2 PRC ????
    {
      e_s.state = 0x86;
    }
    else if (spi_data[3] & 0x08) // bit3 EAQ ??
    {
      e_s.state = 0x83;
    }
    else if (spi_data[3] & 0x40) // bit6 LOCKED ??
    {
      e_s.state = 0x84;
    }
    else
    {
      e_s.state = 0xFF; // NORMAL ????
    }
  }
  else // FCB
  {
    
    if (spi_data[6] & 0x01) // UP ????
    {
      e_s.run_direction = 0x32;
    }
    else if (spi_data[6] & 0x02) // DOWN ????
    {
      e_s.run_direction = 0x31;
    }
    else
    {
      e_s.run_direction = 0x33;
    }
    
    if (spi_data[3] & 0x40) // FRD ??
    {
      e_s.state = 0x81;
    }
    else if (spi_data[6] & 0x80) // LOCKED ??
    {
      e_s.state = 0x84;
    }
    else if (spi_data[3] & 0x20) // HEL
    {
      e_s.state = 0x85;
    }
    else if (spi_data[3] & 0x04) // PRC ????
    {
      e_s.state = 0x86;
    }
    else if (spi_data[3] & 0x01) // OSI ????
    {
      e_s.state = 0x88;
    }
    else if (spi_data[3] & 0x02) // ATS ????
    {
      e_s.state = 0x89;
    }
    else if (spi_data[6] & 0x40) // BPI
    {
      e_s.state = 0x8A;
    }
    else
    {
      e_s.state = 0xFF; // NORMAL ????
    }
  }
  
  

  if(state != e_s.state)
  {
    state = e_s.state;
    /*
    if(e_s.state == 0x81)//????????
    {
    Emergency_state = 0x01;
		}
            else
    {
    Emergency_state = 0x00; 
    
		}
    
    if(e_s.state == 0xFF)//?????????
    {
      spi_err_count = 0x00;
    }
    else
    {
      
      spi_err_count ++;
      if(spi_err_count >=3)
      {
				spi_err_count = 0x00; 
			}
      else
      {
				return;
			}
       
    }
    */
  }       
  memcpy(spi_data_copy,spi_data,7);
  send_lift_state(); // ??????SPI??????,????????????rs232?????????

}
