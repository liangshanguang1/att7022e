#ifndef __INTERNAL_FLASH_H
#define	__INTERNAL_FLASH_H

#include "stm32f10x.h"

/* STM32��������Ʒÿҳ��С2KByte���С�С������Ʒÿҳ��С1KByte */
#if defined (STM32F10X_HD) || defined (STM32F10X_HD_VL) || defined (STM32F10X_CL) || defined (STM32F10X_XL)
  #define FLASH_PAGE_SIZE    ((uint16_t)0x800)	//2048
#else
  #define FLASH_PAGE_SIZE    ((uint16_t)0x400)	//1024
#endif

//д�����ʼ��ַ�������ַ
//#define WRITE_START_ADDR  ((uint32_t)0x0801FC00)
//#define WRITE_END_ADDR    ((uint32_t)0x0801FFFF)




void Flash_W(u32 date) ;   // �����һҳд��u32 ����
u32  Flash_R(u32 )   ;

#endif /* __INTERNAL_FLASH_H */

