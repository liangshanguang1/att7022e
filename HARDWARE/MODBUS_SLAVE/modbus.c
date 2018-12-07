#include "modbus.h"
#include "led.h"
#include "key.h"
#include "delay.h"
#include "bsp_internal_flash.h" 
#include "moter.h" 
///////////////////////////////////////////////////////////
		u32 	RS485_Addr=00;						//�ӻ���ַ ȫ�ֱ���
static 	u8 		RS485_RX_BUFF[1024];			//���ջ�����2048�ֽ�
static 	u16 	RS485_RX_CNT=0;				//���ռ�����
static 	u8 		RS485_FrameFlag=0;			//֡�������
static 	u8 		RS485_TX_BUFF[2048];			//���ͻ�����
		u8 		KeyFlag = 0;						//ȫ�ֱ���������ʾ ����MODBUS��ַ�ı�־���ڰ���������ʹ��
		u32  	i=1,y=1,u=1,t=0;   				//�����������ã�Command1��ȡ�ӻ���·������״̬
vu32 *Modbus_InputIO[100];//���뿪�����Ĵ���ָ��(����ʹ�õ���λ������)
vu32 *Modbus_OutputIO[100];//����������Ĵ���ָ��(����ʹ�õ���λ������)
u16 *Modbus_HoldReg[1000];//���ּĴ���ָ��
u16 Message [28];         //command 3������Ҫ��ȡ�ļĴ��� ������Ϣ����������

