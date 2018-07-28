#ifndef __ADS1256_H__
#define __ADS1256_H__

//*****************************************************************************
//
// IO口对应引脚拉高拉低操作
//
//*****************************************************************************
#define ADS1256_DOUT_IN        GPIOPinRead(GPIO_PORTB_BASE,GPIO_PIN_2)
#define ADS1256_DRDY_IN        GPIOPinRead(GPIO_PORTB_BASE,GPIO_PIN_3)
#define ADS1256_CLK_SET        GPIOPinWrite(GPIO_PORTK_BASE, GPIO_PIN_6, GPIO_PIN_6)
#define ADS1256_CLK_CLR        GPIOPinWrite(GPIO_PORTK_BASE, GPIO_PIN_6, 0)                 
#define ADS1256_DIN_SET        GPIOPinWrite(GPIO_PORTL_BASE, GPIO_PIN_4, GPIO_PIN_4)
#define ADS1256_DIN_CLR        GPIOPinWrite(GPIO_PORTL_BASE, GPIO_PIN_4, 0)
#define ADS1256_CS_SET         GPIOPinWrite(GPIO_PORTP_BASE, GPIO_PIN_2, GPIO_PIN_2)
#define ADS1256_CS_CLR         GPIOPinWrite(GPIO_PORTP_BASE, GPIO_PIN_2, 0)
#define ADS1256_RESET_SET      GPIOPinWrite(GPIO_PORTP_BASE, GPIO_PIN_3, GPIO_PIN_3)
#define ADS1256_RESET_CLR      GPIOPinWrite(GPIO_PORTP_BASE, GPIO_PIN_3, 0)

//*****************************************************************************
//
//寄存器写地址：写寄存器=写操作+寄存器地址：0101+rrrr
//
//*****************************************************************************
#define WRITE_REG_STATUS      0x50
#define WRITE_REG_MUX      		0x51
#define WRITE_REG_ADCON      	0x52
#define WRITE_REG_DRATE     	0x53
#define WRITE_REG_IO      		0x54
#define WRITE_REG_OFC0      	0x55
#define WRITE_REG_OFC1      	0x56
#define WRITE_REG_OFC2      	0x57
#define WRITE_REG_FSC0      	0x58
#define WRITE_REG_FSC1      	0x59
#define WRITE_REG_FSC2      	0x5A

//*****************************************************************************
//
//寄存器读地址：读寄存器=读操作+寄存器地址：0001+rrrr
//
//*****************************************************************************
#define READ_REG_STATUS      	0x10
#define READ_REG_MUX      		0x11
#define READ_REG_ADCON      	0x12
#define READ_REG_DRATE     		0x13
#define READ_REG_IO      			0x14
#define READ_REG_OFC0      		0x15
#define READ_REG_OFC1      		0x16
#define READ_REG_OFC2     	 	0x17
#define READ_REG_FSC0     	 	0x18
#define READ_REG_FSC1     	 	0x19
#define READ_REG_FSC2     	 	0x1A

/*****************************************************************************
ADS1256操作命令：
WAKEUP： 	Completes SYNC and Exits Standby Mode
RDATA：		Read Data
RDATAC：  Read Data Continuously
SDATAC：  Stop Read Data Continuously
RREG：	  Read from REG rrr
WREG： 	  Write to REG rrr
SELFCAL： Offset and Gain Self-Calibration
SELFOCAL：Offset Self-Calibration
SELFGCAL：Gain Self-Calibration
SYSOCAL： System Offset Calibration
SYSGCAL： System Gain Calibration
SYNC：    Synchronize the A/D Conversion
STANDBY： Begin Standby Mode
RESET：	  Reset to Power-Up Values
WAKEUP：  Completes SYNC and Exits Standby Mode
*****************************************************************************/
#define COMMAND_WAKEUP      	0x00
#define COMMAND_RDATA      		0x01
#define COMMAND_RDATAC      	0x03
#define COMMAND_SDATAC     		0x0F
#define COMMAND_RREG      		0x1X
#define COMMAND_WREG      		0x5X
#define COMMAND_SELFCAL      	0xF0
#define COMMAND_SELFOCAL     	0xF1
#define COMMAND_SELFGCAL     	0xF2
#define COMMAND_SYSOCAL     	0xF3
#define COMMAND_SYSGCAL     	0xF4
#define COMMAND_SYNC     	 		0xFC
#define COMMAND_STANDBY     	0xFD
#define COMMAND_RESET     	 	0xFE

