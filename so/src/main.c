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

/**********************
 *	Application Tasks *
 *********************/

/*
 * Example to test tasks priority scheduling
 */

OS_TASK_CREATE(Tarea_1, STACK_SIZE_BYTES, Task1, (void *) 0x11223344, TASK_PRIORITY_HIGH);
OS_TASK_CREATE(Tarea_2, STACK_SIZE_BYTES, Task2, (void *) 0x11223344, TASK_PRIORITY_LOW);
OS_TASK_CREATE(Tarea_3, STACK_SIZE_BYTES, Task3, (void *) 0x11223344, TASK_PRIORITY_MEDIUM);
OS_TASK_CREATE(Tarea_4, STACK_SIZE_BYTES, Task4, (void *) 0x11223344, TASK_PRIORITY_LOW);
OS_TASK_CREATE(Tarea_5, STACK_SIZE_BYTES, Task5, (void *) 0x11223344, TASK_PRIORITY_LOW);
OS_TASK_CREATE(Tarea_6, STACK_SIZE_BYTES, Task6, (void *) 0x11223344, TASK_PRIORITY_HIGH);
OS_TASK_CREATE(Tarea_7, STACK_SIZE_BYTES, Task7, (void *) 0x11223344, TASK_PRIORITY_LOW);
OS_TASK_CREATE(Tarea_8, STACK_SIZE_BYTES, Task8, (void *) 0x11223344, TASK_PRIORITY_MEDIUM);

/************************
 *	Private functions	*
 ***********************/
static void initHardware(void)
{
	// Set up and initialize all required blocks and
    // functions related to the board hardware
	Board_Init();
}
static void * Task1(void *arg)
{
   while(1)
   {
	   os_taskDelay(15);
      //__WFI();
   }

   return NULL;
}
static void * Task2(void *arg)
{
   while(1)
   {
	   //os_taskDelay(15);
      //__WFI();
   }

   return NULL;
}
static void * Task3(void *arg)
{
   while(1)
   {
	   os_taskDelay(15);
      //__WFI();
   }

   return NULL;
}
static void * Task4(void *arg)
{
   while(1)
   {
	   //os_taskDelay(15);
      //__WFI();
   }

   return NULL;
}
static void * Task5(void *arg)
{
   while(1)
   {
	   //os_taskDelay(15);
      //__WFI();
   }

   return NULL;
}
static void * Task6(void *arg)
{
   while(1)
   {
	   os_taskDelay(15);
      //__WFI();
   }

   return NULL;
}
static void * Task7(void *arg)
{
   while(1)
   {
	   //os_taskDelay(15);
      //__WFI();
   }

   return NULL;
}
static void * Task8(void *arg)
{
   while(1)
   {
	   os_taskDelay(15);
      //__WFI();
   }

   return NULL;
}

/************************
 *	Public functions	*
 ***********************/
int main(void)
{
	/* Initialize board hardware */
	initHardware();

	/* Stay forever */
	os_start();
}

