/* 
   Sample program – Hello World for Embedded Systems
   Toggle an LED attached to a GPIO pin
*/

#include <stm32f10x.h> 
#include "GPIO_STM32F10x.h"
#include "Board_LED.h"

#define  LED_0		0					//this is connected to GPIOB pin 8  (PORTB.8)

void delay (uint32_t count);

int main()
{
		LED_Initialize();
		
		while(1) {
			LED_On(LED_0);
			delay(1000);
			LED_Off(LED_0);
			delay(1000);
		}

}	


/*----------------------------------------------------------------------------
  Simple delay routine
*---------------------------------------------------------------------------*/

void delay (uint32_t count)
{
	uint32_t index1, index2;

	for(index1 =0; index1 < count; index1++)
	{
			for (index2 = 0; index2 < 1000; index2++);	
	}
}

