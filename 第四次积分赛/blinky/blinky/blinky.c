/*******************************************
??:???
??:?????????3.6??????
????:3.3V
????:1.645V
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
#include "utils/ads1115.h"
#include "utils/key.h"


#ifdef DEBUG
void
__error__(char *pcFilename, uint32_t ui32Line)
{
}
#endif

#define Pi 3.1415926

typedef struct 
{
    double DC1;
    double DC2;
} DC;

int32_t t;
uint8_t UART_Buffer[20];
uint8_t Array_Count;
uint8_t DrawLCD,DrawPoint;
uint8_t DC_Count;
DC Voltage[210];
uint32_t FreValue=10000;
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

//void UART1IntHandler(void)  
//{  
//    uint8_t i;
//    uint32_t flag = UARTIntStatus(UART1_BASE,1);  
//    //??????  
//    UARTIntClear(UART1_BASE,flag);  
//    
//    if(flag&UART_INT_RX)  
//    {
//           if(UARTCharsAvail(UART1_BASE))
//               {       
//                    while(UART_Buffer[Array_Count]!=0x0d)
//                    {
//                        Array_Count++;
//                        UART_Buffer[Array_Count] = UARTCharGet(UART1_BASE);
//                    }
//                       if(UART_Buffer[1]==0x0a)                
//                       {
//                           if(UART_Buffer[2]==0x01)
//                               DrawLCD = 1-DrawLCD;
//                           else if(UART_Buffer[2]==0x02)
//                               DrawPoint = 1-DrawPoint;
//                       }
//                       
//                        else if(UART_Buffer[1]==0x0b)
//                        {
//                            if(UART_Buffer[2]==0x01)
//                            {
//                                FreValue=0;
//                                for(i=0;i<Array_Count;i++)
//                                {
//                                  if(UART_Buffer[i]>47)
//                                        FreValue+=(UART_Buffer[i]-48)*(pow(10,(Array_Count-i-1)));
//                                }
//                            }
//                            
//                        }
//                        Write_Quadrature(FreValue);
//                        while(UARTCharsAvail(UART1_BASE))
//                            Array_Count=UARTCharGet(UART1_BASE);
//                        UARTRxErrorClear(UART1_BASE);
//                        Array_Count = 0; 
//                }
//    }
//    
//}
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
//    UARTFIFODisable(UART1_BASE);  
//    //??UART0??  
//    IntEnable(INT_UART1);  
//    //??UART0????  
//    UARTIntEnable(UART1_BASE,UART_INT_RX);  
//    //UART??????  
//    UARTIntRegister(UART1_BASE,UART1IntHandler);  
//    //??????  
//    IntMasterEnable();   
}



//???
int main(void)
{
    //???????,?????????????


    uint8_t i,x='\"',f=0xff;
    double ADC_Value1,ADC_Value2;
    uint16_t ADC_Temp;
    signed int PMP,AMP;
    double AMP_Temp;
    double LPF_Offvol1,LPF_Offvol2;
    double PMP_Temp,PMP_Init;
	//????
	SysCtlClockSet(SYSCTL_SYSDIV_2_5| SYSCTL_USE_PLL| SYSCTL_OSC_MAIN |
								SYSCTL_XTAL_16MHZ);
	//????
	ConfigureUART();
    //ADS1115?????
    ADS1115_IO_Init();
   //AD9959???
    Init_AD9959(); 
	//??GPIOF
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
	//??PF1???
	GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_1);
	//??PF1?2mA???
	GPIOPadConfigSet(GPIO_PORTF_BASE, GPIO_PIN_1 ,
                         GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPU);	
	//??
	GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1, GPIO_PIN_1);
    
    //µ÷Áã
     Write_Quadrature(0);
    ADC_Temp = ADS1115_Getdata(2); 
    ADC_Value1 = (ADC_Temp*12.281)/65536;        

    ADC_Temp = ADS1115_Getdata(3); 
    ADC_Value2 = (ADC_Temp*12.281)/65536;      
    
    LPF_Offvol1 = ADC_Value1;
    LPF_Offvol2 = ADC_Value2;

    while(1)
	{
		          if(DrawLCD==1)
                 {
                        ADC_Temp = ADS1115_Getdata(2); 
                        ADC_Value1 = (ADC_Temp*12.281)/65536;        


                        ADC_Temp = ADS1115_Getdata(3); 
                        ADC_Value2 = (ADC_Temp*12.281)/65536;        
                     
                        ADC_Value1= (ADC_Value1 - LPF_Offvol1) / 3.375;
                        ADC_Value1 = -ADC_Value1;
                     
                        ADC_Value2= (ADC_Value2 - LPF_Offvol2) / 3.35;
                        ADC_Value2 = -ADC_Value2;
                         
                        
                        Voltage[DC_Count].DC1 = ADC_Value1;
                        Voltage[DC_Count].DC2 = ADC_Value2;
                        DC_Count++;
        
        
                        if(DC_Count == 215)
                        {
                            printf("ref_stop%c%c%c",f,f,f);
                            printf("cle 1,0%c%c%c",f,f,f);
                            printf("cle 2,0%c%c%c",f,f,f);
                            printf("cle 3,0%c%c%c",f,f,f);
                            FreValue = 8000;
                            for(i=0;i<45;i++)
                            {
                                AMP_Temp = (2*sqrt(pow(Voltage[i].DC1,2.0)+pow(Voltage[i].DC2,2.0)))/0.25;
                                AMP_Temp = 20*log10(AMP_Temp)-6.5;
                                AMP = (uint32_t) AMP_Temp;
                                AMP = 255-AMP*3;
                                
                                if(Voltage[i].DC1<0&&Voltage[i].DC2>0)
                                {
                                    PMP_Temp = atan(-(Voltage[i].DC1/Voltage[i].DC2)); 
                                    PMP_Temp = (PMP_Temp*180)/Pi;
                                }
                                
                                else if(Voltage[i].DC1<0&&Voltage[i].DC2<0)
                                {
                                    PMP_Temp = atan(-(Voltage[i].DC1/Voltage[i].DC2))+Pi; 
                                    PMP_Temp = (PMP_Temp*180)/Pi;
                                }
                                
                               else if(Voltage[i].DC1>0&&Voltage[i].DC2>0)
                                {
                                    PMP_Temp = atan(-(Voltage[i].DC1/Voltage[i].DC2)); 
                                    PMP_Temp = (PMP_Temp*180)/Pi;
                                }
                                
                                else if(Voltage[i].DC1>0&&Voltage[i].DC2<0)
                                {
                                    PMP_Temp = atan(-(Voltage[i].DC1/Voltage[i].DC2))+Pi; 
                                    PMP_Temp = (PMP_Temp*180)/Pi;
                                }    
                                PMP_Temp -= PMP_Init;
                                PMP = (signed int) PMP_Temp;
                                if(PMP<0)
                                    PMP+=360;
                                PMP = PMP*0.7; 
                                
                                printf("add 1,0,%d%c%c%c",AMP,f,f,f);
                                printf("add 1,0,%d%c%c%c",AMP,f,f,f);
                                printf("add 1,0,%d%c%c%c",AMP,f,f,f);
                                printf("add 1,0,%d%c%c%c",AMP,f,f,f);                                
                                printf("add 1,1,%d%c%c%c",PMP,f,f,f);
                                printf("add 1,1,%d%c%c%c",PMP,f,f,f);
                                printf("add 1,1,%d%c%c%c",PMP,f,f,f);
                                printf("add 1,1,%d%c%c%c",PMP,f,f,f);                                          
                            }
                            for(i=45;i<135;i++)
                            {
                                AMP_Temp = (2*sqrt(pow(Voltage[i].DC1,2.0)+pow(Voltage[i].DC2,2.0)))/0.25;
                                AMP_Temp = 20*log10(AMP_Temp)-6.5;
                                AMP = (uint32_t) AMP_Temp;
                                AMP = 255-AMP*3;
                                
                                if(Voltage[i].DC1<0&&Voltage[i].DC2>0)
                                {
                                    PMP_Temp = atan(-(Voltage[i].DC1/Voltage[i].DC2)); 
                                    PMP_Temp = (PMP_Temp*180)/Pi;
                                }
                                
                                else if(Voltage[i].DC1<0&&Voltage[i].DC2<0)
                                {
                                    PMP_Temp = atan(-(Voltage[i].DC1/Voltage[i].DC2))+Pi; 
                                    PMP_Temp = (PMP_Temp*180)/Pi;
                                }
                                
                               else if(Voltage[i].DC1>0&&Voltage[i].DC2>0)
                                {
                                    PMP_Temp = atan(-(Voltage[i].DC1/Voltage[i].DC2)); 
                                    PMP_Temp = (PMP_Temp*180)/Pi;
                                }
                                
                                else if(Voltage[i].DC1>0&&Voltage[i].DC2<0)
                                {
                                    PMP_Temp = atan(-(Voltage[i].DC1/Voltage[i].DC2))+Pi; 
                                    PMP_Temp = (PMP_Temp*180)/Pi;
                                }    
                                if(i==0)
                                    PMP_Init = PMP_Temp;
                                PMP_Temp -= PMP_Init;
                                PMP = (signed int) PMP_Temp;
                                if(PMP<0)
                                    PMP+=360;
                                PMP = PMP*0.7; 
                                
                                printf("add 2,0,%d%c%c%c",AMP,f,f,f);
                                printf("add 2,0,%d%c%c%c",AMP,f,f,f);
                                printf("add 2,1,%d%c%c%c",PMP,f,f,f);
                                printf("add 2,1,%d%c%c%c",PMP,f,f,f);
                            }
                            for(i=135;i<210;i++)
                            {
                                AMP_Temp = (2*sqrt(pow(Voltage[i].DC1,2.0)+pow(Voltage[i].DC2,2.0)))/0.25;
                                AMP_Temp = 20*log10(AMP_Temp)-6.5;
                                AMP = (uint32_t) AMP_Temp;                
                                AMP = 255-AMP*3;
                                
                                if(Voltage[i].DC1<0&&Voltage[i].DC2>0)
                                {
                                    PMP_Temp = atan(-(Voltage[i].DC1/Voltage[i].DC2)); 
                                    PMP_Temp = (PMP_Temp*180)/Pi;
                                }
                                
                                else if(Voltage[i].DC1<0&&Voltage[i].DC2<0)
                                {
                                    PMP_Temp = atan(-(Voltage[i].DC1/Voltage[i].DC2))+Pi; 
                                    PMP_Temp = (PMP_Temp*180)/Pi;
                                }
                                
                               else if(Voltage[i].DC1>0&&Voltage[i].DC2>0)
                                {
                                    PMP_Temp = atan(-(Voltage[i].DC1/Voltage[i].DC2)); 
                                    PMP_Temp = (PMP_Temp*180)/Pi;
                                }
                                
                                else if(Voltage[i].DC1>0&&Voltage[i].DC2<0)
                                {
                                    PMP_Temp = atan(-(Voltage[i].DC1/Voltage[i].DC2))+Pi; 
                                    PMP_Temp = (PMP_Temp*180)/Pi;
                                }    
                                PMP_Temp -= PMP_Init;
                                PMP = (signed int) PMP_Temp;
                                if(PMP<0)
                                    PMP+=360;
                                PMP = PMP*0.7; 
                                
                                printf("add 3,0,%d%c%c%c",AMP,f,f,f);
                                printf("add 3,0,%d%c%c%c",AMP,f,f,f);
                                printf("add 3,1,%d%c%c%c",PMP,f,f,f);
                                printf("add 3,1,%d%c%c%c",PMP,f,f,f);

                            }       
                            printf("ref_star%c%c%c",f,f,f);
                            FreValue = 8000;
                            DC_Count = 0;
                            
                            //µ÷Áã
                            Write_Quadrature(0);
                            ADC_Temp = ADS1115_Getdata(2); 
                            ADC_Value1 = (ADC_Temp*12.281)/65536;        

                            ADC_Temp = ADS1115_Getdata(3); 
                            ADC_Value2 = (ADC_Temp*12.281)/65536;      
                            
                            LPF_Offvol1 = ADC_Value1;
                            LPF_Offvol2 = ADC_Value2;
                            SysCtlDelay(10000);
                            
                      }
//                        
                        if(FreValue<100000)
                        {
                            FreValue += 2000;
                            SysCtlDelay(5000);
                        }
                        else if(FreValue>100000&&FreValue<1000000)
                        {
                            FreValue += 10000;
                            SysCtlDelay(5000);
                        }
                        else 
                        {    
                            FreValue += 40000;
                            SysCtlDelay(5000);
                            if(FreValue > 4400000)
                                FreValue = 10000;
                        }
                        
                        Write_Quadrature(FreValue);
                }
                 
                if(DrawPoint == 1)
                {
                            //µ÷Áã
//                         Write_Quadrature(0);
//                        ADC_Temp = ADS1115_Getdata(2); 
//                        ADC_Value1 = (ADC_Temp*12.281)/65536;        

//                        ADC_Temp = ADS1115_Getdata(3); 
//                        ADC_Value2 = (ADC_Temp*12.281)/65536;      
//                        
//                        LPF_Offvol1 = ADC_Value1;
//                        LPF_Offvol2 = ADC_Value2;
                        
                        Write_Quadrature(10000);
                         ADC_Temp = ADS1115_Getdata(2); 
                        ADC_Value1 = (ADC_Temp*12.281)/65536;        

                        ADC_Temp = ADS1115_Getdata(3); 
                        ADC_Value2 = (ADC_Temp*12.281)/65536;        

                     
                        ADC_Value1= (ADC_Value1 - LPF_Offvol1) / 3.375;
                        ADC_Value1 = -ADC_Value1;
                     
                        ADC_Value2= (ADC_Value2 - LPF_Offvol2) / 3.35;
                        ADC_Value2 = -ADC_Value2;
        
                    
                        if(ADC_Value1<0&&ADC_Value2>0)
                        {
                            PMP_Temp = atan(-(ADC_Value1/ADC_Value2)); 
                            PMP_Temp = (PMP_Temp*180)/Pi;
                        }
                        
                        else if(ADC_Value1<0&&ADC_Value2<0)
                        {
                            PMP_Temp = atan(-(ADC_Value1/ADC_Value2))+Pi; 
                            PMP_Temp = (PMP_Temp*180)/Pi;
                        }
                        
                       else if(ADC_Value1>0&&ADC_Value2>0)
                        {
                            PMP_Temp = atan(-(ADC_Value1/ADC_Value2)); 
                            PMP_Temp = (PMP_Temp*180)/Pi;
                        }
                        
                        else if(ADC_Value1>0&&ADC_Value2<0)
                        {
                            PMP_Temp = atan(-(ADC_Value1/ADC_Value2))-Pi; 
                            PMP_Temp = (PMP_Temp*180)/Pi;
                        }
                        
                            PMP_Init = PMP_Temp;
                            Write_Quadrature(FreValue);      
                    
                         ADC_Temp = ADS1115_Getdata(2); 
                        ADC_Value1 = (ADC_Temp*12.281)/65536;        


                        ADC_Temp = ADS1115_Getdata(3); 
                        ADC_Value2 = (ADC_Temp*12.281)/65536;        
                     
                        ADC_Value1= (ADC_Value1 - LPF_Offvol1) / 3.375;
                        ADC_Value1 = -ADC_Value1;
                     
                        ADC_Value2= (ADC_Value2 - LPF_Offvol2) / 3.35;
                        ADC_Value2 = -ADC_Value2;
        
                        AMP_Temp = (2*sqrt(pow(ADC_Value1,2.0)+pow(ADC_Value2,2.0)))/0.25;
                        AMP_Temp = 20*log10(AMP_Temp);
                        AMP = (signed int) AMP_Temp;
                        
                        if(ADC_Value1<0&&ADC_Value2>0)
                        {
                            PMP_Temp = atan(-(ADC_Value1/ADC_Value2)); 
                            PMP_Temp = (PMP_Temp*180)/Pi;
                        }
                        
                        else if(ADC_Value1<0&&ADC_Value2<0)
                        {
                            PMP_Temp = atan(-(ADC_Value1/ADC_Value2))+Pi; 
                            PMP_Temp = (PMP_Temp*180)/Pi;
                        }
                        
                       else if(ADC_Value1>0&&ADC_Value2>0)
                        {
                            PMP_Temp = atan(-(ADC_Value1/ADC_Value2)); 
                            PMP_Temp = (PMP_Temp*180)/Pi;
                        }
                        
                        else if(ADC_Value1>0&&ADC_Value2<0)
                        {
                            PMP_Temp = atan(-(ADC_Value1/ADC_Value2))-Pi; 
                            PMP_Temp = (PMP_Temp*180)/Pi;
                        }
//                        AMP_Temp = pow(ADC_Value1,2.0)+pow(ADC_Value2,2.0);
                        PMP_Temp -= PMP_Init;
                        PMP = (signed int) PMP_Temp;
                        if(PMP< 0   )
                            PMP += 360;
                        
                        printf("t3.txt=%c%d%c%c%c%c",x,PMP,x,f,f,f);
                        printf("t2.txt=%c%d%c%c%c%c",x,AMP,x,f,f,f);
                }
                
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
                               DrawLCD = 1-DrawLCD;
                           else if(UART_Buffer[2]==0x02)
                               DrawPoint = 1-DrawPoint;
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
                            }
                            
                        }
                        Write_Quadrature(FreValue);
                        while(UARTCharsAvail(UART1_BASE))
                            Array_Count=UARTCharGet(UART1_BASE);
                        UARTRxErrorClear(UART1_BASE);
                        Array_Count = 0; 
                }
                
             
	}
}


