#ifndef _DAC5618_H
#define _DAC5618_H

#ifdef __cplusplus
 extern "C" {
#endif

typedef unsigned char   uchar;
typedef unsigned int    uint;
//GPIO_InitTypeDef GPIO_Initure;
#define TLV5618_SCL_SET        GPIOPinWrite(GPIO_PORTK_BASE, GPIO_PIN_1, GPIO_PIN_1);
#define TLV5618_SCL_CLR        GPIOPinWrite(GPIO_PORTK_BASE, GPIO_PIN_1, 0);       
                 
#define TLV5618_CS_HIGH        GPIOPinWrite(GPIO_PORTK_BASE, GPIO_PIN_2, GPIO_PIN_2);
#define TLV5618_CS_LOW        GPIOPinWrite(GPIO_PORTK_BASE, GPIO_PIN_2, 0);       
                 
#define TLV5618_SDA_SET        GPIOPinWrite(GPIO_PORTK_BASE, GPIO_PIN_0, GPIO_PIN_0);
#define TLV5618_SDA_CLR        GPIOPinWrite(GPIO_PORTK_BASE, GPIO_PIN_0, 0);



void TLV5618_SDAPort_Out(void);
void TLV5618_Init(void);
void DAC_Conver(uint temp);
void Write_A(float out_volt);
void Write_B(float out_volt);
uint VoltToData(float out_volt); //说明ref为参考源out_volt为输出模拟电压值
void DAC5618TaskInit(void);


#ifdef __cplusplus
}
#endif
#endif /*__ pinoutConfig_H */
