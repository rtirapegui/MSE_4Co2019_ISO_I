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

uint32_t sp1, sp2;

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
	   os_delay(1000);
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

int main(void)
{
	/* Initialize board hardware */
	initHardware();

	/* Initialize task 1 stack */
	os_initTaskStack(stack1, STACK_SIZE_BYTES, &sp1, Task1, (void *) 0x11223344);

	/* Initialize task 2 stack */
	os_initTaskStack(stack2, STACK_SIZE_BYTES, &sp2, Task2, (void *) 0x55667788);

	/* Stay forever */
	os_start();
}

