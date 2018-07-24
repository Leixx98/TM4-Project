#ifndef __AD8369_H__
#define __AD8369_H__



#define CS_SET         GPIOPinWrite(GPIO_PORTQ_BASE, GPIO_PIN_1, GPIO_PIN_1);
#define CS_CLR         GPIOPinWrite(GPIO_PORTQ_BASE, GPIO_PIN_1, 0);
#define CLK_SET       GPIOPinWrite(GPIO_PORTQ_BASE, GPIO_PIN_2, GPIO_PIN_2);
#define CLK_CLR       GPIOPinWrite(GPIO_PORTQ_BASE, GPIO_PIN_2, 0);
#define DATA_SET     GPIOPinWrite(GPIO_PORTQ_BASE, GPIO_PIN_3, GPIO_PIN_3);
#define DATA_CLR     GPIOPinWrite(GPIO_PORTQ_BASE, GPIO_PIN_3, 0);


void AD8369_Init(void);
void AD8369_ControlGain(uint8_t data);
void delay_ms(uint8_t ms);

#endif


