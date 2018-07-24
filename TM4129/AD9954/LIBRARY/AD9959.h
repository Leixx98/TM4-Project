#ifndef __AD9959_H
#define __AD9959_H	


#include "delay.h"

#ifndef uchar
#define uchar unsigned char
#endif

#ifndef uint
#define uint unsigned int
#endif

#ifndef ulong
#define ulong unsigned long
#endif



/*****************************************************************************************
P.S. : The IOs.

3V3     |    3V3
P1      |    P0
P3      |    P2
CS      |    IO_UP
NC      |    NC
NC      |    NC
NC      |    NC
NC      |    SCLK
SDIO_1  |    SDIO_0
SDIO_3  |    SDIO_2
RST     |    PD
NC      |    GND
*****************************************************************************************/


#define       AD9959_CS_SET        HAL_GPIO_WritePin(GPIOB,GPIO_PIN_8,GPIO_PIN_SET)    //B8
#define       AD9959_CS_CLR        HAL_GPIO_WritePin(GPIOB,GPIO_PIN_8,GPIO_PIN_RESET)


#define       AD9959_SCLK_SET      HAL_GPIO_WritePin(GPIOB,GPIO_PIN_6,GPIO_PIN_SET)    //B6
#define       AD9959_SCLK_CLR      HAL_GPIO_WritePin(GPIOB,GPIO_PIN_6,GPIO_PIN_RESET)

#define       AD9959_RST_SET       HAL_GPIO_WritePin(GPIOB,GPIO_PIN_7,GPIO_PIN_SET)    //B7
#define       AD9959_RST_CLR       HAL_GPIO_WritePin(GPIOB,GPIO_PIN_7,GPIO_PIN_RESET)

/*****************************************************************************************
P.S. : The GPIO_B3 is defaulted as JTDO and The GPIO_B4 is defaulted as NJRST.
*****************************************************************************************/

#define       AD9959_IOUP_SET      HAL_GPIO_WritePin(GPIOB,GPIO_PIN_5,GPIO_PIN_SET)    //B5
#define       AD9959_IOUP_CLR      HAL_GPIO_WritePin(GPIOB,GPIO_PIN_5,GPIO_PIN_RESET)

//#define       AD9959_PS0_SET       GPIO_SetBits(GPIOB,GPIO_Pin_6)
//#define       AD9959_PS0_CLR       GPIO_ResetBits(GPIOB,GPIO_Pin_6)

//#define       AD9959_PS1_SET       GPIO_SetBits(GPIOB,GPIO_Pin_7) //B7
//#define       AD9959_PS1_CLR       GPIO_ResetBits(GPIOB,GPIO_Pin_7)

//#define       AD9959_PS2_SET       HAL_GPIO_WritePin(GPIOB,GPIO_PIN_15,GPIO_PIN_SET)  //PB15
//#define       AD9959_PS2_CLR       HAL_GPIO_WritePin(GPIOB,GPIO_PIN_15,GPIO_PIN_SET)

#define       AD9959_PS3_SET       HAL_GPIO_WritePin(GPIOB,GPIO_PIN_9,GPIO_PIN_SET)    //B9
#define       AD9959_PS3_CLR       HAL_GPIO_WritePin(GPIOB,GPIO_PIN_9,GPIO_PIN_RESET)

#define       AD9959_SDIO0_SET     HAL_GPIO_WritePin(GPIOB,GPIO_PIN_10,GPIO_PIN_SET)    //B10
#define       AD9959_SDIO0_CLR     HAL_GPIO_WritePin(GPIOB,GPIO_PIN_10,GPIO_PIN_RESET)

#define       AD9959_SDIO1_SET     HAL_GPIO_WritePin(GPIOB,GPIO_PIN_11,GPIO_PIN_SET)    //B11
#define       AD9959_SDIO1_CLR     HAL_GPIO_WritePin(GPIOB,GPIO_PIN_11,GPIO_PIN_RESET)

#define       AD9959_SDIO2_SET     HAL_GPIO_WritePin(GPIOB,GPIO_PIN_12,GPIO_PIN_SET)    //B12
#define       AD9959_SDIO2_CLR     HAL_GPIO_WritePin(GPIOB,GPIO_PIN_12,GPIO_PIN_RESET)

#define       AD9959_SDIO3_SET     HAL_GPIO_WritePin(GPIOB,GPIO_PIN_13,GPIO_PIN_SET)    //B13
#define       AD9959_SDIO3_CLR     HAL_GPIO_WritePin(GPIOB,GPIO_PIN_13,GPIO_PIN_RESET)

#define       AD9959_PWR_SET       HAL_GPIO_WritePin(GPIOB,GPIO_PIN_14,GPIO_PIN_SET)    //B14
#define       AD9959_PWR_CLR       HAL_GPIO_WritePin(GPIOB,GPIO_PIN_14,GPIO_PIN_RESET)







void AD9959_IO_Init(void);
void AD9959_IO_Update(void);
void AD9959_W_SPI(u8 RegisterAddress, u8 NumberofRegisters, u8 *RegisterData,u8 temp);
void AD9959_Init(void);
void AD9959_W_Freq(u8 Channel,u32 Freq);
void AD9959_W_Amp(uchar Channel,uint Amp);
void AD9959_W_Phase(u8 Channel,u16 Phase);





#endif
