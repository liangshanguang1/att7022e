#include "modbus.h"
#include "led.h"
#include "key.h"
#include "delay.h"
#include "bsp_internal_flash.h" 
#include "moter.h" 
///////////////////////////////////////////////////////////
		u32 	RS485_Addr=00;						//从机地址 全局变量
static 	u8 		RS485_RX_BUFF[1024];			//接收缓冲区2048字节
static 	u16 	RS485_RX_CNT=0;				//接收计数器
static 	u8 		RS485_FrameFlag=0;			//帧结束标记
static 	u8 		RS485_TX_BUFF[2048];			//发送缓冲区
		u8 		KeyFlag = 0;						//全局变量用来表示 设置MODBUS地址的标志，在按键长按中使用
		u32  	i=1,y=1,u=1,t=0;   				//这里做测试用，Command1读取从机断路器开关状态
vu32 *Modbus_InputIO[100];//输入开关量寄存器指针(这里使用的是位带操作)
vu32 *Modbus_OutputIO[100];//输出开关量寄存器指针(这里使用的是位带操作)
u16 *Modbus_HoldReg[1000];//保持寄存器指针
u16 Message [28];         //command 3命令需要读取的寄存器 电气信息存贮在这里

void Modbus_RegMap(void)
{
        Modbus_OutputIO[0]=(vu32*)&i;//LED0
        Modbus_OutputIO[1]=(vu32*)&y;//LED1
        Modbus_OutputIO[2]=(vu32*)&u;//LED0
        Modbus_OutputIO[3]=(vu32*)&t;//LED1
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//CRC校验 自己后面添加的

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

//初始化USART2
void RS485_Init(void)
{
		u32 RS485_Baudrate=9600;//通讯波特率
		u8 RS485_Parity=0;//0无校验；1奇校验；2偶校验
		GPIO_InitTypeDef GPIO_InitStructure;
        USART_InitTypeDef USART_InitStructure;
        NVIC_InitTypeDef NVIC_InitStructure;
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOD,ENABLE);
        RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2,ENABLE);
        
        GPIO_InitStructure.GPIO_Pin=GPIO_Pin_2;//PA2（TX）复用推挽输出
        GPIO_InitStructure.GPIO_Mode=GPIO_Mode_AF_PP;
        GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
        GPIO_Init(GPIOA,&GPIO_InitStructure);
        GPIO_SetBits(GPIOA,GPIO_Pin_2);//默认高电平
        
        GPIO_InitStructure.GPIO_Pin=GPIO_Pin_3;//PA3（RX）输入上拉
        GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IN_FLOATING;   //修改原GPIO_Mode_IPU（输入上拉）->GPIO_Mode_IN_FLOATING(浮空输入)/////////////////////////////////////////////
        GPIO_Init(GPIOA,&GPIO_InitStructure);
        
        GPIO_InitStructure.GPIO_Pin=GPIO_Pin_0;//修改PG9（RE/DE）通用推挽输出->PD7（RE/DE）通用推挽输出//////////////////////////////////////////////////////////////////////
        GPIO_InitStructure.GPIO_Mode=GPIO_Mode_Out_PP;
        GPIO_Init(GPIOA,&GPIO_InitStructure);
        GPIO_ResetBits(GPIOA,GPIO_Pin_0);//默认接收状态
        
        USART_DeInit(USART2);//复位串口2
        USART_InitStructure.USART_BaudRate=RS485_Baudrate;
        USART_InitStructure.USART_HardwareFlowControl=USART_HardwareFlowControl_None;
        USART_InitStructure.USART_WordLength=USART_WordLength_8b;
        USART_InitStructure.USART_StopBits=USART_StopBits_1;
        USART_InitStructure.USART_Mode=USART_Mode_Rx|USART_Mode_Tx;//收发模式
        switch(RS485_Parity)
        {
                case 0:USART_InitStructure.USART_Parity=USART_Parity_No;break;//无校验
                case 1:USART_InitStructure.USART_Parity=USART_Parity_Odd;break;//奇校验
                case 2:USART_InitStructure.USART_Parity=USART_Parity_Even;break;//偶校验
        }
        USART_Init(USART2,&USART_InitStructure);
        
        USART_ClearITPendingBit(USART2,USART_IT_RXNE);
        USART_ITConfig(USART2,USART_IT_RXNE,ENABLE);//使能串口2接收中断
        
        NVIC_InitStructure.NVIC_IRQChannel=USART2_IRQn;
        NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=2;
        NVIC_InitStructure.NVIC_IRQChannelSubPriority=1;
        NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;
        NVIC_Init(&NVIC_InitStructure);
        
        USART_Cmd(USART2,ENABLE);//使能串口2
        RS485_RX_EN();		//默认为接收模式
        Timer3_Init(); 		//按键定时器初始化
        Timer2_Init();    	// 用来检测modbus 是否接受完成
        Modbus_RegMap();	//Modbus寄存器映射
}

