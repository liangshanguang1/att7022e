#ifndef _MODBUS_H
#define _MODBUS_H
#include "stm32f10x.h"
#define RS485_TX_EN() GPIO_SetBits(GPIOA,GPIO_Pin_0) 
#define RS485_RX_EN() GPIO_ResetBits(GPIOA,GPIO_Pin_0)
extern u8 KeyFlag ;		//全局变量用来表示 设置MODBUS地址的标志，在按键长按中使用
extern u32 RS485_Addr;//从机地址
extern u16 Message[28];
void RS485_Init(void);
void Timer2_Init(void);
void Timer3_Init(void)   ;
void Key_Exti_Init(void);
void EXTIX_Init(void);
void RS485_Service(void);
void Modbus_02_Solve(void);
void Modbus_01_Solve(void);
void Modbus_05_Solve(void);
void Modbus_15_Solve(void);
void Modbus_03_Solve(void);
void Modbus_06_Solve(void);
void Modbus_16_Solve(void);
void Modbus_FF_Solve(void);
void RS485_SendData(u8 *buff,u8 len);\
void GetLockCode(void);

#endif
