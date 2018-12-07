#include "stm32f10x.h"
#include "key.h"
#include <stdio.h>
#include "modbus.h"
#include "moter.h" 
keysTypedef_t           keys;      //定义一个总的按键变量，支持12个
uint8_t                 keyCountTime;
static uint8_t          keyTotolNum = 0;
keyTypedef_t            singleKey[1]; // 设置按键变量
void Key_Init(void)       //包括PB0 串联信号，按键输入PB11管脚
{
		GPIO_InitTypeDef GPIO_InitStructure; 
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
/*--------------------------PB0 设置输出串联信号-----------------------------*/
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP ;
		GPIO_Init(GPIOB, &GPIO_InitStructure);
		GPIO_ResetBits(GPIOB,GPIO_Pin_0);    //设置引脚一直低电平
		/*--------------------------PB11 设置按键引脚信号-----------------------------*/
		GPIO_InitStructure.GPIO_Pin = KEY1_Pin;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU ;
		GPIO_Init(KEY1_GPIO_Port, &GPIO_InitStructure);
		/*--------------------------PB14 设置输出设置完成信号-----------------------------*/
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14;
		/* 配置为推挽输出 */	
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP ;
		GPIO_Init(GPIOB, &GPIO_InitStructure);
		GPIO_SetBits(GPIOB,GPIO_Pin_14);    //设置引脚一直高电平
}
uint16_t getKey(keysTypedef_t *keyS)
{
    uint8_t i = 0; 
    uint16_t readKey = 0;              //Essential init
    for(i = 0; i < keys.keyTotolNum; i++)
    {
        if(!GPIO_ReadInputDataBit((GPIO_TypeDef*)keyS->singleKey[i].keyPort,keyS->singleKey[i].keyGpio))
        {
            G_SET_BIT(readKey, keyS->singleKey[i].keyNum);
        }
    }
    return readKey;
}

uint16_t readKeyValue(keysTypedef_t *keyS)
{
    static uint8_t keyCheck = 0;
    static uint8_t keyState = 0;
    static uint16_t keyLongCheck = 0;
    static uint16_t keyPrev = 0;      //last key

    uint16_t keyPress = 0;
    uint16_t keyReturn = 0;
    
    keyCountTime ++;
    
    if(keyCountTime >= (DEBOUNCE_TIME / KEY_TIMER_MS))          //keyCountTime 1MS+1  key eliminate jitter 10MS
    {
        keyCountTime = 0;
        keyCheck = 1;
    }
    if(1 == keyCheck)
    {
        keyCheck = 0;
        keyPress = getKey(keyS);
        switch (keyState)
        {
            case 0:
                if(keyPress != 0)
                {
                    keyPrev = keyPress;
                    keyState = 1;
                }
                break;
                
            case 1:
                if(keyPress == keyPrev)
                {
                    keyState = 2;
                    keyReturn= keyPrev | KEY_DOWN;
                }
                else                //Button lift, jitter, no response button
                {
                    keyState = 0;
                }
                break;
                
            case 2:

                if(keyPress != keyPrev)
                {
                    keyState = 0;
                    keyLongCheck = 0;
                    keyReturn = keyPrev | KEY_UP;
                    return keyReturn;
                }
                if(keyPress == keyPrev)
                {
                    keyLongCheck++;
                    if(keyLongCheck >= (PRESS_LONG_TIME / DEBOUNCE_TIME))    //Long press 3S effective
                    {
                        keyLongCheck = 0;
                        keyState = 3;
                        keyReturn= keyPress | KEY_LONG;
                        return keyReturn;
                    }
                }
                break;

            case 3:
                if(keyPress != keyPrev)
                {
                    keyState = 0;
                }
                break;
        }
    }
    return  NO_KEY;
}


void keyHandle(keysTypedef_t *keyS)
{
    uint8_t i = 0;
    uint16_t key_value = 0;

    key_value = readKeyValue(keyS);

    if(!key_value) return;
    
    //Check short press button
    if(key_value & KEY_UP)
    {
        //Valid key is detected
        for(i = 0; i < keyS->keyTotolNum; i++)
        {
            if(G_IS_BIT_SET(key_value, keyS->singleKey[i].keyNum)) 
            {
                //key callback function of short press
                if(keyS->singleKey[i].shortPress) 
                {
                    keyS->singleKey[i].shortPress(); 

                }
            }
        }
    }

    //Check short long button
    if(key_value & KEY_LONG)
    {
        //Valid key is detected
        for(i = 0; i < keyS->keyTotolNum; i++)
        {
            if(G_IS_BIT_SET(key_value, keyS->singleKey[i].keyNum))
            {
                //key callback function of long press
                if(keyS->singleKey[i].longPress) 
                {
                    keyS->singleKey[i].longPress(); 

                }
            }
        }
    }
} 


keyTypedef_t keyInitOne(uint32_t keyRccPeriph, GPIO_TypeDef * keyPort, uint32_t keyGpio, gokitKeyFunction shortPress, gokitKeyFunction longPress)
{
    static int8_t key_total = -1;

    keyTypedef_t singleKey;
    
    //Platform-defined GPIO
    singleKey.keyRccPeriph = keyRccPeriph;
    singleKey.keyPort = keyPort;
    singleKey.keyGpio = keyGpio;
    singleKey.keyNum = ++key_total;
    
    //Button trigger callback type
    singleKey.longPress = longPress;
    singleKey.shortPress = shortPress;
    
    keyTotolNum++;
    
    return singleKey;
}

void keyParaInit(keysTypedef_t *keys)
{
    
    if(NULL == keys)
    {
        return ;
    }
    
    keys->keyTotolNum = keyTotolNum;
    
    if(KEY_MAX_NUMBER < keys->keyTotolNum) 
    {
        keys->keyTotolNum = KEY_MAX_NUMBER; 
    }
}
void TIM3_IRQHandler(void)
{                                                                   
		if(TIM_GetITStatus(TIM3,TIM_IT_Update)!=RESET)
		{
					 keyHandle((keysTypedef_t *)&keys);
					 TIM_ClearITPendingBit(TIM3,TIM_IT_Update);//清除中断标志
		}
	}
void keyInit(void)
{
	singleKey[0] = keyInitOne(NULL, KEY1_GPIO_Port, KEY1_Pin, key1ShortPress, key1LongPress);
    keys.singleKey = (keyTypedef_t *)&singleKey;
    keyParaInit(&keys); 
}
void key1ShortPress(void)		//按键短按执行电机驱动， 电机停止是在中断函数里面
{
	GPIO_SetBits(LED2_GPIO_PORT, LED2_GPIO_PIN);
	LED1_TOGGLE;
}
void key1LongPress(void)		//长按按键是用在MODBUS设置地址里面
{
 		  KeyFlag = 1;
}

