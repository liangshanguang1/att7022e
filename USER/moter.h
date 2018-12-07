#ifndef __MOTER_H
#define	__MOTER_H
#include "stm32f10x.h"
#define ON  0
#define OFF 1
#define LED1_GPIO_PORT    	GPIOB			              /* GPIO端口 */
#define LED1_GPIO_CLK 	    RCC_APB2Periph_GPIOB		/* GPIO端口时钟 */
#define LED1_GPIO_PIN		GPIO_Pin_7			        /* 连接到SCL时钟线的GPIO */
#define LED2_GPIO_PORT    	GPIOB			              /* GPIO端口 */
#define LED2_GPIO_CLK 	    RCC_APB2Periph_GPIOB		/* GPIO端口时钟 */
#define LED2_GPIO_PIN		GPIO_Pin_6			        /* 连接到SCL时钟线的GPIO */
#define	digitalHi(p,i)		 {p->BSRR=i;}	 //输出为高电平		
#define digitalLo(p,i)		 {p->BRR=i;}	 //输出低电平
#define digitalToggle(p,i) 	{p->ODR ^=i;} //输出反转状态
#define LED1_OFF		   	digitalHi(LED1_GPIO_PORT,LED1_GPIO_PIN)
#define LED2_OFF		   	digitalHi(LED2_GPIO_PORT,LED2_GPIO_PIN)
#define LED1_TOGGLE		   	digitalToggle(LED1_GPIO_PORT,LED1_GPIO_PIN)
struct MoterState{
int fenzha;   //
int hezha;
int shoudongfen;
int shoudonghe;
};
extern struct MoterState moterstate;
#endif /* __LED_H */
