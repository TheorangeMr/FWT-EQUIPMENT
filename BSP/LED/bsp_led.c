/*******************************************
	@file ��  bsp_led.c
	@anthor��  �޳�
	@date�� 2022.07.26
	@version��  v1.0
  @brief��  LED-GPIO����
*******************************************/
#include "bsp_led.h"

void LED_Config(void)
{
	GPIO_InitTypeDef  GPIO_InitStruct;
	// GPIOʹ��
	DEBUG_LED_GPIO_APBxClkCmd(DEBUG_LED_GPIO_CLK , ENABLE);
	/*����RX����*/
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;                //��������
	GPIO_InitStruct.GPIO_Pin = (DEBUG_LED1_GPIO_PIN|DEBUG_LED2_GPIO_PIN|
	                            DEBUG_LED3_GPIO_PIN|DEBUG_LED4_GPIO_PIN|
	                            DEBUG_LED5_GPIO_PIN);
	GPIO_Init(DEBUG_LED_GPIO_PORT, &GPIO_InitStruct);
	GPIO_SetBits(DEBUG_LED_GPIO_PORT, (DEBUG_LED1_GPIO_PIN|DEBUG_LED2_GPIO_PIN|
	               DEBUG_LED3_GPIO_PIN|DEBUG_LED4_GPIO_PIN|DEBUG_LED5_GPIO_PIN));
}

