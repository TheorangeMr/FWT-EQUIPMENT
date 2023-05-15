/*******************************************
	@file ：  bsp_can.c
	@anthor：  罗成
	@date： 2022.04.21
	@version：  v1.0
  @brief：  USART源文件
*******************************************/
#include "bsp_usart.h"

/*串口GPIO配置*/
void USART_Gpio_Config(void)
{
	GPIO_InitTypeDef  GPIO_InitStruct;
	// GPIO使能
	DEBUG_USART_GPIO_APBxClkCmd(DEBUG_USART_GPIO_CLK , ENABLE);
	//串口GPIO使能
  DEBUG_USART_APBxClkCmd(DEBUG_USART_CLK , ENABLE);
	/*配置RX引脚*/
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN_FLOATING;     //浮空输入
	GPIO_InitStruct.GPIO_Pin = DEBUG_USART_RX_GPIO_PIN ;
	GPIO_Init(DEBUG_USART_RX_GPIO_PORT, &GPIO_InitStruct);
	
	
	/*配置TX引脚*/
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;      //复位推挽输出
	GPIO_InitStruct.GPIO_Pin = DEBUG_USART_TX_GPIO_PIN;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(DEBUG_USART_TX_GPIO_PORT, &GPIO_InitStruct);
}

/*串口配置*/
void DEBUG_USART_Confog(void)
{
	USART_InitTypeDef USART_InitStruct;
	//设置波特率 USART_BRR
	USART_InitStruct.USART_BaudRate = DEBUG_USART_BAUDRATE;     
	//硬件控制流选择
	USART_InitStruct.USART_HardwareFlowControl =USART_HardwareFlowControl_None ; 
  //接受，发送RE,TE使能
	USART_InitStruct.USART_Mode = USART_Mode_Tx| USART_Mode_Rx;    
  //校验位   USART_CR1:PCE,PS                       
	USART_InitStruct.USART_Parity = USART_Parity_No;
	//停止位 USART_CR2：STOP[1:0]
	USART_InitStruct.USART_StopBits = USART_StopBits_1;
	//数据长度  USART_CR1:M
	USART_InitStruct.USART_WordLength = USART_WordLength_8b;                      
	USART_Init(DEBUG_USARTx,&USART_InitStruct );

	//串口接受中断
	USART_ITConfig(DEBUG_USARTx, USART_IT_RXNE, ENABLE);
	//串口使能
	USART_Cmd(DEBUG_USARTx,ENABLE);
}


/*发送一个字节函数*/
void USART_SendByte(USART_TypeDef* USARTx ,uint8_t data)
{
	USART_SendData(DEBUG_USARTx,data);
	while(USART_GetFlagStatus(DEBUG_USARTx,USART_FLAG_TXE) == RESET);
}

///*在同步模式下配置串口时钟*/
//void USART_Clock_Config(void)
//{
//	USART_ClockInitTypeDef USART_ClockInitStruct;
//	
//	USART_ClockInitStruct.USART_Clock = 
//	USART_ClockInitStruct.USART_CPHA = 
//	USART_ClockInitStruct.USART_CPOL = 
//	USART_ClockInitStruct.USART_LastBit =
//	
//	USART_ClockInit(USART1, &USART_ClockInitStruct);
//}


//串口中断优先级配置
void USART_NVIC_Config(void)
{
	NVIC_InitTypeDef NVIC_InitStruct;
	NVIC_InitStruct.NVIC_IRQChannel = DEBUG_USART_IRQ;
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStruct.NVIC_IRQChannelSubPriority = 0;
	NVIC_Init(&NVIC_InitStruct);
}

void USART_Config(void)
{
	USART_Gpio_Config();
	USART_NVIC_Config();
	DEBUG_USART_Confog();

}

///重定向c库函数printf到串口，重定向后可使用printf函数
int fputc(int ch, FILE *f)
{
		/* 发送一个字节数据到串口 */
		USART_SendData(DEBUG_USARTx, (uint8_t) ch);
		
		/* 等待发送完毕 */
		while (USART_GetFlagStatus(DEBUG_USARTx, USART_FLAG_TXE) == RESET);		
	
		return (ch);
}


///重定向c库函数scanf到串口，重写向后可使用scanf、getchar等函数
int fgetc(FILE *f)
{
		/* 等待串口输入数据 */
		while (USART_GetFlagStatus(DEBUG_USARTx, USART_FLAG_RXNE) == RESET);

		return (int)USART_ReceiveData(DEBUG_USARTx);
}

