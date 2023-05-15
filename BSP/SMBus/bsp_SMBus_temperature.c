/**
  ******************************************************************************
  * @file    bsp_smbus_temperature.c
  * @author  罗成
  * @version V1.0
  * @date    2022-7-26
  * @brief   SMBus 驱动
  ******************************************************************************
  */
#include "bsp_smbus_temperature.h"
#include "bsp_usart.h"
#include "bsp_delay.h"


void SMBus_GPIO_Init()
{
    GPIO_InitTypeDef    GPIO_InitStructure;
	/* Enable SMBUS_PORT clocks */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SMBUS_PORT, ENABLE);
    /*配置SMBUS_SCK、SMBUS_SDA为集电极开漏输出*/
    GPIO_InitStructure.GPIO_Pin = SMBUS_SCK | SMBUS_SDA;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;         //GPIO_Mode_IPU  上拉输入不行
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(SMBUS_PORT, &GPIO_InitStructure);

    SMBUS_SCK_H();
    SMBUS_SDA_H();
}

void SMBus_StartBit(void)
{
    SMBUS_SDA_H();		// Set SDA line
    SMBus_Delay(5);	    // Wait a few microseconds
    SMBUS_SCK_H();		// Set SCL line
    SMBus_Delay(5);	    // Generate bus free time between Stop
    SMBUS_SDA_L();		// Clear SDA line
    SMBus_Delay(5);	    // Hold time after (Repeated) Start
    // Condition. After this period, the first clock is generated.
    //(Thd:sta=4.0us min)在SCK=1时，检测到SDA由1到0表示通信开始（下降沿）
    SMBUS_SCK_L();	    // Clear SCL line
    SMBus_Delay(5);	    // Wait a few microseconds
}

void SMBus_StopBit(void)
{
    SMBUS_SCK_L();		// Clear SCL line
    SMBus_Delay(5);	    // Wait a few microseconds
    SMBUS_SDA_L();		// Clear SDA line
    SMBus_Delay(5);	    // Wait a few microseconds
    SMBUS_SCK_H();		// Set SCL line
    SMBus_Delay(5);	    // Stop condition setup time(Tsu:sto=4.0us min)
    SMBUS_SDA_H();		// Set SDA line
}

u8 SMBus_SendByte(u8 Tx_buffer)
{
    u8	Bit_counter;
    u8	Ack_bit;
    u8	bit_out;

    for(Bit_counter=8; Bit_counter; Bit_counter--)
    {
        if (Tx_buffer&0x80)//如果最高位为1
        {
            bit_out=1;   // 把最高位置1
        }
        else  //如果最高位为0
        {
            bit_out=0;  // 把最高位置0
        }
        SMBus_SendBit(bit_out);	// 把最高位发送出去
        Tx_buffer<<=1;// 左移一位把最高位移出去等待下一个最高位，循环8次，每次都发最高位，就可把一个字节发出去了
    }
     Ack_bit=SMBus_ReceiveBit();    // Get acknowledgment bit
    return	Ack_bit;
}

void SMBus_SendBit(u8 bit_out)
{
    if(bit_out==0)
    {
        SMBUS_SDA_L();
    }
    else
    {
        SMBUS_SDA_H();
    }
    SMBus_Delay(2);					// Tsu:dat = 250ns minimum
    SMBUS_SCK_H();					// Set SCL line
    SMBus_Delay(6);					// High Level of Clock Pulse
    SMBUS_SCK_L();					// Clear SCL line
    SMBus_Delay(3);					// Low Level of Clock Pulse
}

u8 SMBus_ReceiveBit(void)
{
    u8 Ack_bit;

    SMBUS_SDA_H();          //引脚靠外部电阻上拉，当作输入/*将SDA置1，即释放SDA总线，下面的拉高SCK，是为了读取SDA数据，低电平不允许读取*/
	  SMBus_Delay(2);			// High Level of Clock Pulse
    SMBUS_SCK_H();			// Set SCL line
    SMBus_Delay(5);			// High Level of Clock Pulse
    if(SMBUS_SDA_PIN())
    {
        Ack_bit=1;
    }
    else
    {
        Ack_bit=0;
    }
    SMBUS_SCK_L();			// Clear SCL line
    SMBus_Delay(3);			// Low Level of Clock Pulse

    return	Ack_bit;
}

u8 SMBus_ReceiveByte(u8 ack_nack)
{
    u8 	RX_buffer;
    u8	Bit_Counter;

    for(Bit_Counter=8; Bit_Counter; Bit_Counter--)
    {
        if(SMBus_ReceiveBit())// Get a bit from the SDA line
        {
            RX_buffer <<= 1;// If the bit is HIGH save 1  in RX_buffer
            RX_buffer |=0x01;//如果Ack_bit=1，把收到应答信号1与0000 0001 进行或运算，确保为1
        }
        else
        {
            RX_buffer <<= 1;// If the bit is LOW save 0 in RX_buffer
            RX_buffer &=0xfe;//如果Ack_bit=1，把收到应答信号0与1111 1110 进行与运算，确保为0
        }
    }
    SMBus_SendBit(ack_nack);// Sends acknowledgment bit 把应答信号发出去，如果1，就进行下一次通信，如果为0、，就拜拜了
    return RX_buffer;
}

