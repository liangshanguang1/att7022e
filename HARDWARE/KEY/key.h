#ifndef __KEY_H
#define __KEY_H	 
#include "stm32f10x.h"
#define KEY1_Pin GPIO_Pin_11
#define KEY1_GPIO_Port GPIOB

#define KEY0  GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_11)//读取按键0
#define KEY1  GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_12)//读取按键0

#define KEY2  GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_2)//读取按键2 
#define WK_UP   GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_0)//读取按键3(WK_UP) 
#define G_SET_BIT(a,b)                            (a |= (1 << b))
#define G_CLEAR_BIT(a,b)                          (a &= ~(1 << b))
#define G_IS_BIT_SET(a,b)                         (a & (1 << b))

#define KEY_TIMER_MS                            1
#define KEY_MAX_NUMBER                          12
#define DEBOUNCE_TIME                           30
#define PRESS_LONG_TIME                         3000

#define NO_KEY                                  0x0000
#define KEY_DOWN                                0x1000
#define KEY_UP                                  0x2000
#define KEY_LIAN                                0x4000
#define KEY_LONG                                0x8000

 

typedef void (*gokitKeyFunction)(void);

__packed typedef struct
{
    uint8_t          keyNum;
    uint32_t         keyRccPeriph;
    GPIO_TypeDef     *keyPort;
    uint32_t         keyGpio;
    gokitKeyFunction shortPress; 
    gokitKeyFunction longPress; 
}keyTypedef_t;

__packed typedef struct
{
    uint8_t      keyTotolNum;
    keyTypedef_t *singleKey; 
}keysTypedef_t; 
void keyInit(void) ;
void keyGpioInit(void);
void keyHandle(keysTypedef_t *keys);
void keyParaInit(keysTypedef_t *keys);
uint16_t getKey(keysTypedef_t *key);
uint16_t readKeyValue(keysTypedef_t *keys);
keyTypedef_t keyInitOne(uint32_t keyRccPeriph, GPIO_TypeDef * keyPort, uint32_t keyGpio, gokitKeyFunction shortPress, gokitKeyFunction longPress);
void key1LongPress(void)  ;
void key1ShortPress(void) ;
void KEY_Init(void);//IO初始化
u8 KEY_Scan(u8);  	//按键扫描函数					    
void Key_Init(void) ;      //包括PB0 串联信号，按键输入PB11管脚
#endif
