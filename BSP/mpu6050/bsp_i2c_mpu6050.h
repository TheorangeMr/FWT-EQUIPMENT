#ifndef _BSP_I2C_MPU6050_H
#define _BSP_I2C_MPU6050_H

#include <inttypes.h>

#define I2C_WR	0		/* д����bit */
#define I2C_RD	1		/* ������bit */
#define GPIO_PORT_I2C	GPIOE			/* GPIO�˿� */
#define RCC_I2C_PORT 	RCC_APB2Periph_GPIOE		/* GPIO�˿�ʱ�� */
#define I2C_SCL_PIN		GPIO_Pin_0			/* ���ӵ�SCLʱ���ߵ�GPIO */
#define I2C_SDA_PIN		GPIO_Pin_1			/* ���ӵ�SDA�����ߵ�GPIO */

#define I2C_SCL_1()  GPIO_SetBits(GPIO_PORT_I2C, I2C_SCL_PIN)		/* SCL = 1 */
#define I2C_SCL_0()  GPIO_ResetBits(GPIO_PORT_I2C, I2C_SCL_PIN)		/* SCL = 0 */

#define I2C_SDA_1()  GPIO_SetBits(GPIO_PORT_I2C, I2C_SDA_PIN)		/* SDA = 1 */
#define I2C_SDA_0()  GPIO_ResetBits(GPIO_PORT_I2C, I2C_SDA_PIN)		/* SDA = 0 */

#define I2C_SDA_READ()  GPIO_ReadInputDataBit(GPIO_PORT_I2C, I2C_SDA_PIN)	/* ��SDA����״̬ */



void i2c_Start(void);
void i2c_Stop(void);
void i2c_SendByte(uint8_t _ucByte);
uint8_t i2c_ReadByte(uint8_t ack);
uint8_t i2c_WaitAck(void);
void i2c_Ack(void);
void i2c_NAck(void);
uint8_t i2c_CheckDevice(uint8_t _Address);
void i2c_GPIO_Config(void);

#endif
