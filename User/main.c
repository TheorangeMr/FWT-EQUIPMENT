/**
	*******************************************************************************
	@file ��  FWT_BAJA_Text
	@author�� �޳�
	@version��v1.0
	@date��   2022.08.25
  @brief��  �ɽ�������λ�ƣ��ƶ���ѹ���ƶ����¶ȣ�������ٶȲ���,���ֲ���
	*******************************************************************************
*/

#include "stm32f10x.h"                  // Device header

#include "FreeRTOS.h"
#include "task.h"
#include "event_groups.h"

#include "bsp_adc.h"
#include "bsp_usart.h"
#include "bsp_pcm300d.h"
#include "bsp_smbus_temperature.h"
#include "bsp_i2c_mpu6050.h"
#include "bsp_led.h"
#include "bsp_key.h"
#include "bsp_xjwy.h"
#include "mpu6050.h"
#include "bsp_delay.h"


/********************************* �¼� ****************************************/

EventGroupHandle_t Event_Handle = NULL;	//�¼���־����

#define EVENTBIT_0	(1<<0)                          //�ƶ���ѹ�����¼�
#define EVENTBIT_1	(1<<1)                          //�ƶ����¶Ȳ����¼�
#define EVENTBIT_2	(1<<2)                          //����λ�Ʋ����¼�
#define EVENTBIT_3	(1<<3)                          //������ٶȲ����¼�
#define EVENTBIT_4	(1<<4)                          //Ӧ�������¼�
#define EVENTBIT_5	(1<<5)                          //���ֲ����¼�

/*
*************************************************************************
*                             ��������ʼ��
*************************************************************************
*/

static TaskHandle_t AppTaskCreate_Handle = NULL;
static TaskHandle_t KEY_Task_Handle = NULL;
static TaskHandle_t Brake_Disc_Temperature_Task_Handle = NULL;
static TaskHandle_t Three_Axis_Acceleration_Task_Handle = NULL;
static TaskHandle_t Suspension_Displacement_Task_Handle = NULL;
static TaskHandle_t Brake_Oil_Pressure_Task_Handle = NULL;

/*
*************************************************************************
*                             �����ջ����
*************************************************************************
*/


/* �������������ջ */
static StackType_t Idle_Task_Stack[configMINIMAL_STACK_SIZE];
/* ��ʱ�����������ջ */
static StackType_t Timer_Task_Stack[configTIMER_TASK_STACK_DEPTH];

/* ���崴������ջ */
static StackType_t AppTaskCreate_Stack[128];
/* ���尴������ջ */
static StackType_t KEY_Task_Stack[64];
/*
*************************************************************************
*                             ���ƿ�����
*************************************************************************
*/

/* ����������ƿ� */
static StaticTask_t Idle_Task_TCB;
/* ��ʱ��������ƿ� */
static StaticTask_t Timer_Task_TCB;
/* ���崴��������ƿ� */
static StaticTask_t AppTaskCreate_TCB;
/* ����������ƿ� */
static StaticTask_t KEY_Task_TCB;
/*
*************************************************************************
*                             ��������
*************************************************************************
*/


static void AppTaskCreate(void);                                           /* ���ڴ������� */
static void BSP_Init(void);
static void KEY_Task(void* parameter);
static void Brake_Disc_Temperature_Task(void* parameter);
static void Three_Axis_Acceleration_Task(void* parameter);
static void Suspension_Displacement_Task(void* parameter);
static void Brake_Oil_Pressure_Task(void* parameter);


/*
*************************************************************************
*                             ȫ�ֱ���
*************************************************************************
*/

uint8_t KeyValue = 0;

/****************************************************************************/

/* ��ȡ����������ڴ� */

void vApplicationGetIdleTaskMemory(StaticTask_t **ppxIdleTaskTCBBuffer,
	                                 StackType_t **ppxIdleTaskStackBuffer,
                                    uint32_t *pulIdleTaskStackSize)
{
	*ppxIdleTaskTCBBuffer = &Idle_Task_TCB;
	*ppxIdleTaskStackBuffer = Idle_Task_Stack;
	*pulIdleTaskStackSize = configMINIMAL_STACK_SIZE;
}

