#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

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
#include "functions/delay.h"
#include "functions/uartprint.h"
#include "functions/uartreceive.h"



#define UARTRECEIVE_TASK_PRIO          4        //任务优先级
#define UARTRECEIVE_STK_SIZE           256       //任务堆栈大小 

uint8_t SORT_Status;                             //排序标志位
uint8_t UART_ReceiveBuffer[6500];                //UART2串口接收数组
uint32_t UART_ValueBuffer[300];                 //合并后的值的数组
uint8_t FPGA_DrawLine=0;                         //画图标志位

//*****************************************************************************
//
// 接收任务
//
//*****************************************************************************
void
DemoUARTRECEIVETask(void *pvParameters)
{
	uint16_t i=0,j=0;
	while(1)
	{

		GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_4,GPIO_PIN_4);
		if(UARTCharsAvail(UART2_BASE))
		{			
			taskENTER_CRITICAL();                //进入临界段，防止接收被打断
			//接收串口数据
			for(i=0;i<1600;i++)
			{
				UART_ReceiveBuffer[i]=UARTCharGet(UART2_BASE);
			}
		GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_4,0);
			while(UARTCharsAvail(UART2_BASE))
				UART_ReceiveBuffer[i++]=UARTCharGet(UART2_BASE);      //清除串口多余缓存
			for(i=0;i<1600;i++)                    //合并数据
			{
				if(UART_ReceiveBuffer[i]==0xab&&UART_ReceiveBuffer[i+1]==0xcd)
				{
						UART_ValueBuffer[++j]=(uint32_t)(UART_ReceiveBuffer[i-4]<<24)|(uint32_t)(UART_ReceiveBuffer[i-3]<<16)
																	|(uint32_t)(UART_ReceiveBuffer[i-2]<<8)|(uint32_t)(UART_ReceiveBuffer[i-1]);
//					  UARTprintf("%d  %d\n",j,UART_ValueBuffer[j]);
				}	
			}
			j=0;
			taskEXIT_CRITICAL();                 //退出临界段    
			SORT_Status=SORT_ENABLE;
			FPGA_DrawLine=FPGA_DrawLineEnable;
		}
		 vTaskDelay(500);
	}
}

//*****************************************************************************
//
// 创建接收数据任务
//
//*****************************************************************************
void
UARTRECEIVETaskInit(void)
{
    //
    // Create the fpgadrawline task.
    //
    xTaskCreate(
                        DemoUARTRECEIVETask,                       //任务函数
                       (const portCHAR *)"UARTREVEIVE", //任务名称 
                        UARTRECEIVE_STK_SIZE,              // 任务堆栈大小 
                        NULL,       
                        UARTRECEIVE_TASK_PRIO,
                        NULL
    );
    
    
}