//定时器7初始化
void Timer2_Init(void)
{
		u16 RS485_Frame_Distance=4;//数据帧最小间隔（ms),超过此时间则认为是下一帧
        TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
        NVIC_InitTypeDef NVIC_InitStructure;
        RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE); //TIM2时钟使能 
        //TIM7初始化设置
        TIM_TimeBaseStructure.TIM_Period = RS485_Frame_Distance*10; //设置在下一个更新事件装入活动的自动重装载寄存器周期的值
        TIM_TimeBaseStructure.TIM_Prescaler =7200; //设置用来作为TIMx时钟频率除数的预分频值 设置计数频率为10kHz
        TIM_TimeBaseStructure.TIM_ClockDivision = 0; //设置时钟分割:TDTS = Tck_tim
        TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM向上计数模式
        TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure); //根据TIM_TimeBaseInitStruct中指定的参数初始化TIMx的时间基数单位
        TIM_Cmd(TIM2,DISABLE);//停止定时器
        TIM_ITConfig( TIM2, TIM_IT_Update, ENABLE );//TIM2 允许更新中断
		TIM_ClearFlag(TIM2,TIM_FLAG_Update);
        //TIM7中断分组配置
        NVIC_InitStructure.NVIC_IRQChannel =TIM2_IRQn;  //TIM2中断
        NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;  //先占优先级2级
        NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;  //从优先级3级
        NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQ通道被使能
        NVIC_Init(&NVIC_InitStructure);  //根据NVIC_InitStruct中指定的参数初始化外设NVIC寄存器                                                                  
}
/*TIme3定时器 用来产生1ms的按键扫描*/
void Timer3_Init(void)
{
        TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
        NVIC_InitTypeDef NVIC_InitStructure;
        RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE); //TIM3时钟使能 
        //TIM3初始化设置
        TIM_TimeBaseStructure.TIM_Period = 10-1; //设置在下一个更新事件装入活动的自动重装载寄存器周期的值
        TIM_TimeBaseStructure.TIM_Prescaler =7200-1; //设置用来作为TIMx时钟频率除数的预分频值 设置计数频率为10kHz
        TIM_TimeBaseStructure.TIM_ClockDivision = 0; //设置时钟分割:TDTS = Tck_tim
        TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM向上计数模式
        TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure); //根据TIM_TimeBaseInitStruct中指定的参数初始化TIMx的时间基数单位
        TIM_ITConfig( TIM3, TIM_IT_Update, ENABLE );//TIM3 允许更新中断
		TIM_ClearFlag(TIM3,TIM_FLAG_Update);
        //TIM7中断分组配置
        NVIC_InitStructure.NVIC_IRQChannel =TIM3_IRQn;  //TIM2中断
        NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;  //先占优先级2级
        NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;  //从优先级2级
        NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQ通道被使能
        NVIC_Init(&NVIC_InitStructure);  //根据NVIC_InitStruct中指定的参数初始化外设NVIC寄存器                                                                  
        TIM_Cmd(TIM3,ENABLE);//开始计时器
}
//////////////////////////////////////////////////////////////////////////////
//发送n个字节数据
//buff:发送区首地址
//len：发送的字节数
void RS485_SendData(u8 *buff,u8 len)
{ 
        RS485_TX_EN();//切换为发送模式
        while(len--)
        {
                while(USART_GetFlagStatus(USART2,USART_FLAG_TXE)==RESET);//等待发送区为空
                USART_SendData(USART2,*(buff++));
        }
        while(USART_GetFlagStatus(USART2,USART_FLAG_TC)==RESET);//等待发送完成
}


