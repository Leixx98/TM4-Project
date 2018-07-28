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
#include "functions/dac5618.h"
#include "functions/uartprint.h"

//*****************************************************************************

#define DAC5618_TASK_PRIO          2        //任务优先级
#define DAC5618_STK_SIZE           128       //任务堆栈大小

//*****************************************************************************

/*******************************************
Function name              :TLV5618_SDAPort_In
Function use               :Set SDA port as input.
Function parameter         :None
Function return            :None
********************************************/
void TLV5618_SDAPort_Out(void)
{
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOK);
    GPIOPinTypeGPIOOutput(GPIO_PORTK_BASE, GPIO_PIN_0);
    GPIOPadConfigSet(GPIO_PORTK_BASE, GPIO_PIN_0,
                         GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPU);	
}

/*******************************************
函数名称：TLV5618_Init
功    能：初始化SPI总线
参    数：无
返回值  ：无
********************************************/
void TLV5618_Init(void)        //DAC5618  SPI初始化
{

    TLV5618_CS_HIGH;            //拉高片选
    delay_us(3);
    TLV5618_SCL_SET;         //拉高时钟线
    delay_us(3);      //短延时
   TLV5618_SDA_CLR;           //置低数据线
    delay_us(3);
}
/*******************************************
函数名称：DAC_Conver
功    能：对一个16位二进制命令作出响应
参    数：temp-16位二进制命令
返回值  ：无
********************************************/

void DAC_Conver(uint temp)
{
        unsigned char i;
        TLV5618_SDAPort_Out(); //引脚设置为输出
        TLV5618_CS_LOW;       //拉低片选，开启数据传输
        TLV5618_SCL_SET;
        for(i=0;i<16;i++)
        {
        	if((temp & 0x8000) ==0) 
                {TLV5618_SDA_CLR;} //数据最低位为0则置低数据线，否则拉高
            else
                {TLV5618_SDA_SET;}
            TLV5618_SCL_CLR;
            delay_us(1);
            temp<<=1;
            TLV5618_SCL_SET;        //上升沿读取数据
        }
        TLV5618_CS_HIGH;   //拉高片选，关闭数据传输
}

/*******************************************
函数名称： VoltToData
功    能：将模拟电压值转换为16位二进制数
参    数：out_volt-模拟电压值
返回值  ：16位二进制数
补充：//(uint)(out_volt*4095)/(2*2.048) 先将电压值转化为0~4095的整数
        //0x0fff&  转化成16位二进制数，后12位为数据位
********************************************/
uint VoltToData(float out_volt) //说明ref为参考源out_volt为输出模拟电压值
{
   int temp2;
   temp2=(unsigned int)((out_volt*4096)/(4*2.048));
   return(temp2&0x0fff);
}//将电压值转换为对应的12位数字量函数

/*******************************************
函数名称： Write_A
功    能：像A通道写数据
参    数：out_volt-模拟电压值
返回值  ：无
补充：//(uint)(out_volt*4095)/(2*2.048) 先将电压值转化为0~4095的整数
        //0x0fff&  转化成16位二进制数，后12位为数据位
        //0xc000|  前面加上4位为1100（命令位）：快速模式，正常功耗，写数据到A通道
********************************************/
void Write_A(float out_volt)
{
   int Data_A;
   Data_A=0xc000|VoltToData(out_volt);//A通道
   DAC_Conver(Data_A);
}

/*******************************************
函数名称： Write_B
功    能：像A通道写数据
参    数：out_volt-模拟电压值
返回值  ：无
补充：//(uint)(out_volt*4095)/(2*2.048) 先将电压值转化为0~4095的整数
        //0x0fff&  转化成16位二进制数，后12位为数据位
        //0xc000|  前面加上4位为1100（命令位）：快速模式，正常功耗，写数据到A通道
********************************************/
void Write_B(float out_volt)
{
   int Data_A;
   Data_A=0x4000|VoltToData(out_volt);//B通道
   DAC_Conver(Data_A);
}

/*******************************************
Function name              :DAC5618_DemoTask
Function use               :Write the voltage to the dac5618
Function parameter         :None
Function return            :None
********************************************/
void DAC5618_DemoTask(void *pvParameters)
{
  TLV5618_Init();	
	while(1)
	{
			if(Mode==DAC5618_VoltageSet)              //当设置DAC5618通道电压时
			{
				if(DAC5618_Channel==1)                  //如果为通道1，则写入数据
					Write_A(DAC5618Value);
				if(DAC5618_Channel==2)                  //如果为通道2，则写入数据
					Write_B(DAC5618Value);
				Mode=0xff;                              //清除模式
			}
			vTaskDelay(100);
	}
}
/*******************************************
Function name              :DAC5618TaskInit
Function use               :Inie the ADS1256 task
Function parameter         :None
Function return            :None
********************************************/
void
DAC5618TaskInit(void)
{

    //
    // Create the LED task.
    //
    xTaskCreate(
                        DAC5618_DemoTask,    //任务函数
                       (const portCHAR *)"DAC5618", //任务名称 
                        DAC5618_STK_SIZE,              // 任务堆栈大小 
                        NULL,       
                        DAC5618_TASK_PRIO,
                        NULL
    );
        
}

