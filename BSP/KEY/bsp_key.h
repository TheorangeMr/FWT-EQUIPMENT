#ifndef BSP_KEY_H
#define BSP_KEY_H

#include "stm32f10x.h"

#define DEBUG_KEY1_GPIO_CLK              RCC_APB2Periph_GPIOA
#define DEBUG_KEY2_GPIO_CLK              RCC_APB2Periph_GPIOB
#define DEBUG_KEY3_GPIO_CLK              RCC_APB2Periph_GPIOC
#define DEBUG_KEY4_GPIO_CLK              RCC_APB2Periph_GPIOD
#define DEBUG_KEY5_GPIO_CLK              RCC_APB2Periph_GPIOE

#define DEBUG_KEY_GPIO_APBxClkCmd       RCC_APB2PeriphClockCmd

#define DEBUG_KEY1_GPIO_PORT           GPIOA
#define DEBUG_KEY1_GPIO_PIN           GPIO_Pin_8
#define DEBUG_KEY2_GPIO_PORT           GPIOB
#define DEBUG_KEY2_GPIO_PIN           GPIO_Pin_8
#define DEBUG_KEY3_GPIO_PORT           GPIOC
#define DEBUG_KEY3_GPIO_PIN           GPIO_Pin_13
#define DEBUG_KEY4_GPIO_PORT           GPIOD
#define DEBUG_KEY4_GPIO_PIN           GPIO_Pin_10
#define DEBUG_KEY5_GPIO_PORT           GPIOE
#define DEBUG_KEY5_GPIO_PIN           GPIO_Pin_6

#define       KEY1_READ      GPIO_ReadInputDataBit(DEBUG_KEY1_GPIO_PORT,DEBUG_KEY1_GPIO_PIN)
#define       KEY2_READ      GPIO_ReadInputDataBit(DEBUG_KEY2_GPIO_PORT,DEBUG_KEY2_GPIO_PIN)
#define       KEY3_READ      GPIO_ReadInputDataBit(DEBUG_KEY3_GPIO_PORT,DEBUG_KEY3_GPIO_PIN)
#define       KEY4_READ      GPIO_ReadInputDataBit(DEBUG_KEY4_GPIO_PORT,DEBUG_KEY4_GPIO_PIN)
#define       KEY5_READ      GPIO_ReadInputDataBit(DEBUG_KEY5_GPIO_PORT,DEBUG_KEY5_GPIO_PIN)


//°´¼ü¼üÖµ£¨Ã¶¾Ù£©
typedef enum{
	KEY1_VALUE = 0,
	KEY2_VALUE,
	KEY3_VALUE,
	KEY4_VALUE,
	KEY5_VALUE,
}key;

void KEY_Config(void);
uint8_t Key_Scan(uint8_t mode);


#endif

