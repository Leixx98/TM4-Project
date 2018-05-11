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
#include "driverlib/adc.h"
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

//*****************************************************************************
//
// Configure the UART and its pins.  This must be called before UARTprintf().
//
//*****************************************************************************
uint8_t UART_Buffer[20];
uint8_t Array_Count;

int fputc(int ch, FILE *f)
{
  UARTCharPut(UART1_BASE,(uint8_t)ch);
  return ch;
}

void
UART0Configure(void)
{
	//Ê¹ÄÜUART0
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

void AD8253PinConfig()
{
    //ä½¿èƒ½æ—¶é’Ÿ
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOC);
    //å¼•è„šè®¾ç½®ä¸ºè¾“å‡º
    GPIODirModeSet(GPIO_PORTC_BASE,GPIO_PIN_6|GPIO_PIN_7,GPIO_DIR_MODE_OUT);
    //è®¾ç½®ä¸Šæ‹‰
    GPIOPadConfigSet(GPIO_PORTC_BASE,GPIO_PIN_6,GPIO_STRENGTH_2MA,GPIO_PIN_TYPE_STD_WPU);
    GPIOPadConfigSet(GPIO_PORTC_BASE,GPIO_PIN_7,GPIO_STRENGTH_2MA,GPIO_PIN_TYPE_STD_WPU);
    
    GPIOPinWrite(GPIO_PORTC_BASE,GPIO_PIN_6,0);
    GPIOPinWrite(GPIO_PORTC_BASE,GPIO_PIN_7,0);
}




//Ö÷º¯Êý
int main(void)
{	
  uint32_t ADC_Value=1,ADC_TempValue[15],Temp=24300,N0Val;
  //forå¾ªçŽ¯ç”¨å’Œæ”¹å˜å¢žç›Šæ ‡å¿—ä½  
  uint8_t i,Gain=100;       
  uint8_t x='\"',f=0xff;
  double Signal_Value;
//	char low,high;
//	char b=0xff;
	//ÉèÖÃÊ±ÖÓ
	SysCtlClockSet(SYSCTL_SYSDIV_10| SYSCTL_USE_PLL| SYSCTL_OSC_MAIN |
								SYSCTL_XTAL_16MHZ);
	//ÅäÖÃ´®¿Ú
	UART0Configure();
	//Ê¹ÄÜ¶Ë¿ÚF
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
	//Ê¹ÄÜ¶Ë¿ÚE
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE);
	//Ê¹ÄÜADC0
	SysCtlPeripheralEnable(SYSCTL_PERIPH_ADC0);
    
    AD8253PinConfig();
	GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_1);
	GPIOPinTypeADC(GPIO_PORTE_BASE,GPIO_PIN_7);
	GPIOPinWrite(GPIO_PORTF_BASE,GPIO_PIN_1,GPIO_PIN_1);
	
	ADCHardwareOversampleConfigure(ADC0_BASE,8); 
	ADCSequenceConfigure(ADC0_BASE, 0, ADC_TRIGGER_PROCESSOR, 0); 
	ADCSequenceStepConfigure(ADC0_BASE, 0, 0, ADC_CTL_CH0|ADC_CTL_IE | ADC_CTL_END );
	ADCSequenceEnable(ADC0_BASE, 0);
	ADCProcessorTrigger(ADC0_BASE, 0); 
    
     SysCtlDelay(SysCtlClockGet()/10);
     UARTCharPut(UART5_BASE,0x01);
     UARTCharPut(UART5_BASE,Temp>>8);
     UARTCharPut(UART5_BASE,Temp&0xff);

//	ADCIntClear(ADC0_BASE,0);
	while(1)
	{
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
                    {
                        Temp+=1;
                        if(Temp>49999)
                            Temp=0;
                        N0Val+=5;
                    }
                    else if(UART_Buffer[2]==0x02)
                    {
                        Temp-=1;
                        if(Temp<10000)
                            Temp=10026;
//                        N0Val-=5;
                    }
                }
                
                else if(UART_Buffer[1]==0x0b)
                {
                    Temp=0;
                    for(i=0;i<Array_Count;i++)
                    {
                      if(UART_Buffer[i]>47)
                            Temp+=(UART_Buffer[i]-48)*(pow(10,(Array_Count-i-1)));
                    }
                }
                
