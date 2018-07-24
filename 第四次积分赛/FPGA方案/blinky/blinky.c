/*******************************************
项目：移相器
功能：将输入的方波按步进3.6度前移或滞后
标准电压：3.3V
最低电压：1.645V
*******************************************/

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include "math.h"
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
#include "utils/ad9959.h"


#ifdef DEBUG
void
__error__(char *pcFilename, uint32_t ui32Line)
{
}
#endif


typedef struct 
{
    double DC1;
    double DC2;
} DC;

#define Pi  3.1415926;

int32_t t;
uint8_t UART_Buffer[90];
uint8_t UART3_Buffer[100];
uint8_t Array_Count,Watch;
uint16_t i;
DC Voltage[250];
//*****************************************************************************
//
// Delay for the specified number of seconds.  Depending upon the current
// SysTick value, the delay will be between N-1 and N seconds (i.e. N-1 full
// seconds are guaranteed, aint32_t with the remainder of the current second).
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
void
ConfigureUART(void)
{
    //
    // Enable the GPIO Peripheral used by the UART.
    //
    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOC);
    //
    // Enable UART0
    //
    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);
    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_UART1);
    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_UART3);
    //
    // Configure GPIO Pins for UART mode.
    //
    ROM_GPIOPinConfigure(GPIO_PA0_U0RX);
    ROM_GPIOPinConfigure(GPIO_PA1_U0TX);
    ROM_GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);
    ROM_GPIOPinConfigure(GPIO_PC4_U1RX);
    ROM_GPIOPinConfigure(GPIO_PC5_U1TX);   
    ROM_GPIOPinTypeUART(GPIO_PORTC_BASE, GPIO_PIN_4 | GPIO_PIN_5);
    ROM_GPIOPinConfigure(GPIO_PC6_U3RX);
    ROM_GPIOPinConfigure(GPIO_PC7_U3TX);   
    ROM_GPIOPinTypeUART(GPIO_PORTC_BASE, GPIO_PIN_6 | GPIO_PIN_7);    
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
    UARTConfigSetExpClk(UART3_BASE,SysCtlClockGet(),115200,
                                              (UART_CONFIG_WLEN_8|UART_CONFIG_STOP_ONE|
                                              UART_CONFIG_PAR_NONE));                                          
}



