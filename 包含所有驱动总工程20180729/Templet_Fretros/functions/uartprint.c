//*****************************************************************************
//
// led_task.c - A simple flashing LED task.
//
// Copyright (c) 2012-2017 Texas Instruments Incorporated.  All rights reserved.
// Software License Agreement
// 
// Texas Instruments (TI) is supplying this software for use solely and
// exclusively on TI's microcontroller products. The software is owned by
// TI and/or its suppliers, and is protected under applicable copyright
// laws. You may not combine this software with "viral" open-source
// software in order to form a larger program.
// 
// THIS SOFTWARE IS PROVIDED "AS IS" AND WITH ALL FAULTS.
// NO WARRANTIES, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING, BUT
// NOT LIMITED TO, IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE. TI SHALL NOT, UNDER ANY
// CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL, OR CONSEQUENTIAL
// DAMAGES, FOR ANY REASON WHATSOEVER.
// 
// This is part of revision 2.1.4.178 of the EK-TM4C123GXL Firmware Package.
//
//*****************************************************************************
/*****************************************************************************
用来进行串口屏控制的模块，注意与uartreceive.c的区别
******************************************************************************/
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include "math.h"
#include "inc/hw_gpio.h"
#include "inc/hw_ints.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/debug.h"
#include "driverlib/fpu.h"
#include "driverlib/gpio.h"
#include "driverlib/interrupt.h"
#include "driverlib/pin_map.h"
#include "driverlib/rom.h"
#include "driverlib/rom_map.h"
#include "driverlib/sysctl.h"
#include "driverlib/systick.h"
#include "driverlib/uart.h"
#include "utils/uartstdio.h"
#include "FreeRTOSConfig.h"
#include "FreeRTOS.h"
#include "queue.h"
#include "task.h"
#include "functions/uartprint.h"
//*****************************************************************************

#define UARTPRINT_TASK_PRIO          5        //任务优先级
#define UARTPRINT_STK_SIZE          256       //任务堆栈大小
TaskHandle_t UARTPRINTTaskHandler;            //任务句柄

uint8_t UARTReceive_Buffer[100];         //串口接收数组
uint8_t Mode;                            //接收到的命令对应的模式
uint8_t AD9959_Channel;                  //AD9959通道选择
uint8_t ADS1115_Channel;                 //ADS1115通道选择
uint8_t ADS11152_Channel;                //ADS1115通道选择
uint8_t DAC5618_Channel;                 //DAC5618通道选择
uint8_t DAC56182_Channel;                //DAC56182通道选择
uint8_t ADS1256_Channel;                 //ADS1256通道选择
uint8_t DAC8501_Channel;                 //DAC8501通道选择
float DAC5618Value;                      //DAC5618电压值
float DAC56182Value;                     //DAC56182电压值
float DAC8501Value;                      //DAC8501电压值
float DAC8811Value;                      //DAC8811电压值
uint32_t receivevalue;                   //串口接收到的值
uint32_t receivebuffer[4];               //串口接收到的扫描参数

extern uint32_t SysClock;                //系统时钟

//*****************************************************************************
//
// 重定向printf函数
//
//*****************************************************************************
int fputc(int ch, FILE *f)
{
  UARTCharPut(UART5_BASE,(uint8_t)ch);
  return ch;
}

