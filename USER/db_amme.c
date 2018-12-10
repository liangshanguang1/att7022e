/****************SPI ���ö˿�ģ��**********************************/
#include "math.h"
#include "db_amme.h"
#include "delay.h"
#define HFCONST   369//((2.592 * ATT_G * ATT_G*10000*INATTV*INATTI)/(EC*UI*VI)) 
#define P_K   0.002616//2.592*pow(10,10)	/(HFCONST *EC *pow(2,23))  0.01225 //0.965595/HFCONST//0.0025745
u16 Huganqibeilv = 1;                             //����������
u32 EST_Q = 0;  // �¼����ĺ������
u32 EST_Q_buff;  // ֮ǰ���ܺ������
u32 Device_ID_buff ;

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
                if(Address & bTemp)  SPI_DI_HIGH();
                else                 SPI_DI_LOW();
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
                if (RDSPIData) dwData |= 1;
                else  ;
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
                if(Address & bTemp) SPI_DI_HIGH();
                else   SPI_DI_LOW();
                bTemp >>= 1;
		delay_us(1);
                SPI_CLK_LOW();
		delay_us(1);
        }
        delay_us(3);
        dwTemp = 0x00800000;
        for(i=0; i<24; i++)  //Write Data
        {
                if(dwData & dwTemp)SPI_DI_HIGH();
                else  SPI_DI_LOW();
                SPI_CLK_HIGH();
                delay_us(1);
                SPI_CLK_LOW();
		delay_us(1);
                dwTemp >>= 1;
        }
        SPI_CS_HIGH();	    //��������
        delay_us(2);
}
void EMU_init(void)
{
	Write_Att7022(0xD3,0x0000);//rest
	delay_us(2);
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
	Device_ID_buff = READ_Att7022(0x00);	  // Device ID 
}
void ReadAmmeterData(void)
{
	u32 Device_ID_buff;
	Device_ID_buff = READ_Att7022(0x00);	  // Device ID 
	delay_us(100);
}



