/*----------------------------------------------------------------------------
	
	Designers Guide to the Cortex-M Family
	CMSIS RTOS Message Queue Example

*----------------------------------------------------------------------------*/
#include "STM32F10x.h"
#include "cmsis_os.h"
#include "uart.h"
#include "Board_LED.h"

//define the message queue for the IRQ
osMessageQId IRQ1;												        
osMessageQDef (IRQ1,0x16,unsigned char);

osMessageQId IRQ2;												        
osMessageQDef (IRQ2,0x16,unsigned char);

osMessageQId IRQ3;												        
osMessageQDef (IRQ3,0x16,unsigned char);

//define the message queues for UART
osMessageQId UART1toUART2;												
osMessageQDef (UART1toUART2,0x32,unsigned char);

osMessageQId UART2toUART1;												
osMessageQDef (UART2toUART1,0x32,unsigned char);

osMessageQId UART1toUART3;												
osMessageQDef (UART1toUART3,0x32,unsigned char);

osMessageQId UART2toUART3;												
osMessageQDef (UART2toUART3,0x32,unsigned char);

// osEvents to store results of UART 
osEvent result;
osEvent resultUART1;
osEvent resultUART2;
osEvent resultUART3; 

// Calls these from uart.c ofr GetKey
//uint8_t        inKey;				// input charecter itself

// Used to detemine when to send messages to UART3
uint8_t uart1LogSend; 
uint8_t uart2LogSend; 

// Used for max number of charecters
const uint8_t maxCharecters = 50; 
uint8_t numChar1;  
uint8_t numChar2; 

// thread initalization
void person1_Thread (void const *argument);																
void person2_Thread (void const *argument);

// prototype functions 
void SendTextToUART(uint8_t *text, uint8_t UartNum); 
void UART_Terminal_Config(void); 

// IRQ handles
void USART1_IRQHandler (void);
void USART2_IRQHandler (void);
void USART3_IRQHandler (void);

osThreadDef(person1_Thread, osPriorityNormal, 1, 0);
osThreadDef(person2_Thread, osPriorityNormal, 1, 0);

osThreadId T_person1_Thread;
osThreadId T_person2_Thread;
osThreadId T_main; 

osMutexId uart_mutex;
osMutexDef(uart_mutex);

/*-----------------------------------------------------------------------------
USART IRQ Handlers
The hardware automatically clears the interrupt flag, once the ISR is entered
*----------------------------------------------------------------------------*/

// USART1 IRQ
void USART1_IRQHandler (void) {
	uint8_t intKey = (int8_t) (USART1->DR & 0x1FF);
	osMessagePut(IRQ1,intKey,0);
}
// USART2 IRQ
void USART2_IRQHandler (void) {
	uint8_t intKey = (int8_t) (USART2->DR & 0x1FF);
  osMessagePut(IRQ2,intKey,0);
}
// USART3 IRQ
void USART3_IRQHandler (void) {

	uint8_t intKey = (int8_t) (USART3->DR & 0x1FF);
	osMessagePut(IRQ3,intKey,0); 
}

/*----------------------------------------------------------------------------
 Person 1 Thread
 *---------------------------------------------------------------------------*/