//                UARTprintf("%d\r\n",Temp);     
                UARTCharPut(UART5_BASE,0x01);
                UARTCharPut(UART5_BASE,Temp>>8);
                UARTCharPut(UART5_BASE,Temp&0xff);
                printf("n0.val=%d%c%c%c",Temp,f,f,f);
                Array_Count = 0;
                if(UARTCharsAvail(UART1_BASE))
                    Array_Count=UARTCharGet(UART1_BASE);
                Array_Count = 0;
        }
        
        
          
        for(i=0;i<10;i++)
        {
				ADCProcessorTrigger(ADC0_BASE, 0); 
				while(!ADCIntStatus(ADC0_BASE, 0, false)) 
				{
				}  
				ADCIntClear(ADC0_BASE,0);
				ADCSequenceDataGet(ADC0_BASE, 0, &ADC_Value );
				ADC_Value =ADC_Value*825>>10;
                ADC_TempValue[i] = ADC_Value;
                SysCtlDelay(SysCtlClockGet()/100);
        }   
        for(i=2;i<8;i++)
            ADC_Value += ADC_TempValue[i];
        ADC_Value /= 7;
        
        
//        if(Gain==1&&ADC_Value<181)
//         {
//                ADCProcessorTrigger(ADC0_BASE, 0); 
//				while(!ADCIntStatus(ADC0_BASE, 0, false)) 
//				{
//				}  
//				ADCIntClear(ADC0_BASE,0);
//				ADCSequenceDataGet(ADC0_BASE, 0, &ADC_Value );
//				ADC_Value =ADC_Value*825>>10;
//                ADC_TempValue[i] = ADC_Value;
//                SysCtlDelay(SysCtlClockGet()/100);
//              
//             if(Gain==1&&ADC_Value<181)
//             {               
//                 Gain = 10;
//                GPIOPinWrite(GPIO_PORTC_BASE,GPIO_PIN_6,GPIO_PIN_6);
//                GPIOPinWrite(GPIO_PORTC_BASE,GPIO_PIN_7,0);
//             }
//         }
//         
//         if(Gain==10&&ADC_Value>1700)
//         {
//                ADCProcessorTrigger(ADC0_BASE, 0); 
//				while(!ADCIntStatus(ADC0_BASE, 0, false)) 
//				{
//				}  
//				ADCIntClear(ADC0_BASE,0);
//				ADCSequenceDataGet(ADC0_BASE, 0, &ADC_Value );
//				ADC_Value =ADC_Value*825>>10;
//                ADC_TempValue[i] = ADC_Value;
//                SysCtlDelay(SysCtlClockGet()/100);   
//             
//                 if(Gain==10&&ADC_Value>1700)
//                {
//                    Gain = 1;
//                    GPIOPinWrite(GPIO_PORTC_BASE,GPIO_PIN_6,0);
//                    GPIOPinWrite(GPIO_PORTC_BASE,GPIO_PIN_7,0);
//                }
//         }
//         
//         if(Gain==10&&ADC_Value<240)
//         {
//                ADCProcessorTrigger(ADC0_BASE, 0); 
//				while(!ADCIntStatus(ADC0_BASE, 0, false)) 
//				{
//				}  
//				ADCIntClear(ADC0_BASE,0);
//				ADCSequenceDataGet(ADC0_BASE, 0, &ADC_Value );
//				ADC_Value =ADC_Value*825>>10;
//                ADC_TempValue[i] = ADC_Value;
//                SysCtlDelay(SysCtlClockGet()/100);   
//             
//                 if(Gain==10&&ADC_Value<240)
//                {
//                    Gain = 100;
//                    GPIOPinWrite(GPIO_PORTC_BASE,GPIO_PIN_6,0);
//                    GPIOPinWrite(GPIO_PORTC_BASE,GPIO_PIN_7,GPIO_PIN_7);
//                }
//         }
//         
//          if(Gain==100&&ADC_Value>2050)
//         {
//                ADCProcessorTrigger(ADC0_BASE, 0); 
//				while(!ADCIntStatus(ADC0_BASE, 0, false)) 
//				{
//				}  
//				ADCIntClear(ADC0_BASE,0);
//				ADCSequenceDataGet(ADC0_BASE, 0, &ADC_Value );
//				ADC_Value =ADC_Value*825>>10;
//                ADC_TempValue[i] = ADC_Value;
//                SysCtlDelay(SysCtlClockGet()/100);   
//             
//                 if(Gain==100&&ADC_Value>2050)
//                {
//                    Gain = 10;
//                    GPIOPinWrite(GPIO_PORTC_BASE,GPIO_PIN_6,GPIO_PIN_6);
//                    GPIOPinWrite(GPIO_PORTC_BASE,GPIO_PIN_7,0);
//                }
//         }
//         
//         if(Gain==1)
//         {
//             if(ADC_Value>1545)
//             {
//                Signal_Value = (double)(0.0059*ADC_Value-0.6529);
//                 UARTprintf("%d\r\n",ADC_Value);
//                printf("t2.txt=%c%.2fmVrms%c%c%c%c",x,Signal_Value,x,f,f,f);  
//             }             
//             else if(ADC_Value<1545&&ADC_Value>1100)
//             {
//                Signal_Value = (double)(0.0056*ADC_Value-0.2015);
//                 UARTprintf("%d\r\n",ADC_Value);
//                printf("t2.txt=%c%.2fmVrms%c%c%c%c",x,Signal_Value,x,f,f,f);                   
//             }        
//              else if(ADC_Value<1100&&ADC_Value>645)
//             {
//                Signal_Value = (double)(0.0053*ADC_Value+0.1504);
//                 UARTprintf("%d\r\n",ADC_Value);
//                printf("t2.txt=%c%.2fmVrms%c%c%c%c",x,Signal_Value,x,f,f,f);                   
//             }
//             else if(ADC_Value<645)
//             {
//                Signal_Value = (double)(0.0054*ADC_Value-0.0127);
//                 UARTprintf("%d\r\n",ADC_Value);
//                printf("t2.txt=%c%.2fmVrms%c%c%c%c",x,Signal_Value,x,f,f,f);                   
//             }             

