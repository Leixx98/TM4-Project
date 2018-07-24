#include "AD9959.h"


/***************************************************************************
AD9959 Register definition
***************************************************************************/
//-----------------------------------------------------------------------------------------------------------------
#define CSR_ADD  0x00             //CSR : Channel Select register, which include channel choice,the kind of SPI and the high or low data first
                                  //-> AD9958 datasheet Table 27	


/******************************************
P.S.:You can define a new array,which can use "|" to enable different channels at the same time.
******************************************/
u8 CSR_DATA0[1] = {0x10};         // Enable CH0
u8 CSR_DATA1[1] = {0x20};         // Enable CH1
u8 CSR_DATA2[1] = {0x40};         // Enable CH2
u8 CSR_DATA3[1] = {0x80};         // Enable CH3
u8 CSR_DATA4[1] = {0xF0};         // Enable CH3	

																			
#define FR1_ADD  0x01             //FR1 : Function register 1 -> AD9958 datasheet Table 27																		
u8 FR1_DATA[3] = {0xD0,0x00,0x00};//default Value = 0x000000;   20 times of the frequence;  Charge pump control = 75uA
                                                  //FR1<23> -- VCO gain control = 0 --> system clock below 160 MHz; 
                                                  //                            = 1 --> the high range (system clock above 255 MHz

#define FR2_ADD 0x02              //FR2 : Function register 2 -> AD9958 datasheet Table 27	
u8 FR2_DATA[2] = {0x20,0x00};     //default Value = 0x0000

#define CFR_ADD 0x03              //CFR : Channel Function register -> AD9958 datasheet Table 28																		
//u8 CFR_DATA[3] = {0x00,0x23,0x37};//default Value = 0x000302
u8 CFR_DATA[3] = {0x00,0x03,0x00};

#define CFTW0_ADD 0x04            //CTW0 channel frequence register -> AD9958 datasheet Table 28		

/**************************************************************************************
The frequence of crystal : 25MHZ
The frequence of system  : 25MHz * 20 = 500MHz
**************************************************************************************/
//u8 CFTW0_DATA[4] = {0x00,0x0d,0x1B,0x71};       //OUT 100KHZ 
u8 CFTW0_DATA[4] = {0x00,0x83,0x12,0x6F};       //OUT 1MHZ		------- 										
//u8 CFTW0_DATA[4] = {0x05,0x1E,0xB8,0x52};       //OUT 10MHZ	 
//u8 CFTW0_DATA[4] = {0x0a,0x3d,0x70,0xa4};       //OUT 20MHZ	  
//u8 CFTW0_DATA[4] = {0x28,0xF5,0xC2,0x8F};       //OUT 80MHZ  
//u8 CFTW0_DATA[4] = {0x33,0x33,0x33,0x33};      //OUT 100MHZ


#define CPOW0_ADD 0x05                         //CPOW0 : Channel Phase Offset register -> AD9958 datasheet Table 28																		
u8 CPOW0_DATA[2] = {0x00,0x00};                //default Value = 0x0000   @ = POW/2^14*360


#define ACR_ADD 0x06                           //ACR : Amplitude Control register -> AD9958 datasheet Table 28																		
u8 ACR_DATA[3] = {0x00,0x00,0x00};             //default Value = 0x--0000 Rest = 18.91/Iout 


#define LSRR_ADD 0x07                          //LSRR : Linear Sweep Ramp Rate register -> AD9958 datasheet Table 28																		
u8 LSRR_DATA[2] = {0x00,0x00};                 //default Value = 0x----


#define RDW_ADD 0x08                           //RDW : LSR Rising Delta register -> AD9958 datasheet Table 28																		
u8 RDW_DATA[4] = {0x00,0x00,0x00,0x00};        //default Value = 0x--------

#define FDW_ADD 0x09                           //FDW : LSR Falling Delta register -> AD9958 datasheet Table 28																		
u8 FDW_DATA[4] = {0x00,0x00,0x00,0x00};        //default Value = 0x--------



/***************************************************************************
AD9959 Function definition
***************************************************************************/

//------------------------------------------------------------------------------------------------

/***************************************************************************
Function name     :AD9959_IO_Init
Function use      :Init the using IO ports of AD9959
Function parameter:None
Function return   :None
***************************************************************************/
void AD9959_IO_Init(void)
{
 
 GPIO_InitTypeDef  GPIO_InitStructure;
 	
 __HAL_RCC_GPIOB_CLK_ENABLE();	 //使能PB端口时钟

 GPIO_InitStructure.Pin = GPIO_PIN_2|GPIO_PIN_3|GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7|GPIO_PIN_8|GPIO_PIN_9|GPIO_PIN_10|GPIO_PIN_11|GPIO_PIN_12|GPIO_PIN_13|GPIO_PIN_14|GPIO_PIN_15;	    		 //端口配置
 GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_PP; 		 //输出
 GPIO_InitStructure.Pull=GPIO_NOPULL;
 GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_HIGH;		 //IO口速度为高速
 HAL_GPIO_Init(GPIOB,&GPIO_InitStructure);	  				 
}