void SMBus_Delay(u16 time)
{
	delay_us(time);
}


u16 SMBus_ReadMemory(u8 slaveAddress, u8 command)
{
    u16 data;			// Data storage (DataH:DataL)
    u8 Pec;				// PEC byte storage
    u8 DataL=0;			// Low data byte storage
    u8 DataH=0;			// High data byte storage
    u8 arr[6];			// Buffer for the sent bytes
    u8 PecReg;			// Calculated PEC byte storage
    u8 ErrorCounter;	// Defines the number of the attempts for communication with MLX90614

    ErrorCounter=0x00;	// Initialising of ErrorCounter
	slaveAddress <<= 1;	//2-7位表示从机地址 从机地址左移一位，把读写位空出来
	
    do
    {
repeat:
        SMBus_StopBit();			    //If slave send NACK stop comunication
        --ErrorCounter;				    //Pre-decrement ErrorCounter
        if(!ErrorCounter) 			    //ErrorCounter=0xff
        {
						printf("Error\r\n");
            break;					    //Yes,go out from do-while{}
        }

        SMBus_StartBit();				//Start condition
        if(SMBus_SendByte(slaveAddress))//Send SlaveAddress 最低位Wr=0表示接下来写命令
        {
//					printf("1");
            goto	repeat;			    //Repeat comunication again
        }
        if(SMBus_SendByte(command))	    //Send command
        {
//					printf("2");
            goto	repeat;		    	//Repeat comunication again
        }
        SMBus_StartBit();					//Repeated Start condition
        if(SMBus_SendByte(slaveAddress+1))	//Send SlaveAddress 最低位Rd=1表示接下来读数据
        {
//					printf("3");
            goto	repeat;             	//Repeat comunication again
        }

        DataL = SMBus_ReceiveByte(ACK);	//Read low data,master must send ACK
        DataH = SMBus_ReceiveByte(ACK); //Read high data,master must send ACK
        Pec = SMBus_ReceiveByte(NACK);	//Read PEC byte, master must send NACK
        SMBus_StopBit();				//Stop condition

        arr[5] = slaveAddress;		//
        arr[4] = command;			//
        arr[3] = slaveAddress+1;	//Load array arr
        arr[2] = DataL;				//
        arr[1] = DataH;				//
        arr[0] = 0;					//
        PecReg=PEC_Calculation(arr);//Calculate CRC 数据校验   8位的校验码
    }
    while(PecReg != Pec);//If received and calculated CRC are equal go out from do-while{}

	data = (DataH<<8) | DataL;	//data=DataH:DataL
    return data;
}


u8 PEC_Calculation(u8 pec[])
{
    u8 	crc[6];//存放多项式
    u8	BitPosition=47;//存放所有数据最高位，6*8=48 最高位就是47位
    u8	shift;
    u8	i;
    u8	j;
    u8	temp;

    do
    {
        /*Load pattern value 0x00 00 00 00 01 07*/
        crc[5]=0;
        crc[4]=0;
        crc[3]=0;
        crc[2]=0;
        crc[1]=0x01;
        crc[0]=0x07;

        /*Set maximum bit position at 47 ( six bytes byte5...byte0,MSbit=47)*/
        BitPosition=47;

        /*Set shift position at 0*/
        shift=0;

        /*Find first "1" in the transmited message beginning from the MSByte byte5*/
        i=5;
        j=0;
        while((pec[i]&(0x80>>j))==0 && i>0)                                     //循环出最高为1
        {
            BitPosition--;
            if(j<7)
            {
                j++;
            }
            else
            {
                j=0x00;
                i--;
            }
        }/*End of while */

        /*Get shift value for pattern value*/
        shift=BitPosition-8;

        /*Shift pattern value */ 
        while(shift)                                                            //将多相式与检验值对齐
        {
            for(i=5; i<0xFF; i--)
            {
							if((crc[i-1]&0x80) && (i>0))
							{
									temp=1;
							}
							else
							{
									temp=0;
							}
							crc[i]<<=1;
							crc[i]+=temp;
            }/*End of for*/
            shift--;
        }/*End of while*/

        /*Exclusive OR between pec and crc*/
        for(i=0; i<=5; i++)                                                     //异或 = 做除法
        {
            pec[i] ^=crc[i];
        }/*End of for*/
    }
    while(BitPosition>8); /*End of do-while*/                                   //通过除法最终得到校验码

    return pec[0];
}

float SMBus_Read_Body_Temperature(u8 SMBus_ID)
{  
	float temp = 0;
	temp = SMBus_ReadMemory(SMBus_ID, RAM_ACCESS|RAM_TOBJ1)*0.02-273.15;
//	temp = SMBus_ReadMemory(SA, RAM_ACCESS|RAM_TOBJ2)*0.02-273.15;	
	return temp;
}

