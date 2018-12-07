/**
  ******************************************************************************
  * @file    GPIO/IOToggle/stm32f10x_it.c 
  * @author  MCD Application Team
  * @version V3.5.0
  * @date    08-April-2011
  * @brief   Main Interrupt Service Routines.
  *          This file provides template for all exceptions handler and peripherals
  *          interrupt service routine.
  ******************************************************************************
  * @attention
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  * <h2><center>&copy; COPYRIGHT 2011 STMicroelectronics</center></h2>
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x_it.h" 
#include "bsp_exti.h"
#include "moter.h"
#include "modbus.h"
#include "delay.h"
#include "key.h"
struct MoterState moterstate;
void NMI_Handler(void)
{
}
 
void HardFault_Handler(void)
{
  /* Go to infinite loop when Hard Fault exception occurs */
  while (1)
  {
  }
}
 
void MemManage_Handler(void)
{
  /* Go to infinite loop when Memory Manage exception occurs */
  while (1)
  {
  }
}

 
void BusFault_Handler(void)
{
  /* Go to infinite loop when Bus Fault exception occurs */
  while (1)
  {
  }
}
 
void UsageFault_Handler(void)
{
  /* Go to infinite loop when Usage Fault exception occurs */
  while (1)
  {
  }
}
 
void SVC_Handler(void)
{
}
 
void DebugMon_Handler(void)
{
}
 
void PendSV_Handler(void)
{
}
 
void SysTick_Handler(void)
{
}
//引脚定义  KEY1 上上代表分闸
void UpUpsensor_IRQHandler(void)
{
  //确保是否产生了EXTI Line中断
	if(EXTI_GetITStatus(UpUpsensor_EXTI_LINE) != RESET) 
	{
    moterstate.fenzha=1;
		moterstate.hezha=0;
    //清除中断标志位
		EXTI_ClearITPendingBit(UpUpsensor_EXTI_LINE);     
	}  
}
//引脚定义  KEY2 上下代表合闸     这个是上齿轮的信号，因为上齿轮是首先达到的，上齿轮产生中断之后，电机还要继续运行
void UpDownsensor_IRQHandler(void)
{
	if(EXTI_GetITStatus(UpDownsensor_EXTI_LINE) != RESET) 
	{
		moterstate.fenzha=0;
		moterstate.hezha=1;
		EXTI_ClearITPendingBit(UpDownsensor_EXTI_LINE);     
	}  
	if(EXTI_GetITStatus(EXTI_Line12) != RESET) 
	{
		delay_ms(2);    //消抖			 
		if(KEY1==0) 	KeyFlag = 1;
		EXTI_ClearITPendingBit(EXTI_Line12);     
	}  
}
//引脚定义  KEY3 下上代表合闸
void DownUpsensor_IRQHandler(void)
{
  //确保是否产生了EXTI Line中断
	if(EXTI_GetITStatus(DownUpsensor_EXTI_LINE) != RESET) 
	{
			if(moterstate.shoudonghe==0)
		{
			if(moterstate.hezha==1)
			{
				LED1_OFF	;
				LED2_OFF;
				moterstate.shoudongfen=0;
			}
		}
		EXTI_ClearITPendingBit(DownUpsensor_EXTI_LINE);     
	}  
}
//引脚定义  KEY4 下下代表分闸
void DownDownsensor_IRQHandler(void)
{
	if(EXTI_GetITStatus(DownDownsensor_EXTI_LINE) != RESET) 
	{	
		
		if(moterstate.shoudongfen==0)
		{
			if(moterstate.fenzha==1)
			{
				LED1_OFF;
				LED2_OFF;
				moterstate.shoudonghe=0;
			}
		}		
		EXTI_ClearITPendingBit(DownDownsensor_EXTI_LINE);     
	}  
}

/******************************************************************************/
/*                 STM32F10x Peripherals Interrupt Handlers                   */
/*  Add here the Interrupt Handler for the used peripheral(s) (PPP), for the  */
/*  available peripheral interrupt handler's name please refer to the startup */
/*  file (startup_stm32f10x_xx.s).                                            */
/******************************************************************************/
