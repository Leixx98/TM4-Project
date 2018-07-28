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

#include "functions/key.h"
#include "functions/delay.h"
#include "functions/ads1256.h"
#include "functions/uartprint.h"

//ADS1256通道状态变量
//*****************************************************************************

uint8_t ADS1256_Channel0_On=0;
uint8_t ADS1256_Channel1_On=0;
uint8_t ADS1256_Channel2_On=0;
uint8_t ADS1256_Channel3_On=0;
uint8_t ADS1256_Channel4_On=0;
uint8_t ADS1256_Channel5_On=0;
uint8_t ADS1256_Channel6_On=0;
uint8_t ADS1256_Channel7_On=0;

//*****************************************************************************

//*****************************************************************************

#define ADS1256_TASK_PRIO           3        //任务优先级
#define ADS1256_STK_SIZE           256       //任务堆栈大小

//*****************************************************************************

uint8_t ADS1256_ReadBuffer[10];
//*****************************************************************************
//
//?????????
//
//*****************************************************************************


/*?????????
	PK6:      SCLK
	PL4:      DIN
	PB2:      DOUT
	PB3:      DRDY
	PP2:      CS
	PP3:      RESET
*/
/*******************************************
Function name              :ADS1256_IO_Init
Function use               :Init the IO ports of ADS1256
Function parameter         :None
Function return            :None
********************************************/
void ADS1256_IO_Init(void)
{ 
  //???GPIOB,K,L,P
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOK);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOL);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOP);
  // Check if the peripheral access is enabled.
	while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOB))
	{
	}	
	while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOK))
	{
	}		
	while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOL))
	{
	}	
	while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOP))
	{
	}		
	//????PL4,PK6,PP2,PP3????,PB2,PB3?????
	GPIOPinTypeGPIOInput(GPIO_PORTB_BASE,GPIO_PIN_2);
	GPIOPinTypeGPIOInput(GPIO_PORTB_BASE,GPIO_PIN_3);
	GPIOPinTypeGPIOOutput(GPIO_PORTK_BASE,GPIO_PIN_6);
	GPIOPinTypeGPIOOutput(GPIO_PORTL_BASE,GPIO_PIN_4);
	GPIOPinTypeGPIOOutput(GPIO_PORTP_BASE,GPIO_PIN_2|GPIO_PIN_3);
	//????PB2,PB3,PK6,PL4,PP2,PP3?2mA????
	GPIOPadConfigSet(GPIO_PORTB_BASE,GPIO_PIN_2|GPIO_PIN_3,
                         GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPU);
	GPIOPadConfigSet(GPIO_PORTK_BASE,GPIO_PIN_6,
                         GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPU);	
	GPIOPadConfigSet(GPIO_PORTL_BASE,GPIO_PIN_4,
                         GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPU);	
	GPIOPadConfigSet(GPIO_PORTP_BASE,GPIO_PIN_2|GPIO_PIN_3,
                         GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPU);	
	
}

/*******************************************
Function name              :ADS1256_Communicate
Function use               :Communicate with the ADS1256 by SPI 
Function parameter         :transmitdata  --  the transmitdata to be written to ADS1256
Function return            :None
********************************************/
uint8_t ADS1256_Communicate(uint8_t transmitdata)
{
	uint8_t i,receivedata=0;
	for(i=0;i<8;i++)
	{
		ADS1256_CLK_SET;
		delay_us(10);
		receivedata<<=1;
		if(transmitdata&0x80)
			ADS1256_DIN_SET;
		else
			ADS1256_DIN_CLR;
		transmitdata<<=1;
		ADS1256_CLK_CLR;
		delay_us(10);
		if(ADS1256_DOUT_IN)
			receivedata++;
	}
	return receivedata;
	
}

/*******************************************
Function name              :ADS1256_Write_REG
Function use               :Write command to ADS1256
Function parameter         : address--The address of the registers
														 data--The register data to be written
Function return            :none
********************************************/
void ADS1256_Write_REG(unsigned char address,unsigned char data)
{
	ADS1256_CS_CLR;
	delay_us(3);
	while(ADS1256_DRDY_IN);
	ADS1256_Communicate(address);
	delay_us(1);
	ADS1256_Communicate(0x00);
	delay_us(1);
	ADS1256_Communicate(data);
	delay_us(1);
	ADS1256_CS_SET;

}

