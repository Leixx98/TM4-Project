/*******************************************
项目：移相器
功能：将输入的方波按步进3.6度前移或滞后
标准电压：3.3V
最低电压：1.645V
*******************************************/




#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
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
uint8_t RxBuffer[24];
uint8_t UartRecStart=0;                             //开始接收数据标志位
uint8_t i,StartFlag;                                                   //for循环用
uint8_t Array_Count;                                //接收的字节数
uint8_t UartRecFlag;                                  //串口接收标志位
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
// 重定向printf
//
//*****************************************************************************
int fputc(int ch, FILE *f)
{
  UARTCharPut(UART1_BASE,(uint8_t)ch);
  return ch;
}
//*****************************************************************************
//
// Configure the UART and its pins.  This must be called before UARTprintf().
//
//*****************************************************************************


//串口接收函数
void UartReceive(void)
{
      if(UartRecStart == 0)
        {
            StartFlag=UARTCharGet(UART5_BASE);
            if(StartFlag==0x0d)
                UartRecStart = 1;
        }
        if(UartRecStart == 1)
        {
            for(i=0;i<16;i++)
                RxBuffer[i] = UARTCharGet(UART5_BASE);
             StartFlag=UARTCharGet(UART5_BASE);
            if(StartFlag==0x0a)
                UartRecStart = 2;
        }
        if(UartRecStart == 2)
        {
            UartRecStart = 0;
            UartRecFlag =  1;
        }
}

void DataManage()
{
    uint32_t Res0,Res1,Res2,Res3;
    double Output0,Output1,Output2;
    uint8_t x='\"',f=0xff,s='%';
    Res0 = (uint32_t)(RxBuffer[3] << 24) | (uint32_t)(RxBuffer[2] << 16) | (uint32_t)(RxBuffer[1] << 8) | (uint32_t)(RxBuffer[0] );
    Res1 = (uint32_t)(RxBuffer[7] << 24) | (uint32_t)(RxBuffer[6] << 16) | (uint32_t)(RxBuffer[5] << 8) | (uint32_t)(RxBuffer[4] );
    Res2 = (uint32_t)(RxBuffer[11] << 24) | (uint32_t)(RxBuffer[10] << 16) | (uint32_t)(RxBuffer[9] << 8) | (uint32_t)(RxBuffer[8] );
    Res3 = (uint32_t)(RxBuffer[15] << 24) | (uint32_t)(RxBuffer[14] << 16) | (uint32_t)(RxBuffer[13] << 8) | (uint32_t)(RxBuffer[12] );
    
//    Res1 *= 10^5;
//    Res2 *= 10^2;
    
    Output0 = (double) Res1/Res0;
    Output1 = (double) Res2/Res0;
    Output2 = (double) Res3/100;
    
    Output0 *=100000;
    Output1 *=100;
    Output1 -= 0.03;
    printf("t0.txt=%c%.3fkHZ%c%c%c%c",x,Output0,x,f,f,f);
    printf("t1.txt=%c%.2f%c%c%c%c%c",x,Output1,s,x,f,f,f);
    printf("t2.txt=%c%.3fus%c%c%c%c",x,Output2,x,f,f,f);
    

    
    UartRecFlag = 0;
    SysCtlDelay(10000);
}
void
ConfigureUART(void)
{
    //
    // Enable the GPIO Peripheral used by the UART.
    //
    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);
    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE);
    //
    // Enable UART0
    //
    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);
    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_UART1);
    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_UART5);
    //
    // Configure GPIO Pins for UART mode.
    //
    ROM_GPIOPinConfigure(GPIO_PA0_U0RX);
    ROM_GPIOPinConfigure(GPIO_PA1_U0TX);
    ROM_GPIOPinConfigure(GPIO_PB0_U1RX);
    ROM_GPIOPinConfigure(GPIO_PB1_U1TX);    
    ROM_GPIOPinConfigure(GPIO_PE4_U5RX);
    ROM_GPIOPinConfigure(GPIO_PE5_U5TX);      
    ROM_GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);
    ROM_GPIOPinTypeUART(GPIO_PORTB_BASE, GPIO_PIN_0 | GPIO_PIN_1);
    ROM_GPIOPinTypeUART(GPIO_PORTE_BASE, GPIO_PIN_4 | GPIO_PIN_5);    

    //
    // Use the internal 16MHz oscillator as the UART clock source.
    //
    UARTClockSourceSet(UART0_BASE, UART_CLOCK_PIOSC);

    //
    // Initialize the UART for console I/O.
    //
    UARTStdioConfig(0, 115200, 16000000);
    
    UARTConfigSetExpClk(UART1_BASE,SysCtlClockGet(),9600,
                                              (UART_CONFIG_WLEN_8|UART_CONFIG_STOP_ONE|
                                              UART_CONFIG_PAR_NONE));
    UARTConfigSetExpClk(UART5_BASE,SysCtlClockGet(),115200,
                                              (UART_CONFIG_WLEN_8|UART_CONFIG_STOP_ONE|
                                              UART_CONFIG_PAR_NONE));    
}


//主函数
int main(void)
{	
	//设置时钟
	SysCtlClockSet(SYSCTL_SYSDIV_1 | SYSCTL_USE_OSC | SYSCTL_OSC_MAIN |
								SYSCTL_XTAL_16MHZ);
	//配置串口
	ConfigureUART();
	//使能GPIOF
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
	//设置PF1为输出
	GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_1);
	//设置PF1为2mA弱上拉
	GPIOPadConfigSet(GPIO_PORTF_BASE, GPIO_PIN_1 ,
                         GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPU);	
	//点灯
	GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1, GPIO_PIN_1);
    //串口输出
     printf("START!\n");
    UARTprintf("Hello");
	while(1)
	{
        while(!UartRecFlag)
            UartReceive();
        while(UartRecFlag)
            DataManage();
        
	}
	
}
