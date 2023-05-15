/*******************************************
	@file ��  bsp_can.c
	@anthor��  �޳�
	@date�� 2022.04.21
	@version��  v1.0
  @brief��  USARTԴ�ļ�
*******************************************/
#include "bsp_usart.h"

/*����GPIO����*/
void USART_Gpio_Config(void)
{
	GPIO_InitTypeDef  GPIO_InitStruct;
	// GPIOʹ��
	DEBUG_USART_GPIO_APBxClkCmd(DEBUG_USART_GPIO_CLK , ENABLE);
	//����GPIOʹ��
  DEBUG_USART_APBxClkCmd(DEBUG_USART_CLK , ENABLE);
	/*����RX����*/
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN_FLOATING;     //��������
	GPIO_InitStruct.GPIO_Pin = DEBUG_USART_RX_GPIO_PIN ;
	GPIO_Init(DEBUG_USART_RX_GPIO_PORT, &GPIO_InitStruct);
	
	
	/*����TX����*/
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;      //��λ�������
	GPIO_InitStruct.GPIO_Pin = DEBUG_USART_TX_GPIO_PIN;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(DEBUG_USART_TX_GPIO_PORT, &GPIO_InitStruct);
}

/*��������*/
void DEBUG_USART_Confog(void)
{
	USART_InitTypeDef USART_InitStruct;
	//���ò����� USART_BRR
	USART_InitStruct.USART_BaudRate = DEBUG_USART_BAUDRATE;     
	//Ӳ��������ѡ��
	USART_InitStruct.USART_HardwareFlowControl =USART_HardwareFlowControl_None ; 
  //���ܣ�����RE,TEʹ��
	USART_InitStruct.USART_Mode = USART_Mode_Tx| USART_Mode_Rx;    
  //У��λ   USART_CR1:PCE,PS                       
	USART_InitStruct.USART_Parity = USART_Parity_No;
	//ֹͣλ USART_CR2��STOP[1:0]
	USART_InitStruct.USART_StopBits = USART_StopBits_1;
	//���ݳ���  USART_CR1:M
	USART_InitStruct.USART_WordLength = USART_WordLength_8b;                      
	USART_Init(DEBUG_USARTx,&USART_InitStruct );

	//���ڽ����ж�
	USART_ITConfig(DEBUG_USARTx, USART_IT_RXNE, ENABLE);
	//����ʹ��
	USART_Cmd(DEBUG_USARTx,ENABLE);
}


/*����һ���ֽں���*/
void USART_SendByte(USART_TypeDef* USARTx ,uint8_t data)
{
	USART_SendData(DEBUG_USARTx,data);
	while(USART_GetFlagStatus(DEBUG_USARTx,USART_FLAG_TXE) == RESET);
}

///*��ͬ��ģʽ�����ô���ʱ��*/
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


//�����ж����ȼ�����
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

///�ض���c�⺯��printf�����ڣ��ض�����ʹ��printf����
int fputc(int ch, FILE *f)
{
		/* ����һ���ֽ����ݵ����� */
		USART_SendData(DEBUG_USARTx, (uint8_t) ch);
		
		/* �ȴ�������� */
		while (USART_GetFlagStatus(DEBUG_USARTx, USART_FLAG_TXE) == RESET);		
	
		return (ch);
}


///�ض���c�⺯��scanf�����ڣ���д����ʹ��scanf��getchar�Ⱥ���
int fgetc(FILE *f)
{
		/* �ȴ������������� */
		while (USART_GetFlagStatus(DEBUG_USARTx, USART_FLAG_RXNE) == RESET);

		return (int)USART_ReceiveData(DEBUG_USARTx);
}

