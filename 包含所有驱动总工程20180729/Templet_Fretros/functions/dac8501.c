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
#include "driverlib/uart.h"

#include "FreeRTOSConfig.h"
#include "FreeRTOS.h"
#include "queue.h"
#include "task.h"

#include "utils/uartstdio.h"

#include "functions/delay.h"
#include "functions/dac8501.h"
#include "functions/uartprint.h"

//*****************************************************************************

#define DAC8501_TASK_PRIO           2        //任务优先级
#define DAC8501_STK_SIZE           128       //任务堆栈大小

//*****************************************************************************

/*******************************************
函数名称：Dac8501__IO_Init
功    能：初始化SPI总线引脚
参    数：无
返回值  ：无
********************************************/
void DAC8501_IO_Init()       
{
  //Init GPIOB,P
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOP);
  // Check if the peripheral access is enabled.
	while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOB))
	{
	}	
	while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOP))
	{
	}		
	GPIOPinTypeGPIOOutput(GPIO_PORTB_BASE,GPIO_PIN_4|GPIO_PIN_5);
	GPIOPinTypeGPIOOutput(GPIO_PORTP_BASE,GPIO_PIN_0|GPIO_PIN_1);
	//设置驱动强度
	GPIOPadConfigSet(GPIO_PORTB_BASE,GPIO_PIN_4|GPIO_PIN_5,
                         GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPU);
	GPIOPadConfigSet(GPIO_PORTP_BASE,GPIO_PIN_0|GPIO_PIN_1,
                         GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPU);
}

/*******************************************
函数名称：DAC_Transform
功    能：对一个24位二进制命令作出响应
参    数：temp-24位二进制命令
					channel-通道选择：1，2
返回值  ：无
********************************************/
void DAC8501_Transform(uint32_t temp,uint8_t channel)
{
        uint8_t i;
        DAC8501_CLK_CLR;
        delay_us(1);
				if(channel==1)
					{DAC8501_CS1_CLR;}     //拉低片选，开启数据传输
				else if(channel==2)
					{DAC8501_CS2_CLR;}
        delay_us(1);   
        for(i=0;i<24;i++)
        {
        	if(temp & 0x800000) {DAC8501_SDA_SET;} //数据最低位为0则置低数据线，否则拉高
            else  {DAC8501_SDA_CLR;}
            DAC8501_CLK_SET;
            delay_us(1);
            temp<<=1;
            delay_us(1);
            DAC8501_CLK_CLR;        //下降沿读取数据
        }
				if(channel==1)
					{DAC8501_CS1_SET;}     //拉低片选，开启数据传输
				else if(channel==2)
					{DAC8501_CS2_SET;}
        delay_us(1);
        DAC8501_CLK_SET; 

}

/*******************************************
函数名称： VoltToData
功    能：将模拟电压值转换为16位二进制数
参    数：out_volt-模拟电压值
返回值  ：16位二进制数
补充：//(uint)(out_volt*4095)/(2*2.048) 先将电压值转化为0~4095的整数
        //0x0fff&  转化成16位二进制数，后12位为数据位
********************************************/
uint32_t DAC8501_VoltToData(float out_volt) //说明ref为参考源out_volt为输出模拟电压值
{ 
   uint32_t temp;
   temp=(uint32_t)(out_volt*65536/(2*2.5));
	 //设置为正常模式
   return(temp&0x00ffff);
}//将电压值转换为对应的24位数字量函数


/*******************************************
函数名称： Write_Data_Channel1
功    能: 向通道1写入数据
参    数：out_volt-模拟电压值
返回值  ：无
补充：//(uint)(out_volt*4095)/(2*2.048) 先将电压值转化为0~4095的整数
        //0x0fff&  转化成16位二进制数，后12位为数据位
        //0xc000|  前面加上4位为1100（命令位）：快速模式，正常功耗，写数据到A通道
********************************************/
void Write_Data_Channel1(float out_volt)
{
   uint32_t Data;
   Data=DAC8501_VoltToData(out_volt);//A通道
   DAC8501_Transform(Data,1);
}

/*******************************************
函数名称： Write_Data_Channel2
功    能: 向通道2写入数据
参    数：out_volt-模拟电压值
返回值  ：无
补充：//(uint)(out_volt*4095)/(2*2.048) 先将电压值转化为0~4095的整数
        //0x0fff&  转化成16位二进制数，后12位为数据位
        //0xc000|  前面加上4位为1100（命令位）：快速模式，正常功耗，写数据到A通道
********************************************/
void Write_Data_Channel2(float out_volt)
{
   uint32_t Data;
   Data=DAC8501_VoltToData(out_volt);//A通道
   DAC8501_Transform(Data,2);
}


/*******************************************
Function name              :DAC8501_DemoTask
Function use               :Write the voltage to the dac5618
Function parameter         :None
Function return            :None
********************************************/
void DAC8501_DemoTask(void *pvParameters)
{
  DAC8501_IO_Init();	
	while(1)
	{
		if(Mode==DAC8501_VoltageSet)
		{
			if(DAC8501_Channel==1)
				Write_Data_Channel1(DAC8501Value);
			else if(DAC8501_Channel==2)
				Write_Data_Channel2(DAC8501Value);
			Mode=0xff;
		}
		vTaskDelay(100);
	}
}
/*******************************************
Function name              :DAC8501TaskInit
Function use               :Inie the ADS1256 task
Function parameter         :None
Function return            :None
********************************************/
void
DAC8501TaskInit(void)
{

    //
    // Create the LED task.
    //
    xTaskCreate(
                        DAC8501_DemoTask,    //任务函数
                       (const portCHAR *)"DAC8501", //任务名称 
                        DAC8501_STK_SIZE,              // 任务堆栈大小 
                        NULL,       
                        DAC8501_TASK_PRIO,
                        NULL
    );
        
}


