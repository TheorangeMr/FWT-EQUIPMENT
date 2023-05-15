/*******************************************
	@file ：  bsp_led.c
	@anthor：  罗成
	@date： 2022.07.26
	@version：  v1.0
  @brief：  LED-GPIO配置
*******************************************/
#include "bsp_led.h"

void LED_Config(void)
{
	GPIO_InitTypeDef  GPIO_InitStruct;
	// GPIO使能
	DEBUG_LED_GPIO_APBxClkCmd(DEBUG_LED_GPIO_CLK , ENABLE);
	/*配置RX引脚*/
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;                //浮空输入
	GPIO_InitStruct.GPIO_Pin = (DEBUG_LED1_GPIO_PIN|DEBUG_LED2_GPIO_PIN|
	                            DEBUG_LED3_GPIO_PIN|DEBUG_LED4_GPIO_PIN|
	                            DEBUG_LED5_GPIO_PIN);
	GPIO_Init(DEBUG_LED_GPIO_PORT, &GPIO_InitStruct);
	GPIO_SetBits(DEBUG_LED_GPIO_PORT, (DEBUG_LED1_GPIO_PIN|DEBUG_LED2_GPIO_PIN|
	               DEBUG_LED3_GPIO_PIN|DEBUG_LED4_GPIO_PIN|DEBUG_LED5_GPIO_PIN));
}

