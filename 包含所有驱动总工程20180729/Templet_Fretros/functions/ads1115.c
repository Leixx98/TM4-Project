#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
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
#include "functions/ads1115.h"
#include "functions/uartprint.h"

//*****************************************************************************

#define ADS1115_TASK_PRIO           3        //任务优先级
#define ADS1115_STK_SIZE           128       //任务堆栈大小

//*****************************************************************************

char Channel_Value;
char ShowChannel[] = "No.- Channel:";

short D_ADS; 		//转换的数字量

unsigned int Config;
unsigned char Writebuff[4],Readbuff[3];
unsigned char Result[2]; 

int  Format[5]={0};  //转换的数字量转换成十进制

/*******************************************
Function name              :ADS1115_IO_Init
Function use               :Init the IO ports of ADS1115
Function parameter         :None
Function return            :None
********************************************/
void ADS1115_IO_Init(void)
{
   
   //使能GPIOB,F
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOK);
	//设置PB0,PB1为输出
	GPIOPinTypeGPIOOutput(GPIO_PORTK_BASE, GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3);
	//设置PB0,PB1为8mA上拉
	GPIOPadConfigSet(GPIO_PORTK_BASE, GPIO_PIN_0 |GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3,
                         GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPU);	
}



/*******************************************
Function name              :ADS1115_SDAPort_In
Function use               :Set SDA port as input.
Function parameter         :None
Function return            :None
********************************************/
void ADS1115_SDAPort_In(void)
{
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOK);
    GPIOPinTypeGPIOInput(GPIO_PORTK_BASE, GPIO_PIN_0);
    GPIOPadConfigSet(GPIO_PORTK_BASE, GPIO_PIN_0,
                         GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPU);	
}




/*******************************************
Function name              :ADS1115_SDAPort_Out
Function use               :Set SDA port as output.
Function parameter         :None
Function return            :None
********************************************/
void ADS1115_SDAPort_Out(void)
{
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOK);
	//设置PB0,PB1为输出
	GPIOPinTypeGPIOOutput(GPIO_PORTK_BASE, GPIO_PIN_0);
	//设置PB0,PB1为8mA上拉
	GPIOPadConfigSet(GPIO_PORTK_BASE, GPIO_PIN_0 ,
                         GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPU);	
}

/*******************************************
Function name              :ADS1115_I2C_Start
Function use               :Start the I2C Bus of ADS1115
Function parameter         :None
Function return            :None
********************************************/
void ADS1115_I2C_Start(void)
{
	ADS1115_SCL_SET;       //SCL=1;
	ADS1115_SDA_SET;       //SDA=1;
	delay_us(15);
	ADS1115_SDA_CLR;       //SDA=0;
	delay_us(15);
	ADS1115_SCL_CLR;       //SCL=0;
	delay_us(15);
}


/*******************************************
Function name              :ADS1115_I2C_Stop
Function use               :Stop the I2C Bus of ADS1115
Function parameter         :None
Function return            :None
********************************************/
void ADS1115_I2C_Stop(void)
{ 
    ADS1115_SDA_CLR;       //SDA=0;
    delay_us(15);
    ADS1115_SCL_SET;       //SCL=1;
    delay_us(15);
	ADS1115_SDA_SET;       //SDA=1;
    delay_us(15);
}
/*******************************************
Function name              :ADS1115_I2C_Ack
Function use               :The answer of the I2C master
Function parameter         :None
Function return            :None
********************************************/
void ADS1115_I2C_Ack(void)
{
    ADS1115_SDA_CLR;       //SDA=0;
	delay_us(1);
    ADS1115_SCL_SET;       //SCL=1;
    delay_us(15);
    ADS1115_SCL_CLR;       //SCL=0;
	delay_us(1);
	ADS1115_SDA_SET;       //SDA=1;
    delay_us(15);
}


/*******************************************
Function name              :ADS1115_I2C_Nack
Function use               :The I2C master has no answer
Function parameter         :None
Function return            :None
********************************************/
void ADS1115_I2C_Nack(void)
{
	ADS1115_SDA_SET;       //SDA=1;
	delay_us(1);
	ADS1115_SCL_SET;       //SCL=1;
	delay_us(15);
	ADS1115_SCL_CLR;       //SCL=0;
	delay_us(1);
	ADS1115_SDA_CLR;       //SDA=0;   
	delay_us(15);
}



/*******************************************
Function name              :ADS1115_I2C_Check
Function use               :Check the answer of I2C slave
Function parameter         :None
Function return            :TRUE (1)--Slave has answer;
                            FALSE(0)--Slave has no answer
********************************************/
unsigned char ADS1115_I2C_Check(void)
{
    unsigned char slaveack;
    
    ADS1115_SDA_SET;       //SDA=1;
    delay_us(1);
    ADS1115_SCL_SET;       //SCL=1;
    ADS1115_SDAPort_In();	                    //Set SDA port as input.
    delay_us(15);
    slaveack = ADS1115_SDA_IN;   		                //Read the status of SDA port.
    ADS1115_SCL_CLR;       //SCL=0;
    delay_us(15);
	ADS1115_SDAPort_Out();	                    //Set SDA port as output.
	
    if(slaveack)    return FALSE;
    else            return TRUE;
}

