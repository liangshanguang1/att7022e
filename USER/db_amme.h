#ifndef _SPI_H_
#define _SPI_H_
#include "stdio.h"
#include "stm32f10x.h"

//////////��Զ�������汾Ӳ��ʹ���������//////////////
/*
#define SPI_CS_HIGH() 	 GPIO_SetBits(GPIOE, GPIO_Pin_15)
#define SPI_DI_HIGH()    GPIO_SetBits(GPIOB, GPIO_Pin_15)
#define SPI_DO_HIGH()    GPIO_SetBits(GPIOB, GPIO_Pin_14)
#define SPI_CLK_HIGH()   GPIO_SetBits(GPIOB, GPIO_Pin_13)

#define SPI_CS_LOW() 	GPIO_ResetBits(GPIOE, GPIO_Pin_15)
#define SPI_DI_LOW()    GPIO_ResetBits(GPIOB, GPIO_Pin_15)
#define SPI_DO_LOW()    GPIO_ResetBits(GPIOB, GPIO_Pin_14)
#define SPI_CLK_LOW()   GPIO_ResetBits(GPIOB, GPIO_Pin_13)   //��ӦATT7022�ϵĹܽ�

#define RDSPIData       GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_14)//������

*/
//////////Զ�������汾Ӳ��ʹ���������//////////////
#define SPI_CS_HIGH() 	 GPIO_SetBits(GPIOA, GPIO_Pin_8)
#define SPI_DI_HIGH()    GPIO_SetBits(GPIOB, GPIO_Pin_15)
#define SPI_DO_HIGH()    GPIO_SetBits(GPIOB, GPIO_Pin_14)
#define SPI_CLK_HIGH()   GPIO_SetBits(GPIOB, GPIO_Pin_13)

#define SPI_CS_LOW() 	GPIO_ResetBits(GPIOA, GPIO_Pin_8)
#define SPI_DI_LOW()    GPIO_ResetBits(GPIOB, GPIO_Pin_15)
#define SPI_DO_LOW()    GPIO_ResetBits(GPIOB, GPIO_Pin_14)
#define SPI_CLK_LOW()   GPIO_ResetBits(GPIOB, GPIO_Pin_13)   //��ӦATT7022�ϵĹܽ�

#define RDSPIData   GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_14)//������


#define ATT_G  1.163
#define INATTV 	72			      // ģ������8��         
#define INATTI 	5			        //��������Ӧ��ѹ ����1000��  
#define EC     3200 					//������
#define UI	   220 					  //���ѹ
#define VI	   0.0478
#define UI_K   22.5  // ���ֵ�ļ����ǰ��յ������оƬ��RMSΪ50MVʱ ��ʱ�ĵ���ΪIB
                   //�����оƬ�ڲ��޹���Ϊ50MV�ĵ�ѹ��ӦоƬ�ڲ�����õ���ֵΪ60,��ʱ��UI=60/IB,�������IB=5A

#define Exterl_meter_start_number 2
//#define  HFCONST ((2.592*ATT_G*ATT_G*10000*INATTV*INATTI)/(EC*UI*VI))  =76
//#define P_K   1.829E-4//1000000*UI*VI/(ATT_G*ATT_G*INATTV*INATTI*8388608)	//	  2^23 = 8388608

extern u32 CurrentElectric;				//�ܵ���
extern u32 CurrentElectricA;
extern u32 CurrentElectricB;
extern u32 CurrentElectricC;

extern u8 SetGainData;					//������������

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

extern u16 Huganqibeilv;                             //����������
extern u8 RS485_read_command[8];
extern u8 UP_Exter_data[250];					//�ϱ��ⲿ��������
extern u8 ReadAmmeterData_min;																			//�����ķ�������
extern u32 EST_Q_buff;  // ֮ǰ���ܺ������
extern u8 Exter_number_meter;					//��ӵ������
extern u8 Exter_Data_Reading_flag;			//�ⲿ������ݶ�ȡ��
extern u8 Exter_Data_Read_flag;			//�ⲿ������ݶ�ȡ��־
extern u8 Read_Exter_meter_num;				//�ⲿ������
extern u8 Read_Exter_meter_count;			//��ȡ�ⲿ������
extern u8 Read_Exter_meter_Outtime_flag;	//��ȡ�ⲿ���ʱ��־
extern u8 Read_Exter_meter_Outtime_count;	//��ȡ�ⲿ���ʱ��������

extern u8 first_Read_Exter_meter_flag;			//�ϵ��һ�ζ��ⲿ���ı�־
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
void Read_external_meter(void);							//��ȡ�ⲿ�������
#endif
