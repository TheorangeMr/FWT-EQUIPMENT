/*******************************************
	*文件名  ：  bsp_adc.h
	*作   者 ：  罗成
	*修改时间：  2022.06.13
	*版   本：   v1.0
  *说   明：   adc头文件
*******************************************/

#ifndef BSP_ADC_H
#define BSP_ADC_H

#include "stm32f10x.h"


/*************************STM32F103X系列ADC外部通道相关引脚宏定义************************/

/* ADC channel_0 */
#define  ADC_CH0_GPIO_PORT       GPIOA   
#define  ADC_CH0_GPIO_PIN        GPIO_Pin_0
/* ADC channel_1 */
#define  ADC_CH1_GPIO_PORT       GPIOA   
#define  ADC_CH1_GPIO_PIN        GPIO_Pin_1
/* ADC channel_2 */
#define  ADC_CH2_GPIO_PORT       GPIOA   
#define  ADC_CH2_GPIO_PIN        GPIO_Pin_2
/* ADC channel_3 */
#define  ADC_CH3_GPIO_PORT       GPIOA   
#define  ADC_CH3_GPIO_PIN        GPIO_Pin_3
/* ADC channel_4 */
#define  ADC_CH4_GPIO_PORT       GPIOA   
#define  ADC_CH4_GPIO_PIN        GPIO_Pin_4
/* ADC channel_5 */
#define  ADC_CH5_GPIO_PORT       GPIOA   
#define  ADC_CH5_GPIO_PIN        GPIO_Pin_5
/* ADC channel_6 */
#define  ADC_CH6_GPIO_PORT       GPIOA   
#define  ADC_CH6_GPIO_PIN        GPIO_Pin_6
/* ADC channel_7 */
#define  ADC_CH7_GPIO_PORT       GPIOA   
#define  ADC_CH7_GPIO_PIN        GPIO_Pin_7
/* ADC channel_8 */
#define  ADC_CH8_GPIO_PORT       GPIOB   
#define  ADC_CH8_GPIO_PIN        GPIO_Pin_0
/* ADC channel_9 */
#define  ADC_CH9_GPIO_PORT       GPIOB   
#define  ADC_CH9_GPIO_PIN        GPIO_Pin_1
/* ADC channel_10 */
#define  ADC_CH10_GPIO_PORT      GPIOC   
#define  ADC_CH10_GPIO_PIN       GPIO_Pin_0
/* ADC channel_11 */
#define  ADC_CH11_GPIO_PORT      GPIOC   
#define  ADC_CH11_GPIO_PIN       GPIO_Pin_1
/* ADC channel_12 */
#define  ADC_CH12_GPIO_PORT      GPIOC   
#define  ADC_CH12_GPIO_PIN       GPIO_Pin_2
/* ADC channel_13 */
#define  ADC_CH13_GPIO_PORT      GPIOC   
#define  ADC_CH13_GPIO_PIN       GPIO_Pin_3
/* ADC channel_14 */
#define  ADC_CH14_GPIO_PORT      GPIOC   
#define  ADC_CH14_GPIO_PIN       GPIO_Pin_4
/* ADC channel_15 */
#define  ADC_CH15_GPIO_PORT      GPIOC   
#define  ADC_CH15_GPIO_PIN       GPIO_Pin_5

/******************************ADC采集模式*************************************/
#define INDEPENDENT_SINGGLE_CHANNEL     1
#define INDEPENDENT_MULTI_CHANNEL       0

#define ADC_SMOOTHING          5         //adc滤波次数


#define ADCx                           ADC1
#define ADC_APBxClock_FUN              RCC_APB2PeriphClockCmd
#define ADC_CLK                        RCC_APB2Periph_ADC1

#define ADC_GPIO_APBxClock_FUN         RCC_APB2PeriphClockCmd
#define ADC_GPIO_CLK                   RCC_APB2Periph_GPIOA
#define ADC_PORT                       ADC_CH0_GPIO_PORT
#define ADC_PORT_A                     ADC_CH0_GPIO_PORT
#define ADC_PORT_B                     ADC_CH8_GPIO_PORT
#define ADC_PORT_C                     ADC_CH10_GPIO_PORT
#define ADC_DMA_CLK                    RCC_AHBPeriph_DMA1

#define ADCx_SCAN_CHANNEL_NUM  6         //扫描的通道数

#define ADC_PIN0                       ADC_CH0_GPIO_PIN
#define ADC_CHANNEL0                    ADC_Channel_0

#define ADC_PIN1                        ADC_CH1_GPIO_PIN
#define ADC_CHANNEL1                    ADC_Channel_1

#define ADC_PIN2                       ADC_CH2_GPIO_PIN
#define ADC_CHANNEL2                    ADC_Channel_2

#define ADC_PIN3                       ADC_CH3_GPIO_PIN
#define ADC_CHANNEL3                    ADC_Channel_3

#define ADC_PIN4                       ADC_CH4_GPIO_PIN
#define ADC_CHANNEL4                    ADC_Channel_4

#define ADC_PIN5                       ADC_CH5_GPIO_PIN
#define ADC_CHANNEL5                    ADC_Channel_5

#define ADC_PIN6                       ADC_CH6_GPIO_PIN
#define ADC_CHANNEL6                    ADC_Channel_6

#define ADC_PIN7                       ADC_CH7_GPIO_PIN
#define ADC_CHANNEL7                    ADC_Channel_7

#define ADC_PIN8                       ADC_CH8_GPIO_PIN
#define ADC_CHANNEL8                    ADC_Channel_8

#define ADC_PIN9                       ADC_CH9_GPIO_PIN
#define ADC_CHANNEL9                    ADC_Channel_9

#define ADC_PIN10                       ADC_CH10_GPIO_PIN
#define ADC_CHANNEL10                    ADC_Channel_10

#define ADC_PIN11                       ADC_CH11_GPIO_PIN
#define ADC_CHANNEL11                    ADC_Channel_11

#define ADC_PIN12                       ADC_CH12_GPIO_PIN
#define ADC_CHANNEL12                    ADC_Channel_12

#define ADC_PIN13                       ADC_CH13_GPIO_PIN
#define ADC_CHANNEL13                    ADC_Channel_13

#define ADC_PIN14                       ADC_CH14_GPIO_PIN
#define ADC_CHANNEL14                    ADC_Channel_14

#define ADC_PIN15                       ADC_CH15_GPIO_PIN
#define ADC_CHANNEL15                    ADC_Channel_15

#define ADC_DMA_CHANNEL                DMA1_Channel1


/* 单通道采集采用中断读取数据 */
#define ADC_IRQ                        ADC1_2_IRQn
#define ADC_IRQHandler                 ADC1_2_IRQHandler

/* 多通道采集采用DMA总线读取数据 */


/**********************************ADC功能函数*****************************/

void ADC_Configuration(void);
float ADC_Independent_Single_GetValue(u8 channel_x);
void ADC_Independent_Muiti_GetVlaue(float *zdata);
void ADCx_Init(void);
#endif /* !BSP_ADC_H  */