void USART2_IRQHandler(void)//串口2中断服务程序
{
        u8 res;
        u8 err;
        if(USART_GetITStatus(USART2,USART_IT_RXNE)!=RESET)
        {
			if(USART_GetFlagStatus(USART2,USART_FLAG_NE|USART_FLAG_FE|USART_FLAG_PE)) err=1;//检测到噪音、帧错误或校验错误
			else err=0;
			res=USART_ReceiveData(USART2); //读接收到的字节，同时相关标志自动清除
			if((RS485_RX_CNT<2047)&&(err==0))
			{
				RS485_RX_BUFF[RS485_RX_CNT]=res;
				RS485_RX_CNT++;
				TIM_ClearITPendingBit(TIM2,TIM_IT_Update);//清除定时器溢出中断
				TIM_SetCounter(TIM2,0);//当接收到一个新的字节，将定时器7复位为0，重新计时（相当于喂狗）
				TIM_Cmd(TIM2,ENABLE);//开始计时
			}
		}
}

///////////////////////////////////////////////////////////////////////////////////////
//用定时器2判断接收空闲时间，当空闲时间大于指定时间，认为一帧结束
//定时器7中断服务程序         
void TIM2_IRQHandler(void)
{                                                                   
        if(TIM_GetITStatus(TIM2,TIM_IT_Update)!=RESET)
        {
                TIM_ClearITPendingBit(TIM2,TIM_IT_Update);//清除中断标志
                TIM_Cmd(TIM2,DISABLE);//停止定时器
                if(RS485_RX_BUFF[0]==0xFF) RS485_RX_CNT=0;	          //如果进入设置地址模式，那么cnt为0 ，cnt为0主要是为了
				RS485_FrameFlag=1;//置位帧结束标记
        }
}
/////////////////////////////////////////////////////////////////////////////////////
//RS485服务程序，用于处理接收到的数据(请在主函数中循环调用)
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
                  if((RS485_RX_BUFF[1]==01)||(RS485_RX_BUFF[1]==02)||(RS485_RX_BUFF[1]==05)||(RS485_RX_BUFF[1]==03))//功能码正确
                  {
										
                                startRegAddr=(((u16)RS485_RX_BUFF[2])<<8)|RS485_RX_BUFF[3];//获取寄存器起始地址
                                if(startRegAddr<1000)//寄存器地址在范围内
                                {
                                        calCRC=CRC_Compute(RS485_RX_BUFF,RS485_RX_CNT-2);//计算所接收数据的CRC
                                        recCRC=RS485_RX_BUFF[RS485_RX_CNT-1]|(((u16)RS485_RX_BUFF[RS485_RX_CNT-2])<<8);//接收到的CRC(低字节在前，高字节在后)
                                        if(calCRC==recCRC)//CRC校验正确
                                        {																
                                                /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
																					      switch(RS485_RX_BUFF[1])//根据不同的功能码进行处理
                                                {
                                                        case 2://读输入开关量
                                                        {
                                                                Modbus_02_Solve();
                                                                break;
                                                        }
                                                        case 1://读输出开关量
                                                        {
                                                                Modbus_01_Solve();
                                                                break;
                                                        }
                                                        case 5://写单个输出开关量
                                                        {
                                                                Modbus_05_Solve();
                                                                break;
                                                        }
                                                                
                                                        case 15://写多个输出开关量
                                                        {
                                                                Modbus_15_Solve();
                                                                break;
                                                        }
                                                                
                                                        case 03: //读多个寄存器
                                                        {																
                                                                Modbus_03_Solve();
                                                                break;
                                                        }
                                                                
                                                        case 06: //写单个寄存器
                                                        {
                                                                Modbus_06_Solve();
                                                                break;
                                                        }
                                                                
                                                        case 16: //写多个寄存器
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
                                        else//CRC校验错误
                                        {

                                                RS485_TX_BUFF[0]=RS485_RX_BUFF[0];
                                                RS485_TX_BUFF[1]=RS485_RX_BUFF[1]|0x80;
                                                RS485_TX_BUFF[2]=0x04; //异常码
                                                RS485_SendData(RS485_TX_BUFF,3);
                                        }        
                                }                                                
                        }
                        else//功能码错误
                        {
                                RS485_TX_BUFF[0]=RS485_RX_BUFF[0];
                                RS485_TX_BUFF[1]=RS485_RX_BUFF[1]|0x80;
                                RS485_TX_BUFF[2]=0x01; //异常码
                                RS485_SendData(RS485_TX_BUFF,3);
                        }
          }
                                
                else if (RS485_RX_BUFF[0]==0xFF)								//在这里设置MODBUS 自动设置地址
								{
										while(!KeyFlag);						//等待上一个从机传来触发信号
										KeyFlag=0;  							//确保只进入一次	
										RS485_Addr = RS485_RX_BUFF[1];			//设置MODBUS的地址
										delay_ms(10000);		
										Modbus_FF_Solve();						//发送应答信号	
										RS485_FrameFlag=0;                      //接受标志清0，为后面重新接受做准备，这个是全局变量，在中断里会改变
										RS485_RX_CNT=0;                         //这里的cnt接受个数应该清0 ，保证后面接受的时候覆盖之前接受的数据
										while(!RS485_FrameFlag)	;				//重新接受一个地址加一的主机信号
										if(RS485_Addr !=RS485_RX_BUFF[1] )      //这里接受的地址正常是不同的
										{                                      
											RS485_FrameFlag=0;                  //这里清0，为了接受正常的非FF主机信号
											GPIO_SetBits(GPIOB,GPIO_Pin_0);     //
											delay_ms(1000);						//	
										 	GPIO_ResetBits(GPIOB,GPIO_Pin_0);     //这里好像有问题，没有设置B0   
											GPIO_ResetBits(GPIOB,GPIO_Pin_14);    //设置引脚为低电平点亮LED
											Flash_W(RS485_Addr);
										}											
								}
								RS485_FrameFlag=0;//复位帧结束标志
                RS485_RX_CNT=0;//接收计数器清零
                RS485_RX_EN();//开启接收模式 
												
        }                
}

