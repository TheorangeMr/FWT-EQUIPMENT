/**
	*******************************************************************************
	@file ：  FWT_BAJA_Text
	@author： 罗成
	@version：v1.0
	@date：   2022.08.25
  @brief：  可进行悬架位移，制动油压，制动盘温度，三轴加速度测试,四轮测速
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


/********************************* 事件 ****************************************/

EventGroupHandle_t Event_Handle = NULL;	//事件标志组句柄

#define EVENTBIT_0	(1<<0)                          //制动油压测试事件
#define EVENTBIT_1	(1<<1)                          //制动盘温度测试事件
#define EVENTBIT_2	(1<<2)                          //悬架位移测试事件
#define EVENTBIT_3	(1<<3)                          //三轴加速度测试事件
#define EVENTBIT_4	(1<<4)                          //应力测试事件
#define EVENTBIT_5	(1<<5)                          //四轮测速事件

/*
*************************************************************************
*                             任务句柄初始化
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
*                             任务堆栈声明
*************************************************************************
*/


/* 闲置任务任务堆栈 */
static StackType_t Idle_Task_Stack[configMINIMAL_STACK_SIZE];
/* 定时器任务任务堆栈 */
static StackType_t Timer_Task_Stack[configTIMER_TASK_STACK_DEPTH];

/* 定义创建任务栈 */
static StackType_t AppTaskCreate_Stack[128];
/* 定义按键任务栈 */
static StackType_t KEY_Task_Stack[64];
/*
*************************************************************************
*                             控制块声明
*************************************************************************
*/

/* 闲置任务控制块 */
static StaticTask_t Idle_Task_TCB;
/* 定时器任务控制块 */
static StaticTask_t Timer_Task_TCB;
/* 定义创建任务控制块 */
static StaticTask_t AppTaskCreate_TCB;
/* 按键任务控制块 */
static StaticTask_t KEY_Task_TCB;
/*
*************************************************************************
*                             函数声明
*************************************************************************
*/


static void AppTaskCreate(void);                                           /* 用于创建任务 */
static void BSP_Init(void);
static void KEY_Task(void* parameter);
static void Brake_Disc_Temperature_Task(void* parameter);
static void Three_Axis_Acceleration_Task(void* parameter);
static void Suspension_Displacement_Task(void* parameter);
static void Brake_Oil_Pressure_Task(void* parameter);


/*
*************************************************************************
*                             全局变量
*************************************************************************
*/

uint8_t KeyValue = 0;

/****************************************************************************/

/* 获取空闲任务的内存 */

void vApplicationGetIdleTaskMemory(StaticTask_t **ppxIdleTaskTCBBuffer,
	                                 StackType_t **ppxIdleTaskStackBuffer,
                                    uint32_t *pulIdleTaskStackSize)
{
	*ppxIdleTaskTCBBuffer = &Idle_Task_TCB;
	*ppxIdleTaskStackBuffer = Idle_Task_Stack;
	*pulIdleTaskStackSize = configMINIMAL_STACK_SIZE;
}

/* 获取定时器任务的内存 */

void vApplicationGetTimerTaskMemory(StaticTask_t **ppxTimerTaskTCBBuffer,
	                                 StackType_t **ppxTimerTaskStackBuffer,
                                    uint32_t *pulTimerTaskStackSize)
{
	*ppxTimerTaskTCBBuffer = &Timer_Task_TCB;
	*ppxTimerTaskStackBuffer = Timer_Task_Stack;
	*pulTimerTaskStackSize = configTIMER_TASK_STACK_DEPTH;
}

/*****************************************************************
  * @brief  主函数
  * @param  无
  * @retval 无
  * @note   第一步：开发板硬件初始化 
            第二步：创建APP应用任务
            第三步：启动FreeRTOS，开始多任务调度
  ****************************************************************/