/* ��ȡ��ʱ��������ڴ� */

void vApplicationGetTimerTaskMemory(StaticTask_t **ppxTimerTaskTCBBuffer,
	                                 StackType_t **ppxTimerTaskStackBuffer,
                                    uint32_t *pulTimerTaskStackSize)
{
	*ppxTimerTaskTCBBuffer = &Timer_Task_TCB;
	*ppxTimerTaskStackBuffer = Timer_Task_Stack;
	*pulTimerTaskStackSize = configTIMER_TASK_STACK_DEPTH;
}

/*****************************************************************
  * @brief  ������
  * @param  ��
  * @retval ��
  * @note   ��һ����������Ӳ����ʼ�� 
            �ڶ���������APPӦ������
            ������������FreeRTOS����ʼ���������
  ****************************************************************/
int main(void)
{
	BSP_Init();
  printf("�����ӵ�������԰庯������!\r\n");
	
	AppTaskCreate_Handle = xTaskCreateStatic((TaskFunction_t	)AppTaskCreate,		  //������
															(const char* 	)"AppTaskCreate",		                //��������
															(uint32_t 		)128,	                              //�����ջ��С
															(void* 		  	)NULL,				                      //���ݸ��������Ĳ���
															(UBaseType_t 	)3, 	                              //�������ȼ�
															(StackType_t*   )AppTaskCreate_Stack,	            //�����ջ
															(StaticTask_t*  )&AppTaskCreate_TCB);	            //������ƿ� 
  /* ����������� */          
	if(NULL != AppTaskCreate_Handle)                                              /* �����ɹ� */
    vTaskStartScheduler();
	else
		printf("����ʧ��");
  while(1);   /* ��������ִ�е����� */
}


/**********************************************************************
  * @ ������  �� BSP_Task
  * @ ����˵���� BSP_Task��������
  * @ ����    ��   
  * @ ����ֵ  �� ��
  ********************************************************************/

void BSP_Init(void)
{
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	Delay_Init(72);
	ADCx_Init();
	USART_Config();
	SMBus_GPIO_Init();
	i2c_GPIO_Config();
	MPU6050_Init();
	LED_Config();
	KEY_Config();
}

/***********************************************************************
  * @ ������  �� AppTaskCreate
  * @ ����˵���� Ϊ�˷���������е����񴴽����������������������
  * @ ����    �� ��  
  * @ ����ֵ  �� ��
  **********************************************************************/