//*****************************************************************************
//
// This task toggles the user selected LED at a user selected frequency. User
// can make the selections by pressing the left and right buttons.
//
//*****************************************************************************
void
DemoUARTPRINTTask(void *pvParameters)
{
		uint8_t i,j;
		uint16_t arraycount;
		for(i=0;i<4;i++)
			receivebuffer[i]=0;
		arraycount=0;i=0;j=0;receivevalue=0,Mode=0xff;	
    while(1)
    {
			      if(UARTCharsAvail(UART5_BASE))
            { 
               while(UARTReceive_Buffer[arraycount]!=0x0d)               //从串口中获得数据
 							 { 
									arraycount++;
									UARTReceive_Buffer[arraycount] = UARTCharGet(UART5_BASE);     //存入缓存数组
							 }			
							 while(UARTReceive_Buffer[arraycount]!=0x0d)
								 UARTReceive_Buffer[++arraycount] = UARTCharGet(UART5_BASE);
							 if(UARTReceive_Buffer[1]==0x0a)                      //检测0x0a，有则证明接收到了格式正确的数据
							 {
									 if(UARTReceive_Buffer[2]==0x00)                  //判断命令对应的模式：点频，点幅，扫频，扫幅
											 Mode=AD9959_Point_Freq; 
									 else if(UARTReceive_Buffer[2]==0x01)
											 Mode=AD9959_Point_Ampl;
									 else if(UARTReceive_Buffer[2]==0x02)
											 Mode=AD9959_Sweep_Freq;			
									 else if(UARTReceive_Buffer[2]==0x03)
											 Mode=AD9959_Sweep_Ampl;
									 else if(UARTReceive_Buffer[2]==0x04)					 
											 Mode=ADS1115_Channel_Open;					
									 else if(UARTReceive_Buffer[2]==0x05)
											 Mode=ADS1115_Channel_Close;	
									 else if(UARTReceive_Buffer[2]==0x06)
											 Mode=ADS11152_Channel_Open;
									 else if(UARTReceive_Buffer[2]==0x07)
											 Mode=ADS11152_Channel_Close;
									 else if(UARTReceive_Buffer[2]==0x08)
											 Mode=DAC5618_VoltageSet;
									 else if(UARTReceive_Buffer[2]==0x09)
											 Mode=DAC56182_VoltageSet;
									 else if(UARTReceive_Buffer[2]==0x0A)
											 Mode=ADS1256_Channel_Open;									 
									 else if(UARTReceive_Buffer[2]==0x0B)
											 Mode=ADS1256_Channel_Close;	
									 else if(UARTReceive_Buffer[2]==0x0C)
											 Mode=DAC8501_VoltageSet;
									 else if(UARTReceive_Buffer[2]==0x0e)
											 Mode=DAC8811_VoltageSet;
									 else if(UARTReceive_Buffer[2]==0x0f)        //此模块暂时没加
											 Mode=ADF4351_FreqSet;   
									 else if(UARTReceive_Buffer[2]==0x10)       
											 Mode=FPGA_DataSend;
									 else if(UARTReceive_Buffer[2]==0x11)        
										   Mode=FPGA_DrawStart;
									 else if(UARTReceive_Buffer[2]==0x12)
										   Mode=FPGA_DrawStop;
									 
									 
									           
									 if(Mode==AD9959_Point_Freq||Mode==AD9959_Point_Ampl)            //当为点频或者点幅时，只需接收一个数据
									 {
											AD9959_Channel=UARTReceive_Buffer[3];        //获得当前通道值
											for(i=4;i<arraycount;i++)                   //遍历整个数组
											{
												if(UARTReceive_Buffer[i]>47)                //数据的值至少为48，大于47，命令值均小于47
															receivevalue+=(UARTReceive_Buffer[i]-48)*(pow(10,(arraycount-i-1)));  //换算得到对应的整数
											}
//											UARTprintf("%d\n",receivevalue);          //输出该整数
									 }
									 else if(Mode==AD9959_Sweep_Freq||Mode==AD9959_Sweep_Ampl)      //当模式为扫频或者扫幅时，需要接收四个数据
									 {
										 AD9959_Channel=UARTReceive_Buffer[3];        //获得当前通道值
										 j=arraycount-2;arraycount=0;
										 for(i=0;i<4;i++)
										 {
											 while(UARTReceive_Buffer[j]!=0x0b)   //每个数据末尾都有0x0b
											 {
												 	if(UARTReceive_Buffer[j]>47)
														receivebuffer[i]+=(UARTReceive_Buffer[j]-48)*(pow(10,arraycount));
													j--;arraycount++;
											 }
											 j--;
											 arraycount=0;
										 }
									 }
									 else if(Mode==ADS1115_Channel_Open)            //当模式为开启ADS1115通道时
										 ADS1115_Channel=UARTReceive_Buffer[3];       //获得通道
									 else if(Mode==ADS1115_Channel_Close)           //当模式为关闭ADS1115通道时
										 ADS1115_Channel=UARTReceive_Buffer[3];       //关闭通道
									 else if(Mode==ADS11152_Channel_Open)           //当模式为开启ADS11152通道时 
										 ADS11152_Channel=UARTReceive_Buffer[3];       //获得通道
									 else if(Mode==ADS11152_Channel_Close)          //当模式为开启ADS11152通道时 
										 ADS11152_Channel=UARTReceive_Buffer[3];       //获得通道			
									 else if(Mode==DAC5618_VoltageSet)              //当设置DAC5618通道电压时
									 {
										 DAC5618_Channel=UARTReceive_Buffer[3];        //获得通道
										 DAC5618Value=(UARTReceive_Buffer[4]-48)+(float)((UARTReceive_Buffer[6]-48)*0.1)+(float)((UARTReceive_Buffer[7]-48)*0.01)
																											 +(float)((UARTReceive_Buffer[8]-48)*0.001);
									 }
									 else if(Mode==DAC56182_VoltageSet)              //当设置DAC5618通道电压时
									 {
										 DAC56182_Channel=UARTReceive_Buffer[3];        //获得通道
										 DAC56182Value=(UARTReceive_Buffer[4]-48)+(float)((UARTReceive_Buffer[6]-48)*0.1)+(float)((UARTReceive_Buffer[7]-48)*0.01)
																											 +(float)((UARTReceive_Buffer[8]-48)*0.001);
									 }
									 else if(Mode==ADS1256_Channel_Open)             //当模式为打开ADS1256通道时
									 {
										 ADS1256_Channel=UARTReceive_Buffer[3];       //获得通道
									 }
									 else if(Mode==ADS1256_Channel_Close)             //当模式为关闭ADS1256通道时
									 {
										 ADS1256_Channel=UARTReceive_Buffer[3];
									 }
									 else if(Mode==DAC8501_VoltageSet)              //当模式为设置DAC8501通道电压时
									 {
										 DAC8501_Channel=UARTReceive_Buffer[3];       //获得通道
										 DAC8501Value=(UARTReceive_Buffer[4]-48)+(float)((UARTReceive_Buffer[6]-48)*0.1)+(float)((UARTReceive_Buffer[7]-48)*0.01)
																											 +(float)((UARTReceive_Buffer[8]-48)*0.001);
									 }
									 else if(Mode==DAC8811_VoltageSet)       //当模式为设置DAC8811电压时
									 {
										 if(UARTReceive_Buffer[3]==0x2D)      //当检测到有负号时
										 {
											 DAC8811Value=(UARTReceive_Buffer[4]-48)+(float)((UARTReceive_Buffer[6]-48)*0.1)+(float)((UARTReceive_Buffer[7]-48)*0.01)
																											 +(float)((UARTReceive_Buffer[8]-48)*0.001);
											 DAC8811Value=0-DAC8811Value;
											 UARTprintf("%f\n",DAC8811Value);
										 }
										 else                                //没有负号时
										 {
												DAC8811Value=(UARTReceive_Buffer[3]-48)+(float)((UARTReceive_Buffer[5]-48)*0.1)+(float)((UARTReceive_Buffer[6]-48)*0.01)
																											 +(float)((UARTReceive_Buffer[7]-48)*0.001);	
												UARTprintf("%f\n",DAC8811Value);												
										 }
									 }
									 else if(Mode==FPGA_DataSend)
									 {
										 	for(i=3;i<arraycount;i++)                   //遍历整个数组
											{
												if(UARTReceive_Buffer[i]>47)                //数据的值至少为48，大于47，命令值均小于47
															receivevalue+=(UARTReceive_Buffer[i]-48)*(pow(10,(arraycount-i-1)));  //换算得到对应的整数
											}
									 }
									 
							 }	
							arraycount=0;i=0;j=0;
						}		
					vTaskDelay(30);
    }
}

//*****************************************************************************
//
// Initializes the LED task.
//
//*****************************************************************************
void
UARTPRINTTaskInit(void)
{

    //
    // Create the LED task.
    //
    xTaskCreate(
                        DemoUARTPRINTTask,                       //任务函数
                       (const portCHAR *)"UART", //任务名称 
                        UARTPRINT_STK_SIZE,              // 任务堆栈大小 
                        NULL,       
                        UARTPRINT_TASK_PRIO,
                        &UARTPRINTTaskHandler
    );
    
    
}
