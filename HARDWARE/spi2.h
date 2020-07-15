#ifndef __SPI2_H
#define __SPI2_H
#include <includes.h>


#define Dummy2_Byte   0xA5

#define SPI2_CS PBin(12)


extern u8 SPI2_RxData;
extern u8 SPI2_Rx_Buf[];
extern u8 SPI2_Rx_len;
extern u8 SPI2_Rx_time;
extern u8 SPI2_RxDate_Flag;

struct elevator_state {
  u8 floor_h;
  u8 floor_l;
  u8 run_direction;
  u8 state;
};

extern struct elevator_state e_s;



void SPI2_Init(void);
u8 SPI2_SendByte(u8 byte);
u8 SPI2_ReadByte(void);


void left_shift(unsigned char *str, int len);
void handle_SPI2(u8 * cmd_spi,u8 len);
#endif
