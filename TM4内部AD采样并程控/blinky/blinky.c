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
void
UART0Configure(void)
{
	//Ê¹ÄÜUART0
	SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);
	//Ê¹ÄÜGPIOA
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
	UARTClockSourceSet(UART0_BASE,UART_CLOCK_PIOSC);
	//ÅäÖÃUART1Òý½Å
	GPIOPinConfigure(GPIO_PA0_U0RX);
	GPIOPinConfigure(GPIO_PA1_U0TX);
	GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);	
	//ÅäÖÃUART1Ïà¹Ø²ÎÊý
//	UARTConfigSetExpClk(UART1_BASE, SysCtlClockGet(),115200,
//											(UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE |
//												UART_CONFIG_PAR_NONE));
//	//Ê¹ÄÜUART1ÖÐ¶Ï
//	IntEnable(INT_UART1);
//  UARTIntEnable(UART1_BASE, UART_INT_RX | UART_INT_RT);	
//	//UART1×¢²áÖÐ¶Ï
//	UARTIntRegister(UART0_BASE, UART1IntHandler);
	UARTStdioConfig(0,115200,16000000);
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
  uint32_t ADC_Value=1,ADC_LasValue=0;
  //forå¾ªçŽ¯ç”¨å’Œæ”¹å˜å¢žç›Šæ ‡å¿—ä½  
  uint8_t i,Gain=1;           
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
    
	//µãÁÁÀ¶µÆ
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
		UARTprintf(" %dmV ",ADC_Value);     
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
        ADC_Value = true;ADC_LasValue = false;
				SysCtlDelay(SysCtlClockGet()/5);
        
	}
}
