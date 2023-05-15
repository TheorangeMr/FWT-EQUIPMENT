/*******************************************
	@file ��  bsp_adc.c
	@anthor��  �޳�
	@date�� 2022.04.12
	@version��  v1.0
  @brief��  ADC����
*******************************************/
#include "bsp_adc.h"
#include "bsp_usart.h"

/* 
	���ʹ����ɨ��ģʽ��DMA������һ��ADC_Value[][]�������洢ADת�������
	��ADC1_SCAN_CHANNEL_NUM��ͨ����ÿ��ͨ������ADC_SMOOTHING�����ݡ�
*/
#if INDEPENDENT_MULTI_CHANNEL
float ADC_Value[ADCx_SCAN_CHANNEL_NUM][ADC_SMOOTHING] = {0};
vu16 ADC_ConvertedValue[ADCx_SCAN_CHANNEL_NUM]={0,0,0,0,0,0};
#endif

__IO float tempdata = 0;
	
#if INDEPENDENT_SINGGLE_CHANNEL
static void ADC_NVIC_Config(void)
{
	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);
	NVIC_InitStructure.NVIC_IRQChannel = ADC_IRQ;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}
#endif	
	

/* 
	*��������ADCx_GPIO_Config()
	*��  �ܣ�ADC_GPIO��ʼ������
	*��  �ߣ��޳�
	*��  ������
	*����ֵ����
	*ʱ  �䣺2022.08.01
*/

static void ADCx_GPIO_Config(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;
	ADC_GPIO_APBxClock_FUN(ADC_GPIO_CLK,ENABLE);
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;             	/* ����ADCͨ������Ϊģ������ */
	                              
	#if INDEPENDENT_MULTI_CHANNEL
	/*PA0~PA7*/
	GPIO_InitStructure.GPIO_Pin = ADC_PIN0|ADC_PIN1|ADC_PIN2|ADC_PIN3|ADC_PIN4|  
	                              ADC_PIN5|ADC_PIN6|ADC_PIN7;
	GPIO_Init(ADC_PORT_A , &GPIO_InitStructure);
	/*PB0~PB1*/
	GPIO_InitStructure.GPIO_Pin = ADC_PIN8|ADC_PIN9;
	GPIO_Init(ADC_PORT_B , &GPIO_InitStructure);
	/*PC0~PC5*/
	GPIO_InitStructure.GPIO_Pin = ADC_PIN10|ADC_PIN11|ADC_PIN12|ADC_PIN13|ADC_PIN14|ADC_PIN15;
	GPIO_Init(ADC_PORT_C , &GPIO_InitStructure);
	
	#elif INDEPENDENT_SINGGLE_CHANNEL
	/*PA0~PA7*/
	GPIO_InitStructure.GPIO_Pin = ADC_PIN0|ADC_PIN1|ADC_PIN2|ADC_PIN3|ADC_PIN4|  
	                              ADC_PIN5|ADC_PIN6|ADC_PIN7;
	GPIO_Init(ADC_PORT_A , &GPIO_InitStructure);
	/*PB0~PB1*/
	GPIO_InitStructure.GPIO_Pin = ADC_PIN8|ADC_PIN9;
	GPIO_Init(ADC_PORT_B , &GPIO_InitStructure);
	/*PC0~PC5*/
	GPIO_InitStructure.GPIO_Pin = ADC_PIN10|ADC_PIN11|ADC_PIN12|ADC_PIN13|ADC_PIN14|ADC_PIN15;
	GPIO_Init(ADC_PORT_C , &GPIO_InitStructure);
	#endif
}

/* 
	*��������ADC_Configuration()
	*��  �ܣ�ADC��ʼ������
	*��  �ߣ��޳�
	*��  ������
	*����ֵ����
	*ʱ  �䣺2022.08.01
*/
void ADC_Configuration(void)
{
	ADC_InitTypeDef   ADC_InitStructure;
	RCC_AHBPeriphClockCmd(ADC_DMA_CLK,ENABLE);	
	ADC_APBxClock_FUN(ADC_CLK , ENABLE);
	
	#if INDEPENDENT_MULTI_CHANNEL	
	/*****************************DMA ģʽ����***********************************/
	DMA_InitTypeDef   DMA_InitStructure;
	DMA_DeInit(ADC_DMA_CHANNEL);
	DMA_InitStructure.DMA_BufferSize = ADCx_SCAN_CHANNEL_NUM;
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
	DMA_InitStructure.DMA_MemoryBaseAddr = (u32)ADC_ConvertedValue;
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
	DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
	DMA_InitStructure.DMA_PeripheralBaseAddr = (u32)(&(ADCx->DR));
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	DMA_InitStructure.DMA_Priority = DMA_Priority_High;
	DMA_Init(ADC_DMA_CHANNEL,&DMA_InitStructure);
	DMA_Cmd(ADC_DMA_CHANNEL,ENABLE);
	#endif
	/*****************************ADC ģʽ����***********************************/
	/* ����ADC��Ƶ���� */
	RCC_ADCCLKConfig(RCC_PCLK2_Div6);
	ADC_DeInit(ADC1);
	
	/* ��ʼ��ADCת��ģʽ */
	#if INDEPENDENT_MULTI_CHANNEL
	ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;                      //�����ڶ���ģʽ
	ADC_InitStructure.ADC_ScanConvMode = ENABLE;                            //����ɨ��ģʽ
	ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;                      //��������ת��
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;     //ת��������������ⲿ��������
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;                  //�����Ҷ���
	ADC_InitStructure.ADC_NbrOfChannel = ADCx_SCAN_CHANNEL_NUM;             //ͨ������
	ADC_Init(ADCx, &ADC_InitStructure);

/* ���ù������ͨ������ */
	ADC_RegularChannelConfig(ADCx, ADC_CHANNEL1, 1 , ADC_SampleTime_55Cycles5);
	ADC_RegularChannelConfig(ADCx, ADC_CHANNEL2, 2 , ADC_SampleTime_55Cycles5);
	ADC_RegularChannelConfig(ADCx, ADC_CHANNEL3, 3 , ADC_SampleTime_55Cycles5);
	ADC_RegularChannelConfig(ADCx, ADC_CHANNEL4, 4 , ADC_SampleTime_55Cycles5);
	ADC_RegularChannelConfig(ADCx, ADC_CHANNEL5, 5 , ADC_SampleTime_55Cycles5);
	ADC_RegularChannelConfig(ADCx, ADC_CHANNEL6, 6 , ADC_SampleTime_55Cycles5);
	ADC_DMACmd(ADCx, ENABLE);
	
	#elif INDEPENDENT_SINGGLE_CHANNEL
	ADC_NVIC_Config();
	ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;                      //�����ڶ���ģʽ
	ADC_InitStructure.ADC_ScanConvMode = DISABLE;                           //������ɨ��ģʽ
	ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;                      //��������ת��
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;     //ת��������������ⲿ��������
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;                  //�����Ҷ���
	ADC_InitStructure.ADC_NbrOfChannel = 1;                                 //ͨ������Ϊ1
	ADC_Init(ADCx, &ADC_InitStructure);

	/* ���ù������ͨ������ */
	ADC_ITConfig(ADCx,ADC_IT_EOC,ENABLE);
	#endif
	
	ADC_Cmd(ADCx , ENABLE);                      //ʹ��ADC
	ADC_ResetCalibration(ADCx);	                 //ʹ�ܸ�λУ׼  
	while(ADC_GetResetCalibrationStatus(ADCx));	 //�ȴ���λУ׼����
	ADC_StartCalibration(ADCx);	                 //����ADУ׼
	while(ADC_GetCalibrationStatus(ADCx));	     //�ȴ�У׼����
}

