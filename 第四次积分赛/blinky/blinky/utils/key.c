#include <stdbool.h>
#include <stdint.h>
#include "inc/hw_types.h"
#include "inc/hw_memmap.h"
#include "inc/hw_gpio.h"
#include "driverlib/sysctl.h"
#include "driverlib/rom.h"
#include "driverlib/pin_map.h"
#include "driverlib/gpio.h"
#include "utils/buttons.h"
#include "utils/key.h"

extern uint8_t DrawLCD;

void Key_State(uint8_t key)
{
    if(key==1)
        DrawLCD = 1;
    if(key==2)
        DrawLCD = 0;
}