/*******************************************
Function name              :ADS1256_Read_REG
Function use               :Read 1 bytes from ADS1256 regiters
Function parameter         : address--The address of the registers
Function return            :data --The register value;
********************************************/
uint8_t ADS1256_Read_REG(unsigned char address)
{
	uint8_t data;
	ADS1256_CS_CLR;
	delay_us(3);
	while(ADS1256_DRDY_IN);
	ADS1256_Communicate(address);
	delay_us(1);
	ADS1256_Communicate(0x00);
	delay_us(1);
	data=ADS1256_Communicate(0);
	delay_us(1);
	ADS1256_CS_SET;
	return data;

}

/*******************************************
Function name              :ADS1256_ReadData
Function use               :Read Data(24bit) from ADS1256
Function parameter         :None
Function return            :data -- The 24bit data you read from ADS1256
********************************************/
unsigned long ADS1256_ReadData(void)
{
	uint8_t i=0;
	unsigned long data=0;
	unsigned long receive=0;
	
	ADS1256_CS_CLR;
	while(ADS1256_DRDY_IN);
	ADS1256_Communicate(COMMAND_SYNC);
	ADS1256_Communicate(COMMAND_WAKEUP);
	ADS1256_Communicate(COMMAND_RDATA);
	
	for(i=0;i<3;i++)
	{
		data<<=8;
		receive = ADS1256_Communicate(0);
		UARTprintf("%c",receive);
		data = data|receive;
	}
	ADS1256_CS_SET;
	
	return data;
}


/*******************************************
Function name              :ADS1256_Init
Function use               :Init the ADS1115
Function parameter         :mode  --  choose a mode in oneshot mode or continuously mode 
Function return            :None
********************************************/
void ADS1256_Init(uint8_t mode)
{
	ADS1256_IO_Init();
	ADS1256_CS_CLR;
	ADS1256_RESET_SET;
	ADS1256_Write_REG(WRITE_REG_STATUS,STATUS_INIT);
	ADS1256_Write_REG(WRITE_REG_MUX,MUX_INIT);
	ADS1256_Write_REG(WRITE_REG_ADCON,ADS1256_GAIN_1);
	ADS1256_Write_REG(WRITE_REG_DRATE,DRATE_INIT);
	ADS1256_Write_REG(WRITE_REG_IO,IO_INIT);
	ADS1256_Write_REG(WRITE_REG_OFC0,OFC0_INIT);
	ADS1256_Write_REG(WRITE_REG_OFC1,OFC1_INIT);
	ADS1256_Write_REG(WRITE_REG_OFC2,OFC2_INIT);
	ADS1256_Write_REG(WRITE_REG_FSC0,FSC0_INIT);
	ADS1256_Write_REG(WRITE_REG_FSC1,FSC1_INIT);
	ADS1256_Write_REG(WRITE_REG_FSC2,FSC2_INIT);
	ADS1256_CS_SET;
	
	//Continuously sample mode
	if(mode)
	{
		ADS1256_CS_CLR;
		while(ADS1256_DRDY_IN);
		ADS1256_Communicate(COMMAND_SELFGCAL);
		while(ADS1256_DRDY_IN);
		ADS1256_Communicate(COMMAND_SELFOCAL);
		while(ADS1256_DRDY_IN);
		ADS1256_Communicate(COMMAND_SELFCAL);		
		while(ADS1256_DRDY_IN);
		ADS1256_Communicate(COMMAND_WAKEUP);			
		while(ADS1256_DRDY_IN);
		ADS1256_Communicate(COMMAND_RDATAC);	
	}
}