/***************************************************************************
Function name     :AD9959_IO_Update
Function use      :Update AD9959 from IOUP PIN
Function parameter:None
Function return   :None
***************************************************************************/
void AD9959_IO_Update(void)
{
	AD9959_IOUP_CLR;
	delay_us(2);
	AD9959_IOUP_SET;
	delay_us(4);
	AD9959_IOUP_CLR;
	delay_us(2);
}


void IntReset(void)
{
	AD9959_RST_CLR;
	delay_us(1);
	AD9959_RST_SET;
	delay_us(30);
	AD9959_RST_CLR;
}


void Intserve(void)
{
	AD9959_PWR_CLR;
	AD9959_CS_SET;
	AD9959_SCLK_CLR;
	AD9959_IOUP_CLR;
//	AD9959_PS0_CLR;
	
//	AD9959_PS2_CLR;
//	AD9959_PS1_CLR;
	AD9959_PS3_CLR;
	AD9959_SDIO0_CLR;
	AD9959_SDIO1_CLR;
	AD9959_SDIO2_CLR;
	AD9959_SDIO3_CLR;
}



/***************************************************************************
Function name     :AD9959_W_SPI
Function use      :Use SPI to write data/cmd to AD9959
Function parameter:u8 RegisterAddress-------The address of resgister
                   u8 NumberofRegisters-----The Bytes of register
                   u8 *RegisterData---------The beginning address of data 
                   u8 temp------------------1:Update the registers
                                            0:Don't update the registers
Function return   :None
***************************************************************************/
void AD9959_W_SPI(u8 RegisterAddress, u8 NumberofRegisters, u8 *RegisterData,u8 temp)
{
	u8 	ControlValue = 0;
	u8		ValueToWrite = 0;
	u8	  RegisterIndex = 0;
	u8	  i = 0;
		
	ControlValue = RegisterAddress;             //Create the 8-bit header
	AD9959_SCLK_CLR;
	AD9959_CS_CLR;
	for(i=0; i<8; i++)                          //Write out the control word
	{
		AD9959_SCLK_CLR;
		if(0x80 == (ControlValue & 0x80))
		{
			AD9959_SDIO0_SET;                       //Send one to SDIO0pin
		}
		else 
		{
			AD9959_SDIO0_CLR;                       //Send zero to SDIO0pin
		}
		
		AD9959_SCLK_SET;
	  ControlValue <<= 1;	                      //Rotate data
	}
	
	AD9959_SCLK_CLR;
	
	for (RegisterIndex=0; RegisterIndex<NumberofRegisters; RegisterIndex++)
	{
		ValueToWrite = RegisterData[RegisterIndex];
		for (i=0; i<8; i++)
		{
			AD9959_SCLK_CLR;
			if(0x80 == (ValueToWrite & 0x80))
			{
				AD9959_SDIO0_SET;	                   //Send one to SDIO0pin
			}
			else
			{
				AD9959_SDIO0_CLR;	                   //Send zero to SDIO0pin
			}
			AD9959_SCLK_SET;
			ValueToWrite <<= 1;	                   //Rotate data
		}
		AD9959_SCLK_CLR;
	}
	
	if(temp==1)
	{
		AD9959_IO_Update();           //Update or not
	}
	AD9959_CS_SET;
}

/***************************************************************************
Function name     :AD9959_Init
Function use      :Init AD9959
Function parameter:None
Function return   :None
***************************************************************************/
void AD9959_Init(void)
{
	Intserve();
  IntReset();
	AD9959_W_SPI(FR1_ADD,3,FR1_DATA,1);
//	AD9959_W_SPI(CSR_ADD,1,CSR_DATA0,1);
//  AD9959_W_SPI(CFR_ADD,3,CFR_DATA,1);
//	AD9959_W_SPI(CSR_ADD,1,CSR_DATA1,1);
//  AD9959_W_SPI(CFR_ADD,3,CFR_DATA,1);
	delay_us(10);

}

/***************************************************************************
Function name     :AD9959_W_Freq
Function use      :Write frequence to the selected channel of AD9959
Function parameter:uchar Channel----------The channel which you want to control
                  :ulong Freq-------------The Frequence
Function return   :None
Application Note  :While using orthogonality function , the parameter 'temp' 
                   of the function AD9959_W_SPI should be set to 0 . As in this
                   occasion , the phase accumulation register will be updated after
                   the IO_Update.
***************************************************************************/
void AD9959_W_Freq(u8 Channel,u32 Freq)
{
	  u8 CFTW_DATA[4] ={0x00,0x00,0x00,0x00};	
	  u32 Temp;            
	  Temp=(u32)Freq*8.589934592;	                                  //divide the Freq word into 4 Bytes  4.294967296=(2^32)/500000000  8.589934592=(2^32)/250000000
	  CFTW_DATA[3]=(u8)Temp;
	  CFTW_DATA[2]=(u8)(Temp>>8);
	  CFTW_DATA[1]=(u8)(Temp>>16);
	  CFTW_DATA[0]=(u8)(Temp>>24);
		switch(Channel)
		{
			case 0 : AD9959_W_SPI(CSR_ADD,1,CSR_DATA0,0);                 //Write the registers of CH0
               AD9959_W_SPI(CFTW0_ADD,4,CFTW_DATA,0);               //CTW0 address 0x04. Output the frequence
			         break;
			case 1 : AD9959_W_SPI(CSR_ADD,1,CSR_DATA1,0);                 //Write the registers of CH1
               AD9959_W_SPI(CFTW0_ADD,4,CFTW_DATA,0);               //CTW0 address 0x04. Output the frequence
			         break;
			case 2 : AD9959_W_SPI(CSR_ADD,1,CSR_DATA2,0);                 //Write the registers of CH2
               AD9959_W_SPI(CFTW0_ADD,4,CFTW_DATA,0);               //CTW0 address 0x04. Output the frequence
			         break;
			case 3 : AD9959_W_SPI(CSR_ADD,1,CSR_DATA3,0);                 //Write the registers of CH3
               AD9959_W_SPI(CFTW0_ADD,4,CFTW_DATA,0);               //CTW0 address 0x04. Output the frequence
			         break;
			default : ;
		}
}

