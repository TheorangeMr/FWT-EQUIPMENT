#ifndef BSP_XJWY_H
#define BSP_XJWY_H

#include "stm32f10x.h"
#include "bsp_adc.h"
#include "bsp_usart.h"

#define xjwy_datedeal         5/3.3*500/5

uint16_t XJWY_deal(uint8_t Channel);

#endif