void Modbus_RegMap(void)
{
        Modbus_OutputIO[0]=(vu32*)&i;//LED0
        Modbus_OutputIO[1]=(vu32*)&y;//LED1
        Modbus_OutputIO[2]=(vu32*)&u;//LED0
        Modbus_OutputIO[3]=(vu32*)&t;//LED1
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//CRCУ�� �Լ�������ӵ�

const u8 auchCRCHi[] = { 
0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 
0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 
0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1,0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 
0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1,0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 
0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 
0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1,0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 
0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 
0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 
0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 
0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 
0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 
0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1,0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 
0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,0x80, 0x41, 0x00, 0xC1, 0x81, 0x40} ; 


const u8 auchCRCLo[] = { 
0x00, 0xC0, 0xC1, 0x01, 0xC3, 0x03, 0x02, 0xC2, 0xC6, 0x06,0x07, 0xC7, 0x05, 0xC5, 0xC4, 0x04, 0xCC, 0x0C, 0x0D, 0xCD, 
0x0F, 0xCF, 0xCE, 0x0E, 0x0A, 0xCA, 0xCB, 0x0B, 0xC9, 0x09,0x08, 0xC8, 0xD8, 0x18, 0x19, 0xD9, 0x1B, 0xDB, 0xDA, 0x1A, 
0x1E, 0xDE, 0xDF, 0x1F, 0xDD, 0x1D, 0x1C, 0xDC, 0x14, 0xD4,0xD5, 0x15, 0xD7, 0x17, 0x16, 0xD6, 0xD2, 0x12, 0x13, 0xD3, 
0x11, 0xD1, 0xD0, 0x10, 0xF0, 0x30, 0x31, 0xF1, 0x33, 0xF3,0xF2, 0x32, 0x36, 0xF6, 0xF7, 0x37, 0xF5, 0x35, 0x34, 0xF4, 
0x3C, 0xFC, 0xFD, 0x3D, 0xFF, 0x3F, 0x3E, 0xFE, 0xFA, 0x3A,0x3B, 0xFB, 0x39, 0xF9, 0xF8, 0x38, 0x28, 0xE8, 0xE9, 0x29, 
0xEB, 0x2B, 0x2A, 0xEA, 0xEE, 0x2E, 0x2F, 0xEF, 0x2D, 0xED,0xEC, 0x2C, 0xE4, 0x24, 0x25, 0xE5, 0x27, 0xE7, 0xE6, 0x26, 
0x22, 0xE2, 0xE3, 0x23, 0xE1, 0x21, 0x20, 0xE0, 0xA0, 0x60,0x61, 0xA1, 0x63, 0xA3, 0xA2, 0x62, 0x66, 0xA6, 0xA7, 0x67, 
0xA5, 0x65, 0x64, 0xA4, 0x6C, 0xAC, 0xAD, 0x6D, 0xAF, 0x6F,0x6E, 0xAE, 0xAA, 0x6A, 0x6B, 0xAB, 0x69, 0xA9, 0xA8, 0x68, 
0x78, 0xB8, 0xB9, 0x79, 0xBB, 0x7B, 0x7A, 0xBA, 0xBE, 0x7E,0x7F, 0xBF, 0x7D, 0xBD, 0xBC, 0x7C, 0xB4, 0x74, 0x75, 0xB5, 
0x77, 0xB7, 0xB6, 0x76, 0x72, 0xB2, 0xB3, 0x73, 0xB1, 0x71,0x70, 0xB0, 0x50, 0x90, 0x91, 0x51, 0x93, 0x53, 0x52, 0x92, 
0x96, 0x56, 0x57, 0x97, 0x55, 0x95, 0x94, 0x54, 0x9C, 0x5C,0x5D, 0x9D, 0x5F, 0x9F, 0x9E, 0x5E, 0x5A, 0x9A, 0x9B, 0x5B, 
0x99, 0x59, 0x58, 0x98, 0x88, 0x48, 0x49, 0x89, 0x4B, 0x8B,0x8A, 0x4A, 0x4E, 0x8E, 0x8F, 0x4F, 0x8D, 0x4D, 0x4C, 0x8C, 
0x44, 0x84, 0x85, 0x45, 0x87, 0x47, 0x46, 0x86, 0x82, 0x42,0x43, 0x83, 0x41, 0x81, 0x80, 0x40} ;


u16 CRC_Compute(u8 *puchMsg, u16 usDataLen) 
{ 
	u8 uchCRCHi = 0xFF ; 
	u8 uchCRCLo = 0xFF ; 
	u32 uIndex ; 
	while (usDataLen--) 
	{ 
		uIndex = uchCRCHi ^ *puchMsg++ ; 
		uchCRCHi = uchCRCLo ^ auchCRCHi[uIndex] ; 
		uchCRCLo = auchCRCLo[uIndex] ; 
	} 
	return ((uchCRCHi<< 8)  | (uchCRCLo)) ; 
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//��ʼ��USART2
void RS485_Init(void)
{
		u32 RS485_Baudrate=9600;//ͨѶ������
		u8 RS485_Parity=0;//0��У�飻1��У�飻2żУ��
		GPIO_InitTypeDef GPIO_InitStructure;
        USART_InitTypeDef USART_InitStructure;
        NVIC_InitTypeDef NVIC_InitStructure;
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOD,ENABLE);
        RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2,ENABLE);
        
        GPIO_InitStructure.GPIO_Pin=GPIO_Pin_2;//PA2��TX�������������
        GPIO_InitStructure.GPIO_Mode=GPIO_Mode_AF_PP;
        GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
        GPIO_Init(GPIOA,&GPIO_InitStructure);
        GPIO_SetBits(GPIOA,GPIO_Pin_2);//Ĭ�ϸߵ�ƽ
        
        GPIO_InitStructure.GPIO_Pin=GPIO_Pin_3;//PA3��RX����������
        GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IN_FLOATING;   //�޸�ԭGPIO_Mode_IPU������������->GPIO_Mode_IN_FLOATING(��������)/////////////////////////////////////////////
        GPIO_Init(GPIOA,&GPIO_InitStructure);
        
        GPIO_InitStructure.GPIO_Pin=GPIO_Pin_0;//�޸�PG9��RE/DE��ͨ���������->PD7��RE/DE��ͨ���������//////////////////////////////////////////////////////////////////////
        GPIO_InitStructure.GPIO_Mode=GPIO_Mode_Out_PP;
        GPIO_Init(GPIOA,&GPIO_InitStructure);
        GPIO_ResetBits(GPIOA,GPIO_Pin_0);//Ĭ�Ͻ���״̬
        
        USART_DeInit(USART2);//��λ����2
        USART_InitStructure.USART_BaudRate=RS485_Baudrate;
        USART_InitStructure.USART_HardwareFlowControl=USART_HardwareFlowControl_None;
        USART_InitStructure.USART_WordLength=USART_WordLength_8b;
        USART_InitStructure.USART_StopBits=USART_StopBits_1;
        USART_InitStructure.USART_Mode=USART_Mode_Rx|USART_Mode_Tx;//�շ�ģʽ
        switch(RS485_Parity)
        {
                case 0:USART_InitStructure.USART_Parity=USART_Parity_No;break;//��У��
                case 1:USART_InitStructure.USART_Parity=USART_Parity_Odd;break;//��У��
                case 2:USART_InitStructure.USART_Parity=USART_Parity_Even;break;//żУ��
        }
        USART_Init(USART2,&USART_InitStructure);
        
        USART_ClearITPendingBit(USART2,USART_IT_RXNE);
        USART_ITConfig(USART2,USART_IT_RXNE,ENABLE);//ʹ�ܴ���2�����ж�
        
        NVIC_InitStructure.NVIC_IRQChannel=USART2_IRQn;
        NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=2;
        NVIC_InitStructure.NVIC_IRQChannelSubPriority=1;
        NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;
        NVIC_Init(&NVIC_InitStructure);
        
        USART_Cmd(USART2,ENABLE);//ʹ�ܴ���2
        RS485_RX_EN();		//Ĭ��Ϊ����ģʽ
        Timer3_Init(); 		//������ʱ����ʼ��
        Timer2_Init();    	// �������modbus �Ƿ�������
        Modbus_RegMap();	//Modbus�Ĵ���ӳ��
}