static void AppTaskCreate(void)
{
	BaseType_t xReturn = pdPASS;     /* ����һ��������Ϣ����ֵ��Ĭ��ΪpdPASS */	
  taskENTER_CRITICAL();           //�����ٽ���
  
	Event_Handle = xEventGroupCreate();
	if(NULL != Event_Handle)
	printf("Event_Handle �¼������ɹ���");
	xEventGroupClearBits( Event_Handle, EVENTBIT_0|EVENTBIT_1|EVENTBIT_2|EVENTBIT_3|EVENTBIT_4);
  /* ����KEY_Task_Stack���� */
	KEY_Task_Handle = xTaskCreateStatic((TaskFunction_t	)KEY_Task,		            //������
															(const char* 	)"KEY_Task",		                    //��������
															(uint32_t 		)64,					                      //�����ջ��С
															(void* 		  	)NULL,				                      //���ݸ��������Ĳ���
															(UBaseType_t 	)10, 				                        //�������ȼ�
															(StackType_t*   )KEY_Task_Stack,	                //�����ջ
															(StaticTask_t*  )&KEY_Task_TCB);	                //������ƿ�	
	if(NULL != KEY_Task_Handle)                                                   /* �����ɹ� */
		printf("KEY_Task���񴴽��ɹ�!\n");
	else
		printf("KEY_Task���񴴽�ʧ��!\n");
							
	  /* ����Brake_Oil_Pressure_Task���� */
	xReturn = xTaskCreate((TaskFunction_t	)Brake_Oil_Pressure_Task,		            //������
															(const char* 	)"Brake_Oil_Pressure_Task",		      //��������
															(uint16_t 		)128,					                      //�����ջ��С
															(void* 		  	)NULL,				                      //���ݸ��������Ĳ���
															(UBaseType_t 	)8, 				                        //�������ȼ�
															(TaskHandle_t*  )&Brake_Oil_Pressure_Task_Handle);	      //������ƿ�ָ��
	if(pdPASS == xReturn) 																												/* �����ɹ� */
		printf("Brake_Oil_Pressure_Task���񴴽��ɹ�!\r\n");
	else
		printf("Brake_Oil_Pressure_Task���񴴽�ʧ��!\r\n");
	
	  /* ����Suspension_Displacement_Task���� */
	xReturn = xTaskCreate((TaskFunction_t	)Suspension_Displacement_Task,		            //������
															(const char* 	)"Suspension_Displacement_Task",		      //��������
															(uint16_t 		)128,					                      //�����ջ��С
															(void* 		  	)NULL,				                      //���ݸ��������Ĳ���
															(UBaseType_t 	)5, 				                        //�������ȼ�
															(TaskHandle_t*  )&Suspension_Displacement_Task_Handle);	      //������ƿ�ָ��
	if(pdPASS == xReturn) 																												/* �����ɹ� */
		printf("Suspension_Displacement_Task���񴴽��ɹ�!\r\n");
	else
		printf("Suspension_Displacement_Task���񴴽�ʧ��!\r\n");

	  /* ����Three_Axis_Acceleration_Task���� */
	xReturn = xTaskCreate((TaskFunction_t	)Three_Axis_Acceleration_Task,		            //������
															(const char* 	)"Three_Axis_Acceleration_Task",		      //��������
															(uint16_t 		)128,					                      //�����ջ��С
															(void* 		  	)NULL,				                      //���ݸ��������Ĳ���
															(UBaseType_t 	)4, 				                        //�������ȼ�
															(TaskHandle_t*  )&Three_Axis_Acceleration_Task_Handle);	      //������ƿ�ָ��
	if(pdPASS == xReturn) 																												/* �����ɹ� */
		printf("Three_Axis_Acceleration_Task���񴴽��ɹ�!\r\n");
	else
		printf("Three_Axis_Acceleration_Task���񴴽�ʧ��!\r\n");

	  /* ����Brake_Disc_Temperature_Task���� */
	xReturn = xTaskCreate((TaskFunction_t	)Brake_Disc_Temperature_Task,		            //������
															(const char* 	)"Brake_Disc_Temperature_Task",		      //��������
															(uint16_t 		)128,					                      //�����ջ��С
															(void* 		  	)NULL,				                      //���ݸ��������Ĳ���
															(UBaseType_t 	)6, 				                        //�������ȼ�
															(TaskHandle_t*  )&Brake_Disc_Temperature_Task_Handle);	      //������ƿ�ָ��
	if(pdPASS == xReturn) 																												/* �����ɹ� */
		printf("Brake_Disc_Temperature_Task���񴴽��ɹ�!\r\n");
	else
		printf("Brake_Disc_Temperature_Task���񴴽�ʧ��!\r\n");
															
  vTaskDelete(AppTaskCreate_Handle); //ɾ��AppTaskCreate����
  taskEXIT_CRITICAL();                //�˳��ٽ���
}


/**********************************************************************
  * @ ������  �� KEY_Task
  * @ ����˵���� ͨ�������л����Թ���
  * @ ����    ��   
  * @ ����ֵ  �� ��
  ********************************************************************/
