/*
===============================================================================
 Name        : app.c
 Author      : $(author)
 Version     :
 Copyright   : $(copyright)
 Description : main definition
===============================================================================
*/
#include <string.h>
#include "board.h"

/***********************************************
 * Preemptive scheduler.
 *
 * Steps:
 *
 * 1. Save context into stack
 * 2. Use an IRQ handler (Systick)
 *    -> Push registers
 *    -> Save Sp1
 *    -> Save Sp2
 *    -> Pop registers
 *    -> Return from interrupt
 *
 * **********************************************/

/****************
 *	Constants	*
 * *************/
#define STACK_SIZE_BYTES   512

/************
 *	Types	*
 ***********/
typedef void *(*task_type)(void *);

/****************
 *	Variables	*
 ***************/
uint32_t stack1[STACK_SIZE_BYTES/4];
uint32_t stack2[STACK_SIZE_BYTES/4];

uint32_t sp1, sp2;
uint32_t currentTask;

/************************
 *	Private functions	*
 ***********************/
static void initHardware(void)
{
	// Set up and initialize all required blocks and
    // functions related to the board hardware
	Board_Init();

	// Update system core clock rate
	SystemCoreClockUpdate();

	// Initializes the System Timer and its interrupt, and starts the System Tick Timer
	SysTick_Config(SystemCoreClock / 1000);
}

/************************
 *	Public functions	*
 ***********************/
void * Task1(void *arg)
{
   while(1)
   {
      __WFI();
   }

   return NULL;
}

void * Task2(void *arg)
{
   while(1)
   {
      __WFI();
   }

   return NULL;
}

void taskReturnHook(void *ret_val)
{
   while(1)
   {
      __WFI();
   }
}

uint32_t getNextContext(uint32_t currentSp)
{
   uint32_t nextSp;

   switch(currentTask)
   {
      case 0:
         /* Look what to do with the initial context */
         nextSp = sp1;
         currentTask = 1;
         break;
      case 1:
         sp1 = currentSp;
         nextSp = sp2;
         currentTask = 2;
         break;
      case 2:
         sp2 = currentSp;
         nextSp = sp1;
         currentTask = 1;
         break;
      default:
         //HardFault_Handler();
    	  break;
   }

   return nextSp;
}

void initStack(uint32_t stack[],			/*	Pointer to RAM stack section	*/
               uint32_t stackSizeBytes,		/*	RAM stack section size			*/
               uint32_t *sp,				/*	Pointer to stack pointer		*/
               task_type entryPoint,		/*	Pointer to task function		*/
               void *arg)					/*	Pointer to task argument		*/
{
   memset(stack, 0, stackSizeBytes);							/*	Set stack section with zeros	*/
   stack[stackSizeBytes/4 - 1] = 1 << 24;           			/*	xPSR, T = 1  */
   stack[stackSizeBytes/4 - 2] = (uint32_t) entryPoint; 		/*	PC	*/
   stack[stackSizeBytes/4 - 3] = (uint32_t) taskReturnHook;  	/*	LR	*/
   stack[stackSizeBytes/4 - 8] = (uint32_t) arg;         		/*	R0	*/
   stack[stackSizeBytes/4 - 9] = 0xFFFFFFF9;         			/*	LR	*/

   *sp = (uint32_t) &(stack[stackSizeBytes/4 - 17]);    		/* Consider the other 8 registers	*/
}

int main(void)
{
	// Initialize task 1 stack
	initStack(stack1, STACK_SIZE_BYTES, &sp1, Task1, (void *) 0x11223344);

	// Initialize task 2 stack
	initStack(stack2, STACK_SIZE_BYTES, &sp2, Task2, (void *) 0x55667788);

	// Initialize board hardware
	initHardware();

	// Stay forever
	while(1)
		;
}