//��ʱ��7��ʼ��
void Timer2_Init(void)
{
		u16 RS485_Frame_Distance=4;//����֡��С�����ms),������ʱ������Ϊ����һ֡
        TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
        NVIC_InitTypeDef NVIC_InitStructure;
        RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE); //TIM2ʱ��ʹ�� 
        //TIM7��ʼ������
        TIM_TimeBaseStructure.TIM_Period = RS485_Frame_Distance*10; //��������һ�������¼�װ�����Զ���װ�ؼĴ������ڵ�ֵ
        TIM_TimeBaseStructure.TIM_Prescaler =7200; //����������ΪTIMxʱ��Ƶ�ʳ�����Ԥ��Ƶֵ ���ü���Ƶ��Ϊ10kHz
        TIM_TimeBaseStructure.TIM_ClockDivision = 0; //����ʱ�ӷָ�:TDTS = Tck_tim
        TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM���ϼ���ģʽ
        TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure); //����TIM_TimeBaseInitStruct��ָ���Ĳ�����ʼ��TIMx��ʱ�������λ
        TIM_Cmd(TIM2,DISABLE);//ֹͣ��ʱ��
        TIM_ITConfig( TIM2, TIM_IT_Update, ENABLE );//TIM2 ��������ж�
		TIM_ClearFlag(TIM2,TIM_FLAG_Update);
        //TIM7�жϷ�������
        NVIC_InitStructure.NVIC_IRQChannel =TIM2_IRQn;  //TIM2�ж�
        NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;  //��ռ���ȼ�2��
        NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;  //�����ȼ�3��
        NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQͨ����ʹ��
        NVIC_Init(&NVIC_InitStructure);  //����NVIC_InitStruct��ָ���Ĳ�����ʼ������NVIC�Ĵ���                                                                  
}
/*TIme3��ʱ�� ��������1ms�İ���ɨ��*/
void Timer3_Init(void)
{
        TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
        NVIC_InitTypeDef NVIC_InitStructure;
        RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE); //TIM3ʱ��ʹ�� 
        //TIM3��ʼ������
        TIM_TimeBaseStructure.TIM_Period = 10-1; //��������һ�������¼�װ�����Զ���װ�ؼĴ������ڵ�ֵ
        TIM_TimeBaseStructure.TIM_Prescaler =7200-1; //����������ΪTIMxʱ��Ƶ�ʳ�����Ԥ��Ƶֵ ���ü���Ƶ��Ϊ10kHz
        TIM_TimeBaseStructure.TIM_ClockDivision = 0; //����ʱ�ӷָ�:TDTS = Tck_tim
        TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM���ϼ���ģʽ
        TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure); //����TIM_TimeBaseInitStruct��ָ���Ĳ�����ʼ��TIMx��ʱ�������λ
        TIM_ITConfig( TIM3, TIM_IT_Update, ENABLE );//TIM3 ��������ж�
		TIM_ClearFlag(TIM3,TIM_FLAG_Update);
        //TIM7�жϷ�������
        NVIC_InitStructure.NVIC_IRQChannel =TIM3_IRQn;  //TIM2�ж�
        NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;  //��ռ���ȼ�2��
        NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;  //�����ȼ�2��
        NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQͨ����ʹ��
        NVIC_Init(&NVIC_InitStructure);  //����NVIC_InitStruct��ָ���Ĳ�����ʼ������NVIC�Ĵ���                                                                  
        TIM_Cmd(TIM3,ENABLE);//��ʼ��ʱ��
}
//////////////////////////////////////////////////////////////////////////////
//����n���ֽ�����
//buff:�������׵�ַ
//len�����͵��ֽ���
void RS485_SendData(u8 *buff,u8 len)
{ 
        RS485_TX_EN();//�л�Ϊ����ģʽ
        while(len--)
        {
                while(USART_GetFlagStatus(USART2,USART_FLAG_TXE)==RESET);//�ȴ�������Ϊ��
                USART_SendData(USART2,*(buff++));
        }
        while(USART_GetFlagStatus(USART2,USART_FLAG_TC)==RESET);//�ȴ��������
}