int main(void)
{
	BSP_Init();
  printf("飞翼车队电气组测试板函数调试!\r\n");
	
	AppTaskCreate_Handle = xTaskCreateStatic((TaskFunction_t	)AppTaskCreate,		  //任务函数
															(const char* 	)"AppTaskCreate",		                //任务名称
															(uint32_t 		)128,	                              //任务堆栈大小
															(void* 		  	)NULL,				                      //传递给任务函数的参数
															(UBaseType_t 	)3, 	                              //任务优先级
															(StackType_t*   )AppTaskCreate_Stack,	            //任务堆栈
															(StaticTask_t*  )&AppTaskCreate_TCB);	            //任务控制块 
  /* 启动任务调度 */          
	if(NULL != AppTaskCreate_Handle)                                              /* 创建成功 */
    vTaskStartScheduler();
	else
		printf("创建失败");
  while(1);   /* 正常不会执行到这里 */
}


/**********************************************************************
  * @ 函数名  ： BSP_Task
  * @ 功能说明： BSP_Task任务主体
  * @ 参数    ：   
  * @ 返回值  ： 无
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
  * @ 函数名  ： AppTaskCreate
  * @ 功能说明： 为了方便管理，所有的任务创建函数都放在这个函数里面
  * @ 参数    ： 无  
  * @ 返回值  ： 无
  **********************************************************************/
static void AppTaskCreate(void)
{
	BaseType_t xReturn = pdPASS;     /* 定义一个创建信息返回值，默认为pdPASS */	
  taskENTER_CRITICAL();           //进入临界区
  
	Event_Handle = xEventGroupCreate();
	if(NULL != Event_Handle)
	printf("Event_Handle 事件创建成功！");
	xEventGroupClearBits( Event_Handle, EVENTBIT_0|EVENTBIT_1|EVENTBIT_2|EVENTBIT_3|EVENTBIT_4);
  /* 创建KEY_Task_Stack任务 */
	KEY_Task_Handle = xTaskCreateStatic((TaskFunction_t	)KEY_Task,		            //任务函数
															(const char* 	)"KEY_Task",		                    //任务名称
															(uint32_t 		)64,					                      //任务堆栈大小
															(void* 		  	)NULL,				                      //传递给任务函数的参数
															(UBaseType_t 	)10, 				                        //任务优先级
															(StackType_t*   )KEY_Task_Stack,	                //任务堆栈
															(StaticTask_t*  )&KEY_Task_TCB);	                //任务控制块	
	if(NULL != KEY_Task_Handle)                                                   /* 创建成功 */
		printf("KEY_Task任务创建成功!\n");
	else
		printf("KEY_Task任务创建失败!\n");
							
	  /* 创建Brake_Oil_Pressure_Task任务 */
	xReturn = xTaskCreate((TaskFunction_t	)Brake_Oil_Pressure_Task,		            //任务函数
															(const char* 	)"Brake_Oil_Pressure_Task",		      //任务名称
															(uint16_t 		)128,					                      //任务堆栈大小
															(void* 		  	)NULL,				                      //传递给任务函数的参数
															(UBaseType_t 	)8, 				                        //任务优先级
															(TaskHandle_t*  )&Brake_Oil_Pressure_Task_Handle);	      //任务控制块指针
	if(pdPASS == xReturn) 																												/* 创建成功 */
		printf("Brake_Oil_Pressure_Task任务创建成功!\r\n");
	else
		printf("Brake_Oil_Pressure_Task任务创建失败!\r\n");
	
	  /* 创建Suspension_Displacement_Task任务 */
	xReturn = xTaskCreate((TaskFunction_t	)Suspension_Displacement_Task,		            //任务函数
															(const char* 	)"Suspension_Displacement_Task",		      //任务名称
															(uint16_t 		)128,					                      //任务堆栈大小
															(void* 		  	)NULL,				                      //传递给任务函数的参数
															(UBaseType_t 	)5, 				                        //任务优先级
															(TaskHandle_t*  )&Suspension_Displacement_Task_Handle);	      //任务控制块指针
	if(pdPASS == xReturn) 																												/* 创建成功 */
		printf("Suspension_Displacement_Task任务创建成功!\r\n");
	else
		printf("Suspension_Displacement_Task任务创建失败!\r\n");

	  /* 创建Three_Axis_Acceleration_Task任务 */
	xReturn = xTaskCreate((TaskFunction_t	)Three_Axis_Acceleration_Task,		            //任务函数
															(const char* 	)"Three_Axis_Acceleration_Task",		      //任务名称
															(uint16_t 		)128,					                      //任务堆栈大小
															(void* 		  	)NULL,				                      //传递给任务函数的参数
															(UBaseType_t 	)4, 				                        //任务优先级
															(TaskHandle_t*  )&Three_Axis_Acceleration_Task_Handle);	      //任务控制块指针
	if(pdPASS == xReturn) 																												/* 创建成功 */
		printf("Three_Axis_Acceleration_Task任务创建成功!\r\n");
	else
		printf("Three_Axis_Acceleration_Task任务创建失败!\r\n");

	  /* 创建Brake_Disc_Temperature_Task任务 */
	xReturn = xTaskCreate((TaskFunction_t	)Brake_Disc_Temperature_Task,		            //任务函数
															(const char* 	)"Brake_Disc_Temperature_Task",		      //任务名称
															(uint16_t 		)128,					                      //任务堆栈大小
															(void* 		  	)NULL,				                      //传递给任务函数的参数
															(UBaseType_t 	)6, 				                        //任务优先级
															(TaskHandle_t*  )&Brake_Disc_Temperature_Task_Handle);	      //任务控制块指针
	if(pdPASS == xReturn) 																												/* 创建成功 */
		printf("Brake_Disc_Temperature_Task任务创建成功!\r\n");
	else
		printf("Brake_Disc_Temperature_Task任务创建失败!\r\n");
															
  vTaskDelete(AppTaskCreate_Handle); //删除AppTaskCreate任务
  taskEXIT_CRITICAL();                //退出临界区
}