void person1_Thread (void const *argument) 
{
	for (;;) 
	{
			// Wait for IRQ
			result = osMessageGet(IRQ1,osWaitForever);
			osMutexWait(uart_mutex, osWaitForever);
			
		  if(result.value.v == 0x01){

					// Message has been sent from UART 2
					SendTextToUART((unsigned char*)"\n------------------------------", 1);
				  SendTextToUART((unsigned char*)"\nIncomming Text:\n", 1);
				  resultUART1 = osMessageGet(UART2toUART1,0);
					
					while(resultUART1.value.v != NULL){
						SendChar(resultUART1.value.v, 1);
						resultUART1 = osMessageGet(UART2toUART1,0);
					}
					SendTextToUART((unsigned char*)"\n------------------------------\n\n", 1);
					
		//If 'Enter' is pressed, send mail to UART 2 & start new line
		} else if(result.value.v == 0x0D){
				// now there are 0 charecters in UART1
				numChar1 = 0; 
				// Setup log terminal for next send
				uart1LogSend = 0; 
				
				//Start new line
				SendChar('\n', 1);
				SendChar('\n', 1);
				
				//Send to UART2 and UART3
				osMessagePut(IRQ2,0x02,0);
				osMessagePut(IRQ3,0x03,0);
				
			// If 'Backspace' is pressed
			}  else if(result.value.v == 0x08) {
					// go back, hit space, go back
					SendChar('\b', 1);
					SendChar(0x20, 1);
					SendChar('\b', 1);
					
					// delete charecter from the other UART messages
					osMessagePut(UART1toUART2,'\b',0);
					osMessagePut(UART1toUART2,0x20,0);
					osMessagePut(UART1toUART2,'\b',0);
					
					osMessagePut(UART1toUART3,'\b',0);
					osMessagePut(UART1toUART3,0x20,0);
					osMessagePut(UART1toUART3,'\b',0);
					
					// we now have one less char
					numChar1--; 
			
			} else{//publish the character in UART1 for the user
				
					// max charecter detection
					if(numChar1>=maxCharecters) {
						SendTextToUART((unsigned char*)"\n------------------------------\n", 1);
						SendTextToUART((unsigned char*)"ERROR: Exceed max char length\n", 1);
						SendTextToUART((unsigned char*)"No more charecters can be sent\n", 1);
						SendTextToUART((unsigned char*)"Please press ENTER\n", 1);
						SendTextToUART((unsigned char*)"------------------------------\n", 1);
					} else {
						// insert another charecter
						numChar1++; 
				
						// we only want this sent once, but the rest to be sent as a message
						if(uart1LogSend==0){
							SendTextToUART((unsigned char*)"\nPerson 1: ", 3); 
							uart1LogSend++; 
						}
					
						SendChar(result.value.v, 1);
						SendChar(result.value.v, 3);
					
						//Save Character to send to UART2
						osMessagePut(UART1toUART2,result.value.v,0);
						osMessagePut(UART1toUART3,result.value.v,0);
				}
			}
		osMutexRelease(uart_mutex);
	}
}

/*----------------------------------------------------------------------------
  Person 2 Thread
 *---------------------------------------------------------------------------*/
void person2_Thread (void const *argument) {
	for (;;)  {
		  // Wait for IRQ
			result = osMessageGet(IRQ2,osWaitForever);
			osMutexWait(uart_mutex, osWaitForever);
			
		  if(result.value.v == 0x02){
					
					// Message has been sent from UART 1
				  SendTextToUART((unsigned char*)"\n------------------------------\n", 2);
				  SendTextToUART((unsigned char*)"Incomming Text:\n", 2);
				
				  resultUART2 = osMessageGet(UART1toUART2,0);
					while(resultUART2.value.v != NULL){
						SendChar(resultUART2.value.v, 2);
						resultUART2 = osMessageGet(UART1toUART2,0);
					}
					SendTextToUART((unsigned char*)"\n------------------------------\n\n", 2);
			
			//If 'Enter' is pressed, send mail to UART 2 & start new line
			} else if(result.value.v == 0x0D){
				
				// now there are 0 charecters in UART2
				numChar2 = 0; 
				// Setup log terminal for next send
				uart2LogSend = 0; 
				
				//Start new line
				SendChar('\n', 2);
				SendChar('\n', 2);
				
				//Send to UART1 and UART3
				osMessagePut(IRQ1,0x01,0);
				osMessagePut(IRQ3,0x03,0);
				
		  // If 'Backspace' is pressed
			} else if(result.value.v == 0x08) {
				// go back, hit space, go back
				SendChar('\b', 2);
				SendChar(0x20, 2);
				SendChar('\b', 2);

				// delete charecter from the other UART messages
				osMessagePut(UART2toUART1,'\b',0);
				osMessagePut(UART2toUART1,0x20,0);
				osMessagePut(UART2toUART1,'\b',0);
				
				osMessagePut(UART2toUART3,'\b',0);
				osMessagePut(UART2toUART3,0x20,0);
				osMessagePut(UART2toUART3,'\b',0);
				
				// we now have one less char
				numChar2--; 
			
		  } else {//publish the character in UART1 for the user
				
				// max charecter detection
				if(numChar2>=maxCharecters) {
						SendTextToUART((unsigned char*)"\n------------------------------\n", 2);
						SendTextToUART((unsigned char*)"ERROR: Exceed max char length\n", 2);
						SendTextToUART((unsigned char*)"No more charecters can be sent\n", 2);
						SendTextToUART((unsigned char*)"Please press ENTER\n", 2);
						SendTextToUART((unsigned char*)"------------------------------\n", 2);
					} else {
						// insert another charecter
						numChar2++; 
					
						// we only want this sent once, but the rest to be sent as a message
						if(uart2LogSend==0){
							SendTextToUART((unsigned char*)"\nPerson 2: ", 3); 
							uart2LogSend++; 
						}
					
						SendChar(result.value.v, 2);
						SendChar(result.value.v, 3);
					
						//Save Character to send to UART2
						osMessagePut(UART2toUART1,result.value.v,0);
						osMessagePut(UART2toUART3,result.value.v,0);
					}
				}
		osMutexRelease(uart_mutex);
	}
}