/*******************************************
Function name              :ADS1256_Getdata_OneShot
Function use               :Get data from ADS1256 in single channel and oneshot mode.
Function parameter         :Channel  -- A single channel
Function return            :short D_ADS -- The data from ADS1256
该模式目前不可用
********************************************/
uint32_t ADS1256_Getdata_OneShot(uint8_t Channel)
{
		uint32_t finaldata;
    delay_us(3);
		ADS1256_Write_REG(WRITE_REG_MUX,Channel);
		while(ADS1256_DRDY_IN);
		ADS1256_Communicate(COMMAND_SELFGCAL);
		while(ADS1256_DRDY_IN);
		ADS1256_Communicate(COMMAND_SELFOCAL);
		while(ADS1256_DRDY_IN);
		ADS1256_Communicate(COMMAND_SELFCAL);
		finaldata = ADS1256_ReadData();
		while(ADS1256_DRDY_IN);
		ADS1256_Communicate(COMMAND_STANDBY);
  	return finaldata;
}

/*******************************************
Function name              :ADS1256_Getdata_Continuously
Function use               :Get data from ADS1256 in single channel and continuously mode.
Function parameter         :Channel  -- A single channel
Function return            :uint32_t data -- The data from ADS1256
********************************************/
uint32_t ADS1256_Getdata_Continuously(uint8_t Channel)
{
		uint8_t i=0;
		unsigned long data=0;
		unsigned long receive=0;

		while(ADS1256_DRDY_IN);
		for(i=0;i<3;i++)
		{
			data<<=8;
			receive = ADS1256_Communicate(0);
			data = data|receive;
		}
		ADS1256_CS_SET;
	
	return data;
}