/**********************************************************************
  * @ 函数名  ： KEY_Task
  * @ 功能说明： 通过按键切换测试功能
  * @ 参数    ：   
  * @ 返回值  ： 无
  ********************************************************************/
static void KEY_Task(void* parameter)
{
	BaseType_t xResult;
	while(1)
  {
		KeyValue = 0xff;
		KeyValue = Key_Scan(0);
	  /*事件置位*/
		switch(KeyValue)
		{
			case(0):
			{
				GPIO_ResetBits(DEBUG_LED_GPIO_PORT, DEBUG_LED1_GPIO_PIN);
				GPIO_SetBits(DEBUG_LED_GPIO_PORT, (DEBUG_LED2_GPIO_PIN|DEBUG_LED3_GPIO_PIN|
	                     DEBUG_LED4_GPIO_PIN|DEBUG_LED5_GPIO_PIN));
				xEventGroupClearBits( Event_Handle, EVENTBIT_0|EVENTBIT_1|EVENTBIT_2|EVENTBIT_3|EVENTBIT_4);
    		xResult = xEventGroupSetBits(Event_Handle,EVENTBIT_0);
				printf("\r\n 按键0\r\n");
				break;
			}
			case(1):
			{
				GPIO_ResetBits(DEBUG_LED_GPIO_PORT, DEBUG_LED2_GPIO_PIN);
				GPIO_SetBits(DEBUG_LED_GPIO_PORT, (DEBUG_LED1_GPIO_PIN|DEBUG_LED3_GPIO_PIN|
	                     DEBUG_LED4_GPIO_PIN|DEBUG_LED5_GPIO_PIN));
				xEventGroupClearBits( Event_Handle, EVENTBIT_0|EVENTBIT_1|EVENTBIT_2|EVENTBIT_3|EVENTBIT_4);
    		xResult = xEventGroupSetBits(Event_Handle,EVENTBIT_1);
				printf("\r\n 按键1\r\n");
				break;
			}
			case(2):
			{
				GPIO_ResetBits(DEBUG_LED_GPIO_PORT, DEBUG_LED3_GPIO_PIN);
				GPIO_SetBits(DEBUG_LED_GPIO_PORT, (DEBUG_LED2_GPIO_PIN|DEBUG_LED1_GPIO_PIN|
	                     DEBUG_LED4_GPIO_PIN|DEBUG_LED5_GPIO_PIN));
				xEventGroupClearBits( Event_Handle, EVENTBIT_0|EVENTBIT_1|EVENTBIT_2|EVENTBIT_3|EVENTBIT_4);
    		xResult = xEventGroupSetBits(Event_Handle,EVENTBIT_2);
				printf("\r\n 按键2\r\n");
				break;
			}
			case(3):
			{
				GPIO_ResetBits(DEBUG_LED_GPIO_PORT, DEBUG_LED4_GPIO_PIN);
				GPIO_SetBits(DEBUG_LED_GPIO_PORT, (DEBUG_LED2_GPIO_PIN|DEBUG_LED3_GPIO_PIN|
	                     DEBUG_LED1_GPIO_PIN|DEBUG_LED5_GPIO_PIN));
				xEventGroupClearBits( Event_Handle, EVENTBIT_0|EVENTBIT_1|EVENTBIT_2|EVENTBIT_3|EVENTBIT_4);
    		xResult = xEventGroupSetBits(Event_Handle,EVENTBIT_3);
				printf("\r\n 按键3\r\n");
				break;
			}
			default:
				break;
		}
		vTaskDelay(100);
	}
}


