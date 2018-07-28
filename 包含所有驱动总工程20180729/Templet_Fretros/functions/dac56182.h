#ifndef _DAC56182_H
#define _DAC56182_H

#ifdef __cplusplus
 extern "C" {
#endif

typedef unsigned char   uchar;
typedef unsigned int    uint;
//GPIO_InitTypeDef GPIO_Initure;
#define TLV56182_SCL_SET        GPIOPinWrite(GPIO_PORTM_BASE, GPIO_PIN_0, GPIO_PIN_0);
#define TLV56182_SCL_CLR        GPIOPinWrite(GPIO_PORTM_BASE, GPIO_PIN_0, 0);       
                 
#define TLV56182_CS_HIGH        GPIOPinWrite(GPIO_PORTM_BASE, GPIO_PIN_1, GPIO_PIN_1);
#define TLV56182_CS_LOW         GPIOPinWrite(GPIO_PORTM_BASE, GPIO_PIN_1, 0);       
                 
#define TLV56182_SDA_SET        GPIOPinWrite(GPIO_PORTL_BASE, GPIO_PIN_0, GPIO_PIN_0);
#define TLV56182_SDA_CLR        GPIOPinWrite(GPIO_PORTL_BASE, GPIO_PIN_0, 0);



void TLV56182_SDAPort_Out(void);
void TLV56182_Init(void);
void DAC_Conver2(uint temp);
void Write_A2(float out_volt);
void Write_B2(float out_volt);
uint VoltToData2(float out_volt); //说明ref为参考源out_volt为输出模拟电压值
void DAC56182TaskInit(void);


#ifdef __cplusplus
}
#endif
#endif /*__ pinoutConfig_H */