//Modbus功能码02处理程序/////////////////////////////////////////////////////程序已验证OK -----必须先配置PE4 PE3 PE2 PA0 初始化按键才可以OK    KEY_Init();
//读输入开关量
void Modbus_02_Solve(void)
{
        u16 ByteNum;
        u16 i;
        RegNum= (((u16)RS485_RX_BUFF[4])<<8)|RS485_RX_BUFF[5];//获取寄存器数量
        if((startRegAddr+RegNum)<100)//寄存器地址+数量在范围内
        {
                RS485_TX_BUFF[0]=RS485_RX_BUFF[0];
                RS485_TX_BUFF[1]=RS485_RX_BUFF[1];
                ByteNum=RegNum/8;//字节数
                if(RegNum%8) ByteNum+=1;//如果位数还有余数，则字节数+1
                RS485_TX_BUFF[2]=ByteNum;//返回要读取的字节数
                for(i=0;i<RegNum;i++)
                {
                        if(i%8==0) RS485_TX_BUFF[3+i/8]=0x00;
                        RS485_TX_BUFF[3+i/8]>>=1;//低位先发送
                        RS485_TX_BUFF[3+i/8]|=((*Modbus_InputIO[startRegAddr+i])<<7)&0x80;
                        if(i==RegNum-1)//发送到最后一个位了
                        {
                                if(RegNum%8) RS485_TX_BUFF[3+i/8]>>=8-(RegNum%8);//如果最后一个字节还有余数，则剩余MSB填充0
                        }
                }
                calCRC=CRC_Compute(RS485_TX_BUFF,ByteNum+3);
                RS485_TX_BUFF[ByteNum+3]=(calCRC>>8)&0xFF;
                RS485_TX_BUFF[ByteNum+4]=(calCRC)&0xFF;
                RS485_SendData(RS485_TX_BUFF,ByteNum+5);
        }
        else//寄存器地址+数量超出范围
        {
                RS485_TX_BUFF[0]=RS485_RX_BUFF[0];
                RS485_TX_BUFF[1]=RS485_RX_BUFF[1]|0x80;
                RS485_TX_BUFF[2]=0x02; //异常码
                RS485_SendData(RS485_TX_BUFF,3);
        }
}