/*******************************************
Function name              :ADS1115_Write_1_Byte
Function use               :Write 1 byte to ADS1117
Function parameter         :DataByte--The byte you want to write to ADS1115
Function return            :TRUE (1)--Slave has answer;
                            FALSE(0)--Slave has no answer
********************************************/
void ADS1115_Write_1_Byte(unsigned char DataByte)
{
	int i;

	for(i=0;i<8;i++)
	{
		if(DataByte&0x80)  //if((DataByte<<i)&0x80)
            {ADS1115_SDA_SET;}    //SDA=1;
		else
            { ADS1115_SDA_CLR;}       //SDA=0;
        delay_us(15);
		ADS1115_SCL_SET;         //SCL=1;
        delay_us(15);
        ADS1115_SCL_CLR;         //SCL=0;
        delay_us(15);
        DataByte <<= 1;
	}
    ADS1115_SDA_SET;             //SDA=1;
    delay_us(1);
}



/*******************************************
Function name              :ADS1115_Write_N_Byte
Function use               :Write N bytes to ADS1117
Function parameter         :*writebuffer--The point of the array(unsigned char) you want to write to ADS1115
                                     num--The number of the bytes
Function return            :TRUE (1)--Slave has answer;
                            FALSE(0)--Slave has no answer
********************************************/
unsigned char ADS1115_Write_N_Byte(unsigned char *writebuffer,unsigned char num)
{
	int i;

	for(i=0;i<num;i++)
	{
		ADS1115_Write_1_Byte(*writebuffer);
		if(ADS1115_I2C_Check())
		{
			writebuffer ++;
		}
		else
		{
			ADS1115_I2C_Stop();
		    return FALSE;
		}
	}
	ADS1115_I2C_Stop();
	return TRUE;
}



/*******************************************
Function name              :ADS1115_Read_1_Byte
Function use               :Read a byte from ADS1117
Function parameter         :None
Function return            :data_Value -- The byte you read from ADS1115
********************************************/
unsigned char ADS1115_Read_1_Byte(void)
{
    unsigned char data_Value = 0, FLAG, i;
	
    for(i=0;i<8;i++)
    { 
        
        ADS1115_SDA_SET;         //SDA=1;
        delay_us(15);
        ADS1115_SCL_SET;         //SCL=1;
        ADS1115_SDAPort_In();	 //Set SDA port as input.
        delay_us(15);
        FLAG=ADS1115_SDA_IN;
        data_Value <<= 1;
	    if(FLAG)
            data_Value |= 0x01;

        ADS1115_SCL_CLR;         //SCL=0;
        delay_us(15);
        ADS1115_SDAPort_Out();	 //Set SDA port as output.
    }
    return data_Value;
}

/*******************************************
Function name              :ADS1115_Read_N_Byte
Function use               :Read N bytes from ADS1117
Function parameter         :*readbuff -- The point of the array(unsigned char) where you'd like to store the bytes you read.
Function return            :data_Value -- The byte you read from ADS1115
********************************************/
void ADS1115_Read_N_Byte(unsigned char*readbuff, unsigned char num)
{
	unsigned char i;
	
	for(i=0;i<num;i++)
	{
		readbuff[i]=ADS1115_Read_1_Byte();
		if(i==num-1)
			ADS1115_I2C_Nack();  //read bytes uncontinously
		else 
			ADS1115_I2C_Ack();   //read bytes continously
	}
	ADS1115_I2C_Stop();

}


/*******************************************
Function name              :ADS1115_Init
Function use               :Init the ADS1115
Function parameter         :S_MUX_0,S_MUX_1  --  choose a single channel
Function return            :None
********************************************/
void ADS1115_Init(u8 S_MUX_0, u8 S_MUX_1)
{

	
	if (S_MUX_0 == 0 && S_MUX_1 == 0)
	Config = OS+MUX_A0+PGA+DR+COMP_QUE+MODE;
	if (S_MUX_0 == 0 && S_MUX_1 == 1) 
	Config = OS+MUX_A1+PGA+DR+COMP_QUE+MODE;
	if (S_MUX_0 == 1 && S_MUX_1 == 0)
	Config = OS+MUX_A2+PGA+DR+COMP_QUE+MODE;
	if (S_MUX_0 == 1 && S_MUX_1 == 1)
	Config = OS+MUX_A3+PGA+DR+COMP_QUE+MODE;
    
    Writebuff[0]=ADDRESS_W;
    Writebuff[1]=Pointer_1;
    Writebuff[2]=Config/256;
    Writebuff[3]=Config%256;
        
    Readbuff[0]=ADDRESS_W;
    Readbuff[1]=Pointer_0;
        
    Readbuff[2]=ADDRESS_R;
}