/**********************************************************************
  * @ 函数名  ： Brake_Oil_Pressure_Task
  * @ 功能说明： 测量制动油压
  * @ 参数    ：   
  * @ 返回值  ： 无
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
  * @ 函数名  ： Suspension_Displacement_Task
  * @ 功能说明： 测量悬架位移
  * @ 参数    ：   
  * @ 返回值  ： 无
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
  * @ 函数名  ： Three_Axis_Acceleration_Task
  * @ 功能说明：测量三轴加速度 
  * @ 参数    ：   
  * @ 返回值  ： 无
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
			printf("\r\n加速度： x = %-6.3fg，y = %-6.3fg，z = %-6.3fg",Accel[0]*1.0/16384,Accel[1]*1.0/16384,Accel[2]*1.0/16384);
			MPU6050_ReturnTemp(&Temp);
			printf("\r\n温度： %8.2f \r\n",Temp);
		}
		vTaskDelay(1000);
	}
}

/**********************************************************************
  * @ 函数名  ： Brake_Disc_Temperature_Task
  * @ 功能说明： 测量制动盘温度
  * @ 参数    ：   
  * @ 返回值  ： 无
  ********************************************************************/

static void Brake_Disc_Temperature_Task(void* parameter)
{
	EventBits_t r_event;
  SMBUS_SCK_L();
	delay_xms(3);
  SMBUS_SCK_H();
/*********************修改地址*****************************/
//	u8 SMBus_ID = 0x36;
//	SMBus_WriteMemory(SA, EEPROM_ACCESS|EEPROM_SMBus , SMBus_ID);
/**********************************************************/	
	while(1)
	{
		r_event = xEventGroupWaitBits(Event_Handle,EVENTBIT_1,pdFALSE,pdTRUE,portMAX_DELAY);
		if((r_event&EVENTBIT_1) == EVENTBIT_1)
		{
			printf("红外线温度传感器-1\r\n");
			printf("Body_temporature = %f \r\n", SMBus_Read_Body_Temperature(SA_1));
			printf("Enviroment_temporature = %f \r\n", SMBus_Read_Enviroment_Temperature(SA_1));		
			vTaskDelay(100);
			printf("红外线温度传感器-2\r\n");
			printf("Body_temporature = %f \r\n", SMBus_Read_Body_Temperature(SA_2));
			printf("Enviroment_temporature = %f \r\n", SMBus_Read_Enviroment_Temperature(SA_2));		
			vTaskDelay(100);
			printf("红外线温度传感器-3\r\n");
			printf("Body_temporature = %f \r\n", SMBus_Read_Body_Temperature(SA_3));
			printf("Enviroment_temporature = %f \r\n", SMBus_Read_Enviroment_Temperature(SA_3));	
			vTaskDelay(100);
		}
	}
}

