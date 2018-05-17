#ifndef _AD9959_H_
#define _AD9959_H_
#include "stdint.h"
#ifdef __cplusplus
extern "C"
{
#endif
    
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
//AD9959寄存器地址定义
#define CSR_ADD   0x00   //CSR 通道选择寄存器
#define FR1_ADD   0x01   //FR1 功能寄存器1
#define FR2_ADD   0x02   //FR2 功能寄存器2
#define CFR_ADD   0x03   //CFR 通道功能寄存器
#define CFTW0_ADD 0x04   //CTW0 通道频率转换字寄存器
#define CPOW0_ADD 0x05   //CPW0 通道相位转换字寄存器
#define ACR_ADD   0x06   //ACR 幅度控制寄存器
#define LSRR_ADD  0x07   //LSR 通道线性扫描寄存器
#define RDW_ADD   0x08   //RDW 通道线性向上扫描寄存器
#define FDW_ADD   0x09   //FDW 通道线性向下扫描寄存器
//AD9959管脚宏定义
#define PS0_SET			    GPIOPinWrite(GPIO_PORTB_BASE, GPIO_PIN_5,GPIO_PIN_5)
#define PS0_CLR			    GPIOPinWrite(GPIO_PORTB_BASE, GPIO_PIN_5,0)
#define PS1_SET			    GPIOPinWrite(GPIO_PORTB_BASE, GPIO_PIN_0,GPIO_PIN_0)
#define PS1_CLR			    GPIOPinWrite(GPIO_PORTB_BASE, GPIO_PIN_0,0)
#define PS2_SET			    GPIOPinWrite(GPIO_PORTB_BASE, GPIO_PIN_1,GPIO_PIN_1)
#define PS2_CLR			    GPIOPinWrite(GPIO_PORTB_BASE, GPIO_PIN_1,0)
#define PS3_SET			    GPIOPinWrite(GPIO_PORTE_BASE, GPIO_PIN_4,GPIO_PIN_4)
#define PS3_CLR			    GPIOPinWrite(GPIO_PORTE_BASE, GPIO_PIN_4,0)
#define SDIO0_SET	        GPIOPinWrite(GPIO_PORTE_BASE, GPIO_PIN_5,GPIO_PIN_5)
#define SDIO0_CLR		    GPIOPinWrite(GPIO_PORTE_BASE, GPIO_PIN_5,0)
#define SDIO1_SET		    GPIOPinWrite(GPIO_PORTB_BASE, GPIO_PIN_4,GPIO_PIN_4)
#define SDIO1_CLR	    	GPIOPinWrite(GPIO_PORTB_BASE, GPIO_PIN_4,0)
#define SDIO2_SET	    	GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_5,GPIO_PIN_5)
#define SDIO2_CLR	    	GPIOPinWrite(GPIO_PORTA_BASE, GPIO_PIN_5,0)
#define AD9959_PWR_SET		    GPIOPinWrite(GPIO_PORTD_BASE, GPIO_PIN_0,GPIO_PIN_0)
#define AD9959_PWR_CLR		    GPIOPinWrite(GPIO_PORTD_BASE, GPIO_PIN_0,0)
#define Reset_SET		    GPIOPinWrite(GPIO_PORTD_BASE, GPIO_PIN_1,GPIO_PIN_1)
#define Reset_CLR		    GPIOPinWrite(GPIO_PORTD_BASE, GPIO_PIN_1,0)
#define UPDATE_SET		GPIOPinWrite(GPIO_PORTD_BASE, GPIO_PIN_2,GPIO_PIN_2)
#define UPDATE_CLR		GPIOPinWrite(GPIO_PORTD_BASE, GPIO_PIN_2,0)
#define CS_SET		        GPIOPinWrite(GPIO_PORTD_BASE, GPIO_PIN_3,GPIO_PIN_3)
#define CS_CLR		        GPIOPinWrite(GPIO_PORTD_BASE, GPIO_PIN_3,0)
#define SCLK_SET		    GPIOPinWrite(GPIO_PORTE_BASE, GPIO_PIN_1,GPIO_PIN_1)
#define SCLK_CLR		    GPIOPinWrite(GPIO_PORTE_BASE, GPIO_PIN_1,0)
#define SDIO3_SET		    GPIOPinWrite(GPIO_PORTE_BASE, GPIO_PIN_2,GPIO_PIN_2)
#define SDIO3_CLR		    GPIOPinWrite(GPIO_PORTE_BASE, GPIO_PIN_2,0)


void delay1 (u32 length);
void IntReset(void);	  //AD9959复位
void IO_Update(void);   //AD9959更新数据
void Intserve(void);		   //IO口初始化
void WriteData_AD9959(u8 RegisterAddress, u8 NumberofRegisters, u8 *RegisterData,u8 temp);
void Init_AD9959(void);
void Write_frequence(u8 Channel,u32 Freq);
void Write_Amplitude(u8 Channel, u16 Ampli);
void Write_Phase(u8 Channel,u16 Phase);
void Write_Quadrature(uint32_t Freq);















#ifdef __cplusplus
}
#endif

//*****************************************************************************
//
// Prototypes for the globals exported by this driver.
//
//*****************************************************************************

#endif // __BUTTONS_H__

