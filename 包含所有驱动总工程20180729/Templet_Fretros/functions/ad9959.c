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
#include "utils/buttons.h"

#include "functions/delay.h"
#include "functions/ad9959.h"
#include "functions/uartprint.h"

//*****************************************************************************

#define AD9959_TASK_PRIO           3        //任务优先级
#define AD9959_STK_SIZE           256       //任务堆栈大小

//*****************************************************************************

u8 CSR_DATA0[1] = {0x10};     // 开 CH0
u8 CSR_DATA1[1] = {0x20};      // 开 CH1
u8 CSR_DATA2[1] = {0x40};      // 开 CH2
u8 CSR_DATA3[1] = {0x80};      // 开 CH3		
u8 CSR_DATA[1]   = {0xc0};      //开CH2,CH3
																	
u8 FR1_DATA[3] = {0xD0,0x00,0x00};
u8 FR2_DATA[2] = {0x08,0x00};//default Value = 0x0000
u8 CFR_DATA[3] = {0x00,0x03,0x00};//default Value = 0x000302	   
																	
u8 CPOW0_DATA[2] = {0x00,0x00};//default Value = 0x0000   @ = POW/2^14*360
																	


u8 LSRR_DATA[2] = {0x00,0x00};//default Value = 0x----
																	
u8 RDW_DATA[4] = {0x00,0x00,0x00,0x00};//default Value = 0x--------
																	
u8 FDW_DATA[4] = {0x00,0x00,0x00,0x00};//default Value = 0x--------

//AD9959初始化
void Init_AD9959(void)  
{ 

// 	u8 FR1_DATA[3] = {0xD0,0x00,0x00};//20倍频 Charge pump control = 75uA FR1<23> -- VCO gain control =0时 system clock below 160 MHz;
    
    //时钟初始化
   SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOH);

	//设置引脚为输出
	GPIODirModeSet(GPIO_PORTA_BASE, GPIO_PIN_2|GPIO_PIN_3|GPIO_PIN_4|GPIO_PIN_5,GPIO_DIR_MODE_OUT);
    GPIODirModeSet(GPIO_PORTB_BASE, GPIO_PIN_4|GPIO_PIN_5,GPIO_DIR_MODE_OUT);
    GPIODirModeSet(GPIO_PORTE_BASE, GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2,GPIO_DIR_MODE_OUT);
    GPIODirModeSet(GPIO_PORTH_BASE, GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3,GPIO_DIR_MODE_OUT);
	//设置引脚为2mA弱上拉
	GPIOPadConfigSet(GPIO_PORTA_BASE, GPIO_PIN_2|GPIO_PIN_3|GPIO_PIN_4|GPIO_PIN_5 ,
                         GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPU);	
    GPIOPadConfigSet(GPIO_PORTB_BASE, GPIO_PIN_4|GPIO_PIN_5 ,
                         GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPU);	
    GPIOPadConfigSet(GPIO_PORTE_BASE, GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2 ,
                         GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPU);	    
    GPIOPadConfigSet(GPIO_PORTH_BASE, GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3 ,
                         GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPU);	        
		
	Intserve();  //IO口初始化
    IntReset();  //AD9959复位  

  WriteData_AD9959(FR1_ADD,3,FR1_DATA,1);//写功能寄存器1
//  WriteData_AD9959(FR2_ADD,2,FR2_DATA,1);
//  WriteData_AD9959(CFR_ADD,3,CFR_DATA,1);
//  WriteData_AD9959(CPOW0_ADD,2,CPOW0_DATA,0);
//  WriteData_AD9959(ACR_ADD,3,ACR_DATA,0);
//  WriteData_AD9959(LSRR_ADD,2,LSRR_DATA,0);
//  WriteData_AD9959(RDW_ADD,2,RDW_DATA,0);
//  WriteData_AD9959(FDW_ADD,4,FDW_DATA,1);
   //写入初始频率
//  Write_Quadrature(10000);
	
////	Write_frequence(3,50);
	Write_frequence(1,100000);
