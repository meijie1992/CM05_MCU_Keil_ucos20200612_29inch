#ifndef __BIB_H
#define __BIB_H
#include <includes.h> 

#define BIB_INFO_EN

#ifdef   BIB_INFO_EN

#define  BIB_ADDRESS   0x0800F800  //FLASH最后2K  

#define SYSTEM_INFO   0x19890702


typedef union
{
  u8 u8FlashBuffer[100];
  
  struct 
  {
    u8   Systerm_Key_Date_Begin;                        // ??????,??
    
    u8   IC_state;                                      //?????0xAA?;0x55?
    
    u8   IC_group;                                      //??

    u8   IC_sector;                                     //??
    
    u8   IC_key[6];                                     //??

    u8   IC_config[48];                                 //????
    
    u32  System_Flag;                                   //????
    
    u8   Key_Password_Flag;                             //????????? 0x55?????
      
    u8   burden_floor_num;                              //????
      
    u8   a_door_room_num;                               //A????
      
    u8   a_door_direction;                              //A????
    
    u8   dop_acs_id;                                    //?????DOP_ID,?????ACS_ID

    u8   dop_floor_id;                                  //FLOOR_ID
    
    u8   finger_every_room;                             // ?????,??10
    
    u8   IS_online;                                     // ????
    
    u8   Systerm_Key_Date_End;                           // ??????,??

    u8   u8First_Run;					//(?????) ???????????:0x55 -- ????;?0x55 -- ????
    
  } xValue;
} U_ParameterStruct;

//需要确保结构大小能被4整除
typedef struct _t_bootloaer_info
{
  u32	  length;
  u32 	  type;
  u32     chksum;
  U_ParameterStruct g_xSystemParameter_OnChip;
}tbl_info;

extern tbl_info  bib_info; 
int WriteBIB(tbl_info * info);

int ChksumBib(tbl_info * info);

void SaveSystemStatus(void);
#endif

#endif 