float SMBus_Read_Enviroment_Temperature(u8 SMBus_ID)
{
	float temp = 0;
	temp = SMBus_ReadMemory(SMBus_ID, RAM_ACCESS|RAM_TA)*0.02-273.15;
	return temp;
}


void SMBus_Read_MLX90614_ID(u8 SMBus_ID,u16* addr)
{
	u8 i;
	for(i = 0; i < 4;i++)
	{
		addr[i] = SMBus_ReadMemory(SMBus_ID, EEPROM_ACCESS|(EEPROM_ID_1+i));
	}
	printf(" MLX90614_ID \r\n");
	for(i = 0; i < 4;i++)	
	{
		printf("%#x \r\n",addr[i]);
	}
}

#if One_MLX90614

void SMBus_Read_SMBus_ID(void)
{
	u16 addr = 0;
	addr = SMBus_ReadMemory(0x00, EEPROM_ACCESS|(EEPROM_SMBus));
	printf("SMBus_ID = %#x \r\n",addr);
}

#endif

void SMBus_WriteMemory(u8 slaveAddress, u8 command , u8 data)
{
	
	u8 arr[6] = {0};			// Buffer for the sent bytes
	u8 ErrorCounter = 0;	// Defines the number of the attempts for communication with MLX90614
	
	arr[5] = 0;
	arr[4] = 0x00;		
	arr[3] = command;
	arr[2] = data;			
	arr[1] = 0xbe;
	arr[0] = 0;					
	
	arr[2] = PEC_Calculation(arr);//Calculate CRC 数据校验   8位的校验码
	arr[0] = data;
	arr[1] = 0xbe;
	
	ErrorCounter=0x00;	// Initialising of ErrorCounter
	slaveAddress <<= 1;	//2-7位表示从机地址 从机地址左移一位，把读写位空出来
	
	  do
    {
repeat:
        SMBus_StopBit();			    //If slave send NACK stop comunication
        --ErrorCounter;				    //Pre-decrement ErrorCounter
        if(!ErrorCounter) 			    //ErrorCounter=0xff
        {
						printf("Error\r\n");
            break;					    //Yes,go out from do-while{}
        }
				SMBus_CleanEEPROM(slaveAddress,command);
				delay_ms(5);
				SMBus_StartBit();				//Start condition				
				if(SMBus_SendByte(0x00))//Send SlaveAddress 最低位Wr=0表示接下来写命令
				{
//					printf("1");
						goto	repeat;			    //Repeat comunication again
				}
				if(SMBus_SendByte(command))	    //Send command
				{
//					printf("2");
						goto	repeat;		    	//Repeat comunication again
				}	
        if(SMBus_SendByte(arr[0]))	    //Send data
        {
//					printf("3");
            goto	repeat;		    	//Repeat comunication again
        }
				if(SMBus_SendByte(arr[1]))	    //Send data
        {
//					printf("4");
            goto	repeat;		    	//Repeat comunication again
        }
    }
    while(SMBus_SendByte(arr[2]));
		SMBus_StopBit();
		printf("写入完成\r\n");
}

void SMBus_CleanEEPROM(u8 slaveAddress,u8 command)
{
	u8 arr[6];
	u8 ErrorCounter = 0;	// Defines the number of the attempts for communication with MLX90614	
	arr[5] = 0;
	arr[4] = slaveAddress;		
	arr[3] = command;
	arr[2] = 0x00;			
	arr[1] = 0x00;
	arr[0] = 0;
	
	arr[2] = PEC_Calculation(arr);//Calculate CRC 数据校验   8位的校验码
	arr[0] = 0x00;
	arr[1] = 0x00;

	ErrorCounter=0x00;	// Initialising of ErrorCounter
//	slaveAddress <<= 1;	//2-7位表示从机地址 从机地址左移一位，把读写位空出来
	do{
repeat:
			SMBus_StopBit();			    //If slave send NACK stop comunication
			--ErrorCounter;				    //Pre-decrement ErrorCounter
			if(!ErrorCounter) 			    //ErrorCounter=0xff
			{
					printf("Error\r\n");
					break;					    //Yes,go out from do-while{}
			}
			SMBus_StartBit();				//Start condition
			if(SMBus_SendByte(slaveAddress))//Send SlaveAddress 最低位Wr=0表示接下来写命令
			{
				printf("1");
					goto	repeat;			    //Repeat comunication again
			}
			if(SMBus_SendByte(command))	    //Send command
			{
				printf("2");
					goto	repeat;		    	//Repeat comunication again
			}	
			if(SMBus_SendByte(arr[0]))	    //Send data
			{
				printf("3");
					goto	repeat;		    	//Repeat comunication again
			}
			if(SMBus_SendByte(arr[1]))	    //Send data
			{
				printf("4");
					goto	repeat;		    	//Repeat comunication again
			}
		}while(SMBus_SendByte(arr[2]));
	SMBus_StopBit();
}

