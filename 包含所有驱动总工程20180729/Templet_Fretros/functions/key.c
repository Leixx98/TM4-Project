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
#include "functions/key.h"

#define KEY_TASK_PRIO          2        //任务优先级
#define KEY_STK_SIZE           128       //任务堆栈大小 
extern TaskHandle_t UARTPRINTTaskHandler;            //任务句柄
//*****************************************************************************
//
// 按键任务
//
//*****************************************************************************
void
DemoKEYTask(void *pvParameters)
{
    uint8_t ui8Buttons;
    uint8_t ui8ButtonsChanged;
		static uint8_t keychanged; //按键标志位，当按键按下时标为1 
    static uint8_t keyvalue;
    while(1)
    {
            //检测按键状态
            ui8Buttons = ButtonsPoll(&ui8ButtonsChanged, 0);
            if(BUTTON_PRESSED(USR_SW1, ui8Buttons, ui8ButtonsChanged))
						{
                keyvalue=keyvalue>=6?1:keyvalue+1;
								keychanged = 1;
						}
            else if(BUTTON_PRESSED(USR_SW2, ui8Buttons, ui8ButtonsChanged)) 
						{
                keyvalue=keyvalue<=1?6:keyvalue-1;
								keychanged = 1;
						}
            if(keychanged)
						{
							UARTprintf("当前按键值：%d\n",keyvalue);
							keychanged = 0;
							switch(keyvalue)
							{
									case 1:
											taskENTER_CRITICAL();   /* 进入临界区 */                   
											UARTprintf("关闭中断\n");
											taskEXIT_CRITICAL();  	/* 退出临界区 */
											portDISABLE_INTERRUPTS();      /* 关闭中断*/
											break;      
									case 2:
											taskENTER_CRITICAL();   /* 进入临界区 */	
											UARTprintf("恢复中断\n");
											taskEXIT_CRITICAL();  	/* 退出临界区 */ 
											portENABLE_INTERRUPTS();     /*恢复中断*/
											break;
									case 3:
											taskENTER_CRITICAL();   /* 进入临界区 */	
											UARTprintf("关闭TIM\n");
											IntDisable(INT_TIMER0A);	
											IntDisable(INT_TIMER1A);
											taskEXIT_CRITICAL();  	/* 退出临界区 */ 
											break;
									case 4:
											taskENTER_CRITICAL();   /* 进入临界区 */	
											UARTprintf("恢复TIM\n");
											IntEnable(INT_TIMER0A);	
											IntEnable(INT_TIMER1A);
											taskEXIT_CRITICAL();  	/* 退出临界区 */ 
											break;
									case 5:
											taskENTER_CRITICAL();   /* 进入临界区 */	
											UARTprintf("挂起UART\n");
											vTaskSuspend(UARTPRINTTaskHandler);
											taskEXIT_CRITICAL();  	/* 退出临界区 */ 
											break;		
									case 6:
											taskENTER_CRITICAL();   /* 进入临界区 */	
											UARTprintf("恢复UART\n");
											vTaskResume(UARTPRINTTaskHandler);
											taskEXIT_CRITICAL();  	/* 退出临界区 */ 
											break;												
							}
					}
    }
}
//*****************************************************************************
//
// 创建按键任务
//
//*****************************************************************************
void
KEYTaskInit(void)
{
		//按键初始化
		ButtonsInit();
    //
    // Create the LED task.
    //
    xTaskCreate(
                        DemoKEYTask,                       //任务函数
                       (const portCHAR *)"KEY", //任务名称 
                        KEY_STK_SIZE,              // 任务堆栈大小 
                        NULL,       
                        KEY_TASK_PRIO,
                        NULL
    );
    
    
}


	