void UART_Terminal_Config() {
	
	// initalize UART terminal
	USART1_Init ();						
	USART2_Init ();					
	USART3_Init ();						
	
}
/*----------------------------------------------------------------------------
  Main: Initialize and start RTX Kernel
 *---------------------------------------------------------------------------*/
int main (void)  {
	
	// initialize CMSIS-RTOS
	osKernelInitialize ();       

  // These will be used for sending messages to the log file	
	uart1LogSend = 0; 
  uart2LogSend = 0; 
	
	// used for max input charecter
	numChar1 = 0; 
	numChar2 = 0; 
	
	// The following code is from lab 3, and is used for user inout via UART
	uart_mutex = osMutexCreate(osMutex(uart_mutex));	
	UART_Terminal_Config(); 
	
	//create the message queues for the interupts
	IRQ1 = osMessageCreate(osMessageQ(IRQ1),NULL);					
	IRQ2 = osMessageCreate(osMessageQ(IRQ2),NULL);					
	IRQ3 = osMessageCreate(osMessageQ(IRQ3),NULL);					
 
	// Create message queues for messages
	UART1toUART2 = osMessageCreate(osMessageQ(UART1toUART2),NULL);					
	UART2toUART1 = osMessageCreate(osMessageQ(UART2toUART1),NULL);					
	
	UART1toUART3 = osMessageCreate(osMessageQ(UART1toUART3),NULL);					
	UART2toUART3 = osMessageCreate(osMessageQ(UART2toUART3),NULL);
	
	// create threads
	T_person1_Thread =	osThreadCreate(osThread(person1_Thread), NULL);
	T_person2_Thread = osThreadCreate(osThread(person2_Thread), NULL);
	
	osKernelStart ();                         						// start thread execution 
}

//send a string of characters to the UART1
void SendTextToUART(uint8_t *text, uint8_t UartNum) {
	uint8_t i=0;
	switch(UartNum) {
		//send a string of characters to the UART1
		case 1:
			// every string end in \0
      while(text[i] != '\0') {
        SendChar(text[i], 1);
        i++;
			}
		break; 
		//send a string of characters to the UART2
		case 2:
			// every string end in \0
			while(text[i] != '\0') {
        SendChar(text[i], 2);
        i++;
			}
		break; 
		//send a string of characters to the UART3
		case 3:
			// every string end in \0
			while(text[i] != '\0') {
        SendChar(text[i], 3);
        i++;
			}
		break; 
		
	}      
}