/*******************************************
Function name              :ADS1115_WriteWord
Function use               :Write words which is set in Writebuff to ADS1115.
Function parameter         :None
Function return            :None
********************************************/
void ADS1115_WriteWord(void)
{
     int t;
     ADS1115_I2C_Start();   
     do
     {
          t=ADS1115_Write_N_Byte(Writebuff,4);
     }while(t==0);                                  //Write 4 bytes in Writebuff to ADS1115.
}


/*******************************************
Function name              :ADS1115_ReadWord
Function use               :Read word and stored in Result from ADS1115.
Function parameter         :None
Function return            :None
********************************************/
void ADS1115_ReadWord(void)
{
    int t;
    ADS1115_I2C_Start();                           //Write 2 bytes.
    do
    {
      t=ADS1115_Write_N_Byte(Readbuff,2);
    }while(t==0);

    ADS1115_I2C_Start();                           //Write 2 bytes.
    do
    {
      t=ADS1115_Write_N_Byte(&Readbuff[2],1);
    }while(t==0);
    
    ADS1115_Read_N_Byte(Result,2);                 //Read 2 bytes.

}


/*******************************************
Function name              :ADS1115_Getdata
Function use               :Get data from ADS1115 in single channel mode.
Function parameter         :Channel              -- A single channel
Function return            :short D_ADS -- The data from ADS1115
********************************************/
short ADS1115_Getdata(u8 Channel)
{
	u8 S_MUX_0, S_MUX_1;

	switch(Channel)
	{
		case 0:
			S_MUX_0 = 0;
			S_MUX_1 = 0;
			Channel_Value = Channel + '0';
			break;
		case 1:
			S_MUX_0 = 0;
			S_MUX_1 = 1;
			Channel_Value = Channel + '0';
			break;
		case 2:
			S_MUX_0 = 1;
			S_MUX_1 = 0;
			Channel_Value = Channel + '0';
			break;
		case 3:
			S_MUX_0 = 1;
			S_MUX_1 = 1;
			Channel_Value = Channel + '0';
			break;
		default :
			break;
	}
    AD_START;
    delay_us(10);
    ADS1115_Init(S_MUX_0, S_MUX_1);
    ADS1115_WriteWord();
    delay_us(10); 
    ADS1115_ReadWord();
    delay_us(10);
	  AD_END;
    D_ADS=Result[0]*256+Result[1];  //转换的数字量

  	return D_ADS;
}

/*******************************************
Function name              :ADS1115_DemoTask
Function use               :Run the AD9959 to set Frequency of channel 2,3
Function parameter         :None
Function return            :None
********************************************/
void ADS1115_DemoTask(void *pvParameters)
{
  //变量：获得AD初始值
  static uint32_t AD_Temp=0;;
  //变量：计算转换后的电压值
  static uint32_t AD_Value=0;;	
	uint8_t ADS1115_Channel2_On=0;
	uint8_t ADS1115_Channel3_On=0;
	uint8_t x='\"',f=0xff;
	ADS1115_IO_Init();
	while(1)
	{
		if(Mode==ADS1115_Channel_Open)                     //当模式为开启ADS1115通道时
		{
			if(ADS1115_Channel==2)                   //如果当前通道为2，则2通道对应变量置1
				ADS1115_Channel2_On=1;
			else if(ADS1115_Channel==3)               //如果当前通道为3，则3通道对应变量置1
				ADS1115_Channel3_On=1;
			Mode=0xff;                                //清除模式
		}
		if(Mode==ADS1115_Channel_Close)                     //当模式为关闭ADS1115通道时
		{
			if(ADS1115_Channel==2)                    //选择关闭通道
				ADS1115_Channel2_On=0;
			else if(ADS1115_Channel==3)
				ADS1115_Channel3_On=0;
			Mode=0xff;			
		}
			if(ADS1115_Channel2_On)                         //当对应通道变量打开时，获得数据
			{
				AD_Temp = ADS1115_Getdata(3);                  //采样数据反了，目前原因未知
				AD_Value =(AD_Temp* 12281)/65536;
				printf("t50.txt=%c%dmV%c%c%c%c",x,AD_Value,x,f,f,f); 
			}
			if(ADS1115_Channel3_On)
			{
				AD_Temp = ADS1115_Getdata(2);
				AD_Value =(AD_Temp* 12281)/65536;
				printf("t51.txt=%c%dmV%c%c%c%c",x,AD_Value,x,f,f,f);
			}

		vTaskDelay(1000);
	}
}

/*******************************************
Function name              :ADS1115TaskInit
Function use               :Inie the ADS1256 task
Function parameter         :None
Function return            :None
********************************************/
void
ADS1115TaskInit(void)
{

    //
    // Create the LED task.
    //
    xTaskCreate(
                        ADS1115_DemoTask,    //任务函数
                       (const portCHAR *)"ADS1115", //任务名称 
                        ADS1115_STK_SIZE,              // 任务堆栈大小 
                        NULL,       
                        ADS1115_TASK_PRIO,
                        NULL
    );
        
}


