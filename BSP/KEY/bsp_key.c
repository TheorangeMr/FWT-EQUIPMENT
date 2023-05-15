/*******************************************
	@file ��  bsp_key.c
	@anthor��  �޳�
	@date�� 2022.07.26
	@version��  v1.0
  @brief��  kay����
*******************************************/
#include "bsp_key.h"
#include "bsp_usart.h"
#include "bsp_delay.h"

void KEY_Config(void)
{
	GPIO_InitTypeDef  GPIO_InitStruct;
	// GPIOʹ��
	DEBUG_KEY_GPIO_APBxClkCmd (DEBUG_KEY1_GPIO_CLK|DEBUG_KEY2_GPIO_CLK|DEBUG_KEY3_GPIO_CLK|
														DEBUG_KEY4_GPIO_CLK|DEBUG_KEY5_GPIO_CLK , ENABLE);
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IPU;                //��������
	/*KEY1*/
	GPIO_InitStruct.GPIO_Pin = DEBUG_KEY1_GPIO_PIN;
	GPIO_Init(DEBUG_KEY1_GPIO_PORT, &GPIO_InitStruct);
	/*KEY2*/
	GPIO_InitStruct.GPIO_Pin = DEBUG_KEY2_GPIO_PIN;
	GPIO_Init(DEBUG_KEY2_GPIO_PORT, &GPIO_InitStruct);	
	/*KEY3*/
	GPIO_InitStruct.GPIO_Pin = DEBUG_KEY3_GPIO_PIN;
	GPIO_Init(DEBUG_KEY3_GPIO_PORT, &GPIO_InitStruct);	
	/*KEY4*/
	GPIO_InitStruct.GPIO_Pin = DEBUG_KEY4_GPIO_PIN;
	GPIO_Init(DEBUG_KEY4_GPIO_PORT, &GPIO_InitStruct);	
	/*KEY5*/
	GPIO_InitStruct.GPIO_Pin = DEBUG_KEY5_GPIO_PIN;
	GPIO_Init(DEBUG_KEY5_GPIO_PORT, &GPIO_InitStruct);
}

//����ɨ�躯��
uint8_t Key_Scan(uint8_t mode)
{
//	printf("\r\n 1\r\n");
	static u8 key_up=1;  //1��ʾ��һʱ�̰���δ�����£�0��ʾ��һʱ�̰����ѱ�����
	if(mode==1) key_up=1;//֧����������״̬����ʼ������
	if(key_up == 1)
	{
//		printf("\r\n 2\r\n");
		if(KEY1_READ==0 || KEY2_READ==0 || KEY3_READ==0 || KEY4_READ==0 || KEY5_READ==0)
		{
			delay_ms(20);//��ʱ����
			key_up=0;//�����ΰ����ѱ�����
			if(KEY1_READ == 0) return KEY1_VALUE;
			if(KEY2_READ == 0) return KEY2_VALUE;
			if(KEY3_READ == 0) return KEY3_VALUE;
			if(KEY4_READ == 0) return KEY4_VALUE;
			if(KEY5_READ == 0) return KEY5_VALUE;
			printf("\r\n 3 \r\n");
		}
	}
	else if(KEY1_READ==1 && KEY2_READ==1 && KEY3_READ==1 && KEY4_READ==1 && KEY5_READ==1) key_up=1;
	return 0xff;//����0xff��ʾû��KEY������
}