//Modbus功能码01处理程序 ///////////////////////////////////////////////////////////程序已验证OK
//读输出开关量
void Modbus_01_Solve(void)				//返回的是一个字节，一个字节的最后一位代表分合状态
{
        u16 ByteNum;
        u16 i;
        RegNum= (((u16)RS485_RX_BUFF[4])<<8)|RS485_RX_BUFF[5];//获取寄存器数量
        if((startRegAddr+RegNum)<100)//寄存器地址+数量在范围内
        {
                RS485_TX_BUFF[0]=RS485_RX_BUFF[0];
                RS485_TX_BUFF[1]=RS485_RX_BUFF[1];
                ByteNum=RegNum/8;//字节数
                if(RegNum%8) ByteNum+=1;//如果位数还有余数，则字节数+1
                RS485_TX_BUFF[2]=ByteNum;//返回要读取的字节数
                for(i=0;i<RegNum;i++)
                {
                        if(i%8==0) RS485_TX_BUFF[3+i/8]=0x00;
                        RS485_TX_BUFF[3+i/8]>>=1;//低位先发送
                        RS485_TX_BUFF[3+i/8]|=((*Modbus_OutputIO[startRegAddr+i])<<7)&0x80;
                        if(i==RegNum-1)//发送到最后一个位了
                        {
                                if(RegNum%8) RS485_TX_BUFF[3+i/8]>>=8-(RegNum%8);//如果最后一个字节还有余数，则剩余MSB填充0
                        }
                }
                calCRC=CRC_Compute(RS485_TX_BUFF,ByteNum+3);
                RS485_TX_BUFF[ByteNum+3]=(calCRC>>8)&0xFF;
                RS485_TX_BUFF[ByteNum+4]=(calCRC)&0xFF;
                RS485_SendData(RS485_TX_BUFF,ByteNum+5);
        }
        else//寄存器地址+数量超出范围
        {
                RS485_TX_BUFF[0]=RS485_RX_BUFF[0];
                RS485_TX_BUFF[1]=RS485_RX_BUFF[1]|0x80;
                RS485_TX_BUFF[2]=0x02; //异常码
                RS485_SendData(RS485_TX_BUFF,3);
        }
}

