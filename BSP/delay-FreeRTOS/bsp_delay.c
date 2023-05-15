/*******************************************
	*�ļ��� ��  bsp_delay.c
	*��   �ߣ�  �޳�
	*�޸�ʱ�䣺 2022.06.13
	*��   ����  v1.0
  *˵   ����  ��ʱ��������Դ�ļ�
*******************************************/

#include "bsp_delay.h"

static u8 fac_us = 0;	 //us��ʱ�������ӣ�1us��Ӧsystickʱ������
static u16 fac_ms = 0; //ms��ʱ�������ӣ�1ms��Ӧsystickʱ������


void Delay_Init(u8 SYSCLK)
{
	#if SYSTEM_SUPPORT_OS
	u32 reload;
	#endif
	SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK); //ѡ���ⲿʱ��  HCLK
	fac_us = SYSCLK;
	
	#if SYSTEM_SUPPORT_OS  							//�����Ҫ֧��OS.		
	reload = SYSCLK;
	reload*=1000000/configTICK_RATE_HZ;
	
	fac_ms = 1000/configTICK_RATE_HZ;
	SysTick->CTRL |= SysTick_CTRL_TICKINT_Msk;          //����SYSTICK�ж�
	SysTick->LOAD = reload;
	SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;          //����SYSTICK
	
	#else
		fac_ms=(u16)fac_us*1000;					//��OS��,����ÿ��ms��Ҫ��systickʱ����   
	#endif	
}
	


#if SYSTEM_SUPPORT_OS  							//�����Ҫ֧��OS.

void delay_us(u32 nus)
{
	u32 ticks;
	u32 told,tnow,tcnt=0;
	u32 reload=SysTick->LOAD;					//LOAD��ֵ	    	 
	ticks=nus*fac_us; 							//��Ҫ�Ľ�����
	told=SysTick->VAL;        					//�ս���ʱ�ļ�����ֵ
	while(1)
	{
		tnow=SysTick->VAL;	
		if(tnow!=told)
		{	    
			if(tnow<told)tcnt+=told-tnow;		//����ע��һ��SYSTICK��һ���ݼ��ļ������Ϳ�����.
			else tcnt+=reload-tnow+told;	    
			told=tnow;
			if(tcnt>=ticks)break;				//ʱ�䳬��/����Ҫ�ӳٵ�ʱ��,���˳�.
		}  
	};
}

//���ᱻ����������
void delay_xms(u32 nms)
{
	u32 i;
	for(i = 0;i<nms;i++)delay_us(1000);
}

//�ᱻ����������
void delay_ms(u16 nms)
{	
	if(xTaskGetSchedulerState()!=taskSCHEDULER_NOT_STARTED)	//���OS�Ѿ�������,���Ҳ������ж�����(�ж����治���������)	    
	{		 
		if(nms>=fac_ms)							//��ʱ��ʱ�����OS������ʱ������ 
		{ 
   			vTaskDelay(nms/fac_ms);		//OS��ʱ
		}
		nms%=fac_ms;							//OS�Ѿ��޷��ṩ��ôС����ʱ��,������ͨ��ʽ��ʱ    
	}
	delay_us((u32)(nms*1000));					//��ͨ��ʽ��ʱ  
}

#else //����OSʱ
//��ʱnus
//nusΪҪ��ʱ��us��.		    								   
void delay_us(u32 nus)
{		
	u32 temp;	    	 
	SysTick->LOAD=nus*fac_us; 					//ʱ�����	  		 
	SysTick->VAL=0x00;        					//��ռ�����
	SysTick->CTRL|=SysTick_CTRL_ENABLE_Msk ;	//��ʼ����	  
	do
	{
		temp=SysTick->CTRL;
	}while((temp&0x01)&&!(temp&(1<<16)));		//�ȴ�ʱ�䵽��   
	SysTick->CTRL&=~SysTick_CTRL_ENABLE_Msk;	//�رռ�����
	SysTick->VAL =0X00;      					 //��ռ�����	 
}
//��ʱnms
//ע��nms�ķ�Χ
//SysTick->LOADΪ24λ�Ĵ���,����,�����ʱΪ:
//nms<=0xffffff*8*1000/SYSCLK
//SYSCLK��λΪHz,nms��λΪms
//��72M������,nms<=1864 
void delay_ms(u16 nms)
{	 		  	  
	u32 temp;		   
	SysTick->LOAD=(u32)nms*fac_ms;				//ʱ�����(SysTick->LOADΪ24bit)
	SysTick->VAL =0x00;							//��ռ�����
	SysTick->CTRL|=SysTick_CTRL_ENABLE_Msk ;	//��ʼ����  
	do
	{
		temp=SysTick->CTRL;
	}while((temp&0x01)&&!(temp&(1<<16)));		//�ȴ�ʱ�䵽��   
	SysTick->CTRL&=~SysTick_CTRL_ENABLE_Msk;	//�رռ�����
	SysTick->VAL =0X00;       					//��ռ�����	  	    
}
#endif

