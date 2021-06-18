/*----------------------------------------------------------------------------
 * CMSIS-RTOS 'main' function template
 *---------------------------------------------------------------------------*/

#define osObjectsPublic                     // define objects in main module
#include "osObjects.h"                      // RTOS object definitions

extern int Init_Thread (void);//Addthis line 

osThreadId main_id;   //create the thread handle for the main function

osThreadId thread1_id;//thread handle
void  thread1(void const *argument);//function prototype of thread1
osThreadDef(thread1, osPriorityNormal, 1, 0);//thread definition structure


/*
 * main: initialize and start the system
 * keep in mind main is also a thread
 */
int main (void) {
	
	main_id = osThreadGetId (); // great the main thread ID
  osKernelInitialize ();                   
	Init_Thread (); // initialize the thread
	osKernelStart ();  

	thread1_id = osThreadCreate(osThread(thread1), NULL);
  
}
