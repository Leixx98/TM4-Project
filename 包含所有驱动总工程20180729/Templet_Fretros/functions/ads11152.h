/*****************************************************************************

STM32F405RGT6

By：NUPT Z.L.Y.

version 1.0  --  2016.12.1

******************************************************************************/

#ifndef __ADS11152_H
#define __ADS11152_H

   

///****************仿51***********************/
//#define SCL PAout(3)	// PA3
//#define SDA PAout(2)	// PA2	
//#define	SDA_IN  PAin(2)  	


/****************************************************************8******/
#define ADS11152_SCL_SET        GPIOPinWrite(GPIO_PORTM_BASE, GPIO_PIN_0, GPIO_PIN_0)
#define ADS11152_SCL_CLR        GPIOPinWrite(GPIO_PORTM_BASE, GPIO_PIN_0, 0)                 
#define ADS11152_SDA_SET        GPIOPinWrite(GPIO_PORTL_BASE, GPIO_PIN_0, GPIO_PIN_0)
#define ADS11152_SDA_CLR        GPIOPinWrite(GPIO_PORTL_BASE, GPIO_PIN_0, 0)
#define ADS11152_SDA_IN         GPIOPinRead(GPIO_PORTL_BASE,GPIO_PIN_0)

////ADS11152_01_SDAPort_IN()和ADS11152_01_SDAPort_OUT()函数定义在.c中，适应不同的单片机，更改IO口方向


#define AD_CS_CLR   	GPIOPinWrite(GPIO_PORTM_BASE, GPIO_PIN_2, 0)
#define AD_CS_SET     GPIOPinWrite(GPIO_PORTM_BASE, GPIO_PIN_2,GPIO_PIN_2)


void ADS11152_I2C_Start(void);
void ADS11152_I2C_Stop(void);
void ADS11152_Ack(void);
void ADS11152_Nack(void);
unsigned char ADS11152_Check(void);
void ADS11152_Write_1_Byte(unsigned char DataByte);
unsigned char ADS11152_Write_N_Byte(unsigned char *writebuffer,unsigned char n);
unsigned char ADS11152_Read_1_Byte(void);
void ADS11152_Read_N_Byte(unsigned char*readbuff, unsigned char n);
void ADS11152_Init(u8 S_MUX_0, u8 S_MUX_1);
void ADS11152_WriteWord(void);
void ADS11152_ReadWord(void);
short ADS11152_Getdata(u8 Channel);
void ADS11152_IO_Init(void);
void ADS11152_SDAPort_In(void);
void ADS11152_SDAPort_Out(void);
void ADS11152TaskInit(void);

#endif

