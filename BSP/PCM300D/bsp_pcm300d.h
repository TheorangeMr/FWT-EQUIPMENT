/*******************************************
	*文件名  ：  bsp_adc.h
	*作   者 ：  罗成
	*修改时间：  2022.08.01
	*版   本：   v1.0
  *说   明：   adc头文件
*******************************************/

#ifndef BSP_PCM300D_H
#define BSP_PCM300D_H

#include "stm32f10x.h"
#include "bsp_adc.h"
#include "bsp_usart.h"

#define PCM300D_Constant_Pa        25.0*10e6/3.3
#define PCM300D_Constant_MPa       25.0/3.3

float PCM300D_deal(uint8_t Channel);

#endif //BSP_ADC_H
