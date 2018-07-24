#include <stdint.h>
#include <stdbool.h>
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
#include "utils/buttons.h"
#include "functions/usart.h"
#include "functions/ad8369.h"

//*****************************************************************************
//
//! ms级延时
//!
//!
//! \return None.
//
//*****************************************************************************
void delay_ms(uint8_t ms)
{
   do
   {
          SysCtlDelay(SysCtlClockGet()/1000/3);
   }while(ms--);       
}
//*****************************************************************************
//
//! AD8369引脚初始化
//!
//!
//! \return None.
//
//*****************************************************************************
void AD8369_Init(void)
{
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOQ);

    //
    // Check if the peripheral access is enabled.
    //
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOQ))
    {
    }
    // enable the GPIO pin for digital function.
    //
    GPIOPinTypeGPIOOutput(GPIO_PORTQ_BASE, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3);
    GPIOPadConfigSet(GPIO_PORTQ_BASE, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3,
                         GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPU);	  
}
//*****************************************************************************
//
//! AD8369增益控制.
//!
//!
//! \return None.
//
//*****************************************************************************
void AD8369_ControlGain(uint8_t data)
{
    uint8_t i;
    if(data < 16)
    {
        CLK_CLR;
        CS_SET;
        DATA_SET;
        
        CS_CLR;
        delay_ms(1);
        data &=0xf;       
        
        for(i=0;i<4;i++)
        {
            if(data&0x8)
                {DATA_SET;}
            else
                {DATA_CLR;}
            delay_ms(1);
           CLK_SET;
            data<<=1;
            delay_ms(3);
            CLK_CLR;
            delay_ms(1);
        }
        
        CS_SET;
        delay_ms(1);
            
    }
}

