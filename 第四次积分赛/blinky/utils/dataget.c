#include <stdbool.h>
#include <stdint.h>
#include "inc/hw_types.h"
#include "inc/hw_memmap.h"
#include "inc/hw_gpio.h"
#include "driverlib/sysctl.h"
#include "driverlib/rom.h"
#include "driverlib/pin_map.h"
#include "driverlib/gpio.h"
#include "driverlib/uart.h"
#include "utils/dataget.h"

extern uint8_t UART_Buffer[20];
extern uint8_t Array_Count;
extern uint8_t Data_Flag;

void Dataget(void)
{
        if(UARTCharsAvail(UART4_BASE))
        {
                while(UART_Buffer[Array_Count]!=0x0d)
                {
                    Array_Count++;
                    UART_Buffer[Array_Count] = UARTCharGet(UART4_BASE);
                }
                Data_Flag=1;
        }
}