void USART2_IRQHandler(void)//����2�жϷ������
{
        u8 res;
        u8 err;
        if(USART_GetITStatus(USART2,USART_IT_RXNE)!=RESET)
        {
			if(USART_GetFlagStatus(USART2,USART_FLAG_NE|USART_FLAG_FE|USART_FLAG_PE)) err=1;//��⵽������֡�����У�����
			else err=0;
			res=USART_ReceiveData(USART2); //�����յ����ֽڣ�ͬʱ��ر�־�Զ����
			if((RS485_RX_CNT<2047)&&(err==0))
			{
				RS485_RX_BUFF[RS485_RX_CNT]=res;
				RS485_RX_CNT++;
				TIM_ClearITPendingBit(TIM2,TIM_IT_Update);//�����ʱ������ж�
				TIM_SetCounter(TIM2,0);//�����յ�һ���µ��ֽڣ�����ʱ��7��λΪ0�����¼�ʱ���൱��ι����
				TIM_Cmd(TIM2,ENABLE);//��ʼ��ʱ
			}
		}
}

///////////////////////////////////////////////////////////////////////////////////////
//�ö�ʱ��2�жϽ��տ���ʱ�䣬������ʱ�����ָ��ʱ�䣬��Ϊһ֡����
//��ʱ��7�жϷ������         
void TIM2_IRQHandler(void)
{                                                                   
        if(TIM_GetITStatus(TIM2,TIM_IT_Update)!=RESET)
        {
                TIM_ClearITPendingBit(TIM2,TIM_IT_Update);//����жϱ�־
                TIM_Cmd(TIM2,DISABLE);//ֹͣ��ʱ��
                if(RS485_RX_BUFF[0]==0xFF) RS485_RX_CNT=0;	          //����������õ�ַģʽ����ôcntΪ0 ��cntΪ0��Ҫ��Ϊ��
				RS485_FrameFlag=1;//��λ֡�������
        }
}
/////////////////////////////////////////////////////////////////////////////////////
//RS485����������ڴ�����յ�������(������������ѭ������)
u16 startRegAddr;
u16 RegNum;
u16 calCRC;
void RS485_Service(void)
{
        u16 recCRC;
        if(RS485_FrameFlag==1)
        {
                if(RS485_RX_BUFF[0]==RS485_Addr)
                {
                  if((RS485_RX_BUFF[1]==01)||(RS485_RX_BUFF[1]==02)||(RS485_RX_BUFF[1]==05)||(RS485_RX_BUFF[1]==03))//��������ȷ
                  {
										
                                startRegAddr=(((u16)RS485_RX_BUFF[2])<<8)|RS485_RX_BUFF[3];//��ȡ�Ĵ�����ʼ��ַ
                                if(startRegAddr<1000)//�Ĵ�����ַ�ڷ�Χ��
                                {
                                        calCRC=CRC_Compute(RS485_RX_BUFF,RS485_RX_CNT-2);//�������������ݵ�CRC
                                        recCRC=RS485_RX_BUFF[RS485_RX_CNT-1]|(((u16)RS485_RX_BUFF[RS485_RX_CNT-2])<<8);//���յ���CRC(���ֽ���ǰ�����ֽ��ں�)
                                        if(calCRC==recCRC)//CRCУ����ȷ
                                        {																
                                                /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
																					      switch(RS485_RX_BUFF[1])//���ݲ�ͬ�Ĺ�������д���
                                                {
                                                        case 2://�����뿪����
                                                        {
                                                                Modbus_02_Solve();
                                                                break;
                                                        }
                                                        case 1://�����������
                                                        {
                                                                Modbus_01_Solve();
                                                                break;
                                                        }
                                                        case 5://д�������������
                                                        {
                                                                Modbus_05_Solve();
                                                                break;
                                                        }
                                                                
                                                        case 15://д������������
                                                        {
                                                                Modbus_15_Solve();
                                                                break;
                                                        }
                                                                
                                                        case 03: //������Ĵ���
                                                        {																
                                                                Modbus_03_Solve();
                                                                break;
                                                        }
                                                                
                                                        case 06: //д�����Ĵ���
                                                        {
                                                                Modbus_06_Solve();
                                                                break;
                                                        }
                                                                
                                                        case 16: //д����Ĵ���
                                                        {
                                                                Modbus_16_Solve();
                                                                break;
                                                        }
                                                        case 0xff: 
                                                        {
                                                                Modbus_FF_Solve();
                                                                break;
                                                        }                               
                                                }
                                                //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
                                        }
                                        else//CRCУ�����
                                        {

                                                RS485_TX_BUFF[0]=RS485_RX_BUFF[0];
                                                RS485_TX_BUFF[1]=RS485_RX_BUFF[1]|0x80;
                                                RS485_TX_BUFF[2]=0x04; //�쳣��
                                                RS485_SendData(RS485_TX_BUFF,3);
                                        }        
                                }                                                
                        }
                        else//���������
                        {
                                RS485_TX_BUFF[0]=RS485_RX_BUFF[0];
                                RS485_TX_BUFF[1]=RS485_RX_BUFF[1]|0x80;
                                RS485_TX_BUFF[2]=0x01; //�쳣��
                                RS485_SendData(RS485_TX_BUFF,3);
                        }
          }
                                
                else if (RS485_RX_BUFF[0]==0xFF)								//����������MODBUS �Զ����õ�ַ
								{
										while(!KeyFlag);						//�ȴ���һ���ӻ����������ź�
										KeyFlag=0;  							//ȷ��ֻ����һ��	
										RS485_Addr = RS485_RX_BUFF[1];			//����MODBUS�ĵ�ַ
										delay_ms(10000);		
										Modbus_FF_Solve();						//����Ӧ���ź�	
										RS485_FrameFlag=0;                      //���ܱ�־��0��Ϊ�������½�����׼���������ȫ�ֱ��������ж����ı�
										RS485_RX_CNT=0;                         //�����cnt���ܸ���Ӧ����0 ����֤������ܵ�ʱ�򸲸�֮ǰ���ܵ�����
										while(!RS485_FrameFlag)	;				//���½���һ����ַ��һ�������ź�
										if(RS485_Addr !=RS485_RX_BUFF[1] )      //������ܵĵ�ַ�����ǲ�ͬ��
										{                                      
											RS485_FrameFlag=0;                  //������0��Ϊ�˽��������ķ�FF�����ź�
											GPIO_SetBits(GPIOB,GPIO_Pin_0);     //
											delay_ms(1000);						//	
										 	GPIO_ResetBits(GPIOB,GPIO_Pin_0);     //������������⣬û������B0   
											GPIO_ResetBits(GPIOB,GPIO_Pin_14);    //��������Ϊ�͵�ƽ����LED
											Flash_W(RS485_Addr);
										}											
								}
								RS485_FrameFlag=0;//��λ֡������־
                RS485_RX_CNT=0;//���ռ���������
                RS485_RX_EN();//��������ģʽ 
												
        }                
}