static void KEY_Task(void* parameter)
{
	BaseType_t xResult;
	while(1)
  {
		KeyValue = 0xff;
		KeyValue = Key_Scan(0);
	  /*�¼���λ*/
		switch(KeyValue)
		{
			case(0):
			{
				GPIO_ResetBits(DEBUG_LED_GPIO_PORT, DEBUG_LED1_GPIO_PIN);
				GPIO_SetBits(DEBUG_LED_GPIO_PORT, (DEBUG_LED2_GPIO_PIN|DEBUG_LED3_GPIO_PIN|
	                     DEBUG_LED4_GPIO_PIN|DEBUG_LED5_GPIO_PIN));
				xEventGroupClearBits( Event_Handle, EVENTBIT_0|EVENTBIT_1|EVENTBIT_2|EVENTBIT_3|EVENTBIT_4);
    		xResult = xEventGroupSetBits(Event_Handle,EVENTBIT_0);
				printf("\r\n ����0\r\n");
				break;
			}
			case(1):
			{
				GPIO_ResetBits(DEBUG_LED_GPIO_PORT, DEBUG_LED2_GPIO_PIN);
				GPIO_SetBits(DEBUG_LED_GPIO_PORT, (DEBUG_LED1_GPIO_PIN|DEBUG_LED3_GPIO_PIN|
	                     DEBUG_LED4_GPIO_PIN|DEBUG_LED5_GPIO_PIN));
				xEventGroupClearBits( Event_Handle, EVENTBIT_0|EVENTBIT_1|EVENTBIT_2|EVENTBIT_3|EVENTBIT_4);
    		xResult = xEventGroupSetBits(Event_Handle,EVENTBIT_1);
				printf("\r\n ����1\r\n");
				break;
			}
			case(2):
			{
				GPIO_ResetBits(DEBUG_LED_GPIO_PORT, DEBUG_LED3_GPIO_PIN);
				GPIO_SetBits(DEBUG_LED_GPIO_PORT, (DEBUG_LED2_GPIO_PIN|DEBUG_LED1_GPIO_PIN|
	                     DEBUG_LED4_GPIO_PIN|DEBUG_LED5_GPIO_PIN));
				xEventGroupClearBits( Event_Handle, EVENTBIT_0|EVENTBIT_1|EVENTBIT_2|EVENTBIT_3|EVENTBIT_4);
    		xResult = xEventGroupSetBits(Event_Handle,EVENTBIT_2);
				printf("\r\n ����2\r\n");
				break;
			}
			case(3):
			{
				GPIO_ResetBits(DEBUG_LED_GPIO_PORT, DEBUG_LED4_GPIO_PIN);
				GPIO_SetBits(DEBUG_LED_GPIO_PORT, (DEBUG_LED2_GPIO_PIN|DEBUG_LED3_GPIO_PIN|
	                     DEBUG_LED1_GPIO_PIN|DEBUG_LED5_GPIO_PIN));
				xEventGroupClearBits( Event_Handle, EVENTBIT_0|EVENTBIT_1|EVENTBIT_2|EVENTBIT_3|EVENTBIT_4);
    		xResult = xEventGroupSetBits(Event_Handle,EVENTBIT_3);
				printf("\r\n ����3\r\n");
				break;
			}
			default:
				break;
		}
		vTaskDelay(100);
	}
}


/**********************************************************************
  * @ ������  �� Brake_Oil_Pressure_Task
  * @ ����˵���� �����ƶ���ѹ
  * @ ����    ��   
  * @ ����ֵ  �� ��
  ********************************************************************/

static void Brake_Oil_Pressure_Task(void* parameter)
{
	EventBits_t r_event;
	float dat1,dat2 = 1;
	while(1)
	{
		r_event = xEventGroupWaitBits(Event_Handle,EVENTBIT_0,pdFALSE,pdTRUE,portMAX_DELAY);
		if((r_event&EVENTBIT_0) == EVENTBIT_0)
		{	
			dat1 = PCM300D_deal(ADC_CHANNEL0);
			dat2 = PCM300D_deal(ADC_CHANNEL1);
			printf(" PCM300D value1 = %.2f MPa, value2 = %.2f MPa \r\n",dat1,dat2);
			printf("\r\n\r\n");
		}
		vTaskDelay(1000);
	}
}


