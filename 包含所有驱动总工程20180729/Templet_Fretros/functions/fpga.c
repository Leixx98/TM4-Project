#include <stdint.h>
#include <stdbool.h>

#include "inc/hw_gpio.h"
#include "inc/hw_ints.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "inc/hw_nvic.h"

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

#include "FreeRTOSConfig.h"
#include "FreeRTOS.h"
#include "queue.h"
#include "task.h"

#include "functions/led_task.h"
#include "functions/uartprint.h"
#include "functions/fpga.h"

#define FPGA_TASK_PRIO          2        //任务优先级
#define FPGA_STK_SIZE           128       //任务堆栈大小 

//*****************************************************************************
//
// 与FPGA通信任务
//
//*****************************************************************************
void
DemoFPGATask(void *pvParameters)
{
	uint32_t fren;
	while(1)
	{
		if(Mode==FPGA_DataSend)
		{
			fren=(receivevalue*4294967296)/200000000;
			UARTCharPut(UART2_BASE,0x01);
			UARTCharPut(UART2_BASE,(uint8_t)(fren>>24));
			UARTCharPut(UART2_BASE,(uint8_t)(fren>>16));
			UARTCharPut(UART2_BASE,(uint8_t)(fren>>8));
			UARTCharPut(UART2_BASE,(uint8_t)fren);
			receivevalue=0;
			Mode=0xff;
		}
		vTaskDelay(128);
	}
}
//*****************************************************************************
//
// 创建按键任务
//
//*****************************************************************************
void
FPGATaskInit(void)
{

    //
    // Create the LED task.
    //
    xTaskCreate(
                        DemoFPGATask,                       //任务函数
                       (const portCHAR *)"FPGA", //任务名称 
                        FPGA_STK_SIZE,              // 任务堆栈大小 
                        NULL,       
                        FPGA_TASK_PRIO,
                        NULL
    );
    
    
}