//Modbus������02�������/////////////////////////////////////////////////////��������֤OK -----����������PE4 PE3 PE2 PA0 ��ʼ�������ſ���OK    KEY_Init();
//�����뿪����
void Modbus_02_Solve(void)
{
        u16 ByteNum;
        u16 i;
        RegNum= (((u16)RS485_RX_BUFF[4])<<8)|RS485_RX_BUFF[5];//��ȡ�Ĵ�������
        if((startRegAddr+RegNum)<100)//�Ĵ�����ַ+�����ڷ�Χ��
        {
                RS485_TX_BUFF[0]=RS485_RX_BUFF[0];
                RS485_TX_BUFF[1]=RS485_RX_BUFF[1];
                ByteNum=RegNum/8;//�ֽ���
                if(RegNum%8) ByteNum+=1;//���λ���������������ֽ���+1
                RS485_TX_BUFF[2]=ByteNum;//����Ҫ��ȡ���ֽ���
                for(i=0;i<RegNum;i++)
                {
                        if(i%8==0) RS485_TX_BUFF[3+i/8]=0x00;
                        RS485_TX_BUFF[3+i/8]>>=1;//��λ�ȷ���
                        RS485_TX_BUFF[3+i/8]|=((*Modbus_InputIO[startRegAddr+i])<<7)&0x80;
                        if(i==RegNum-1)//���͵����һ��λ��
                        {
                                if(RegNum%8) RS485_TX_BUFF[3+i/8]>>=8-(RegNum%8);//������һ���ֽڻ�����������ʣ��MSB���0
                        }
                }
                calCRC=CRC_Compute(RS485_TX_BUFF,ByteNum+3);
                RS485_TX_BUFF[ByteNum+3]=(calCRC>>8)&0xFF;
                RS485_TX_BUFF[ByteNum+4]=(calCRC)&0xFF;
                RS485_SendData(RS485_TX_BUFF,ByteNum+5);
        }
        else//�Ĵ�����ַ+����������Χ
        {
                RS485_TX_BUFF[0]=RS485_RX_BUFF[0];
                RS485_TX_BUFF[1]=RS485_RX_BUFF[1]|0x80;
                RS485_TX_BUFF[2]=0x02; //�쳣��
                RS485_SendData(RS485_TX_BUFF,3);
        }
}