////	Write_frequence(1,50);
////	Write_frequence(2,50);

	
//    Write_Phase(2,83);
} 
//延时
void delay1 (u32 length)
{
	length = length*12;
   while(length--);
}
//IO口初始化
void Intserve(void)		   
{   
    AD9959_PWR_CLR;
    CS_SET;
    SCLK_CLR;
    UPDATE_CLR;
    PS0_CLR;
    PS1_CLR;
    PS2_CLR;
    PS3_CLR;
    SDIO0_CLR;
    SDIO1_CLR;
    SDIO2_CLR;
    SDIO3_CLR;
}
//AD9959复位
void IntReset(void)	  
{
  Reset_CLR;
	delay1(1);
	Reset_SET;
	delay1(30);
	Reset_CLR;
}
 //AD9959更新数据
void IO_Update(void)  
{
	UPDATE_CLR;
	delay1(4);
	UPDATE_SET;
	delay1(4);
	UPDATE_CLR;
}
/*--------------------------------------------
函数功能：控制器通过SPI向AD9959写数据
RegisterAddress: 寄存器地址
NumberofRegisters: 所含字节数
*RegisterData: 数据起始地址
temp: 是否更新IO寄存器
----------------------------------------------*/
void WriteData_AD9959(u8 RegisterAddress, u8 NumberofRegisters, u8 *RegisterData,u8 temp)
{
	u8	ControlValue = 0;
	u8	ValueToWrite = 0;
	u8	RegisterIndex = 0;
	u8	i = 0;

	ControlValue = RegisterAddress;
//写入地址
	SCLK_CLR;
	CS_CLR;	 
	for(i=0; i<8; i++)
	{
		SCLK_CLR;
		if(0x80 == (ControlValue & 0x80))
		SDIO0_SET;	  
		else
		SDIO0_CLR;	  
		SCLK_SET;
		ControlValue <<= 1;
         delay1(2);
	}
	SCLK_CLR;
//写入数据
	for (RegisterIndex=0; RegisterIndex<NumberofRegisters; RegisterIndex++)
	{
		ValueToWrite = RegisterData[RegisterIndex];
		for (i=0; i<8; i++)
		{
			SCLK_CLR;
			if(0x80 == (ValueToWrite & 0x80))
			SDIO0_SET;	  
			else
			SDIO0_CLR;	  
			SCLK_SET;
			ValueToWrite <<= 1;
             delay1(2);
		}
		SCLK_CLR;		
	}	
	if(temp==1)
        IO_Update();	
    CS_SET;
} 
/*---------------------------------------
函数功能：设置通道输出频率
Channel:  输出通道
Freq:     输出频率
---------------------------------------*/
void Write_frequence(u8 Channel,u32 Freq)
{	 
		u8 CFTW0_DATA[4] ={0x00,0x00,0x00,0x00};	//中间变量
	  u32 Temp;            
	  Temp=(u32)Freq*8.589934592;	   //将输入频率因子分为四个字节  4.294967296=(2^32)/500000000
	  CFTW0_DATA[3]=(u8)Temp;
	  CFTW0_DATA[2]=(u8)(Temp>>8);
	  CFTW0_DATA[1]=(u8)(Temp>>16);
	  CFTW0_DATA[0]=(u8)(Temp>>24);
	  if(Channel==0)	  
	  {
			WriteData_AD9959(CSR_ADD,1,CSR_DATA0,0);//控制寄存器写入CH0通道
      WriteData_AD9959(CFTW0_ADD,4,CFTW0_DATA,1);//CTW0 address 0x04.输出CH0设定频率
		}
	  else if(Channel==1)	
	  {
			WriteData_AD9959(CSR_ADD,1,CSR_DATA1,0);//控制寄存器写入CH1通道
      WriteData_AD9959(CFTW0_ADD,4,CFTW0_DATA,1);//CTW0 address 0x04.输出CH1设定频率	
	  }
	  else if(Channel==2)	
	  {
			WriteData_AD9959(CSR_ADD,1,CSR_DATA2,0);//控制寄存器写入CH2通道
      WriteData_AD9959(CFTW0_ADD,4,CFTW0_DATA,1);//CTW0 address 0x04.输出CH2设定频率	
	  }
	  else if(Channel==3)	
	  {
			WriteData_AD9959(CSR_ADD,1,CSR_DATA3,0);//控制寄存器写入CH3通道
      WriteData_AD9959(CFTW0_ADD,4,CFTW0_DATA,1);//CTW0 address 0x04.输出CH3设定频率	
	  }																																																																										 
	
} 
/*---------------------------------------
函数功能：设置通道输出幅度
Channel:  输出通道
Ampli:    输出幅度
---------------------------------------*/
void Write_Amplitude(u8 Channel, u16 Ampli)
{ 
	u16 A_temp;//=0x23ff;
	u8 ACR_DATA[3] = {0x00,0x00,0x00};//default Value = 0x--0000 Rest = 18.91/Iout 
	
  A_temp=Ampli|0x1000;
	ACR_DATA[2] = (u8)A_temp;  //低位数据
  ACR_DATA[1] = (u8)(A_temp>>8); //高位数据
  if(Channel==0)
  {
		WriteData_AD9959(CSR_ADD,1,CSR_DATA0,0); 
    WriteData_AD9959(ACR_ADD,3,ACR_DATA,1); 
	}
  else if(Channel==1)
  {
		WriteData_AD9959(CSR_ADD,1,CSR_DATA1,0); 
    WriteData_AD9959(ACR_ADD,3,ACR_DATA,1);
	}
  else if(Channel==2)
  {
	  WriteData_AD9959(CSR_ADD,1,CSR_DATA2,0); 
    WriteData_AD9959(ACR_ADD,3,ACR_DATA,1); 
	}
  else if(Channel==3)
  {
		WriteData_AD9959(CSR_ADD,1,CSR_DATA3,0); 
    WriteData_AD9959(ACR_ADD,3,ACR_DATA,1); 
	}
}
/*---------------------------------------
函数功能：设置通道输出相位
Channel:  输出通道
Phase:    输出相位,范围：0~16383(对应角度：0°~360°)
---------------------------------------*/
void Write_Phase(u8 Channel,u16 Phase)
{
	u16 P_temp=0;
  P_temp=(u16)((Phase*16384)/360);
	CPOW0_DATA[1]=(u8)P_temp;
	CPOW0_DATA[0]=(u8)(P_temp>>8);
	if(Channel==0)
  {
		WriteData_AD9959(CSR_ADD,1,CSR_DATA0,0); 
    WriteData_AD9959(CPOW0_ADD,2,CPOW0_DATA,1);
  }
  else if(Channel==1)
  {
		WriteData_AD9959(CSR_ADD,1,CSR_DATA1,0); 
    WriteData_AD9959(CPOW0_ADD,2,CPOW0_DATA,1);
  }
  else if(Channel==2)
  {
		WriteData_AD9959(CSR_ADD,1,CSR_DATA2,0); 
    WriteData_AD9959(CPOW0_ADD,2,CPOW0_DATA,1);
  }
  else if(Channel==3)
  {
		WriteData_AD9959(CSR_ADD,1,CSR_DATA3,0); 
    WriteData_AD9959(CPOW0_ADD,2,CPOW0_DATA,1);
  }
}	 


