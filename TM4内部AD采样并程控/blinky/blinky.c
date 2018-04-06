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
	// πƒ‹UART0
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
    //‰ΩøËÉΩÊó∂Èíü
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOC);
    //ÂºïËÑöËÆæÁΩÆ‰∏∫ËæìÂá∫
    GPIODirModeSet(GPIO_PORTC_BASE,GPIO_PIN_6|GPIO_PIN_7,GPIO_DIR_MODE_OUT);
    //ËÆæÁΩÆ‰∏äÊãâ
    GPIOPadConfigSet(GPIO_PORTC_BASE,GPIO_PIN_6,GPIO_STRENGTH_2MA,GPIO_PIN_TYPE_STD_WPU);
    GPIOPadConfigSet(GPIO_PORTC_BASE,GPIO_PIN_7,GPIO_STRENGTH_2MA,GPIO_PIN_TYPE_STD_WPU);
    
    GPIOPinWrite(GPIO_PORTC_BASE,GPIO_PIN_6,0);
    GPIOPinWrite(GPIO_PORTC_BASE,GPIO_PIN_7,0);
}




//÷˜∫Ø ˝
int main(void)
{	
  uint32_t ADC_Value=1,ADC_LasValue=0,Temp;
  //forÂæ™ÁéØÁî®ÂíåÊîπÂèòÂ¢ûÁõäÊ†áÂøó‰Ωç  
  uint8_t i,Gain=1;       
  uint8_t x='\"',f=0xff;
//	char low,high;
//	char b=0xff;
	//…Ë÷√ ±÷”
	SysCtlClockSet(SYSCTL_SYSDIV_10| SYSCTL_USE_PLL| SYSCTL_OSC_MAIN |
								SYSCTL_XTAL_16MHZ);
	//≈‰÷√¥Æø⁄
	UART0Configure();
	// πƒ‹∂Àø⁄F
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
	// πƒ‹∂Àø⁄E
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE);
	// πƒ‹ADC0
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
                    Temp = UART_Buffer[2];
                }
                else if(UART_Buffer[1]==0x0b)
                {
                    for(i=0;i<Array_Count;i++)
                    {
                      if(UART_Buffer[i]>47)
                            Temp+=(UART_Buffer[i]-48)*(pow(10,(Array_Count-i-1)));
                    }
                }
                
                UARTprintf("%d\r\n",Temp);     
                UARTCharPut(UART5_BASE,0x01);
                UARTCharPut(UART5_BASE,Temp>>8);
                UARTCharPut(UART5_BASE,Temp&0xff);
                Array_Count = 0;Temp=0;
        }
        
        
          
        while(ADC_Value!=ADC_LasValue)
        {
                ADC_LasValue = ADC_Value;
				ADCProcessorTrigger(ADC0_BASE, 0); 
				while(!ADCIntStatus(ADC0_BASE, 0, false)) 
				{
				}  
				ADCIntClear(ADC0_BASE,0);
				ADCSequenceDataGet(ADC0_BASE, 0, &ADC_Value );
				ADC_Value =ADC_Value*825>>10;
        }   
            
        if(Gain==1&&ADC_Value<130)
         {
             Gain = 10;
            GPIOPinWrite(GPIO_PORTC_BASE,GPIO_PIN_6,GPIO_PIN_6);
            GPIOPinWrite(GPIO_PORTC_BASE,GPIO_PIN_7,0);
         }
         
         if(Gain==10&&ADC_Value>2400)
         {
            Gain = 1;
            GPIOPinWrite(GPIO_PORTC_BASE,GPIO_PIN_6,0);
            GPIOPinWrite(GPIO_PORTC_BASE,GPIO_PIN_7,0);
         }
         
        UARTprintf(" %dmV ",ADC_Value);  
        ADC_Value = true;ADC_LasValue = false;
				SysCtlDelay(SysCtlClockGet()/5);
        
	}
}
