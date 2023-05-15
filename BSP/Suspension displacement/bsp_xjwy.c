/*******************************************
	@file £∫  bsp_xjwy.c
	@anthor£∫  ¬ﬁ≥…
	@date£∫ 2022.07.26
	@version£∫  v1.0
  @brief£∫  xjwy≈‰÷√
*******************************************/
#include "bsp_xjwy.h"



uint16_t XJWY_deal(uint8_t Channel)
{
	float adc_dat = 0;
	float XJWY_dat = 0;
	adc_dat = ADC_Independent_Single_GetValue(Channel);
//	printf(" voltage value = %f V \r\n",adc_dat);
	
	XJWY_dat = adc_dat*xjwy_datedeal;
	return XJWY_dat;
}