/*---------------------------------------
函数功能：设置双通道输出频率
Freq:     输出频率
Phase:    输出频率：0~100M
---------------------------------------*/
void Write_Quadrature(uint32_t Freq)
{		
      u8 ACR_DATA[3] = {0x00,0x00,0x00};//default Value = 0x--0000 Rest = 18.91/Iout 
      u8 CFTW0_DATA[4] ={0x00,0x00,0x00,0x00};	//中间变量
	  u32 Temp;      
      u16 A_temp;//=0x23ff;
      u16 P_temp=0;
      P_temp=(u16)((90*16384)/360);
	 CPOW0_DATA[1]=(u8)P_temp;
	 CPOW0_DATA[0]=(u8)(P_temp>>8);
      
      A_temp=867|0x1000;      
	  Temp=(u32)Freq*8.589934592;	   //将输入频率因子分为四个字节  4.294967296=(2^32)/500000000
	  
      CFTW0_DATA[3]=(u8)Temp;
	  CFTW0_DATA[2]=(u8)(Temp>>8);
	  CFTW0_DATA[1]=(u8)(Temp>>16);
	  CFTW0_DATA[0]=(u8)(Temp>>24);
      
      ACR_DATA[2] = (u8)A_temp;  //低位数据
      ACR_DATA[1] = (u8)(A_temp>>8); //高位数据
      
      WriteData_AD9959(CSR_ADD,1,CSR_DATA,0);   //选中通道2，3
      WriteData_AD9959(ACR_ADD,3,ACR_DATA,1);   //写入幅度
      WriteData_AD9959(CFTW0_ADD,4,CFTW0_DATA,1);//CTW0 address 0x04.输出CH2,3设定频率	
      
       WriteData_AD9959(CSR_ADD,1,CSR_DATA3,0);   //选中通道3
       WriteData_AD9959(CPOW0_ADD,2,CPOW0_DATA,1); //写入相位值
      
}

