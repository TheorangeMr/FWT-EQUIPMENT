#ifndef BSP_LED_H
#define BSP_LED_H

#include "stm32f10x.h"


#define DEBUG_LED_GPIO_CLK              RCC_APB2Periph_GPIOD

#define DEBUG_LED_GPIO_APBxClkCmd       RCC_APB2PeriphClockCmd

#define DEBUG_LED_GPIO_PORT           GPIOD
#define DEBUG_LED1_GPIO_PIN           GPIO_Pin_3
#define DEBUG_LED2_GPIO_PIN           GPIO_Pin_4
#define DEBUG_LED3_GPIO_PIN           GPIO_Pin_5
#define DEBUG_LED4_GPIO_PIN           GPIO_Pin_6
#define DEBUG_LED5_GPIO_PIN           GPIO_Pin_7

void LED_Config(void);

#endif

