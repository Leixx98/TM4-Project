#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "math.h"

#include "inc/hw_gpio.h"
#include "inc/hw_ints.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "inc/hw_nvic.h"

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

#include "utils/uartstdio.h"
#include "utils/buttons.h"

#include "FreeRTOSConfig.h"
#include "FreeRTOS.h"
#include "queue.h"
#include "task.h"

#include "functions/led_task.h"
#include "functions/uartprint.h"
#include "functions/uartreceive.h"
#include "functions/sort.h"


#define SORT_TASK_PRIO          2        //»ŒŒÒ”≈œ»º∂
#define SORT_STK_SIZE           256       //»ŒŒÒ∂—’ª¥Û–° 
#define Pi                    3.1415926f               

typedef struct fft
{
	uint32_t Value;
	uint8_t Address;
}fft;

fft fft_array[150];
fft fft_xiebo[15];
float Jibo_Amp=0,Erci_Amp=0,Sanci_Amp=0,Sici_Amp=0,Wuci_Amp=0,Liuci_Amp=0,Qici_Amp=0,Baci_Amp=0,Jiuci_Amp=0;
float Jibo_Sta=0,Erci_Sta=0,Sanci_Sta=0,Sici_Sta=0,Wuci_Sta=0,Liuci_Sta=0,Qici_Sta=0,Baci_Sta=0,Jiuci_Sta=0;
/********************************
 *???:swap
 *??:???????
 *??:??????
 *???:?
 ********************************/

void swap(uint32_t *a, uint32_t *b)  
{
    int temp;
 
    temp = *a;
    *a = *b;
    *b = temp;
 
    return ;
}



/************************************
 *???:quicksort
 *??:??????
 *??:
 *???:?
 ************************************/
void quicksort(uint32_t array[], int maxlen, int begin, int end)
{
    int i, j;
 
    if(begin < end)
    {
        i = begin + 1;  // ?array[begin]?????,???array[begin+1]????????!
        j = end;        // array[end]????????
          
        while(i < j)
        {
            if(array[i] > array[begin])  // ??????????????,??????
            {
                swap(&array[i], &array[j]);  // ?????
                j--;
            }
            else
            {
                i++;  // ????????,?????????
            }
        }
 
        /* ??while???,i = j?
         * ????????????  -->  array[begin+1] ~ array[i-1] < array[begin]
         *                           -->  array[i+1] ~ array[end] > array[begin]
         * ???????array??????,??array[i]?array[begin]????,??array[i]????
         * ???array[i]?array[begin]??,???????????!????,????i = j????????!
         */
 
        if(array[i] >= array[begin])  // ???????ì>=î,????????????,?????!
        {
            i--;
        }
 
        swap(&array[begin], &array[i]);  // ??array[i]?array[begin]
        
        quicksort(array, maxlen, begin, i);
        quicksort(array, maxlen, j, end);
    }
}

