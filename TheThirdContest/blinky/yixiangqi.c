#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_ints.h"
#include "inc/hw_memmap.h"
#include "inc/hw_nvic.h"
#include "inc/hw_types.h"
#include "inc/hw_gpio.h"
#include "driverlib/debug.h"
#include "driverlib/fpu.h"
#include "driverlib/gpio.h"
#include "driverlib/interrupt.h"
#include "driverlib/pin_map.h"
#include "driverlib/rom.h"
#include "driverlib/sysctl.h"
#include "driverlib/systick.h"
#include "driverlib/uart.h"
#include "utils/uartstdio.h"
#include "utils/buttons.h"

#ifdef DEBUG
void
__error__(char *pcFilename, uint32_t ui32Line)
{
}
#endif

int32_t t;

//*****************************************************************************
//
// Delay for the specified number of seconds.  Depending upon the current
// SysTick value, the delay will be between N-1 and N seconds (i.e. N-1 full
// seconds are guaranteed, aint32_t with the remainder of the current second).
//
//*****************************************************************************
void
Delay(uint32_t ui32Seconds)
{
    //
    // Loop while there are more seconds to wait.
    //
    while(ui32Seconds--) 
    {
        //
        // Wait until the SysTick value is less than 1000.
        //
        while(ROM_SysTickValueGet() > 1000)
        {
        }

        //
        // Wait until the SysTick value is greater than 1000.
        //
        while(ROM_SysTickValueGet() < 1000)
        {
        }
    }
}

//*****************************************************************************
//
// Configure the UART and its pins.  This must be called before UARTprintf().
//
//*****************************************************************************
void
ConfigureUART(void)
{
    //
    // Enable the GPIO Peripheral used by the UART.
    //
    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);

    //
    // Enable UART0
    //
    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);

    //
    // Configure GPIO Pins for UART mode.
    //
    ROM_GPIOPinConfigure(GPIO_PA0_U0RX);
    ROM_GPIOPinConfigure(GPIO_PA1_U0TX);
    ROM_GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);

    //
    // Use the internal 16MHz oscillator as the UART clock source.
    //
    UARTClockSourceSet(UART0_BASE, UART_CLOCK_PIOSC);

    //
    // Initialize the UART for console I/O.
    //
    UARTStdioConfig(0, 115200, 16000000);
}

//中断服务函数
void PortFIntHandler(void)
{
	//设置中断触发源标志位
	uint32_t ulIntSta;
	
	//读取中断触发源
	ulIntSta = GPIOIntStatus(GPIO_PORTF_BASE,true);
	//清除中断标志
	GPIOIntClear(GPIO_PORTF_BASE, GPIO_PIN_2|GPIO_PIN_0|GPIO_PIN_4);
	
	//判断中断触发源
	if(ulIntSta == GPIO_PIN_0)
	{//延时自增
		t += 5;
		if(t > 510)	t = 510;
	}if(ulIntSta == GPIO_PIN_4)
	{//延时自减
		t -= 5;
		if(t <= 0)	t = 5;
	}
}

//中断服务函数
void PortDIntHandler(void)
{
	//设置标志位
	static uint8_t flag=1;

	//清除中断标志
	GPIOIntClear(GPIO_PORTD_BASE, GPIO_PIN_2);
	
	//判断PF1电平
	if(flag)		
	{//PF1为高电平时置低
		SysCtlDelay(t);
		GPIOPinWrite(GPIO_PORTD_BASE, GPIO_PIN_1,0);
		flag = 0;
	}
	else
	{//PF1为低电平时置高
		SysCtlDelay(t);
		GPIOPinWrite(GPIO_PORTD_BASE, GPIO_PIN_1, GPIO_PIN_1);
		flag = 1;
	}
}
/*******************************************************
/*主函数
/*
/*引脚：PD1	方波输入		PD2	方波输出
/*			PF0 增大按键		PF4	减小按键
/*			PF1 提示灯
*******************************************************/
int main(void)
{	
	//设置时钟
	SysCtlClockSet(SYSCTL_SYSDIV_5 | SYSCTL_USE_OSC | SYSCTL_OSC_MAIN |
								SYSCTL_XTAL_25MHZ);
	//配置串口
	ConfigureUART();
	//配置按键
	ButtonsInit();
	
	
	//使能GPIOF
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
	//设置PF1为输出
	GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_1);
	//设置PF1为2mA弱上拉
	GPIOPadConfigSet(GPIO_PORTF_BASE, GPIO_PIN_1 ,
                         GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPU);	
	//使能GPIOD
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOD);
	//设置PD1为输出
	GPIOPinTypeGPIOOutput(GPIO_PORTD_BASE, GPIO_PIN_1);
	//设置PD1为2mA弱上拉
	GPIOPadConfigSet(GPIO_PORTD_BASE, GPIO_PIN_1 ,
                         GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPU);
	//设置PD2为输入
	GPIOPinTypeGPIOInput(GPIO_PORTD_BASE, GPIO_PIN_2);
	//设置PD2为2mA弱上拉
	GPIOPadConfigSet(GPIO_PORTD_BASE, GPIO_PIN_2 ,
                         GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPU);
												 
	
	//GPIOF注册中断，使中断发生时执行第二个参数代表的函数
	GPIOIntRegister(GPIO_PORTF_BASE, PortFIntHandler);	
	//设置PF0、PF4中断触发
	GPIOIntTypeSet(GPIO_PORTF_BASE, GPIO_PIN_0|GPIO_PIN_4 , GPIO_FALLING_EDGE);
	//使能PF0、FP4中断
	GPIOIntEnable(GPIO_PORTF_BASE, GPIO_PIN_0|GPIO_PIN_4);
	
	//GPIOD注册中断，使中断发生时执行第二个参数代表的函数
	GPIOIntRegister(GPIO_PORTD_BASE, PortDIntHandler);
	//设置PD2中断触发
	GPIOIntTypeSet(GPIO_PORTD_BASE, GPIO_PIN_2 , GPIO_BOTH_EDGES);
	//使能PD2中断
	GPIOIntEnable(GPIO_PORTD_BASE, GPIO_PIN_2);
	
	//使能全局中断
	IntMasterEnable();
	
	
	//串口输出
	UARTprintf("START!\n");
	//给t赋初值
	t = 5;
	//点灯
	GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1, GPIO_PIN_1);
	
	while(1)
	{

	}
	
}
