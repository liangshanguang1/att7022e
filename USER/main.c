/************************************************
	零点科技modbus测试板
	作者：梁闪光
************************************************/
#include "sys.h"	
#include "delay.h"	
#include "modbus.h"
#include "key.h"
#include "stm32f10x.h"
#include "bsp_internal_flash.h"   
#include "bsp_exti.h"
 int main(void)
{
	SystemInit();                       //设置时钟72MHZ
	GetLockCode();                      //得到CPU ID号
	EXTI_Key_Config( );					//设置电机四个外部中断，加上MODBUS级联外部中断
	delay_init();	    	            //延时函数初始化SYSTICK
	Key_Init() ;                        //包括PB0 串联信号，按键输入PB11管脚
	keyInit() ;                         //设置把单个按键添加到总的按键s中,支持12个按键
	RS485_Init();						//MODBUS通信初始化，包括两个定时器	
	RS485_Addr= Flash_R(0x0801FC00) ;   //读取最后一页的数据 就是读取写入的MODBUS地址
	while(1)
	{
		RS485_Service();
	}
}
			
		

		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
	












		


