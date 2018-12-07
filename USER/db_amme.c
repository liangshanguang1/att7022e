/****************SPI ���ö˿�ģ��**********************************/
#include "math.h"
#include "db_amme.h"
#include "delay.h"
extern u32 Device_ID;
u32 CurrentElectric;				//�ܵ���
//u32 CurrentElectricA;
//u32 CurrentElectricB;
//u32 CurrentElectricC;
DIANBIAOTypeDef dianbiao;
 extern u32 diaobiao [4];
 #define HFCONST   369//((2.592 * ATT_G * ATT_G*10000*INATTV*INATTI)/(EC*UI*VI)) 
 #define P_K   0.002616//2.592*pow(10,10)	/(HFCONST *EC *pow(2,23))  0.01225 //0.965595/HFCONST//0.0025745
u32 VoltageA = 0;   //A ���ѹ��Чֵ
u32 VolA = 0; // A  ���ѹ
u32 PA_Q = 0;  // A����
float CurrentA = 0;	 //A �������Чֵ
float  TempA = 0; //A �๦������ 
unsigned int Up_VolA = 0;
unsigned int Up_CurrentA = 0;
u32 Up_PA_Q = 0;
u32 Up_PA_Power = 0;
u32 Up_QA_Power = 0;
u32 VoltageB = 0;   //B ���ѹ��Чֵ
//u32 PB_Q = 0;  // B����
float CurrentB = 0;	 //B �������Чֵ
float TempB = 0; //B �๦������


unsigned int Up_VolB = 0;
unsigned int Up_CurrentB = 0;
u32 Up_PB_Q = 0;
u32 Up_PB_Power = 0;
u32 Up_QB_Power = 0;


u32 VoltageC = 0;   //C ���ѹ��Чֵ
//u32 PC_Q = 0;  // C����
float CurrentC = 0;	 //C �������Чֵ
float  TempC = 0; //C �๦������
unsigned int Up_VolC = 0;
unsigned int Up_CurrentC = 0;
u32 Up_PC_Q = 0;
u32 Up_PC_Power = 0;
u32 Up_QC_Power = 0;

u16 Huganqibeilv = 1;                             //����������

u32 EST_Q = 0;  // �¼����ĺ������
u32 EST_Q_buff;  // ֮ǰ���ܺ������
u8 SetGainData;
 /*****************SPI GPIO ���ܶ���******************************/
void SPI_GPIO_Config(void)
{		
		//����һ��GPIO_InitTypeDef���͵Ľṹ��//
		GPIO_InitTypeDef GPIO_InitStructure;

		//����GPIOB�͵�����ʱ��//
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB|RCC_APB2Periph_GPIOA, ENABLE); 

		//��������ģʽΪͨ���������//
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;   

		//������������Ϊ50MHz //   
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 

		 		//ѡ��Ҫ���Ƶ�GPIOB����//															   
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15|GPIO_Pin_13;	
		//���ÿ⺯������ʼ��GPIOB//
		GPIO_Init(GPIOB, &GPIO_InitStructure);
		
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;	
		GPIO_Init(GPIOA, &GPIO_InitStructure);

		GPIO_ResetBits(GPIOB, GPIO_Pin_13);	   // 
																	   
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14;

		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;	//��������

		GPIO_Init(GPIOB, &GPIO_InitStructure);
}

/************************* READ_Att7022*************************************************/
u32 READ_Att7022(u8 Address)
{
        u8 i, bTemp;
        u32 dwData;

        SPI_CS_HIGH();
        SPI_CLK_LOW();
        SPI_CS_LOW();    // ����SPI����
        bTemp = 0x80;
        for(i=0; i<8; i++)   //Write the Address
        {
                SPI_CLK_HIGH();
				
                if(Address & bTemp)
				{
                   SPI_DI_HIGH();
				}
                else
                     SPI_DI_LOW();
                bTemp >>= 1;
		delay_us(1);
                SPI_CLK_LOW();
		delay_us(1);
        }
		delay_us(3);
        dwData = 0x00000000;//Read 24bit
        for(i=0; i<24; i++)
        {
                dwData <<= 1;
                SPI_CLK_HIGH();
               // SysTick_Delay_us(3);
                if (RDSPIData)
                {
                      dwData |= 1;
                }
                else
                        ;
		    //	SysTick_Delay_us(1);
                SPI_CLK_LOW();
		delay_us(1);
        }
        SPI_CS_HIGH();	  //�ر�SPI����
		delay_us(1);
        return dwData;
}
/**************************Wr_Dat******************************/