//Modbus������01������� ///////////////////////////////////////////////////////////��������֤OK
//�����������
void Modbus_01_Solve(void)				//���ص���һ���ֽڣ�һ���ֽڵ����һλ����ֺ�״̬
{
        u16 ByteNum;
        u16 i;
        RegNum= (((u16)RS485_RX_BUFF[4])<<8)|RS485_RX_BUFF[5];//��ȡ�Ĵ�������
        if((startRegAddr+RegNum)<100)//�Ĵ�����ַ+�����ڷ�Χ��
        {
                RS485_TX_BUFF[0]=RS485_RX_BUFF[0];
                RS485_TX_BUFF[1]=RS485_RX_BUFF[1];
                ByteNum=RegNum/8;//�ֽ���
                if(RegNum%8) ByteNum+=1;//���λ���������������ֽ���+1
                RS485_TX_BUFF[2]=ByteNum;//����Ҫ��ȡ���ֽ���
                for(i=0;i<RegNum;i++)
                {
                        if(i%8==0) RS485_TX_BUFF[3+i/8]=0x00;
                        RS485_TX_BUFF[3+i/8]>>=1;//��λ�ȷ���
                        RS485_TX_BUFF[3+i/8]|=((*Modbus_OutputIO[startRegAddr+i])<<7)&0x80;
                        if(i==RegNum-1)//���͵����һ��λ��
                        {
                                if(RegNum%8) RS485_TX_BUFF[3+i/8]>>=8-(RegNum%8);//������һ���ֽڻ�����������ʣ��MSB���0
                        }
                }
                calCRC=CRC_Compute(RS485_TX_BUFF,ByteNum+3);
                RS485_TX_BUFF[ByteNum+3]=(calCRC>>8)&0xFF;
                RS485_TX_BUFF[ByteNum+4]=(calCRC)&0xFF;
                RS485_SendData(RS485_TX_BUFF,ByteNum+5);
        }
        else//�Ĵ�����ַ+����������Χ
        {
                RS485_TX_BUFF[0]=RS485_RX_BUFF[0];
                RS485_TX_BUFF[1]=RS485_RX_BUFF[1]|0x80;
                RS485_TX_BUFF[2]=0x02; //�쳣��
                RS485_SendData(RS485_TX_BUFF,3);
        }
}

//Modbus������05�������   ///////////////////////////////////////////////////////��������֤OK
//д�������������
void Modbus_05_Solve(void)
{
        if(startRegAddr<100)//�Ĵ�����ַ�ڷ�Χ��
        {
                if((RS485_RX_BUFF[4]==0xFF)||(RS485_RX_BUFF[5]==0x00)) //����5�������ôӻ��Ŀ���״̬���������,���۷ֺ��źŶ����õ��ת��
				{
					GPIO_SetBits(LED2_GPIO_PORT, LED2_GPIO_PIN);		
					LED1_TOGGLE;
				}		
                
                RS485_TX_BUFF[0]=RS485_RX_BUFF[0];
                RS485_TX_BUFF[1]=RS485_RX_BUFF[1];
                RS485_TX_BUFF[2]=RS485_RX_BUFF[2];
                RS485_TX_BUFF[3]=RS485_RX_BUFF[3];
                RS485_TX_BUFF[4]=RS485_RX_BUFF[4];
                RS485_TX_BUFF[5]=RS485_RX_BUFF[5];
                
                calCRC=CRC_Compute(RS485_TX_BUFF,6);
                RS485_TX_BUFF[6]=(calCRC>>8)&0xFF;
                RS485_TX_BUFF[7]=(calCRC)&0xFF;
                RS485_SendData(RS485_TX_BUFF,8);
        }
        else//�Ĵ�����ַ������Χ
        {
                RS485_TX_BUFF[0]=RS485_RX_BUFF[0];
                RS485_TX_BUFF[1]=RS485_RX_BUFF[1]|0x80;
                RS485_TX_BUFF[2]=0x02; //�쳣��
                RS485_SendData(RS485_TX_BUFF,3);
        }
}