//         }
         
         
//         if(Gain==10)
//         {
//             if(ADC_Value>2180)
//             {
//                Signal_Value = (double)(0.66*ADC_Value-272.5);   
//                UARTprintf("%d\r\n",ADC_Value);
//                printf("t2.txt=%c%.1fuVrms%c%c%c%c",x,Signal_Value,x,f,f,f);  
//             }
//             else if(ADC_Value<2180 &&ADC_Value >1870)
//             {
//                Signal_Value = (double)(0.58*ADC_Value-98.896);  
//                UARTprintf("%d\r\n",ADC_Value);
//                printf("t2.txt=%c%.1fuVrms%c%c%c%c",x,Signal_Value,x,f,f,f);  
//             }
//              else if(ADC_Value<2180 &&ADC_Value >1535)
//             {
//                Signal_Value = (double)(0.5903*ADC_Value-122.39);  
//                UARTprintf("%d\r\n",ADC_Value);
//                printf("t2.txt=%c%.1fuVrms%c%c%c%c",x,Signal_Value,x,f,f,f);  
//             }            
//              else if(ADC_Value<1535 &&ADC_Value >710)
//             {
//                Signal_Value = (double)(0.5342*ADC_Value-39.677);   
//                UARTprintf("%d\r\n",ADC_Value);
//                 printf("t2.txt=%c%.1fuVrms%c%c%c%c",x,Signal_Value,x,f,f,f);  
//             }
//             else if(ADC_Value<710)
//             {
//                Signal_Value = (double)(0.58*ADC_Value-56.354);   
//                UARTprintf("%d\r\n",ADC_Value);
//                 printf("t2.txt=%c%.1fuVrms%c%c%c%c",x,Signal_Value,x,f,f,f);  
//             }             
//         }
//         
         if(Gain==100)
         {
             if(ADC_Value>2040)
             {
                 Signal_Value = (double)(0.067*ADC_Value-68.905);   
                UARTprintf("%d\r\n",ADC_Value);
                 printf("t2.txt=%c%.2fuVrms%c%c%c%c",x,Signal_Value,x,f,f,f);  
             }
             else if(ADC_Value<2040&&ADC_Value>1540)
             {
                Signal_Value = (double)(0.0604*ADC_Value-52.485);   
                UARTprintf("%d\r\n",ADC_Value);
                 printf("t2.txt=%c%.2fuVrms%c%c%c%c",x,Signal_Value,x,f,f,f);  
             }             
             else if(ADC_Value<1540&&ADC_Value>1290)
             {
                Signal_Value = (double)(0.0537*ADC_Value-42.469);   
                UARTprintf("%d\r\n",ADC_Value);
                 printf("t2.txt=%c%.2fuVrms%c%c%c%c",x,Signal_Value,x,f,f,f);  
             }  
             else if(ADC_Value<1095)
             {
                Signal_Value = (double)(0.0247*ADC_Value-12.0571);   
                UARTprintf("%d\r\n",ADC_Value);
                 printf("t2.txt=%c%.2fuVrms%c%c%c%c",x,Signal_Value,x,f,f,f);                 
             }
         }
                UARTprintf("%d\r\n",ADC_Value);
				SysCtlDelay(SysCtlClockGet()/5);
        
	}
}