//主函数
int main(void)
{
    uint8_t x='\"',f=0xff;
    uint8_t RecFlag;
    uint16_t i;
    uint8_t DrawLCD,DrawPoint;
    uint8_t data0,data1,data2,data3;
    uint16_t AmpValue=801,Array3_Count,Count,DC_Count;
    uint32_t FreValue=8000,FreN,PhaN;
    double PhaValue;
    signed short int ReciValue,RecqValue;
    double ShowValue;
    signed int LCDShow;
    signed int PMP,AMP;
    double AMP_Temp,PMP_Temp,PMP_Theory;

	//设置时钟
	SysCtlClockSet(SYSCTL_SYSDIV_10| SYSCTL_USE_PLL| SYSCTL_OSC_MAIN |
								SYSCTL_XTAL_16MHZ);
    //打开FPU
    FPULazyStackingEnable();
    FPUEnable();
	//配置串口
	ConfigureUART();
    //AD9959初始化
    Init_AD9959(); 
	//使能GPIOF
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
	//设置PF1为输出
	GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_1);
	//设置PF1为2mA弱上拉
	GPIOPadConfigSet(GPIO_PORTF_BASE, GPIO_PIN_1 ,
                         GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPU);	
	//点灯
	GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1, GPIO_PIN_1);
    
		while(1)
	{
               //???????
               if(UARTCharsAvail(UART1_BASE))
               {       
                    while(UART_Buffer[Array_Count]!=0x0d)
                    {
                        Array_Count++;
                        UART_Buffer[Array_Count] = UARTCharGet(UART1_BASE);
                    }
                       if(UART_Buffer[1]==0x0a)                
                       {
                           if(UART_Buffer[2]==0x01)
                               DrawLCD=1-DrawLCD;
                           else if(UART_Buffer[2]==0x02)
                               DrawPoint=1-DrawPoint;
                       }
                       
                        else if(UART_Buffer[1]==0x0b)
                        {
                             if(UART_Buffer[2]==0x01)
                            {
                                FreValue=0;
                                for(i=0;i<Array_Count;i++)
                                {
                                  if(UART_Buffer[i]>47)
                                        FreValue+=(UART_Buffer[i]-48)*(pow(10,(Array_Count-i-1)));
                                }
                                FreN = (FreValue*4294967296)/200000000;
                                data0 = (uint8_t)(FreN>>24);
                                data1 = (uint8_t)(FreN>>16);
                                data2 = (uint8_t)(FreN>>8);
                                data3 = (uint8_t)FreN;
                      
                                UARTCharPut(UART3_BASE,0x01);
                                UARTCharPut(UART3_BASE,(uint8_t)data0);
                                UARTCharPut(UART3_BASE,(uint8_t)data1);
                                UARTCharPut(UART3_BASE,(uint8_t)data2);
                                UARTCharPut(UART3_BASE,(uint8_t)data3);
                                
                                printf("t1.txt=%c%d%c%c%c%c",x,FreValue,x,f,f,f);
                                
                                
                            }
                        }
                    
                        while(UARTCharsAvail(UART1_BASE))
                            Array_Count=UARTCharGet(UART1_BASE);
                        Array_Count = 0;
                }
               
                     if(DrawLCD==1)
                     {
                         
                          if(FreValue<100000)
                        {
                            FreValue += 2000;
                        }
                        else if(FreValue>100000&&FreValue<1000000)
                        {
                            FreValue += 10000;
                        }
                        else 
                        {    
                            FreValue += 40000;
                            if(FreValue > 4400000)
                            {
                                FreValue = 8000;
                            }
                        }
                        
                        FreN = (FreValue*4294967296)/200000000;
                        data0 = (uint8_t)(FreN>>24);
                        data1 = (uint8_t)(FreN>>16);
                        data2 = (uint8_t)(FreN>>8);
                        data3 = (uint8_t)FreN;
                      
                        UARTCharPut(UART3_BASE,0x01);
                        UARTCharPut(UART3_BASE,(uint8_t)data0);
                        UARTCharPut(UART3_BASE,(uint8_t)data1);
                        UARTCharPut(UART3_BASE,(uint8_t)data2);
                        UARTCharPut(UART3_BASE,(uint8_t)data3);
                        
                        
                       while(!UARTCharsAvail(UART3_BASE));
                        
                      if(UARTCharsAvail(UART3_BASE))
                      {
                                    while(RecFlag!=2)
                                    {
                                        Array3_Count++;
                                        UART3_Buffer[Array3_Count] = UARTCharGet(UART3_BASE);
                                        if(UART3_Buffer[Array3_Count]==0xab || UART3_Buffer[Array3_Count]==0xcd)
                                            RecFlag++;
                                    }
                                    while(UARTCharsAvail(UART3_BASE))
                                        RecFlag = UARTCharGet(UART3_BASE);
                                        RecFlag = 0;
                            
                            ReciValue = (signed short int)(UART3_Buffer[Array3_Count-5] << 8) | (signed short int) UART3_Buffer[Array3_Count-4];
                            RecqValue =(signed short int)(UART3_Buffer[Array3_Count-3] << 8) | (signed short int) UART3_Buffer[Array3_Count-2];
                                                     
                            Array3_Count = 0;
                          
                            Voltage[DC_Count].DC1 = ReciValue;
                            Voltage[DC_Count].DC2 = RecqValue;
                            DC_Count++;
                            
                            SysCtlDelay(10000);
                            
                          if(DC_Count==210)
                           {
                                FreValue = 10000;
                                printf("ref_stop%c%c%c",f,f,f);
                                printf("cle 1,0%c%c%c",f,f,f);
                                printf("cle 2,0%c%c%c",f,f,f);
                                printf("cle 3,0%c%c%c",f,f,f);
                               for(i=0;i<45;i++)
                               {
                                    PMP_Temp = atan2(Voltage[i].DC1,Voltage[i].DC2); 
                                    PMP_Temp = (PMP_Temp*180)/Pi;                                        
                                     PMP = (signed int) PMP_Temp;
                                    if(PMP<0)
                                        PMP+=360;
                                    PMP = PMP*0.7;       
                                    
                                    UARTprintf("%d\n",PMP);
                                    printf("add 1,0,%d%c%c%c",PMP,f,f,f);
                                    printf("add 1,0,%d%c%c%c",PMP,f,f,f);
                                    printf("add 1,0,%d%c%c%c",PMP,f,f,f);
                                    printf("add 1,0,%d%c%c%c",PMP,f,f,f);        

                                    FreValue += 2000;
                               }
                               
                               for(i=45;i<135;i++)
                               {
                                    PMP_Temp = atan2(Voltage[i].DC1,Voltage[i].DC2); 
                                    PMP_Temp = (PMP_Temp*180)/Pi;                             
                                     PMP = (signed int) PMP_Temp;
                                    if(PMP<0)
                                        PMP+=360;
                                    PMP = PMP*0.7;       
                                    
                                    UARTprintf("%d\n",PMP);
                                    printf("add 2,0,%d%c%c%c",PMP,f,f,f);
                                    printf("add 2,0,%d%c%c%c",PMP,f,f,f);                  

                                     FreValue += 10000;
                               }
                               
                               for(i=135;i<210;i++)
                               {
                                    PMP_Temp = atan2(Voltage[i].DC1,Voltage[i].DC2); 
                                    PMP_Temp = (PMP_Temp*180)/Pi;                                     
                                     PMP = (signed int) PMP_Temp;
                                    if(PMP<0)
                                        PMP+=360;
                                    PMP = PMP*0.7;       
                                    
                                    UARTprintf("%d\n",PMP);
                                    printf("add 3,0,%d%c%c%c",PMP,f,f,f);
                                    printf("add 3,0,%d%c%c%c",PMP,f,f,f);    

                                     FreValue += 40000;
                               }
                               printf("ref_star%c%c%c",f,f,f);
                               DC_Count = 0;
                               FreValue = 8000;
                               
                               SysCtlDelay(10000);
                           }
                    }
                      
                 }  
                     
                if(DrawPoint == 1)
                {
                                data0 = (uint8_t)(FreN>>24);
                                data1 = (uint8_t)(FreN>>16);
                                data2 = (uint8_t)(FreN>>8);
                                data3 = (uint8_t)FreN;
                              
                                UARTCharPut(UART3_BASE,0x01);
                                UARTCharPut(UART3_BASE,(uint8_t)data0);
                                UARTCharPut(UART3_BASE,(uint8_t)data1);
                                UARTCharPut(UART3_BASE,(uint8_t)data2);
                                UARTCharPut(UART3_BASE,(uint8_t)data3);

                    
                               if(UARTCharsAvail(UART3_BASE))
                               {
                                    while(RecFlag!=2)
                                    {
                                        Array3_Count++;
                                        UART3_Buffer[Array3_Count] = UARTCharGet(UART3_BASE);
                                        if(UART3_Buffer[Array3_Count]==0xab || UART3_Buffer[Array3_Count]==0xcd)
                                            RecFlag++;
                                    }
                                         while(UARTCharsAvail(UART3_BASE))
                                            RecFlag = UARTCharGet(UART3_BASE);
                                        RecFlag = 0;
                                        RecFlag = 0;

                                        ReciValue = (signed short int)(UART3_Buffer[Array3_Count-5] << 8) | (signed short int) UART3_Buffer[Array3_Count-4];
                                        RecqValue =(signed short int)(UART3_Buffer[Array3_Count-3] << 8) | (signed short int) UART3_Buffer[Array3_Count-2];
                                             
                                        Array3_Count = 0;
                                                                  
                                        PMP_Temp = atan2(ReciValue,RecqValue);
                                        PMP_Temp = (PMP_Temp*180)/Pi;    
                                         PMP = (signed int) PMP_Temp;
                                        if(PMP<0)
                                            PMP+=360;
                                        
                                        printf("t3.txt=%c%d%c%c%c%c",x,PMP,x,f,f,f); 
                                        UARTprintf("TSET");
                                        DrawPoint = 1;
                                        
                                        SysCtlDelay(1000000);
                              }           
                                         
                }
    }
}