/*******************************************
Function name              :ADS1256_Getdata_DemoTask
Function use               :Get data from ADS1256 in single channel and continuously mode.
Function parameter         :Channel  -- A single channel
Function return            :uint32_t data -- The data from ADS1256
********************************************/
void ADS1256_Getdata_DemoTask(void *pvParameters)
{
	static uint32_t adtemp=0;
	float advalue=0;
	uint8_t regstatus=0;
	uint8_t x='\"',f=0xff;
	ADS1256_Init(OneshotMode);
	while(1)
	{
		if(Mode==ADS1256_Channel_Open)               //检测到为ADS1256通道打开模式
		{
			if(ADS1256_Channel==0)                     //判断选择通道
				ADS1256_Channel0_On=1;
			else if(ADS1256_Channel==1)
				ADS1256_Channel1_On=1;
			else if(ADS1256_Channel==2)
				ADS1256_Channel2_On=1;
			else if(ADS1256_Channel==3)
				ADS1256_Channel3_On=1;
			else if(ADS1256_Channel==4)
				ADS1256_Channel4_On=1;	
			else if(ADS1256_Channel==5)
				ADS1256_Channel5_On=1;
			else if(ADS1256_Channel==6)
				ADS1256_Channel6_On=1;
			else if(ADS1256_Channel==7)
				ADS1256_Channel7_On=1;			
		}
		if(Mode==ADS1256_Channel_Close)            //检测到ADS1256通道关闭模式
		{
			if(ADS1256_Channel==0)
				ADS1256_Channel0_On=0;
			else if(ADS1256_Channel==1)
				ADS1256_Channel1_On=0;
			else if(ADS1256_Channel==2)
				ADS1256_Channel2_On=0;
			else if(ADS1256_Channel==3)
				ADS1256_Channel3_On=0;
			else if(ADS1256_Channel==4)
				ADS1256_Channel4_On=0;	
			else if(ADS1256_Channel==5)
				ADS1256_Channel5_On=0;
			else if(ADS1256_Channel==6)
				ADS1256_Channel6_On=0;
			else if(ADS1256_Channel==7)
				ADS1256_Channel7_On=0;			
		}		
		
		if(ADS1256_Channel0_On)                   //输出对应通道的值
		{
			adtemp=ADS1256_Getdata_OneShot(ADS1256_MUXP_AIN0 | ADS1256_MUXN_AINCOM);
			regstatus = ADS1256_Read_REG(READ_REG_ADCON);     //???PGA????????
			if(advalue<0x800000)
				advalue=(float)(adtemp*2.518021*2)/8388608/regstatus;
			printf("t60.txt=%c%.5f%c%c%c%c",x,advalue,x,f,f,f);			
		}
		if(ADS1256_Channel1_On)
		{
			adtemp=ADS1256_Getdata_OneShot(ADS1256_MUXP_AIN1 | ADS1256_MUXN_AINCOM);
			regstatus = ADS1256_Read_REG(READ_REG_ADCON);     //???PGA????????
			if(advalue<0x800000)
				advalue=(float)(adtemp*2.518021*2)/8388608/regstatus;
			printf("t61.txt=%c%.5f%c%c%c%c",x,advalue,x,f,f,f);		
		}	
		if(ADS1256_Channel2_On)
		{
			adtemp=ADS1256_Getdata_OneShot(ADS1256_MUXP_AIN2 | ADS1256_MUXN_AINCOM);
			regstatus = ADS1256_Read_REG(READ_REG_ADCON);     //???PGA????????
			if(advalue<0x800000)
				advalue=(float)(adtemp*2.518021*2)/8388608/regstatus;
			printf("t62.txt=%c%.5f%c%c%c%c",x,advalue,x,f,f,f);	
		}		
		if(ADS1256_Channel3_On)
		{
			adtemp=ADS1256_Getdata_OneShot(ADS1256_MUXP_AIN3 | ADS1256_MUXN_AINCOM);
			regstatus = ADS1256_Read_REG(READ_REG_ADCON);     //???PGA????????
			if(advalue<0x800000)
				advalue=(float)(adtemp*2.518021*2)/8388608/regstatus;
			printf("t63.txt=%c%.5f%c%c%c%c",x,advalue,x,f,f,f);		
		}
		if(ADS1256_Channel4_On)
		{
			adtemp=ADS1256_Getdata_OneShot(ADS1256_MUXP_AIN4 | ADS1256_MUXN_AINCOM);
			regstatus = ADS1256_Read_REG(READ_REG_ADCON);     //???PGA????????
			if(advalue<0x800000)
				advalue=(float)(adtemp*2.518021*2)/8388608/regstatus;
			printf("t64.txt=%c%.5f%c%c%c%c",x,advalue,x,f,f,f);		
		}
		if(ADS1256_Channel5_On)
		{
			adtemp=ADS1256_Getdata_OneShot(ADS1256_MUXP_AIN5 | ADS1256_MUXN_AINCOM);
			regstatus = ADS1256_Read_REG(READ_REG_ADCON);     //???PGA????????
			if(advalue<0x800000)
				advalue=(float)(adtemp*2.518021*2)/8388608/regstatus;
			printf("t65.txt=%c%.5f%c%c%c%c",x,advalue,x,f,f,f);		
		}
		if(ADS1256_Channel6_On)
		{
			adtemp=ADS1256_Getdata_OneShot(ADS1256_MUXP_AIN6 | ADS1256_MUXN_AINCOM);
			regstatus = ADS1256_Read_REG(READ_REG_ADCON);     //???PGA????????
			if(advalue<0x800000)
				advalue=(float)(adtemp*2.518021*2)/8388608/regstatus;
			printf("t66.txt=%c%.5f%c%c%c%c",x,advalue,x,f,f,f);			
		}	
		if(ADS1256_Channel7_On)
		{
			adtemp=ADS1256_Getdata_OneShot(ADS1256_MUXP_AIN7 | ADS1256_MUXN_AINCOM);
			regstatus = ADS1256_Read_REG(READ_REG_ADCON);     //???PGA????????
			if(advalue<0x800000)
				advalue=(float)(adtemp*2.518021*2)/8388608/regstatus;
			printf("t67.txt=%c%.5f%c%c%c%c",x,advalue,x,f,f,f);			
		}		
		vTaskDelay(1000);
	}
}

/*******************************************
Function name              :ADS1256TaskInit
Function use               :Inie the ADS1256 task
Function parameter         :None
Function return            :None
********************************************/
void
ADS1256TaskInit(void)
{

    //
    // Create the LED task.
    //
    xTaskCreate(
                        ADS1256_Getdata_DemoTask,    //任务函数
                       (const portCHAR *)"ADS1256", //任务名称 
                        ADS1256_STK_SIZE,              // 任务堆栈大小 
                        NULL,       
                        ADS1256_TASK_PRIO,
                        NULL
    );
        
}