//*****************************************************************************
//
// ≈≈–Ú»ŒŒÒ
//
//*****************************************************************************
void
DemoSORTTask(void *pvParameters)
{
	uint8_t i=0,j=0;
	uint8_t x='\"',f=0xff;
	uint8_t ampgain=0;
  float frevalue=0,ampvalue=0;
	uint16_t ampvalue_temp=0;
	fft temp;
	while(1)
	{
		if(SORT_Status==SORT_ENABLE)
		{					
			SORT_Status=SORT_DISABLE;	
			
			for(i=4;i<254;i++)
			{
				fft_array[i].Address=i-3;
				fft_array[i].Value=UART_ValueBuffer[i];	
			}
			UARTprintf("------------------------------------\n");
			//∂‘Ω·ππÃÂΩ¯––√∞≈›≈≈–Ú
			for(i=6;i<254;i++)
			{
					for(j=6;j<254-i-1;j++)
					{
						if(fft_array[j].Value>fft_array[j+1].Value)
						{
									temp=fft_array[j];
									fft_array[j]=fft_array[j+1];
									fft_array[j+1]=temp;
						}
					}
			}
				//Ω¯––∆µ¬ ◊™ªª£¨Õ¨ ±∞—∑˘∂»÷µ∫ÕµÁ—π∑˘∂»“‘º∞µÁ—π‘ˆ“ÊÃ·»°≥ˆ¿¥
			  UARTprintf("%d  %d\n",UART_ValueBuffer[255],UART_ValueBuffer[256]);
				frevalue=(float)UART_ValueBuffer[256]*50000000/UART_ValueBuffer[255];
				ampgain=(uint16_t)(UART_ValueBuffer[257]);		
				ampvalue_temp=(uint16_t)(UART_ValueBuffer[257]>>16);
				ampvalue=(float)(ampvalue_temp*5000)/32768;
				if(ampgain==0)
					ampvalue=2.3613f*ampvalue;
				else if(ampgain==1)
					ampvalue=(2.3613f*ampvalue)/10;
				else if(ampgain==2)
					ampvalue=(2.3613f*ampvalue)/100;
				else if(ampgain==3)
					ampvalue=0.00235f*ampvalue;

				//∂‘À˘”– ˝æ›ø™∏˘∫≈
				for(i=0;i<254;i++)
				{
						fft_array[i].Value=sqrt(fft_array[i].Value);
						UART_ValueBuffer[i]=sqrt(UART_ValueBuffer[i]);
//						UARTprintf("%d %d\n",fft_array[i].Address,fft_array[i].Value);
				}
				
				//—∞’“≈º¥Œ–≥≤®,œ»–¥»Î≈≈–Ú∫√µƒ◊Ó¥Ûµƒ«∞ Æ∏ˆ÷µ
				for(i=0;i<12;i++)
				{
					fft_xiebo[i]=fft_array[247-i];
//					UARTprintf("%d  %d\n",fft_xiebo[i].Address,fft_xiebo[i].Value);
				}
							
				printf("t5.txt=%cX%d%c%c%c%c",x,ampgain,x,f,f,f);
				printf("t3.txt=%c%.3fHZ%c%c%c%c",x,frevalue,x,f,f,f);
				printf("t10.txt=%c%.2f%c%c%c%c",x,0.00,x,f,f,f);				
				printf("t12.txt=%c%.2f%c%c%c%c",x,0.00,x,f,f,f);
				printf("t14.txt=%c%.2f%c%c%c%c",x,0.00,x,f,f,f);				
				printf("t16.txt=%c%.2f%c%c%c%c",x,0.00,x,f,f,f);	
				printf("t18.txt=%c%.2f%c%c%c%c",x,0.00,x,f,f,f);				
				printf("t20.txt=%c%.2f%c%c%c%c",x,0.00,x,f,f,f);	
				printf("t22.txt=%c%.2f%c%c%c%c",x,0.00,x,f,f,f);				
				printf("t24.txt=%c%.2f%c%c%c%c",x,0.00,x,f,f,f);		
												
				for(i=0;i<12;i++)
				{
						if(fft_xiebo[i].Address==fft_xiebo[0].Address)
						{
							Jibo_Amp=fft_xiebo[i].Value;
							Jibo_Amp=(float)(Jibo_Amp*5000)/32768;	
							if(ampgain==0)
								Jibo_Amp=2.3613f*Jibo_Amp;
							else if(ampgain==1)
								Jibo_Amp=(2.3613f*Jibo_Amp)/10;
							else if(ampgain==2)
								Jibo_Amp=(2.3613f*Jibo_Amp)/100;
							else if(ampgain==3)
								Jibo_Amp=0.00235f*Jibo_Amp;							
								if(frevalue<151)
								{
										if(frevalue>=80)
										{
											if(Jibo_Amp>4500)
												Jibo_Amp=Jibo_Amp*1.006f;
											else if(Jibo_Amp<4500&&ampvalue>3500)
												Jibo_Amp=Jibo_Amp*1.005f;
											else if(Jibo_Amp<3500&&Jibo_Amp>2500)
												Jibo_Amp=Jibo_Amp*1.003f;		
											else if(ampvalue<2500)
												Jibo_Amp=Jibo_Amp*1.003f;							
										}
										else if(frevalue<80&&frevalue>=49)
										{
												if(Jibo_Amp>4500)
												Jibo_Amp=Jibo_Amp*1.014f;
											else if(Jibo_Amp<4500&&ampvalue>3500)
												Jibo_Amp=Jibo_Amp*1.012f;
											else if(Jibo_Amp<3500&&Jibo_Amp>2500)
												Jibo_Amp=Jibo_Amp*1.009f;		
											else if(ampvalue<2500)
												Jibo_Amp=Jibo_Amp*1.007f;			
										}
									printf("t8.txt=%c%.3fmA%c%c%c%c",x,Jibo_Amp/10,x,f,f,f);
								}
								else
									printf("t8.txt=%c%.3fmA%c%c%c%c",x,Jibo_Amp/10,x,f,f,f);				
								
						}
						else if((fft_xiebo[i].Address>=(fft_xiebo[0].Address*2-5))&&(fft_xiebo[i].Address<=(fft_xiebo[0].Address*2+5)))
						{
							if(Erci_Sta==0)
							{
								Erci_Sta=1;
								Erci_Amp=fft_xiebo[i].Value;
								Erci_Amp=(float)(Erci_Amp*5000)/32768;	
								if(ampgain==0)
									Erci_Amp=2.3613f*Erci_Amp;
								else if(ampgain==1)
									Erci_Amp=(2.3613f*Erci_Amp)/10;
								else if(ampgain==2)
									Erci_Amp=(2.3613f*Erci_Amp)/100;	
							else if(ampgain==3)
								Erci_Amp=0.00235f*Erci_Amp;										
								printf("t10.txt=%c%.3fmA%c%c%c%c",x,Erci_Amp/10,x,f,f,f);
							}
						}
						else if((fft_xiebo[i].Address>=(fft_xiebo[0].Address*3-5))&&(fft_xiebo[i].Address<=(fft_xiebo[0].Address*3+5)))
						{
							if(Sanci_Sta==0)
							{
								Sanci_Sta=1;
								Sanci_Amp=fft_xiebo[i].Value;
								Sanci_Amp=(float)(Sanci_Amp*5000)/32768;	
								if(ampgain==0)
									Sanci_Amp=2.3613f*Sanci_Amp;
								else if(ampgain==1)
									Sanci_Amp=(2.3613f*Sanci_Amp)/10;
								else if(ampgain==2)
									Sanci_Amp=(2.3613f*Sanci_Amp)/100;		
								else if(ampgain==3)
									Sanci_Amp=0.00235f*Sanci_Amp;									
								printf("t12.txt=%c%.3fmA%c%c%c%c",x,Sanci_Amp/10,x,f,f,f);
							}
						}
						else if((fft_xiebo[i].Address>=(fft_xiebo[0].Address*4-5))&&(fft_xiebo[i].Address<=(fft_xiebo[0].Address*4+5)))
						{
							if(Sici_Sta==0)
							{
								Sici_Sta=1;	
								Sici_Amp=fft_xiebo[i].Value;
								Sici_Amp=(float)(Sici_Amp*5000)/32768;	
								if(ampgain==0)
									Sici_Amp=2.3613f*Sici_Amp;
								else if(ampgain==1)
									Sici_Amp=(2.3613f*Sici_Amp)/10;
								else if(ampgain==2)
									Sici_Amp=(2.3613f*Sici_Amp)/100;
								else if(ampgain==3)
									Sici_Amp=0.00235f*Sici_Amp;								
								printf("t14.txt=%c%.3fmA%c%c%c%c",x,Sici_Amp/10,x,f,f,f);	
							}							
						}				
						else if((fft_xiebo[i].Address>=(fft_xiebo[0].Address*5-5))&&(fft_xiebo[i].Address<=(fft_xiebo[0].Address*5+5)))
						{
							if(Wuci_Sta==0)
							{
								Wuci_Sta=1;					
								Wuci_Amp=fft_xiebo[i].Value;
								Wuci_Amp=(float)(Wuci_Amp*5000)/32768;	
								if(ampgain==0)
									Wuci_Amp=2.3613f*Wuci_Amp;
								else if(ampgain==1)
									Wuci_Amp=(2.3613f*Wuci_Amp)/10;
								else if(ampgain==2)
									Wuci_Amp=(2.3613f*Wuci_Amp)/100;
								else if(ampgain==3)
									Wuci_Amp=0.00235f*Wuci_Amp;								
								printf("t16.txt=%c%.3fmA%c%c%c%c",x,Wuci_Amp/10,x,f,f,f);
							}
						}
						else if((fft_xiebo[i].Address>=(fft_xiebo[0].Address*6-6))&&(fft_xiebo[i].Address<=(fft_xiebo[0].Address*6+6)))
						{
							if(Liuci_Sta==0)
							{
								Liuci_Sta=1;					
								Liuci_Amp=fft_xiebo[i].Value;
								Liuci_Amp=(float)(Liuci_Amp*5000)/32768;		
								if(ampgain==0)
									Liuci_Amp=2.3613f*Liuci_Amp;
								else if(ampgain==1)
									Liuci_Amp=(2.3613f*Liuci_Amp)/10;
								else if(ampgain==2)
									Liuci_Amp=(2.3613f*Liuci_Amp)/100;	
								else if(ampgain==3)
									Liuci_Amp=0.00235f*Liuci_Amp;									
								printf("t18.txt=%c%.3fmA%c%c%c%c",x,Liuci_Amp/10,x,f,f,f);			
							}
						}				
						else if((fft_xiebo[i].Address>=(fft_xiebo[0].Address*7-7))&&(fft_xiebo[i].Address<=(fft_xiebo[0].Address*7+7)))
						{
							if(Qici_Sta==0)
							{
								Qici_Sta=1;							
								Qici_Amp=fft_xiebo[i].Value;
								Qici_Amp=(float)(Qici_Amp*5000)/32768;	
								if(ampgain==0)
									Qici_Amp=2.3613f*Qici_Amp;
								else if(ampgain==1)
									Qici_Amp=(2.3613f*Qici_Amp)/10;
								else if(ampgain==2)
									Qici_Amp=(2.3613f*Qici_Amp)/100;		
								else if(ampgain==3)
									Qici_Amp=0.00235f*Qici_Amp;									
								printf("t20.txt=%c%.3fmA%c%c%c%c",x,Qici_Amp/10,x,f,f,f);
							}
						}						
						else if((fft_xiebo[i].Address>=(fft_xiebo[0].Address*8-8))&&(fft_xiebo[i].Address<=(fft_xiebo[0].Address*8+8)))
						{
							if(Baci_Sta==0)
							{
								Baci_Sta=1;							
								Baci_Amp=fft_xiebo[i].Value;
								Baci_Amp=(float)(Baci_Amp*5000)/32768;	
								if(ampgain==0)
									Baci_Amp=2.3613f*Baci_Amp;
								else if(ampgain==1)
									Baci_Amp=(2.3613f*Baci_Amp)/10;
								else if(ampgain==2)
									Baci_Amp=(2.3613f*Baci_Amp)/100;	
								else if(ampgain==3)
									Baci_Amp=0.00235f*Baci_Amp;									
								printf("t22.txt=%c%.3fmA%c%c%c%c",x,Baci_Amp/10,x,f,f,f);	
							}								
						}
						else if((fft_xiebo[i].Address>=(fft_xiebo[0].Address*9-9))&&(fft_xiebo[i].Address<=(fft_xiebo[0].Address*9+9)))
						{
							if(Jiuci_Sta==0)
							{
								Jiuci_Sta=1;							
								Jiuci_Amp=fft_xiebo[i].Value;
								Jiuci_Amp=(float)(Jiuci_Amp*5000)/32768;	
								if(ampgain==0)
									Jiuci_Amp=2.3613f*Jiuci_Amp;
								else if(ampgain==1)
									Jiuci_Amp=(2.3613f*Jiuci_Amp)/10;
								else if(ampgain==2)
									Jiuci_Amp=(2.3613f*Jiuci_Amp)/100;	
								else if(ampgain==3)
									Jiuci_Amp=0.00235f*Jiuci_Amp;										
								printf("t24.txt=%c%.3fmA%c%c%c%c",x,Jiuci_Amp/10,x,f,f,f);
							}
						}						
				}

				if(frevalue<151)
				{
						if(frevalue>=80)
						{
							if(ampvalue>9000)
								ampvalue=ampvalue*1.006f;
							else if(ampvalue<9000&&ampvalue>7000)
								ampvalue=ampvalue*1.005f;
							else if(ampvalue<7000&&ampvalue>5000)
								ampvalue=ampvalue*1.003f;		
							else if(ampvalue<5000)
								ampvalue=ampvalue*1.003f;							
						}
						else if(frevalue<80&&frevalue>=49)
						{
							if(ampvalue>9000)
								ampvalue=ampvalue*1.015f;
							else if(ampvalue<9000&&ampvalue>7000)
								ampvalue=ampvalue*1.013f;
							else if(ampvalue<7000&&ampvalue>5000)
								ampvalue=ampvalue*1.009f;		
							else if(ampvalue<5000)
								ampvalue=ampvalue*1.007f;			
						}
						if(Sanci_Amp<0.1f*Jibo_Amp)
							printf("t1.txt=%c%.3fmA%c%c%c%c",x,ampvalue/10,x,f,f,f);
						else 
							printf("t1.txt=%c%c%c%c%c",x,x,f,f,f);
				}
				else
				{
						if(Sanci_Amp<0.1f*Jibo_Amp)
							printf("t1.txt=%c%.3fmA%c%c%c%c",x,ampvalue/10,x,f,f,f);
						else 
							printf("t1.txt=%c%c%c%c%c",x,x,f,f,f);						
				}
				
			Jibo_Sta=0;Erci_Sta=0;Sanci_Sta=0;Sici_Sta=0;Wuci_Sta=0;Liuci_Sta=0;Qici_Sta=0;Baci_Sta=0;Jiuci_Sta=0;	
			vTaskDelay(100);
		}
	}
}


//*****************************************************************************
//
// ¥¥Ω® ˝æ›≈≈–Ú»ŒŒÒ
//
//*****************************************************************************
void
SORTTaskInit(void)
{
    //
    // Create the fpgadrawline task.
    //
    xTaskCreate(
                        DemoSORTTask,                       //»ŒŒÒ∫Ø ˝
                       (const portCHAR *)"SORT", //»ŒŒÒ√˚≥∆ 
                        SORT_STK_SIZE,              // »ŒŒÒ∂—’ª¥Û–° 
                        NULL,       
                        SORT_TASK_PRIO,
                        NULL
    );
    
    
}