void Write_Att7022(u8 Address, u32 dwData)
{
        u8 i, bTemp;
        u32 dwTemp;

        SPI_CS_HIGH();
        SPI_CLK_LOW();
        SPI_CS_LOW();	 //����SPI����

        Address |= 0x80;//write
        bTemp = 0x80;
        for(i=0; i<8; i++)         //Write the Address
        {
               SPI_CLK_HIGH();
                if(Address & bTemp)
				{
					SPI_DI_HIGH();
				}
                else
                     SPI_DI_LOW();
                bTemp >>= 1;
		delay_us(1);
                SPI_CLK_LOW();
		delay_us(1);
        }
        SysTick_Delay_us(3);

        dwTemp = 0x00800000;
        for(i=0; i<24; i++)  //Write Data
        {
                if(dwData & dwTemp)
                {
                    SPI_DI_HIGH();
                }
                else
                     SPI_DI_LOW();
                SPI_CLK_HIGH();
                SysTick_Delay_us(1);
                SPI_CLK_LOW();
				SysTick_Delay_us(1);
                dwTemp >>= 1;
        }
        
        SPI_CS_HIGH();	    //��������
        SysTick_Delay_us(2);
}
void EMU_init(void)
{
		
		Write_Att7022(0xD3,0x0000);//rest
		SysTick_Delay_us(2);
		Write_Att7022(0xC3,0x000000);  //���У������
		Write_Att7022(0xc9,0x00005A);  //ʹ��У��
		//Wr_Dat(0xC5,0x000000); //�ر�ͬ��
		Write_Att7022(0x01,0xB97E);  //����ģʽ�Ĵ���
		Write_Att7022(0x03,0xF804); //EMUcfg   ��������		 0xF884   �͹����й�
		Write_Att7022(0x02,0x0200);   	//��������1  ��ѹ8 
	
		//   Write_Att7022(0x30,0x0000); // �ر������ж�
		Write_Att7022(0x31, 0x3437); //ģ��ģ��Ĵ���
		Write_Att7022(0x6D, 0xFF00);                //Vrefgain �Ĳ�������ϵ��TCcoffA ��TCcoffB ��TCcoffC(0x6D~0x6F)
		Write_Att7022(0x6E, 0x0DB8);                //Vrefgain �Ĳ�������ϵ��TCcoffA ��TCcoffB ��TCcoffC(0x6D~0x6F)
		Write_Att7022(0x6F, 0xD1DA);                //Vrefgain �Ĳ�������ϵ��TCcoffA ��TCcoffB ��TCcoffC(0x6D~0x6F)
		Write_Att7022(0x1E,HFCONST); // HFconst 3200  109	
	
		Write_Att7022(0x17,(int)(38842));	 	    //A���ѹ����У׼
		Write_Att7022(0x18,(int)(0.99*32768));  //B ���ѹ����У׼
		Write_Att7022(0x19,(int)(0.99*32768));  //C ���ѹ����У׼ 
		Write_Att7022(0x6A,0x00); //A�����Offsetֵ��У׼
		Write_Att7022(0x27, 0);		//  A�����Offsetֵ��У׼
    /******************************************************************************
    * ��������У׼
    *****************************************************************************/		
		Write_Att7022(0x1A,  0); //A��������� У׼		
		Write_Att7022(0x1B,  64359); //B��������� У׼	
		Write_Att7022(0x1C,1.918*32768); //C��������� У׼
						
		Write_Att7022(0x04, 45694); //A�й������� У׼    
		Write_Att7022(0x07,45694); //A�޹������� У׼
		Write_Att7022(0x0A,45694);	//A���ڹ������� У׼

		Write_Att7022(0x05,1.437*32768); //B�й������� У׼	
		Write_Att7022(0x08,1.437*32768); //B�޹������� У׼
		Write_Att7022(0x0B,1.437*32768);	//B���ڹ������� У׼
		
		Write_Att7022(0x06,1.437*32768); //C�й������� У׼
		Write_Att7022(0x09,1.437*32768); //C�޹������� У׼1.378*(pow(2,15))
		Write_Att7022(0x0C,1.437*32768);//C���ڹ������� У׼   1.378*(pow(2,15)


		Write_Att7022(0x35,0x080F); 	  //   I/O
		Write_Att7022(0xC5,0x0002);			///��ͬ��
		Write_Att7022(0xc9,0x000000); 	//Close the write protection
//		Write_Att7022(0xC6,0x00005A) ;		//�򿪼���У׼�Ĵ�������
		Write_Att7022(0xC6,0x0000) ;		//�򿪼������ܼĴ�������
}
void InitAmmeter(void)
{
  	
	SPI_GPIO_Config(); //�ܽų�ʼ��
	EMU_init();    // ����ʼ��
	Device_ID = READ_Att7022(0x00);	  // Device ID 
}
void ReadAmmeterData(void)
{

	u32 Device_ID_buff;
//	u32 TempSSAPwoer[3];

	u32 PA_Power = 0; //A ���й���
	u32 QA_Power = 0; //A ���޹�����
	u32 SA_Power = 0; //A �� ���ڹ���
	float PPA_Power = 0;
	float QQA_Power = 0;
	float SSA_Power = 0;


	u32 PB_Power = 0; //B ���й�����
	u32 QB_Power = 0; //B ���޹�����
	u32 SB_Power = 0; // B�����ڹ���
//	float PPB_Power = 0;
//	float QQB_Power = 0;
//	float SSB_Power = 0;

//	u32 PC_Power = 0; //C ���й�����
//	u32 QC_Power = 0; //C ���޹�����
//	u32 SC_Power = 0; //C�����ڹ���
//	float PPC_Power = 0;
//	float QQC_Power = 0;
//	float SSC_Power = 0;
	/*************A ��********************************/
	
		Device_ID_buff = READ_Att7022(0x00);	  // Device ID 
		SysTick_Delay_us(100);
	
		if (Device_ID_buff != Device_ID)
		{
				EMU_init();    // ����ʼ��
				SysTick_Delay_ms(500);	
		}
		diaobiao [0] =  READ_Att7022(0x0D);  // A���ѹ ,
		#if 0
		printf("\r\n A��δ�����ѹ��Чֵ is  %x \r\n",dianbiao.VolA);
		#endif 
		SysTick_Delay_us(10);
	  diaobiao [1] = READ_Att7022(0x10);   // A�����
	  SysTick_Delay_us(10);
	  PA_Power = READ_Att7022(0x01);  //A���й�����
	  SysTick_Delay_us(10);  
	  QA_Power = READ_Att7022(0x05);  //A���޹�����
	  SysTick_Delay_us(10); 
	  SA_Power = READ_Att7022(0x09);  //A�����ڹ���
	  SysTick_Delay_us(10); 
	  PA_Q = (READ_Att7022(0x35))/3200;  //A�����ڵ���
	  SysTick_Delay_us(10);
		VoltageA = (u32)(dianbiao.VolA/8192)	;   //��ѹ��Чֵ
		#ifdef  DEBUG
		printf("\r\n A���ѹ��Чֵ is  %d \r\n",VoltageA);
		#endif
		CurrentA = ((float)dianbiao.CurrA/8192)/UI_K;   //A�������Чֵ
		#ifdef  DEBUG
		printf("\r\n A�������Чֵ is  %f \r\n",CurrentA);
		printf("\r\n A��Ĵ���������Чֵ is  %d \r\n",CurrA);
		#endif
	  SysTick_Delay_us(100);
//	  TempA = READ_Att7022(0x14); //A�๦������
//	  if(TempA > 8388608)
//	  {
//	  	TempA =(float) 	((16777216 - TempA)/8388608);
//	  }
//	  else 
//		{
//			TempA = (float) (TempA /8388608);
//		}
//  
	  if(PA_Power > 8388608) //         A���й�����
	  {
	     PPA_Power = ((16777216 - (PA_Power))*P_K);//	
		  
		 // PPA_Power = ((PA_Power)- ((pow(2,24))))*P_K;
	  } 
	  else 
		{
	    PPA_Power =  (PA_Power)*P_K;		   //*0.000865
		}
		#ifdef  DEBUG
		printf("\r\n A���й����� is  %f \r\n",PPA_Power);
		#endif
	   if (QA_Power > 8388608)	 //         A���޹�����
	   {
	   		QQA_Power = ((16777216 - QA_Power )*P_K);
	   }
	   else 
		 {
	   		QQA_Power = (QA_Power*P_K);
		 }

	    if (SA_Power> 8388608)	//         A�����ڹ�����
	   {
	   		SSA_Power = ((16777216 - SA_Power )*P_K);
	   }
	   else 
         {
                SSA_Power = (SA_Power*P_K);	
         }
	   	
//	   Up_VolA = (unsigned int)VoltageA;          						//�ϴ���ѹ
//	   Up_CurrentA = (unsigned int)(CurrentA*Huganqibeilv + 0.5);     						//�ϵ���
//	   Up_PA_Power = (unsigned int)(PPA_Power*Huganqibeilv/1000 + 0.5);		       			    //�й�����
//	   Up_QA_Power = (unsigned int)(QQA_Power*Huganqibeilv/1000 + 0.5);			   			    //�޹����� 
////	   CurrentElectricA = PA_Q*Huganqibeilv;							//A�����ڵ����
//	   
//	   TempSSAPwoer[0] = (unsigned int)(SSA_Power*Huganqibeilv/1000 + 0.5);			    	    //���ʺ�����ڹ���
//	   
//	  /***********B��**************************/

//	  VolB =  READ_Att7022(0x0E);  //Vp4 B��ѹ ,
//	  SysTick_Delay_us(100);
	    diaobiao [2] = READ_Att7022(0x11);   //B�������Чֵ
//	  SysTick_Delay_us(100);
//	  PB_Power = READ_Att7022(0x02);  //B���й�����
//	  SysTick_Delay_us(100);  
//	  QB_Power = READ_Att7022(0x06);  //B���޹�����
//	  SysTick_Delay_us(100); 
////	  PB_Q =(READ_Att7022(0x36))/3200;   //B���й�����+PB_Q
//	  SysTick_Delay_us(100);
//	  SB_Power = READ_Att7022(0x0A);
//	  VoltageB = (u8)(VolB/8192)	;   //��ѹ��Чֵ   �д�
	   diaobiao [3]= ((float)dianbiao.CurrB/8192)/UI_K;  //         B�������Чֵ
			#ifdef  DEBUG
			printf("\r\n B�������Чֵ is  %f \r\n",CurrentB);
			printf("\r\n B��Ĵ���������Чֵ is  %d \r\n",CurrB);
			#endif
//	  SysTick_Delay_us(100);
//	  TempB = READ_Att7022(0x15); //A�๦������
//	  if(TempB > 8388608)
//	  {
//	  	TempB =(float)((16777216 - TempB)/8388608);
//	  }
//	  else 
//		{
//		  TempB = (float)(TempB /8388608) ; 
//		}
//					
//	  if(PB_Power > 8388608)    //B���й�����
//	  {
//	     PPB_Power = (float)((16777216- PB_Power  )*P_K);
//	  } 
//	  else 
//		{
//	      PPB_Power = (float) (PB_Power*P_K);
//		}

//	   if (QB_Power> 8388608)	  //B���޹�����
//	   {
//	   		QQB_Power = (float)((16777216 - QB_Power)*P_K);
//	   }
//	   else 
//		 {
//	   		QQB_Power = (float)(QB_Power*P_K);
//		 }

//	   if (SB_Power> 8388608)	 //B�����ڹ�����
//	   {
//	   		SSB_Power = (float)((16777216- SB_Power )*P_K);
//	   }
//	   else 
//		 {
//	   		SSB_Power = (float)(SB_Power*P_K);	 
//		 }
//		   
//	   
//	   Up_VolB = (unsigned int)VoltageB;          						//�ϴ���ѹ
//	   Up_CurrentB = (unsigned int)(CurrentB*Huganqibeilv + 0.5);     						//�ϵ���
//	   Up_PB_Power = (unsigned int)(PPB_Power*Huganqibeilv/1000 + 0.5);		       			    //�й�����
//	   Up_QB_Power = (unsigned int)(QQB_Power*Huganqibeilv/1000 + 0.5);			   			    //�޹����� 
////	   CurrentElectricB = PB_Q*Huganqibeilv;							//A�����ڵ����
//	   
// 	   TempSSAPwoer[1] = (unsigned int)(SSB_Power*Huganqibeilv/1000 + 0.5);			    	    //���ʺ�����ڹ���
//	   
//	  /********************C��*************************/

//	  VolC =  READ_Att7022(0x0F);  //Vp4 C��ѹ ,
//	  SysTick_Delay_us(100);
//	  CurrC = READ_Att7022(0x12);   //Vp5 C��ѹ  Vp1 A��ѹ	  VP3 B
//	  SysTick_Delay_us(100);
//	  PC_Power = READ_Att7022(0x03);  //C���й�����
//	  SysTick_Delay_us(100);  
//	  QC_Power = READ_Att7022(0x07);  //C���޹�����
//	  SysTick_Delay_us(100); 
////	  PC_Q = (READ_Att7022(0x37));   //C�����ڵ���/3200+PC_Q
////	  SysTick_Delay_us(100);
//	  SC_Power = READ_Att7022(0x0B);		//c�����ڹ���	 
//	  VoltageC = (u8)(VolC/8192);   //��ѹ��Чֵ   �д�
//	  CurrentC = ((float)CurrC/8192)/UI_K;  //C �������Ч
//	  SysTick_Delay_us(100);
//	  TempC = READ_Att7022(0x16); //A�๦������
//	  if(TempC > 8388608)
//	  {
//	  	TempC =(float) 	((16777216 - TempC)/8388608);
//	  }
//	  else 
//		{
//			TempC = (float) (TempC /8388608) ; 

//		}
//		
//	  if(PC_Power > 8388608)   //C���й�����
//	  {
//	     PPC_Power = (float)((16777216 - PC_Power )*P_K);
//	  } 
//	  else 
//		{
//	     PPC_Power = (float) (PC_Power*P_K);
//		}

//	   if (QC_Power> 8388608)  //C���޹�����
//	   {
//	   		QQC_Power = (float)((16777216 - QC_Power)*P_K);
//	   }
//	   else 
//		 {
//				QQC_Power = (float)(QC_Power*P_K);
//		 }
//	   		
//	    if (SC_Power> 8388608)	    //C�����ڹ�����
//	   {
//	   		SSC_Power = (float)((16777216 - SC_Power)*P_K);
//	   }
//	   else 
//		 {
//				SSC_Power = (float)(SC_Power*P_K);
//		 }
//		 
//		if (ReadPowerFlag)							//����֮ǰ�������Ա�����ʱ�ϱ�
//		{
//			 EST_Q = READ_Att7022(0x38);
//			
//			 SysTick_Delay_us(100);
//			
//			 EST_Q_buff += ((EST_Q+16)>>5)*Huganqibeilv;						//	 
//			 CurrentElectric = EST_Q_buff/100;			//�ϴ�����	
//		}
//		 
//	   Up_VolC = (unsigned int)VoltageC;          						//�ϴ���ѹ
//	   Up_CurrentC = (unsigned int)(CurrentC*Huganqibeilv + 0.5);     						//�ϵ���
//	   Up_PC_Power = (unsigned int)(PPC_Power*Huganqibeilv/1000 + 0.5);		       			    //�й�����
//	   Up_QC_Power = (unsigned int)(QQC_Power*Huganqibeilv/1000 + 0.5);			   			    //�޹����� 
//	   CurrentElectricC = PC_Q*Huganqibeilv;							//B�����ڵ����
//	   TempSSAPwoer[2] = (unsigned int)(SSC_Power*Huganqibeilv/1000 + 0.5);			    	    //���ʺ�����ڹ���
//	   CurrentElectric = CurrentElectricC+CurrentElectricB+CurrentElectricA; //�ܵ���
//	   
//	 while(1)
//{	
//		printf("VolA=%d\r\n",VolA);	
//		SysTick_Delay_ms(500);
//    printf("VolB=%d\r\n",VolB);	
//}	 

}



