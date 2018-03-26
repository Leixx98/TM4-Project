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
            StartFlag=UARTCharGet(UART1_BASE);
            if(StartFlag==0x0d)
                UartRecStart = 1;
        }
        if(UartRecStart == 1)
        {
            for(i=0;i<16;i++)
                RxBuffer[i] = UARTCharGet(UART1_BASE);
             StartFlag=UARTCharGet(UART1_BASE);
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
    Res0 = (uint32_t)(RxBuffer[0] << 24) | (uint32_t)(RxBuffer[1] << 16) | (uint32_t)(RxBuffer[2] << 8) | (uint32_t)(RxBuffer[3] );
    Res1 = (uint32_t)(RxBuffer[4] << 24) | (uint32_t)(RxBuffer[5] << 16) | (uint32_t)(RxBuffer[6] << 8) | (uint32_t)(RxBuffer[7] );
    Res2 = (uint32_t)(RxBuffer[8] << 24) | (uint32_t)(RxBuffer[9] << 16) | (uint32_t)(RxBuffer[10] << 8) | (uint32_t)(RxBuffer[11] );
    Res3 = (uint32_t)(RxBuffer[12] << 24) | (uint32_t)(RxBuffer[13] << 16) | (uint32_t)(RxBuffer[14] << 8) | (uint32_t)(RxBuffer[15] );
    
    Output0 = (double) Res0/1.0;
    Output1 = (double) Res3/Res1;
    Output2 = (double) Res2/Res1;
    
    printf("%.3f\r\n",Output0);
    printf("%.3f\r\n",Output1);
    printf("%.3f\r\n",Output2);
    
    UartRecFlag = 0;
}
void
ConfigureUART(void)
{
    //
    // Enable the GPIO Peripheral used by the UART.
    //
    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);
    //
    // Enable UART0
    //
    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);
    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_UART1);
    //
    // Configure GPIO Pins for UART mode.
    //
    ROM_GPIOPinConfigure(GPIO_PA0_U0RX);
    ROM_GPIOPinConfigure(GPIO_PA1_U0TX);
    ROM_GPIOPinConfigure(GPIO_PB0_U1RX);
    ROM_GPIOPinConfigure(GPIO_PB1_U1TX);    
    ROM_GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);
    ROM_GPIOPinTypeUART(GPIO_PORTB_BASE, GPIO_PIN_0 | GPIO_PIN_1);

    //
    // Use the internal 16MHz oscillator as the UART clock source.
    //
    UARTClockSourceSet(UART0_BASE, UART_CLOCK_PIOSC);

    //
    // Initialize the UART for console I/O.
    //
    UARTStdioConfig(0, 115200, 16000000);
    
    UARTConfigSetExpClk(UART1_BASE,SysCtlClockGet(),115200,
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
	while(1)
	{
        while(!UartRecFlag)
            UartReceive();
        while(UartRecFlag)
            DataManage();
	}
	
}