//Modbus功能码05处理程序   ///////////////////////////////////////////////////////程序已验证OK
//写单个输出开关量
void Modbus_05_Solve(void)
{
        if(startRegAddr<100)//寄存器地址在范围内
        {
                if((RS485_RX_BUFF[4]==0xFF)||(RS485_RX_BUFF[5]==0x00)) //功能5用来设置从机的开关状态，驱动电机,无论分合信号都是让电机转动
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
        else//寄存器地址超出范围
        {
                RS485_TX_BUFF[0]=RS485_RX_BUFF[0];
                RS485_TX_BUFF[1]=RS485_RX_BUFF[1]|0x80;
                RS485_TX_BUFF[2]=0x02; //异常码
                RS485_SendData(RS485_TX_BUFF,3);
        }
}

//Modbus功能码15处理程序   //////////////////////////////////////////////////////程序已验证OK
//写多个输出开关量
void Modbus_15_Solve(void)
{
        u16 i;
        RegNum=(((u16)RS485_RX_BUFF[4])<<8)|RS485_RX_BUFF[5];//获取寄存器数量
        if((startRegAddr+RegNum)<100)//寄存器地址+数量在范围内
        {        
                for(i=0;i<RegNum;i++)
                {
                        if(RS485_RX_BUFF[7+i/8]&0x01) *Modbus_OutputIO[startRegAddr+i]=0x01;
                        else *Modbus_OutputIO[startRegAddr+i]=0x00;
                        RS485_RX_BUFF[7+i/8]>>=1;//从低位开始
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
        else//寄存器地址+数量超出范围
        {
                RS485_TX_BUFF[0]=RS485_RX_BUFF[0];
                RS485_TX_BUFF[1]=RS485_RX_BUFF[1]|0x80;
                RS485_TX_BUFF[2]=0x02; //异常码
                RS485_SendData(RS485_TX_BUFF,3);
        }
}

//Modbus功能码03处理程序///////////////////////////////////////////////////////////////////////////////////////已验证程序OK
//读保持寄存器
void Modbus_03_Solve(void)
{
        u8 i;
        RegNum= (((u16)RS485_RX_BUFF[4])<<8)|RS485_RX_BUFF[5];//获取寄存器数量
        if((startRegAddr+RegNum)<1000)//寄存器地址+数量在范围内
        {
                RS485_TX_BUFF[0]=RS485_RX_BUFF[0];
                RS485_TX_BUFF[1]=RS485_RX_BUFF[1];
                RS485_TX_BUFF[2]=RegNum*2;
                for(i=0;i<RegNum;i++)
                {
                        RS485_TX_BUFF[3+i*2]=(Message [startRegAddr+i]>>8)&0xFF;//           /////////先发送高字节--在发送低字节
                        RS485_TX_BUFF[4+i*2]=(Message [startRegAddr+i])&0xFF; //
                }
                calCRC=CRC_Compute(RS485_TX_BUFF,RegNum*2+3);
                RS485_TX_BUFF[RegNum*2+3]=(calCRC>>8)&0xFF;         //CRC高地位不对吗？  // 先高后低
                RS485_TX_BUFF[RegNum*2+4]=(calCRC)&0xFF;
                RS485_SendData(RS485_TX_BUFF,RegNum*2+5);
        }
        else//寄存器地址+数量超出范围
        {
                RS485_TX_BUFF[0]=RS485_RX_BUFF[0];
                RS485_TX_BUFF[1]=RS485_RX_BUFF[1]|0x80;
                RS485_TX_BUFF[2]=0x02; //异常码
                RS485_SendData(RS485_TX_BUFF,3);
        }
}


//Modbus功能码06处理程序   //////////////////////////////////////////////////////////////////////////////////已验证程序OK
//写单个保持寄存器
void Modbus_06_Solve(void)
{
        *Modbus_HoldReg[startRegAddr]=RS485_RX_BUFF[4]<<8;//高字节在前                    ////////修改为高字节在前，低字节在后
        *Modbus_HoldReg[startRegAddr]|=((u16)RS485_RX_BUFF[5]);//低字节在后
        
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

//Modbus功能码16处理程序 /////////////////////////////////////////////////////////////////////////////////////////////////已验证程序OK
//写多个保持寄存器
void Modbus_16_Solve(void)
{
        u8 i;
        RegNum= (((u16)RS485_RX_BUFF[4])<<8)|((RS485_RX_BUFF[5]));//获取寄存器数量
        if((startRegAddr+RegNum)<1000)//寄存器地址+数量在范围内
        {
                for(i=0;i<RegNum;i++)
                {
                        *Modbus_HoldReg[startRegAddr+i]=RS485_RX_BUFF[7+i*2]; //低字节在前                 /////// 低字节在前，高字节在后正常
                        *Modbus_HoldReg[startRegAddr+i]|=((u16)RS485_RX_BUFF[8+i*2])<<8; //高字节在后
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
        else//寄存器地址+数量超出范围
        {
                RS485_TX_BUFF[0]=RS485_RX_BUFF[0];
                RS485_TX_BUFF[1]=RS485_RX_BUFF[1]|0x80;
                RS485_TX_BUFF[2]=0x02; //异常码
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
		RS485_RX_EN();//开启接收模式
}
 

void GetLockCode(void)      //得到断路器ID 并存储在Command3寄存器中
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
