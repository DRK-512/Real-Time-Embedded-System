/*
   Sample program 2 - Hello World for Embedded Systems
   Toggle an LED attached to a GPIO pin
     Use UART inputs to control the operating mode of the program
*/

#include <stm32f10x.h>
#include "GPIO_STM32F10x.h"
#include "Board_LED.h"
#include "uart.h"

#define  LED_0        0                    //this is connected to GPIOB pin 8  (PORTB.8)
#define  LED_1        1                    // Pin 9
#define  LED_2        2                    // Pin 10

// Prototype Functions
void delay (uint32_t count);
void SendText(uint8_t *txt);
void USART1_IRQHandler (void);
 
// Calls this from uart.c
volatile uint8_t        inKey;

int main()
{
    
        /* Improved UART code */
        USART1_Init ();//Configure and enable USART1 interrupt
        NVIC->IP[USART1_IRQn] = 0x80;//set priority to 0x80
        //NVIC_SetPriority(USART1_IRQn, 0x80);
        NVIC->ISER[USART1_IRQn/32] = 1UL << (USART1_IRQn%32);//set interrupt enable bit
        //NVIC_EnableIRQ(USART1_IRQn);
        USART1->CR1 |= USART_CR1_RXNEIE;//enable receiver not empty interrupt
    
    
        LED_Initialize();
    
        //send a character to UART for testing
        SendChar('H');            
        SendChar('i');
        SendChar('\n');            //send newline character
    
        //use your function SendText to send a text string to the UART
        SendText((unsigned  char*) "Welcome!\n");    
    
        while (1) {
            //inKey = GetKey();            //get input character from the UART
            
            //include a command here to echo the input you received back to the terminal window
        
            // Normal Mode
            if (inKey == '1') {        
                // Turn on the GREEN LED (while RED and YELLOW are off)
                LED_Off(LED_0);        // Turn off Red Light
                LED_Off(LED_1);        // Turn off Yellow Light
                LED_On(LED_2);        // Turn on Green Light
                
                //Wait for 3000 time units
                delay(3000);
                
                // Turn on the Yellow LED (while RED and Green are off)
                LED_Off(LED_0);        // Turn off Red Light
                LED_Off(LED_2);        // Turn off Green Light
                LED_On(LED_1);        // Turn on Yellow Light
							
                //Wait for 1000 time units
                delay(1000);
                
                // Turn on the Red LED (while Green and YELLOW are off)
                
                LED_Off(LED_1);        // Turn off Yellow Light
								LED_On(LED_0);        // Turn on Red Light
                LED_Off(LED_2);        // Turn off Green Light
                
                //Wait for 3000 time units
                delay(3000);
                
                // Override mode
            } else if (inKey == '0') {
                /* While in this mode only the RED LED blinks on and off
                every 1000 time units(while the YELLOW and GREEN are off). */
                
                LED_On(LED_0);        // Turn on Red Light
                LED_Off(LED_1);        // Turn off Yellow Light
                LED_Off(LED_2);        // Turn off Green Light
                
                delay(1000);
                
                LED_Off(LED_0); 				// Turn off Red Light
							  LED_Off(LED_1);        // Turn off Yellow Light
                LED_Off(LED_2);        // Turn off Green Light
                delay(1000);
							
            } else {
                // Made a third mode for fun, if the mode is not 0 or 1 then all LED are off
                LED_Off(LED_0);        // Turn off Green Light
                LED_Off(LED_1);        // Turn off Yellow Light
                LED_Off(LED_2);        // Turn off Red Light
            }
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

//complete this function for sending a string of characters to the UART
void SendText(uint8_t *text) {
    
    uint8_t i=0;
    
    // every string end in \0
    while(text[i] != '\0') {
        SendChar(text[i]);
        i++;
    }
}