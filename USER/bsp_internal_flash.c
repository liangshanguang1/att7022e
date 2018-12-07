/**
  ******************************************************************************
  * @file    bsp_internalFlash.c
  * @author  fire
  * @version V1.0
  * @date    2015-xx-xx
  * @brief   内部FLASH读写测试范例
  ******************************************************************************
  * @attention
  *
  * 实验平台:秉火  STM32 霸道 开发板  
  * 论坛    :http://www.firebbs.cn
  * 淘宝    :https://fire-stm32.taobao.com
  *
  ******************************************************************************
  */
  
#include "bsp_internal_flash.h"







/**
  * @brief  InternalFlash_Test,对内部FLASH进行读写测试
  * @param  None
  * @retval None
  */
	#if 0
int InternalFlash_Test(void)
{
	uint32_t EraseCounter = 0x00; 	//记录要擦除多少页
	uint32_t Address = 0x00;				//记录写入的地址
	uint32_t Data = 0x3210ABCD;			//记录写入的数据
	uint32_t NbrOfPage = 0x00;			//记录写入多少页
	
	FLASH_Status FLASHStatus = FLASH_COMPLETE; //记录每次擦除的结果	
	TestStatus MemoryProgramStatus = PASSED;//记录整个测试结果
	

  /* 解锁 */
  FLASH_Unlock();

  /* 计算要擦除多少页 */
  NbrOfPage = (WRITE_END_ADDR - WRITE_START_ADDR) / FLASH_PAGE_SIZE;

  /* 清空所有标志位 */
  FLASH_ClearFlag(FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPRTERR);	

  /* 按页擦除*/
  for(EraseCounter = 0; (EraseCounter < NbrOfPage) && (FLASHStatus == FLASH_COMPLETE); EraseCounter++)
  {
    FLASHStatus = FLASH_ErasePage(WRITE_START_ADDR + (FLASH_PAGE_SIZE * EraseCounter));
  
	}
  
  /* 向内部FLASH写入数据 */
  Address = WRITE_START_ADDR;

  while((Address < WRITE_END_ADDR) && (FLASHStatus == FLASH_COMPLETE))
  {
    FLASHStatus = FLASH_ProgramWord(Address, Data);
    Address = Address + 4;
  }

  FLASH_Lock();
  
  /* 检查写入的数据是否正确 */
  Address = WRITE_START_ADDR;

  while((Address < WRITE_END_ADDR) && (MemoryProgramStatus != FAILED))
  {
    if((*(__IO uint32_t*) Address) != Data)
    {
      MemoryProgramStatus = FAILED;
    }
    Address += 4;
  }
	return MemoryProgramStatus;
}
#endif

void Flash_W(u32 date)     // 向最后一页写入u32 数据  用来写入modbus 地址
{
	u32 Address;
	u32 WRITE_START_ADDR =0x0801FC00 ;
	FLASH_Unlock();
	FLASH_ClearFlag(FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPRTERR);	
 FLASH_ErasePage(WRITE_START_ADDR  );
 FLASH_ProgramWord(WRITE_START_ADDR, date);
	Address = Address + 4;
	FLASH_Lock();

}
u32  Flash_R(u32 b)
{
    u32 a;
	a= (*(__IO uint32_t*) b) ;
	return a;

}
