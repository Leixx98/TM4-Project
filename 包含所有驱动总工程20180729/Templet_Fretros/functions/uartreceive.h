#ifndef __UARTRECEIVE_H__
#define __UARTRECEIVE_H__

#define SORT_ENABLE    0x01
#define SORT_DISABLE   0x00
#define FPGA_DrawLineEnable  0x02
#define FPGA_DrawLineDisable  0x03


extern uint8_t SORT_Status;                             //排序标志位
extern uint8_t UART_ReceiveBuffer[6500];                //UART2串口接收数组
extern uint32_t UART_ValueBuffer[300];                   //储存合并后的值的数组 
extern uint8_t FPGA_DrawLine;                         //画图标志位


void UARTRECEIVETaskInit(void);

#endif



