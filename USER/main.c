/************************************************
	���Ƽ�modbus���԰�
	���ߣ�������
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
	SystemInit();                       //����ʱ��72MHZ
	GetLockCode();                      //�õ�CPU ID��
	EXTI_Key_Config( );					//���õ���ĸ��ⲿ�жϣ�����MODBUS�����ⲿ�ж�
	delay_init();	    	            //��ʱ������ʼ��SYSTICK
	Key_Init() ;                        //����PB0 �����źţ���������PB11�ܽ�
	keyInit() ;                         //���ðѵ���������ӵ��ܵİ���s��,֧��12������
	RS485_Init();						//MODBUSͨ�ų�ʼ��������������ʱ��	
	RS485_Addr= Flash_R(0x0801FC00) ;   //��ȡ���һҳ������ ���Ƕ�ȡд���MODBUS��ַ
	while(1)
	{
		RS485_Service();
	}
}
			
		

		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
		
	












		