/***************************************************************************
Function name     :AD9959_W_Amp
Function use      :Write amplitude to the selected channel of AD9959
Function parameter:uchar Channel----------The channel which you want to control
                  :uint  Amp--------------The Amplitude
Function return   :None
***************************************************************************/
void AD9959_W_Amp(uchar Channel,uint Amp)
{
  uint A_temp;
  A_temp=Amp|0x1000;
  ACR_DATA[2]=(uchar)A_temp;                                            //LSB
  ACR_DATA[1]=(uchar)(A_temp>>8);                                       //MSB
	switch(Channel)
	{
		case 0 : AD9959_W_SPI(CSR_ADD,1,CSR_DATA0,1);                    //Write the registers of CH0
						 AD9959_W_SPI(ACR_ADD,3,ACR_DATA,1);                     //CTW0 address 0x04. Output the amplitude
						 break;
		case 1 : AD9959_W_SPI(CSR_ADD,1,CSR_DATA1,1);                    //Write the registers of CH1
						 AD9959_W_SPI(ACR_ADD,3,ACR_DATA,1);                     //CTW0 address 0x04. Output the amplitude
						 break;
		case 2 : AD9959_W_SPI(CSR_ADD,1,CSR_DATA2,1);                    //Write the registers of CH2
						 AD9959_W_SPI(ACR_ADD,3,ACR_DATA,1);                     //CTW0 address 0x04. Output the amplitude
						 break;
		case 3 : AD9959_W_SPI(CSR_ADD,1,CSR_DATA3,1);                    //Write the registers of CH3
						 AD9959_W_SPI(ACR_ADD,3,ACR_DATA,1);                     //CTW0 address 0x04. Output the amplitude
						 break;
		default : ;
	}
}


/***************************************************************************
Function name     :AD9959_W_Phase
Function use      :Write phase to the selected channel of AD9959
Function parameter:u8   Channel----------The channel which you want to control
                  :u16  Phase------------The Phase
Function return   :None
Application Note  :1、While using orthogonality function , the parameter 'temp' 
                   of the function AD9959_W_SPI should be set to 0 . As in this
                   occasion , the phase accumulation register will be updated after
                   the IO_Update.
                   2、By measurement , the CH0 has about 13-degree phase difference
                   with the CH1 . Maybe other channels have the similar situation? 
                   Therefore , while using orthogonality function , the phase of 
                   CH0 should be set to 13 degree with the phase of CH1 is 90 degree.
***************************************************************************/
void AD9959_W_Phase(u8 Channel,u16 Phase)
{
  u16 P_temp=0;
	P_temp=(u16)Phase*45.511111;                                 //Write the phase : each degree->45.511111=(2^14)/360
  CPOW0_DATA[1]=(u8)P_temp;
  CPOW0_DATA[0]=(u8)(P_temp>>8);
	switch(Channel)
	{
		case 0 : AD9959_W_SPI(CSR_ADD,1,CSR_DATA0,0);               //Write the registers of CH0
						 AD9959_W_SPI(CPOW0_ADD,2,CPOW0_DATA,0);            //CTW0 address 0x04. Output the phase
						 break;
		case 1 : AD9959_W_SPI(CSR_ADD,1,CSR_DATA1,0);               //Write the registers of CH1
						 AD9959_W_SPI(CPOW0_ADD,2,CPOW0_DATA,0);            //CTW0 address 0x04. Output the phase
						 break;
		case 2 : AD9959_W_SPI(CSR_ADD,1,CSR_DATA2,0);               //Write the registers of CH2
						 AD9959_W_SPI(CPOW0_ADD,2,CPOW0_DATA,0);            //CTW0 address 0x04. Output the phase
						 break;
		case 3 : AD9959_W_SPI(CSR_ADD,1,CSR_DATA3,0);               //Write the registers of CH3
						 AD9959_W_SPI(CPOW0_ADD,2,CPOW0_DATA,0);            //CTW0 address 0x04. Output the phase
						 break;
		default : ;
	}
}
