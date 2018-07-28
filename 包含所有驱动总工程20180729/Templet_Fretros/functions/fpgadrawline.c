#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include "math.h"

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
#include "functions/uartreceive.h"
#include "functions/fpgadrawline.h"


#define FPGADRAWLINE_TASK_PRIO          3        //任务优先级
#define FPGADRAWLINE_STK_SIZE           128       //任务堆栈大小 
uint8_t UART2_ReceiveBuffer[2047];                //UART2串口接收数组
int16_t UART2_ValueBuffer[600];                   //储存合并后的值的数组   
//*****************************************************************************
//
// 画图任务
//
//*****************************************************************************
void
DemoFPGADRAWLINETask(void *pvParameters)
{
	uint16_t i=0;
	uint8_t f=0xff;
	uint32_t linepoint=0;
	while(1)
	{
		if(FPGA_DrawLine==FPGA_DrawLineEnable)
		{
			FPGA_DrawLine=FPGA_DrawLineDisable;
			taskENTER_CRITICAL();                //进入临界段，防止接收被打断
			//接收串口数据        
			printf("ref_stop%c%c%c",f,f,f);             //关闭屏幕刷新，等数据发送完后再刷新屏幕
			printf("cle 1,0%c%c%c",f,f,f);              //清除屏幕
			for(i=0;i<4;i++)
			{
				printf("add 1,0,%d%c%c%c",0,f,f,f);
				printf("add 1,0,%d%c%c%c",0,f,f,f);	
			}				
			for(i=4;i<254;i++)
			{
					linepoint=sqrt(UART_ValueBuffer[i]);
					printf("add 1,0,%d%c%c%c",linepoint/100,f,f,f);
					printf("add 1,0,%d%c%c%c",linepoint/100,f,f,f);
			}
			printf("ref_star%c%c%c",f,f,f);     //恢复刷新
			taskEXIT_CRITICAL();                 //退出临界段
		}
		 vTaskDelay(100);
	}
}
//*****************************************************************************
//
// 创建接收数据画图任务
//
//*****************************************************************************
void
FPGADRAWLINETaskInit(void)
{
    //
    // Create the fpgadrawline task.
    //
    xTaskCreate(
                        DemoFPGADRAWLINETask,                       //任务函数
                       (const portCHAR *)"FPGADRAWLINE", //任务名称 
                        FPGADRAWLINE_STK_SIZE,              // 任务堆栈大小 
                        NULL,       
                        FPGADRAWLINE_TASK_PRIO,
                        NULL
    );
    
    
}