/*******************************************
Function name              :AD9959_DemoTask
Function use               :Run the AD9959 to set Frequency of channel 2,3
Function parameter         :None
Function return            :None
********************************************/
void AD9959_DemoTask(void *pvParameters){
	uint8_t Sweep_Freq=0,Sweep_Ampl=0;                         //对应扫频和扫幅
	uint32_t start=0,final=0,step=0,time=0;                        //对应起始，终止，步进，时长
	uint32_t frequency=10000,amplitude=500;                //设置初始频率，幅度
	Init_AD9959();
	Write_frequence(0,frequency);
	Write_frequence(0,amplitude);
	while(1)
	{
		if(Mode==AD9959_Point_Freq)                          //检测到模式为点频模式时
		{
			UARTprintf("%d\n",receivevalue);          					//输出该整数
			Sweep_Ampl=0;Sweep_Freq=0;                          //关闭扫频和扫幅度
			Write_frequence(AD9959_Channel,receivevalue);       //写入得到的频率值
			receivevalue=0;                                     //清0
			Mode=0xff;                                          //清除该模式
		}
		else if(Mode==AD9959_Point_Ampl)                      //检测到模式为点频模式时
		{
			UARTprintf("%d\n",receivevalue);          //输出该整数
			Sweep_Ampl=0;Sweep_Freq=0;													//关闭扫频和扫幅度
			Write_Amplitude(AD9959_Channel,receivevalue);				//写入得到的幅度值
			receivevalue=0; 																		//清0
			Mode=0xff; 																					//清除该模式
		}
		else if(Mode==AD9959_Sweep_Freq)                     //检测到模式为扫频模式时
		{
			UARTprintf("%d\n",receivebuffer[3]);
			UARTprintf("%d\n",receivebuffer[2]);
			UARTprintf("%d\n",receivebuffer[1]);
			UARTprintf("%d\n",receivebuffer[0]);
			Sweep_Freq=1;                												//打开扫频模式
			start=receivebuffer[3];receivebuffer[3]=0;          //得到四个参数，并将其清零
			final=receivebuffer[2];receivebuffer[2]=0;
			step=receivebuffer[1];receivebuffer[1]=0;
			time=receivebuffer[0];receivebuffer[0]=0;
			
			frequency=start;                                    //写入初始频率
			
			Mode=0xff;                                          //清除该模式，下同
		}
		else if(Mode==AD9959_Sweep_Ampl)
		{
			UARTprintf("%d\n",receivebuffer[3]);
			UARTprintf("%d\n",receivebuffer[2]);
			UARTprintf("%d\n",receivebuffer[1]);
			UARTprintf("%d\n",receivebuffer[0]);			
			Sweep_Ampl=1;
			start=receivebuffer[3];receivebuffer[3]=0;
			final=receivebuffer[2];receivebuffer[2]=0;
			step=receivebuffer[1];receivebuffer[1]=0;
			time=receivebuffer[0];receivebuffer[0]=0;
			
			amplitude=start;
			Mode=0xff;
		}		
		
		if(Sweep_Freq)
		{
			UARTprintf("%d\n",frequency);
			Write_frequence(AD9959_Channel,frequency);
			frequency+=step;
			if(frequency==final)
				frequency=start;
			vTaskDelay(time);
		}
		
		if(Sweep_Ampl)
		{
			Write_Amplitude(AD9959_Channel,amplitude);
			amplitude+=step;
			if(amplitude==final)
				amplitude=start;
			vTaskDelay(time);
		}
		
		vTaskDelay(10);
		
	}
}

/*******************************************
Function name              :AD9959TaskInit
Function use               :Get data from ADS1256 in single channel and continuously mode.
Function parameter         :Channel  -- A single channel
Function return            :uint32_t data -- The data from ADS1256
********************************************/
void
AD9959TaskInit(void)
{

    //
    // Create the LED task.
    //
    xTaskCreate(
                        AD9959_DemoTask,    //任务函数
                       (const portCHAR *)"AD9959", //任务名称 
                        AD9959_STK_SIZE,              // 任务堆栈大小 
                        NULL,       
                        AD9959_TASK_PRIO,
                        NULL
    );
        
}