#if INDEPENDENT_SINGGLE_CHANNEL

/* 
	*��������ADC_SingleMode_GetValue()
	*��  �ܣ���ȡ����ģʽADCֵ
	*��  �ߣ��޳�
	*��  ����channel_x:ADCͨ����
	*����ֵ����
	*ʱ  �䣺2022.08.01
*/
float ADC_Independent_Single_GetValue(u8 channel_x)
{
	float Min = 4095;
	float Sum,Average,Max = 0;
	
	/* ����ͨ������ */
	ADC_RegularChannelConfig(ADCx, channel_x, 1 , ADC_SampleTime_55Cycles5);
	
	/* ����˲����� */
	for(u8 i = 0; i < ADC_SMOOTHING; i++)
	{
		/* �������ת�� */
		ADC_SoftwareStartConvCmd(ADCx , ENABLE);
		
		Max = (Max>tempdata)?Max:tempdata;
		Min = (Min<tempdata)?Min:tempdata;
		Sum += tempdata;
	}
	
	/* ȥ�����ֵ����Сֵ����ƽ��ֵ */
	Average = (Sum-Max-Min)/(ADC_SMOOTHING-2);
	
	return Average;
}


/*ADC�жϷ�����*/
void ADC_IRQHandler(void)
{
		/* �ȴ�ת����� */
	if(ADC_GetITStatus(ADCx,ADC_IT_EOC) == SET)
	{
		/* ��ȡ���� */
		tempdata = (float)ADC_GetConversionValue(ADCx)/4096*3.3;
//		printf("2");
	}
	ADC_ClearITPendingBit(ADCx,ADC_IT_EOC);
}

#endif

/* 
	*��������ADC_ScanMode_GetVlaue()
	*��  �ܣ���ȡɨ��ģʽADCֵ
	*��  �ߣ��޳�
	*��  ����*zdata������ָ��
	*����ֵ����
	*ʱ  �䣺2022.08.01
*/

#if INDEPENDENT_MULTI_CHANNEL

void ADC_Independent_Muiti_GetVlaue(float *zdata)
{
	float Sum,Max= 0;
	float Min = 4095;

	/* �������ת�� */
	ADC_SoftwareStartConvCmd(ADCx , ENABLE);
	
	/* �ȴ�ת����� */
	//while(ADC_GetFlagStatus(ADC1 , ADC_FLAG_EOC) == RESET);
	//���ڵȴ�ת����ɣ���ʹ��ʵʱ����ϵͳʱ������ʹ����ʱ��������CPUʹ��Ȩ��
	//�Ӷ������γ�����
//	vTaskDelay(10);
	
	u8 i,j = 0;
	for(i = 0 ; i< ADC_SMOOTHING; i++)
	{
		for(j = 0 ; j< ADCx_SCAN_CHANNEL_NUM; j++)
		{
			ADC_Value[j][i] =(float) ADC_ConvertedValue[j]/4096*3.3;
		}
	}
	/* ����˲�����ֵ�˲��� */ 
	for(i = 0; i < ADCx_SCAN_CHANNEL_NUM; i++)
	{
		Sum = 0;
		Max = 0;
		Min = 4095;
		for(j = 0; j < ADC_SMOOTHING; j++)
		{
			Max = (Max>ADC_Value[i][j])?Max:ADC_Value[i][j];
		  Min = (Min<ADC_Value[i][j])?Min:ADC_Value[i][j];
			Sum += ADC_Value[i][j];
		}
		zdata[i] = (Sum-Min-Max)/(ADC_SMOOTHING-2);
	}
}

#endif


/**
  * @brief  ADC��ʼ��
  * @param  ��
  * @retval ��
  */
void ADCx_Init(void)
{
	ADCx_GPIO_Config();
	ADC_Configuration();
}



