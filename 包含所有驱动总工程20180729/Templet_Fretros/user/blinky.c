//*****************************************************************************
//
// blinky.c - Simple example to blink the on-board LED.
//
// Copyright (c) 2013-2017 Texas Instruments Incorporated.  All rights reserved.
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
// This is part of revision 2.1.4.178 of the EK-TM4C1294XL Firmware Package.
//
//*****************************************************************************

#include <stdint.h>
#include <stdbool.h>

#include "inc/hw_gpio.h"
#include "inc/hw_ints.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "inc/hw_nvic.h"

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
#include "driverlib/timer.h"

#include "utils/uartstdio.h"
#include "utils/buttons.h"

#include "FreeRTOSConfig.h"
#include "FreeRTOS.h"
#include "queue.h"
#include "task.h"

#include "functions/led_task.h"
#include "functions/uartprint.h"
#include "functions/tim.h"
#include "functions/key.h"
#include "functions/ads1256.h"
#include "functions/taskinit.h"
#include "functions/ad9959.h"
#include "functions/ads1115.h"
#include "functions/dac5618.h"
#include "functions/dac8811.h"
#include "functions/ads11152.h"
#include "functions/dac56182.h"
#include "functions/dac8501.h"
#include "functions/fpga.h"
#include "functions/fpgadrawline.h"
#include "functions/uartreceive.h"
#include "functions/sort.h"

//串口接收所需要用到的全局变量
//*****************************************************************************
//uint8_t Mode;                            //接收到的命令对应的模式
//uint8_t AD9959_Channel;                  //AD9959通道选择
//uint32_t receivevalue;                   //串口接收到的值
//uint32_t receivebuffer[4];               //串口接收到的扫描参数
//*****************************************************************************

uint32_t SysClock;
//*****************************************************************************
//
//! \addtogroup example_list
//! <h1>Blinky (blinky)</h1>
//!
//! A very simple example that blinks the on-board LED using direct register
//! access.
//
//*****************************************************************************

//*****************************************************************************
//
// The error routine that is called if the driver library encounters an error.
//
//*****************************************************************************
#ifdef DEBUG
void
__error__(char *pcFilename, uint32_t ui32Line)
{
    while(1);
}
#endif

uint32_t SysCtlClockFreq;

void demoSerialTask(void *pvParameters);


//*****************************************************************************
//
// Configure the UART and its pins.  This must be called before UARTprintf().
//
//*****************************************************************************
void USART_Init(void)
{
	    //
    // Enable the GPIO Peripheral used by the UART.
    //
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOC);
    //
    // Enable UART0
    //
    SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_UART2);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_UART5);
    //
    // Configure GPIO Pins for UART Mode.
    //
    GPIOPinConfigure(GPIO_PA0_U0RX);
    GPIOPinConfigure(GPIO_PA1_U0TX);
    GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);
    GPIOPinConfigure(GPIO_PA6_U2RX);
    GPIOPinConfigure(GPIO_PA7_U2TX);   
    GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_6 | GPIO_PIN_7);
    GPIOPinConfigure(GPIO_PC6_U5RX);
    GPIOPinConfigure(GPIO_PC7_U5TX);   
    GPIOPinTypeUART(GPIO_PORTC_BASE, GPIO_PIN_6 | GPIO_PIN_7);    
    //
    // Use the internal 16MHz oscillator as the UART clock source.
    //
    UARTClockSourceSet(UART0_BASE, UART_CLOCK_PIOSC);

    //
    // Initialize the UART for console I/O.
    //
    UARTStdioConfig(0, 115200, 16000000);
    UARTConfigSetExpClk(UART2_BASE,SysClock,115200,
                                              (UART_CONFIG_WLEN_8|UART_CONFIG_STOP_ONE|
                                              UART_CONFIG_PAR_NONE));
    UARTConfigSetExpClk(UART5_BASE,SysClock,115200,
                                              (UART_CONFIG_WLEN_8|UART_CONFIG_STOP_ONE|
                                              UART_CONFIG_PAR_NONE));  
}

//*****************************************************************************
//
// Blink the on-board LED.
//
//*****************************************************************************
int
main(void)
{
    //设置抢占优先级，共三位8个优先级，全部设置为抢占优先级（FREERTOS官方推荐设置） 
    IntPriorityGroupingSet(4);
    //使能系统时钟
   SysClock=SysCtlClockFreqSet((SYSCTL_XTAL_25MHZ |
                                             SYSCTL_OSC_MAIN |
                                             SYSCTL_USE_PLL |
                                             SYSCTL_CFG_VCO_480), 120000000);
    //
    // Enable the GPIO port that is used for the on-board LED.
    //
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPION);
		SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
		//
    // Enable the FPU
    //
//		FPULazyStackingEnable();
    //
    // Check if the peripheral access is enabled.
    //
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPION))
    {
    }
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOF))
    {
    }		
    // Enable the GPIO pin for the LED (PN0).  Set the direction as output, and
    // enable the GPIO pin for digital function.
    // Enable the button
    GPIOPinTypeGPIOOutput(GPIO_PORTN_BASE, GPIO_PIN_0);
    GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_0);
    GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_4);		

    //定时器初始化
//    TIM0_Init(100);
//    TIM1_Init(500);
		USART_Init();
		
#ifdef KEYTASK_INIT
		KEYTaskInit();                                        //按键任务创建
#endif
		
#ifdef LEDTASK_INIT            
    LEDTaskInit();                                        //点灯任务创建
#endif
		
#ifdef UARTPRINTTASK_INIT    
		UARTPRINTTaskInit();                                       //UART任务创建
#endif	

#ifdef ADS1256TASK_INIT
		ADS1256TaskInit();                                    //ADS1256任务创建
#endif

#ifdef AD9959TASK_INIT
		AD9959TaskInit();                                    //AD9959任务创建
#endif

#ifdef ADS1115TASK_INIT
		ADS1115TaskInit();                                    //ADS1115任务创建
#endif

#ifdef DAC5618TASK_INIT
		DAC5618TaskInit();                                    //DAC5618任务创建
#endif

#ifdef DAC8811TASK_INIT
		DAC8811TaskInit();                                    //DAC8811任务创建
#endif

#ifdef ADS11152TASK_INIT
		ADS11152TaskInit();                                    //ADS11152任务创建
#endif

#ifdef DAC56182TASK_INIT
		DAC56182TaskInit();                                    //DAC56182任务创建
#endif

#ifdef DAC8501TASK_INIT
		DAC8501TaskInit();                                    //DAC56182任务创建
#endif

#ifdef FPGATASK_INIT
		FPGATaskInit();                                       //FPGA通信任务创建
#endif

#ifdef FPGADRAWLINETASK_INIT
		FPGADRAWLINETaskInit();                               //FPGA画图任务创建
#endif

#ifdef UARTRECEIVETASK_INIT
		UARTRECEIVETaskInit();                               //FPGA数据接收任务创建
#endif

#ifdef SORTTASK_INIT
		SORTTaskInit();                               				//快速排序任务创建
#endif


    vTaskStartScheduler();                         				//开启任务调度

    return 0;
}


