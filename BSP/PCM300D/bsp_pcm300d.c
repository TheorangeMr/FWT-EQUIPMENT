/*******************************************
	@file £º  bsp_pcm300d.c
	@anthor£º  ÂÞ³É
	@date£º   2022.07.26
	@version£º  v1.0
  @brief£º  ÒºÑ¹´«¸ÐÆ÷ÅäÖÃ
*******************************************/
#include "bsp_pcm300d.h"

float PCM300D_deal(uint8_t Channel)
{
	float adc_dat = 0;
	float PCM300D_dat = 0;
	adc_dat = ADC_Independent_Single_GetValue(Channel);
//	printf(" voltage value = %f V \r\n",adc_dat);
	PCM300D_dat = adc_dat*PCM300D_Constant_MPa - 0.04;
	return PCM300D_dat;
}