//Modbus������15�������   //////////////////////////////////////////////////////��������֤OK
//д������������
void Modbus_15_Solve(void)
{
        u16 i;
        RegNum=(((u16)RS485_RX_BUFF[4])<<8)|RS485_RX_BUFF[5];//��ȡ�Ĵ�������
        if((startRegAddr+RegNum)<100)//�Ĵ�����ַ+�����ڷ�Χ��
        {        
                for(i=0;i<RegNum;i++)
                {
                        if(RS485_RX_BUFF[7+i/8]&0x01) *Modbus_OutputIO[startRegAddr+i]=0x01;
                        else *Modbus_OutputIO[startRegAddr+i]=0x00;
                        RS485_RX_BUFF[7+i/8]>>=1;//�ӵ�λ��ʼ
                }
                
                RS485_TX_BUFF[0]=RS485_RX_BUFF[0];
                RS485_TX_BUFF[1]=RS485_RX_BUFF[1];
                RS485_TX_BUFF[2]=RS485_RX_BUFF[2];
                RS485_TX_BUFF[3]=RS485_RX_BUFF[3];
                RS485_TX_BUFF[4]=RS485_RX_BUFF[4];
                RS485_TX_BUFF[5]=RS485_RX_BUFF[5];
                calCRC=CRC_Compute(RS485_TX_BUFF,6);
                RS485_TX_BUFF[6]=(calCRC>>8)&0xFF;
                RS485_TX_BUFF[7]=(calCRC)&0xFF;
                RS485_SendData(RS485_TX_BUFF,8);
        }
        else//�Ĵ�����ַ+����������Χ
        {
                RS485_TX_BUFF[0]=RS485_RX_BUFF[0];
                RS485_TX_BUFF[1]=RS485_RX_BUFF[1]|0x80;
                RS485_TX_BUFF[2]=0x02; //�쳣��
                RS485_SendData(RS485_TX_BUFF,3);
        }
}

//Modbus������03�������///////////////////////////////////////////////////////////////////////////////////////����֤����OK
//�����ּĴ���
void Modbus_03_Solve(void)
{
        u8 i;
        RegNum= (((u16)RS485_RX_BUFF[4])<<8)|RS485_RX_BUFF[5];//��ȡ�Ĵ�������
        if((startRegAddr+RegNum)<1000)//�Ĵ�����ַ+�����ڷ�Χ��
        {
                RS485_TX_BUFF[0]=RS485_RX_BUFF[0];
                RS485_TX_BUFF[1]=RS485_RX_BUFF[1];
                RS485_TX_BUFF[2]=RegNum*2;
                for(i=0;i<RegNum;i++)
                {
                        RS485_TX_BUFF[3+i*2]=(Message [startRegAddr+i]>>8)&0xFF;//           /////////�ȷ��͸��ֽ�--�ڷ��͵��ֽ�
                        RS485_TX_BUFF[4+i*2]=(Message [startRegAddr+i])&0xFF; //
                }
                calCRC=CRC_Compute(RS485_TX_BUFF,RegNum*2+3);
                RS485_TX_BUFF[RegNum*2+3]=(calCRC>>8)&0xFF;         //CRC�ߵ�λ������  // �ȸߺ��
                RS485_TX_BUFF[RegNum*2+4]=(calCRC)&0xFF;
                RS485_SendData(RS485_TX_BUFF,RegNum*2+5);
        }
        else//�Ĵ�����ַ+����������Χ
        {
                RS485_TX_BUFF[0]=RS485_RX_BUFF[0];
                RS485_TX_BUFF[1]=RS485_RX_BUFF[1]|0x80;
                RS485_TX_BUFF[2]=0x02; //�쳣��
                RS485_SendData(RS485_TX_BUFF,3);
        }
}


