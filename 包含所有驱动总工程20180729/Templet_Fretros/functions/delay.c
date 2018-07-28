#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
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
#include "functions/delay.h"

extern uint32_t SysClock;

void delay_us(uint32_t us)
{
    us/=2;           //一次延时为2us
    do
    {
        SysCtlDelay(SysClock/1000000/3);
    }while(us--);
}

void delay_ms(uint16_t ms)
{
    do
    {
            delay_us(1000);
    }while(ms--);
}

