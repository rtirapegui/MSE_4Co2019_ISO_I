/*
===============================================================================
 Name        : main.c
 Author      : $(Rodrigo Tirapegui)
 Version     :
 Copyright   : $(copyright)
 Description :
===============================================================================
*/
#include <string.h>
#include "board.h"
#include "os.h"

/****************
 *	Constants	*
 * *************/
#define STACK_SIZE_BYTES   512

/****************
 *	Variables	*
 ***************/
uint32_t stack1[STACK_SIZE_BYTES/4];
uint32_t stack2[STACK_SIZE_BYTES/4];
uint32_t stack3[STACK_SIZE_BYTES/4];
uint32_t stack4[STACK_SIZE_BYTES/4];
uint32_t stack5[STACK_SIZE_BYTES/4];
uint32_t stack6[STACK_SIZE_BYTES/4];
uint32_t stack7[STACK_SIZE_BYTES/4];
uint32_t stack8[STACK_SIZE_BYTES/4];

uint32_t sp1, sp2, sp3, sp4, sp5, sp6, sp7, sp8;

/************************
 *	Private functions	*
 ***********************/
static void initHardware(void)
{
	// Set up and initialize all required blocks and
    // functions related to the board hardware
	Board_Init();
}

/************************
 *	Public functions	*
 ***********************/
void * Task1(void *arg)
{
   while(1)
   {
	   os_delay(10);
      __WFI();
   }

   return NULL;
}
void * Task2(void *arg)
{
   while(1)
   {
	   //os_delay(10);
      __WFI();
   }

   return NULL;
}
void * Task3(void *arg)
{
   while(1)
   {
	   //os_delay(10);
      __WFI();
   }

   return NULL;
}
void * Task4(void *arg)
{
   while(1)
   {
	   //os_delay(10);
      __WFI();
   }

   return NULL;
}
void * Task5(void *arg)
{
   while(1)
   {
	   //os_delay(10);
      __WFI();
   }

   return NULL;
}
void * Task6(void *arg)
{
   while(1)
   {
	   os_delay(15);
      __WFI();
   }

   return NULL;
}
void * Task7(void *arg)
{
   while(1)
   {
	   //os_delay(10);
      __WFI();
   }

   return NULL;
}
void * Task8(void *arg)
{
   while(1)
   {
	   //os_delay(10);
      __WFI();
   }

   return NULL;
}
int main(void)
{
	/* Initialize board hardware */
	initHardware();

	/*
	 * Example to test tasks priority scheduling
	 */

	/* Initialize task 1 stack */
	os_initTaskStack(stack1, STACK_SIZE_BYTES, &sp1, Task1, (void *) 0x11223344, TASK_PRIORITY_HIGH);

	/* Initialize task 2 stack */
	os_initTaskStack(stack2, STACK_SIZE_BYTES, &sp2, Task2, (void *) 0x55667788, TASK_PRIORITY_LOW);

	/* Initialize task 2 stack */
	os_initTaskStack(stack3, STACK_SIZE_BYTES, &sp3, Task3, (void *) 0x55667788, TASK_PRIORITY_MEDIUM);

	/* Initialize task 2 stack */
	os_initTaskStack(stack4, STACK_SIZE_BYTES, &sp4, Task4, (void *) 0x55667788, TASK_PRIORITY_LOW);

	/* Initialize task 2 stack */
	os_initTaskStack(stack5, STACK_SIZE_BYTES, &sp5, Task5, (void *) 0x55667788, TASK_PRIORITY_LOW);

	/* Initialize task 2 stack */
	os_initTaskStack(stack6, STACK_SIZE_BYTES, &sp6, Task6, (void *) 0x55667788, TASK_PRIORITY_HIGH);

	/* Initialize task 2 stack */
	os_initTaskStack(stack7, STACK_SIZE_BYTES, &sp7, Task7, (void *) 0x55667788, TASK_PRIORITY_LOW);

	/* Initialize task 2 stack */
	os_initTaskStack(stack8, STACK_SIZE_BYTES, &sp8, Task8, (void *) 0x55667788, TASK_PRIORITY_MEDIUM);

	/* Stay forever */
	os_start();
}

