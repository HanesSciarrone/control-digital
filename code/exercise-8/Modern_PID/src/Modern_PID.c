/*=============================================================================
 * Author: Hanes <hanessciarrone@gmail.com>
 * Date: 2022/04/30
 * Version: v1.0.0
 *===========================================================================*/

/*=====[Inclusions of function dependencies]=================================*/

#include "Modern_PID.h"

#include "FreeRTOS.h"
#include "FreeRTOSConfig.h"
#include "task.h"

#include "sapi.h"
#include "userTasks.h"

/*=====[Definition macros of private constants]==============================*/

/*=====[Definitions of extern global variables]==============================*/

/*=====[Definitions of public global variables]==============================*/

StackType_t myTaskStack[configMINIMAL_STACK_SIZE];
StaticTask_t myTaskTCB;

/*=====[Definitions of private global variables]=============================*/

/*=====[Main function, program entry point after power on or reset]==========*/

int main( void )
{
   boardInit();
   uartInit(UART_USB, 115200);


   // Create a task in freeRTOS with static memory
   xTaskCreateStatic(
      pidControlTask,                   // Function that implements the task.
      (const char *)"pidControl",   // Text name for the task.
      configMINIMAL_STACK_SIZE, // Stack size in words, not bytes.
      0,                        // Parameter passed into the task.
      tskIDLE_PRIORITY+1,       // Priority at which the task is created.
      myTaskStack,              // Array to use as the task's stack.
      &myTaskTCB                // Variable to hold the task's data structure.
   );

   vTaskStartScheduler(); // Initialize scheduler

   while( true ); // If reach heare it means that the scheduler could not start

   // YOU NEVER REACH HERE, because this program runs directly or on a
   // microcontroller and is not called by any Operating System, as in the 
   // case of a PC program.
   return 0;
}
