#ifndef __EXTI_H
#define	__EXTI_H


#include "stm32f10x.h"
#define KEY_ON	1
#define KEY_OFF	0



//引脚定义  KEY1 上上代表分闸
#define UpUpsensor_GPIO_PORT         GPIOA
#define UpUpsensor_GPIO_CLK          (RCC_APB2Periph_GPIOA|RCC_APB2Periph_AFIO)
#define UpUpsensor_GPIO_PIN          GPIO_Pin_3
#define UpUpsensor_EXTI_PORTSOURCE   GPIO_PortSourceGPIOA
#define UpUpsensor_EXTI_PINSOURCE    GPIO_PinSource3
#define UpUpsensor_EXTI_LINE         EXTI_Line3
#define UpUpsensor_EXTI_IRQ          EXTI3_IRQn
#define UpUpsensor_IRQHandler            EXTI3_IRQHandler

//引脚定义  KEY2 上下代表合闸
#define UpDownsensor_GPIO_PORT         GPIOC
#define UpDownsensor_GPIO_CLK          (RCC_APB2Periph_GPIOC|RCC_APB2Periph_AFIO)
#define UpDownsensor_GPIO_PIN          GPIO_Pin_13
#define UpDownsensor_EXTI_PORTSOURCE   GPIO_PortSourceGPIOC
#define UpDownsensor_EXTI_PINSOURCE    GPIO_PinSource13
#define UpDownsensor_EXTI_LINE         EXTI_Line13
#define UpDownsensor_EXTI_IRQ          EXTI15_10_IRQn

#define UpDownsensor_IRQHandler            EXTI15_10_IRQHandler
//引脚定义  KEY3 下上代表合闸
#define DownUpsensor_GPIO_PORT         GPIOA
#define DownUpsensor_GPIO_CLK          (RCC_APB2Periph_GPIOA|RCC_APB2Periph_AFIO)
#define DownUpsensor_GPIO_PIN          GPIO_Pin_2
#define DownUpsensor_EXTI_PORTSOURCE   GPIO_PortSourceGPIOA
#define DownUpsensor_EXTI_PINSOURCE    GPIO_PinSource2
#define DownUpsensor_EXTI_LINE         EXTI_Line2
#define DownUpsensor_EXTI_IRQ          EXTI2_IRQn

#define DownUpsensor_IRQHandler            EXTI2_IRQHandler
//引脚定义  KEY4 下下代表分闸
#define DownDownsensor_GPIO_PORT         GPIOA
#define DownDownsensor_GPIO_CLK          (RCC_APB2Periph_GPIOA|RCC_APB2Periph_AFIO)
#define DownDownsensor_GPIO_PIN          GPIO_Pin_5
#define DownDownsensor_EXTI_PORTSOURCE   GPIO_PortSourceGPIOA
#define DownDownsensor_EXTI_PINSOURCE    GPIO_PinSource5
#define DownDownsensor_EXTI_LINE         EXTI_Line5
#define DownDownsensor_EXTI_IRQ           EXTI9_5_IRQn 
#define DownDownsensor_IRQHandler             EXTI9_5_IRQHandler 
void EXTI_Key_Config(void);


#endif /* __EXTI_H */