//Modbus������06�������   //////////////////////////////////////////////////////////////////////////////////����֤����OK
//д�������ּĴ���
void Modbus_06_Solve(void)
{
        *Modbus_HoldReg[startRegAddr]=RS485_RX_BUFF[4]<<8;//���ֽ���ǰ                    ////////�޸�Ϊ���ֽ���ǰ�����ֽ��ں�
        *Modbus_HoldReg[startRegAddr]|=((u16)RS485_RX_BUFF[5]);//���ֽ��ں�
        
        RS485_TX_BUFF[0]=RS485_RX_BUFF[0];
        RS485_TX_BUFF[1]=RS485_RX_BUFF[1];
        RS485_TX_BUFF[2]=RS485_RX_BUFF[2];
        RS485_TX_BUFF[3]=RS485_RX_BUFF[3];
        RS485_TX_BUFF[4]=RS485_RX_BUFF[4];
        RS485_TX_BUFF[5]=RS485_RX_BUFF[5];
        
        calCRC=CRC_Compute(RS485_TX_BUFF,6);
        RS485_TX_BUFF[6]=(calCRC>>8)&0xFF;
        RS485_TX_BUFF[7]=(calCRC)&0xFF;
        RS485_SendData(RS485_TX_BUFF,8);
}

//Modbus������16������� /////////////////////////////////////////////////////////////////////////////////////////////////����֤����OK
//д������ּĴ���
void Modbus_16_Solve(void)
{
        u8 i;
        RegNum= (((u16)RS485_RX_BUFF[4])<<8)|((RS485_RX_BUFF[5]));//��ȡ�Ĵ�������
        if((startRegAddr+RegNum)<1000)//�Ĵ�����ַ+�����ڷ�Χ��
        {
                for(i=0;i<RegNum;i++)
                {
                        *Modbus_HoldReg[startRegAddr+i]=RS485_RX_BUFF[7+i*2]; //���ֽ���ǰ                 /////// ���ֽ���ǰ�����ֽ��ں�����
                        *Modbus_HoldReg[startRegAddr+i]|=((u16)RS485_RX_BUFF[8+i*2])<<8; //���ֽ��ں�
                }
                
                RS485_TX_BUFF[0]=RS485_RX_BUFF[0];
                RS485_TX_BUFF[1]=RS485_RX_BUFF[1];
                RS485_TX_BUFF[2]=RS485_RX_BUFF[2];
                RS485_TX_BUFF[3]=RS485_RX_BUFF[3];
                RS485_TX_BUFF[4]=RS485_RX_BUFF[4];
                RS485_TX_BUFF[5]=RS485_RX_BUFF[5];
                
                calCRC=CRC_Compute(RS485_TX_BUFF,6);
                RS485_TX_BUFF[6]=(calCRC>>8)&0xFF;
                RS485_TX_BUFF[7]=(calCRC)&0xFF;
                RS485_SendData(RS485_TX_BUFF,8);
        }
        else//�Ĵ�����ַ+����������Χ
        {
                RS485_TX_BUFF[0]=RS485_RX_BUFF[0];
                RS485_TX_BUFF[1]=RS485_RX_BUFF[1]|0x80;
                RS485_TX_BUFF[2]=0x02; //�쳣��
                RS485_SendData(RS485_TX_BUFF,3);
        }
}
void Modbus_FF_Solve(void)
{ 
		RS485_TX_BUFF[0]=RS485_RX_BUFF [0];
		RS485_TX_BUFF[1]=RS485_RX_BUFF [1];
		RS485_TX_BUFF[2]=RS485_RX_BUFF [2];
		RS485_TX_BUFF[3]=RS485_RX_BUFF [3];
		RS485_TX_BUFF[4]=RS485_RX_BUFF [4];
		RS485_TX_BUFF[5]=RS485_RX_BUFF [5];
		RS485_TX_BUFF[5]=RS485_RX_BUFF [6];
		RS485_SendData(RS485_TX_BUFF,7);
		RS485_RX_EN();//��������ģʽ
}
 

void GetLockCode(void)      //�õ���·��ID ���洢��Command3�Ĵ�����
{
	u32  CpuID[3]  ;
	CpuID[0]=*(vu32*)(0x1ffff7e8);
	CpuID[1]=*(vu32*)(0x1ffff7ec);
	CpuID[2]=*(vu32*)(0x1ffff7f0);
	Message[2]=CpuID[0];
	Message[3]=CpuID[0]>>16;
	Message[4]=CpuID[1];
	Message[5]=CpuID[1]>>16;
	Message[6]=CpuID[2];
	Message[7]=CpuID[2]>>16;
}
