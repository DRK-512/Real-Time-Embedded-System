/*----------------------------------------------------------------------------
	
	Designers Guide to the Cortex-M Family
	CMSIS RTOS Signal Example

*----------------------------------------------------------------------------*/
#include "stm32f10x.h"
#include <cmsis_os.h>
#include "Board_LED.h"

void led_Thread1 (void const *argument);
void led_Thread2 (void const *argument);

osThreadDef(led_Thread1, osPriorityNormal, 1, 0);
osThreadDef(led_Thread2, osPriorityNormal, 1, 0);

osThreadId T_led_ID1;
osThreadId T_led_ID2;

void delay (void)
{
unsigned int index;
const unsigned int count = 1000000;
	for(index =0;index<count;index++)
	{
		;
	}
}
/*----------------------------------------------------------------------------
  Flash LED 1 when signaled by the led_Thread2
 *---------------------------------------------------------------------------*/
void led_Thread1 (void const *argument) 
{
	for (;;) 
	{
		osSignalWait (0x00,osWaitForever);
		LED_On(1);                          
		osSignalWait (0x00,osWaitForever);	
		LED_Off(1);
	}
}
/*----------------------------------------------------------------------------
  Flash LED two and synchronise the flashing of LED 1 by setting a signal flag
 *---------------------------------------------------------------------------*/
void led_Thread2 (void const *argument) 
{
	for (;;) 
	{
		LED_On(2);		
		osSignalSet	(T_led_ID1,0x01);
		delay();
		LED_Off(2);
		osSignalSet	(T_led_ID1,0x02);
		delay();
	}
}




/*----------------------------------------------------------------------------
  Start the threads
 *---------------------------------------------------------------------------*/
int main (void) 
{
	osKernelInitialize ();                    // initialize CMSIS-RTOS
	
	LED_Initialize();
	T_led_ID2 = osThreadCreate(osThread(led_Thread2), NULL);
	T_led_ID1 = osThreadCreate(osThread(led_Thread1), NULL);	
	
	osKernelStart ();                         // start thread execution 
}