/**********************************************************************
  * @ ������  �� Suspension_Displacement_Task
  * @ ����˵���� ��������λ��
  * @ ����    ��   
  * @ ����ֵ  �� ��
  ********************************************************************/

static void Suspension_Displacement_Task(void* parameter)
{
	uint16_t dat1,dat2,dat3,dat4 = 0;
	EventBits_t r_event;
	while(1)
	{
		r_event = xEventGroupWaitBits(Event_Handle,EVENTBIT_2,pdFALSE,pdTRUE,portMAX_DELAY);
		if((r_event&EVENTBIT_2) == EVENTBIT_2)
		{
			dat1 = XJWY_deal(ADC_CHANNEL2);
			dat2 = XJWY_deal(ADC_CHANNEL3);
			dat3 = XJWY_deal(ADC_CHANNEL4);
			dat4 = XJWY_deal(ADC_CHANNEL5);
			printf(" XJWY value1 = %d mm, value2 = %d mm \r\n",dat1,dat2);
			printf(" XJWY value3 = %d mm, value4 = %d mm \r\n",dat3,dat4);
			printf("\r\n\r\n");
		}
		vTaskDelay(1000);
	}
}

/**********************************************************************
  * @ ������  �� Three_Axis_Acceleration_Task
  * @ ����˵��������������ٶ� 
  * @ ����    ��   
  * @ ����ֵ  �� ��
  ********************************************************************/

static void Three_Axis_Acceleration_Task(void* parameter)
{
	short Accel[3] = {0};
	float Temp = 0;
	EventBits_t r_event;
	while(1)
	{
		r_event = xEventGroupWaitBits(Event_Handle,EVENTBIT_3,pdFALSE,pdTRUE,portMAX_DELAY);
		if((r_event&EVENTBIT_3) == EVENTBIT_3)
		{
			MPU6050ReadAcc(Accel);
			printf("\r\n���ٶȣ� x = %-6.3fg��y = %-6.3fg��z = %-6.3fg",Accel[0]*1.0/16384,Accel[1]*1.0/16384,Accel[2]*1.0/16384);
			MPU6050_ReturnTemp(&Temp);
			printf("\r\n�¶ȣ� %8.2f \r\n",Temp);
		}
		vTaskDelay(1000);
	}
}

/**********************************************************************
  * @ ������  �� Brake_Disc_Temperature_Task
  * @ ����˵���� �����ƶ����¶�
  * @ ����    ��   
  * @ ����ֵ  �� ��
  ********************************************************************/

static void Brake_Disc_Temperature_Task(void* parameter)
{
	EventBits_t r_event;
  SMBUS_SCK_L();
	delay_xms(3);
  SMBUS_SCK_H();
/*********************�޸ĵ�ַ*****************************/
//	u8 SMBus_ID = 0x36;
//	SMBus_WriteMemory(SA, EEPROM_ACCESS|EEPROM_SMBus , SMBus_ID);
/**********************************************************/	
	while(1)
	{
		r_event = xEventGroupWaitBits(Event_Handle,EVENTBIT_1,pdFALSE,pdTRUE,portMAX_DELAY);
		if((r_event&EVENTBIT_1) == EVENTBIT_1)
		{
			printf("�������¶ȴ�����-1\r\n");
			printf("Body_temporature = %f \r\n", SMBus_Read_Body_Temperature(SA_1));
			printf("Enviroment_temporature = %f \r\n", SMBus_Read_Enviroment_Temperature(SA_1));		
			vTaskDelay(100);
			printf("�������¶ȴ�����-2\r\n");
			printf("Body_temporature = %f \r\n", SMBus_Read_Body_Temperature(SA_2));
			printf("Enviroment_temporature = %f \r\n", SMBus_Read_Enviroment_Temperature(SA_2));		
			vTaskDelay(100);
			printf("�������¶ȴ�����-3\r\n");
			printf("Body_temporature = %f \r\n", SMBus_Read_Body_Temperature(SA_3));
			printf("Enviroment_temporature = %f \r\n", SMBus_Read_Enviroment_Temperature(SA_3));	
			vTaskDelay(100);
		}
	}
}

