/*******************************************
	@file ：  bsp_adc.c
	@anthor：  罗成
	@date： 2022.04.12
	@version：  v1.0
  @brief：  ADC配置
*******************************************/
#include "bsp_adc.h"
#include "bsp_usart.h"

/* 
	如果使能了扫描模式和DMA，则定义一个ADC_Value[][]数组来存储AD转换结果，
	共ADC1_SCAN_CHANNEL_NUM个通道，每个通道保存ADC_SMOOTHING组数据。
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
	*函数名：ADCx_GPIO_Config()
	*功  能：ADC_GPIO初始化函数
	*作  者：罗成
	*参  数：无
	*返回值：无
	*时  间：2022.08.01
*/

static void ADCx_GPIO_Config(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;
	ADC_GPIO_APBxClock_FUN(ADC_GPIO_CLK,ENABLE);
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;             	/* 设置ADC通道引脚为模拟输入 */
	                              
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
	*函数名：ADC_Configuration()
	*功  能：ADC初始化函数
	*作  者：罗成
	*参  数：无
	*返回值：无
	*时  间：2022.08.01
*/
void ADC_Configuration(void)
{
	ADC_InitTypeDef   ADC_InitStructure;
	RCC_AHBPeriphClockCmd(ADC_DMA_CLK,ENABLE);	
	ADC_APBxClock_FUN(ADC_CLK , ENABLE);
	
	#if INDEPENDENT_MULTI_CHANNEL	
	/*****************************DMA 模式配置***********************************/
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
	/*****************************ADC 模式配置***********************************/
	/* 设置ADC分频因子 */
	RCC_ADCCLKConfig(RCC_PCLK2_Div6);
	ADC_DeInit(ADC1);
	
	/* 初始化ADC转换模式 */
	#if INDEPENDENT_MULTI_CHANNEL
	ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;                      //工作在独立模式
	ADC_InitStructure.ADC_ScanConvMode = ENABLE;                            //启动扫描模式
	ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;                      //启动连续转换
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;     //转换由软件而不是外部触发启动
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;                  //数据右对齐
	ADC_InitStructure.ADC_NbrOfChannel = ADCx_SCAN_CHANNEL_NUM;             //通道数量
	ADC_Init(ADCx, &ADC_InitStructure);

/* 配置规则组各通道参数 */
	ADC_RegularChannelConfig(ADCx, ADC_CHANNEL1, 1 , ADC_SampleTime_55Cycles5);
	ADC_RegularChannelConfig(ADCx, ADC_CHANNEL2, 2 , ADC_SampleTime_55Cycles5);
	ADC_RegularChannelConfig(ADCx, ADC_CHANNEL3, 3 , ADC_SampleTime_55Cycles5);
	ADC_RegularChannelConfig(ADCx, ADC_CHANNEL4, 4 , ADC_SampleTime_55Cycles5);
	ADC_RegularChannelConfig(ADCx, ADC_CHANNEL5, 5 , ADC_SampleTime_55Cycles5);
	ADC_RegularChannelConfig(ADCx, ADC_CHANNEL6, 6 , ADC_SampleTime_55Cycles5);
	ADC_DMACmd(ADCx, ENABLE);
	
	#elif INDEPENDENT_SINGGLE_CHANNEL
	ADC_NVIC_Config();
	ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;                      //工作在独立模式
	ADC_InitStructure.ADC_ScanConvMode = DISABLE;                           //不启动扫描模式
	ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;                      //启动连续转换
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;     //转换由软件而不是外部触发启动
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;                  //数据右对齐
	ADC_InitStructure.ADC_NbrOfChannel = 1;                                 //通道数量为1
	ADC_Init(ADCx, &ADC_InitStructure);

	/* 配置规则组各通道参数 */
	ADC_ITConfig(ADCx,ADC_IT_EOC,ENABLE);
	#endif
	
	ADC_Cmd(ADCx , ENABLE);                      //使能ADC
	ADC_ResetCalibration(ADCx);	                 //使能复位校准  
	while(ADC_GetResetCalibrationStatus(ADCx));	 //等待复位校准结束
	ADC_StartCalibration(ADCx);	                 //开启AD校准
	while(ADC_GetCalibrationStatus(ADCx));	     //等待校准结束
}

#if INDEPENDENT_SINGGLE_CHANNEL

/* 
	*函数名：ADC_SingleMode_GetValue()
	*功  能：获取单次模式ADC值
	*作  者：罗成
	*参  数：channel_x:ADC通道号
	*返回值：无
	*时  间：2022.08.01
*/
float ADC_Independent_Single_GetValue(u8 channel_x)
{
	float Min = 4095;
	float Sum,Average,Max = 0;
	
	/* 配置通道参数 */
	ADC_RegularChannelConfig(ADCx, channel_x, 1 , ADC_SampleTime_55Cycles5);
	
	/* 软件滤波采样 */
	for(u8 i = 0; i < ADC_SMOOTHING; i++)
	{
		/* 开启软件转换 */
		ADC_SoftwareStartConvCmd(ADCx , ENABLE);
		
		Max = (Max>tempdata)?Max:tempdata;
		Min = (Min<tempdata)?Min:tempdata;
		Sum += tempdata;
	}
	
	/* 去掉最大值和最小值再求平均值 */
	Average = (Sum-Max-Min)/(ADC_SMOOTHING-2);
	
	return Average;
}


/*ADC中断服务函数*/
void ADC_IRQHandler(void)
{
		/* 等待转换完成 */
	if(ADC_GetITStatus(ADCx,ADC_IT_EOC) == SET)
	{
		/* 读取数据 */
		tempdata = (float)ADC_GetConversionValue(ADCx)/4096*3.3;
//		printf("2");
	}
	ADC_ClearITPendingBit(ADCx,ADC_IT_EOC);
}

#endif

/* 
	*函数名：ADC_ScanMode_GetVlaue()
	*功  能：获取扫描模式ADC值
	*作  者：罗成
	*参  数：*zdata：数据指针
	*返回值：无
	*时  间：2022.08.01
*/

#if INDEPENDENT_MULTI_CHANNEL

void ADC_Independent_Muiti_GetVlaue(float *zdata)
{
	float Sum,Max= 0;
	float Min = 4095;

	/* 开启软件转换 */
	ADC_SoftwareStartConvCmd(ADCx , ENABLE);
	
	/* 等待转换完成 */
	//while(ADC_GetFlagStatus(ADC1 , ADC_FLAG_EOC) == RESET);
	//对于等待转换完成，在使用实时操作系统时，可以使用延时函数交出CPU使用权，
	//从而不会形成阻塞
//	vTaskDelay(10);
	
	u8 i,j = 0;
	for(i = 0 ; i< ADC_SMOOTHING; i++)
	{
		for(j = 0 ; j< ADCx_SCAN_CHANNEL_NUM; j++)
		{
			ADC_Value[j][i] =(float) ADC_ConvertedValue[j]/4096*3.3;
		}
	}
	/* 软件滤波（均值滤波） */ 
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
  * @brief  ADC初始化
  * @param  无
  * @retval 无
  */
void ADCx_Init(void)
{
	ADCx_GPIO_Config();
	ADC_Configuration();
}



