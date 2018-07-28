#ifndef _DAC8501_H
#define _DAC8501_H


//GPIO_InitTypeDef GPIO_Initure;
#define DAC8501_SDA_SET        GPIOPinWrite(GPIO_PORTP_BASE, GPIO_PIN_1, GPIO_PIN_1);
#define DAC8501_SDA_CLR        GPIOPinWrite(GPIO_PORTP_BASE, GPIO_PIN_1, 0);       
                 
#define DAC8501_CLK_SET        GPIOPinWrite(GPIO_PORTB_BASE, GPIO_PIN_5, GPIO_PIN_5);
#define DAC8501_CLK_CLR        GPIOPinWrite(GPIO_PORTB_BASE, GPIO_PIN_5, 0);       
                 
#define DAC8501_CS2_SET        GPIOPinWrite(GPIO_PORTB_BASE, GPIO_PIN_4, GPIO_PIN_4);
#define DAC8501_CS2_CLR        GPIOPinWrite(GPIO_PORTB_BASE, GPIO_PIN_4, 0);

#define DAC8501_CS1_SET        GPIOPinWrite(GPIO_PORTP_BASE, GPIO_PIN_0, GPIO_PIN_0);
#define DAC8501_CS1_CLR        GPIOPinWrite(GPIO_PORTP_BASE, GPIO_PIN_0, 0);


void DAC8501_IO_Init(void);
void DAC8501_Transform(uint32_t temp,uint8_t channel);
uint32_t DAC8501_VoltToData(float out_volt);
void Write_Data_Channel1(float out_volt);
void Write_Data_Channel2(float out_volt);
void DAC8501TaskInit(void);

#endif