/*****************************************************************************
寄存器初始化配置数据
ADD RES    VAL BIT7   BIT6  	BIT5  	BIT4  	BIT3   		BIT2   		BIT1   		BIT0
00h STATUS x1H ID3		ID2 		ID1 		ID0 		ORDER 		ACAL 			BUFEN 		DRDY
01h MUX 	 01H PSEL3 	PSEL2 	PSEL1 	PSEL0 	NSEL3 		NSEL2 		NSEL1 		NSEL0
02h ADCON  20H 0 			CLK1 		CLK0 		SDCS1 	SDCS0 		PGA2 			PGA1 			PGA0
03h DRATE  F0H DR7 		DR6 		DR5 		DR4 		DR3 			DR2 			DR1 			DR0
04h IO 		 E0H DIR3 	DIR2 		DIR1 		DIR0 		DIO3 			DIO2 			DIO1 			DIO0
05h OFC0   xxH OFC07 	OFC06 	OFC05 	OFC04 	OFC03 		OFC02 		OFC01 		OFC00
06h OFC1   xxH OFC15 	OFC14 	OFC13 	OFC12 	OFC11 		OFC10 		OFC09 		OFC08
07h OFC2   xxH OFC23 	OFC22 	OFC21 	OFC20 	OFC19 		OFC18 		OFC17 		OFC16
08h FSC0   xxH FSC07 	FSC06 	FSC05 	FSC04 	FSC03 		FSC02 		FSC01 		FSC00
09h FSC1   xxH FSC15 	FSC14 	FSC13 	FSC12 	FSC11 		FSC10 		FSC09 		FSC08
0Ah FSC2   xxH FSC23 	FSC22 	FSC21 	FSC20 	FSC19 		FSC18 		FSC17 		FSC16

*****************************************************************************/
#define STATUS_INIT      			0x06
#define MUX_INIT      				0x08
#define ADCON_INIT      			0x00
#define DRATE_INIT     				0xA1
#define IO_INIT      					0x00
#define OFC0_INIT      				0x00
#define OFC1_INIT      				0x00
#define OFC2_INIT     	 			0x00
#define FSC0_INIT     	 			0x08
#define FSC1_INIT     	 			0x40
#define FSC2_INIT     	 			0x49

//*****************************************************************************
//
//ADS1256通道选择
//
//*****************************************************************************
#define ADS1256_MUXP_AIN0   0x00 
#define ADS1256_MUXP_AIN1   0x10 
#define ADS1256_MUXP_AIN2   0x20 
#define ADS1256_MUXP_AIN3   0x30 
#define ADS1256_MUXP_AIN4   0x40 
#define ADS1256_MUXP_AIN5   0x50 
#define ADS1256_MUXP_AIN6   0x60 
#define ADS1256_MUXP_AIN7   0x70 
#define ADS1256_MUXP_AINCOM 0x80 
 
#define ADS1256_MUXN_AIN0   0x00 
#define ADS1256_MUXN_AIN1   0x01 
#define ADS1256_MUXN_AIN2   0x02 
#define ADS1256_MUXN_AIN3   0x03 
#define ADS1256_MUXN_AIN4   0x04 
#define ADS1256_MUXN_AIN5   0x05 
#define ADS1256_MUXN_AIN6   0x06 
#define ADS1256_MUXN_AIN7   0x07 
#define ADS1256_MUXN_AINCOM 0x08   

//*****************************************************************************
//
//PGA放大倍数选择
//
//*****************************************************************************

#define ADS1256_GAIN_1      0x00 
#define ADS1256_GAIN_2      0x01 
#define ADS1256_GAIN_4      0x02 
#define ADS1256_GAIN_8      0x03 
#define ADS1256_GAIN_16     0x04 
#define ADS1256_GAIN_32     0x05 
#define ADS1256_GAIN_64     0x06 

//*****************************************************************************
//
//采样速率设置
//
//***************************************************************************** 
#define ADS1256_DRATE_30000SPS   0xF0 
#define ADS1256_DRATE_15000SPS   0xE0 
#define ADS1256_DRATE_7500SPS   0xD0 
#define ADS1256_DRATE_3750SPS   0xC0 
#define ADS1256_DRATE_2000SPS   0xB0 
#define ADS1256_DRATE_1000SPS   0xA1 
#define ADS1256_DRATE_500SPS    0x92 
#define ADS1256_DRATE_100SPS    0x82 
#define ADS1256_DRATE_60SPS     0x72 
#define ADS1256_DRATE_50SPS     0x63 
#define ADS1256_DRATE_30SPS     0x53 
#define ADS1256_DRATE_25SPS     0x43 
#define ADS1256_DRATE_15SPS     0x33 
#define ADS1256_DRATE_10SPS     0x23 
#define ADS1256_DRATE_5SPS      0x13 
#define ADS1256_DRATE_2_5SPS    0x03

//*****************************************************************************
//
//采样模式设置
//
//***************************************************************************** 
#define OneshotMode             0
#define ContinuouslyMode        1


void ADS1256_IO_Init(void);
void ADS1256_Write_REG(unsigned char address,unsigned char data);
void ADS1256_Init(uint8_t mode);
unsigned long ADS1256_ReadData(void);
uint8_t ADS1256_Communicate(uint8_t transmitdata);
uint32_t ADS1256_Getdata_OneShot(uint8_t Channel);
uint32_t ADS1256_Getdata_Continuously(uint8_t Channel);
uint8_t ADS1256_Read_REG(unsigned char address);
void ADS1256TaskInit(void);

#endif

