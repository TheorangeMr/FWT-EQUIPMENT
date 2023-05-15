#ifndef __BSP_SMBUS_TEMPERATURE_H
#define __BSP_SMBUS_TEMPERATURE_H

#include "stm32f10x.h"                  // Device header
   

#define One_MLX90614                               0


#define ACK	                                       0
#define	NACK                                       1    //不应答或否定的应答
#define SA_1    				                           0x34 //从机地址,0x34，0x35，0x36单个MLX90614时地址为0x00,多个时地址默认为0x5a
#define SA_2    				                           0x35
#define SA_3    				                           0x36



#define RAM_ACCESS		                             0x00 //RAM access command
#define EEPROM_ACCESS	                             0x20 //EEPROM access command


/*****************************EEPROM_Configuration*****************************/

#define EEPROM_Tomax                                0x00 
#define EEPROM_Tomin                                0x01 
#define EEPROM_TA_Range                             0x03 
#define EEPROM_Emissivity_Coefficient               0x04
#define EEPROM_Configuration_Register               0x05
#define EEPROM_SMBus		                            0x0E //SMBus address in the eeprom
#define EEPROM_Emissivity_Coefficient               0x04
#define EEPROM_ID_1                                 0x1c 
#define EEPROM_ID_2                                 0x1d 
#define EEPROM_ID_3                                 0x1e 
#define EEPROM_ID_4                                 0x1f 

/*****************************RAM_Configuration********************************/


#define RAM_TA		                                  0x06 //TA  address in the eeprom
#define RAM_TOBJ1		                                0x07 //To1 address in the eeprom
#define RAM_TOBJ2		                                0x08 //To2 address in the eeprom



#define SMBUS_PORT	                                GPIOD
#define SMBUS_SCK		                                GPIO_Pin_8
#define SMBUS_SDA		                                GPIO_Pin_9

#define RCC_APB2Periph_SMBUS_PORT		RCC_APB2Periph_GPIOB

#define SMBUS_SCK_H()	      SMBUS_PORT->BSRR = SMBUS_SCK
#define SMBUS_SCK_L()	      SMBUS_PORT->BRR = SMBUS_SCK
#define SMBUS_SDA_H()	      SMBUS_PORT->BSRR = SMBUS_SDA
#define SMBUS_SDA_L()	      SMBUS_PORT->BRR = SMBUS_SDA
    
#define SMBUS_SDA_PIN()	    SMBUS_PORT->IDR & SMBUS_SDA                         //读取引脚电平

void SMBus_StartBit(void);
void SMBus_StopBit(void);
void SMBus_SendBit(u8);
u8 SMBus_SendByte(u8);
u8 SMBus_ReceiveBit(void);
u8 SMBus_ReceiveByte(u8);
void SMBus_Delay(u16);
void SMBus_GPIO_Init(void);
u16 SMBus_ReadMemory(u8, u8);
u8 PEC_Calculation(u8*);
float SMBus_Read_Body_Temperature(u8 SMBus_ID);       //获取温度值
float SMBus_Read_Enviroment_Temperature(u8 SMBus_ID); //获取温度值
void SMBus_Read_MLX90614_ID(u8 SMBus_ID,u16* addr);
u16 SMBus_Read_SMBus_ID(void);
void SMBus_WriteMemory(u8 slaveAddress, u8 command , u8 data);

void SMBus_CleanEEPROM(u8 slaveAddress,u8 command);

#endif
