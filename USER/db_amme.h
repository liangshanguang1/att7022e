#ifndef _SPI_H_
#define _SPI_H_
#include "stdio.h"
#include "stm32f10x.h"

//////////非远程升级版本硬件使用下面代码//////////////
/*
#define SPI_CS_HIGH() 	 GPIO_SetBits(GPIOE, GPIO_Pin_15)
#define SPI_DI_HIGH()    GPIO_SetBits(GPIOB, GPIO_Pin_15)
#define SPI_DO_HIGH()    GPIO_SetBits(GPIOB, GPIO_Pin_14)
#define SPI_CLK_HIGH()   GPIO_SetBits(GPIOB, GPIO_Pin_13)

#define SPI_CS_LOW() 	GPIO_ResetBits(GPIOE, GPIO_Pin_15)
#define SPI_DI_LOW()    GPIO_ResetBits(GPIOB, GPIO_Pin_15)
#define SPI_DO_LOW()    GPIO_ResetBits(GPIOB, GPIO_Pin_14)
#define SPI_CLK_LOW()   GPIO_ResetBits(GPIOB, GPIO_Pin_13)   //对应ATT7022上的管脚

#define RDSPIData       GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_14)//读数据

*/
//////////远程升级版本硬件使用下面代码//////////////
#define SPI_CS_HIGH() 	 GPIO_SetBits(GPIOA, GPIO_Pin_8)
#define SPI_DI_HIGH()    GPIO_SetBits(GPIOB, GPIO_Pin_15)
#define SPI_DO_HIGH()    GPIO_SetBits(GPIOB, GPIO_Pin_14)
#define SPI_CLK_HIGH()   GPIO_SetBits(GPIOB, GPIO_Pin_13)

#define SPI_CS_LOW() 	GPIO_ResetBits(GPIOA, GPIO_Pin_8)
#define SPI_DI_LOW()    GPIO_ResetBits(GPIOB, GPIO_Pin_15)
#define SPI_DO_LOW()    GPIO_ResetBits(GPIOB, GPIO_Pin_14)
#define SPI_CLK_LOW()   GPIO_ResetBits(GPIOB, GPIO_Pin_13)   //对应ATT7022上的管脚

#define RDSPIData   GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_14)//读数据


#define ATT_G  1.163
#define INATTV 	72			      // 模拟扩大8倍         
#define INATTI 	5			        //互感器感应电压 扩大1000倍  
#define EC     3200 					//电表表常数
#define UI	   220 					  //额定电压
#define VI	   0.0478
#define UI_K   22.5  // 这个值的计算是按照当输入进芯片的RMS为50MV时 此时的电流为IB
                   //这个跟芯片内部无关因为50MV的电压对应芯片内部计算得到的值为60,此时的UI=60/IB,这个板子IB=5A

#define Exterl_meter_start_number 2
//#define  HFCONST ((2.592*ATT_G*ATT_G*10000*INATTV*INATTI)/(EC*UI*VI))  =76
//#define P_K   1.829E-4//1000000*UI*VI/(ATT_G*ATT_G*INATTV*INATTI*8388608)	//	  2^23 = 8388608

extern u32 CurrentElectric;				//总电量
extern u32 CurrentElectricA;
extern u32 CurrentElectricB;
extern u32 CurrentElectricC;

extern u8 SetGainData;					//设置增益数据

extern u32 Up_PA_Q;
extern u32 Up_PA_Power;
extern u32 Up_QA_Power;

extern u32 Up_PB_Q;
extern u32 Up_PB_Power;
extern u32 Up_QB_Power;

extern u32 Up_PC_Q;
extern u32 Up_PC_Power;
extern u32 Up_QC_Power;

extern u32 Up_CurrentA;
extern u32 Up_CurrentB;
extern u32 Up_CurrentC;

extern u32 Up_VolA;
extern u32 Up_VolB;
extern u32 Up_VolC;

extern u16 Huganqibeilv;                             //互感器倍率
extern u8 RS485_read_command[8];
extern u8 UP_Exter_data[250];					//上报外部电量数据
extern u8 ReadAmmeterData_min;																			//读电表的分钟数据
extern u32 EST_Q_buff;  // 之前的总合相电量
extern u8 Exter_number_meter;					//外接电表数量
extern u8 Exter_Data_Reading_flag;			//外部电表数据读取中
extern u8 Exter_Data_Read_flag;			//外部电表数据读取标志
extern u8 Read_Exter_meter_num;				//外部电表标编号
extern u8 Read_Exter_meter_count;			//读取外部电表计数
extern u8 Read_Exter_meter_Outtime_flag;	//读取外部电表超时标志
extern u8 Read_Exter_meter_Outtime_count;	//读取外部电表超时次数计数

extern u8 first_Read_Exter_meter_flag;			//上电第一次读外部电表的标志
typedef struct
{
  u32 VolA;           
  u32 VolB;  
  u32 VolC;
  u32 CurrA;           
  u32 CurrB;  
  u32 CurrC; 
                                      
}DIANBIAOTypeDef;
typedef struct
{
   float  Curr	;
   float  Vol   ;
   float  Pt	;
   float  Qt    ;
   float  Pft   ;
   float  Ept   ;
   float  Freq  ;
	
}Update;
void SPI_GPIO_Config(void);
u32 READ_Att7022(u8 Address);
void Write_Att7022(u8 address, u32 write_data);
void EMU_init(void);
void InitAmmeter(void);
void ReadAmmeterData(void);
void Read_external_meter(void);							//读取外部电表数据
#endif
