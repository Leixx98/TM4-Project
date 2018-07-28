/*****************************************************************************

STM32F405RGT6

By：NUPT Z.L.Y.

version 1.0  --  2016.12.1

******************************************************************************/

#ifndef __ADS1115_H
#define __ADS1115_H

#include <stdbool.h>
#include <stdint.h>
#include "inc/hw_types.h"
#include "inc/hw_memmap.h"
#include "inc/hw_gpio.h"
#include "driverlib/sysctl.h"
#include "driverlib/rom.h"
#include "driverlib/pin_map.h"
#include "driverlib/gpio.h"
#include "utils/buttons.h"
/*****************Keywords define***********************/
#ifndef u8
    #define u8 unsigned char
#endif

#ifndef u16
    #define u16 unsigned short
#endif
    
#ifndef u32
    #define u32 unsigned long
#endif
    
#ifndef u64
    #define u64 unsigned long long
#endif   
    

///****************仿51***********************/
//#define SCL PAout(3)	// PA3
//#define SDA PAout(2)	// PA2	
//#define	SDA_IN  PAin(2)  	


/****************************************************************8******/
#define ADS1115_SCL_SET        GPIOPinWrite(GPIO_PORTK_BASE, GPIO_PIN_1, GPIO_PIN_1)
#define ADS1115_SCL_CLR        GPIOPinWrite(GPIO_PORTK_BASE, GPIO_PIN_1, 0)               
#define ADS1115_SDA_SET        GPIOPinWrite(GPIO_PORTK_BASE, GPIO_PIN_0, GPIO_PIN_0)
#define ADS1115_SDA_CLR        GPIOPinWrite(GPIO_PORTK_BASE, GPIO_PIN_0, 0)
#define ADS1115_SDA_IN            GPIOPinRead(GPIO_PORTK_BASE,GPIO_PIN_0)
//ADS1115_SDAPort_IN()和ADS1115_SDAPort_OUT()函数定义在.c中，适应不同的单片机，更改IO口方向


#define AD_START    GPIOPinWrite(GPIO_PORTK_BASE, GPIO_PIN_3, 0)
#define AD_END       GPIOPinWrite(GPIO_PORTK_BASE, GPIO_PIN_3,GPIO_PIN_3)

#define TRUE             1
#define FALSE            0




//*************ADDR Initial********************/
#define ADDRESS    ADDRESS_0     //ADDR PIN ->GND
#define ADDRESS_W  ADDRESS|0x00  //写地址
#define ADDRESS_R  ADDRESS|0x01  //读地址




/*************Config Initial*********************/
#define OS         OS_1
#define MUX_A0     MUX_0      //AINp=AIN0, AINn=GND
#define MUX_A1     MUX_5      //AINp=AIN1, AINn=GND
#define MUX_A2     MUX_6      //AINp=AIN2, AINn=GND
#define MUX_A3     MUX_7      //AINp=AIN3, AINn=GND
#define PGA        PGA_0      //FS=6.144V
#define MODE       MODE_1     //Continuous conversion mode
#define DR         DR_7       //Data Rate = 860
#define COMP_QUE   COMP_QUE_3





/**********ADDR宏定义***************/
#define ADDRESS_0  0x90   //ADDR PIN ->GND
#define ADDRESS_1  0x92   //ADDR PIN ->VDD
#define ADDRESS_2  0x94   //ADDR PIN ->SDA
#define ADDRESS_3  0x96   //ADDR PIN ->SCL





/************POINTER REGISTER*****************/
#define Pointer_0  0x00   //Convertion register
#define Pointer_1  0x01   //Config register
#define Pointer_2  0x02   //Lo_thresh register
#define Pointer_3  0x03   //Hi_thresh register




/************CONFIG REGISTER*****************/
#define OS_0   0x0000
#define OS_1   0x8000

#define MUX_0  0x0000  //AINp=AIN0, AINn=AIN1
#define MUX_1  0x1000  //AINp=AIN0, AINn=AIN3
#define MUX_2  0x2000  //AINp=AIN1, AINn=AIN3
#define MUX_3  0x3000  //AINp=AIN2, AINn=AIN3
#define MUX_4  0x4000  //AINp=AIN0, AINn=GND
#define MUX_5  0x5000  //AINp=AIN1, AINn=GND
#define MUX_6  0x6000  //AINp=AIN2, AINn=GND
#define MUX_7  0x7000  //AINp=AIN3, AINn=GND

#define PGA_0  0x0000   //FS=6.144V
#define PGA_1  0x0200   //FS=4.096V
#define PGA_2  0x0400   //FS=2.048V
#define PGA_3  0x0600   //FS=1.024V
#define PGA_4  0x0800   //FS=0.512V
#define PGA_5  0x0A00   //FS=0.256V
#define PGA_6  0x0C00   //FS=0.256V
#define PGA_7  0x0E00   //FS=0.256V

#define MODE_0 0x0000
#define MODE_1 0x0100

#define DR_0   0x0000   //Data Rate = 8
#define DR_1   0x0020   //Data Rate = 16
#define DR_2   0x0040   //Data Rate = 32
#define DR_3   0x0060   //Data Rate = 64
#define DR_4   0x0080   //Data Rate = 128
#define DR_5   0x00A0   //Data Rate = 250
#define DR_6   0x00C0   //Data Rate = 475
#define DR_7   0x00E0   //Data Rate = 860

#define COMP_MODE_0  0x0000
#define COMP_MODE_1  0x0010

#define COMP_POL_0   0x0000
#define COMP_POL_1   0x0008

#define COMP_LAT_0   0x0000
#define COMP_LAT_1   0x0040

#define COMP_QUE_0   0x0000
#define COMP_QUE_1   0x0001
#define COMP_QUE_2   0x0002
#define COMP_QUE_3   0x0003

void ADS1115_I2C_Start(void);
void ADS1115_I2C_Stop(void);
void ADS1115_Ack(void);
void ADS1115_Nack(void);
unsigned char ADS1115_Check(void);
void ADS1115_Write_1_Byte(unsigned char DataByte);
unsigned char ADS1115_Write_N_Byte(unsigned char *writebuffer,unsigned char n);
unsigned char ADS1115_Read_1_Byte(void);
void ADS1115_Read_N_Byte(unsigned char*readbuff, unsigned char n);
void ADS1115_Init(u8 S_MUX_0, u8 S_MUX_1);
void ADS1115_WriteWord(void);
void ADS1115_ReadWord(void);
short ADS1115_Getdata(u8 Channel);
void ADS1115_IO_Init(void);
void ADS1115_SDAPort_In(void);
void ADS1115_SDAPort_Out(void);
void ADS1115TaskInit(void);

#endif

