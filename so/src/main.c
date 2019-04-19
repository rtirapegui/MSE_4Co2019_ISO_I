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
 * Ejercicio 1
 */

OS_TASK_CREATE(Tarea_1, STACK_SIZE_BYTES, buttonSampling_Task, (void *) 0x11223344, TASK_PRIORITY_IDLE + 1);
OS_TASK_CREATE(Tarea_2, STACK_SIZE_BYTES, ledBlink_Task, (void *) 0x11223344, TASK_PRIORITY_IDLE + 2);

OS_SEMPHR_CREATE(Sem_T1_T2);

/*************
 * Variables *
 ************/
static tick_t g_samplingTickCount;

/*********
 * Tasks *
 ********/
void * buttonSampling_Task(void * params)
{
   #define DEBOUNCE  40 // In ms

   tick_t t0, t1, diff;

   while(true)
   {
      if(true == Board_BUTTON_isPressed(TEC_1))
      {
         t0 = os_task_getTickCount();

         while(true == Board_BUTTON_isPressed(TEC_1))
            ;

         t1 = os_task_getTickCount();

         diff = (t1 > t0 ? t1 - t0 : 0xFFFFFFFF - t0 + t1);

         if((DEBOUNCE / OS_TICK_PERIOD_MS) < diff)
         {
        	 /* @todo lock with mutex here */
        	 g_samplingTickCount = diff;
        	 /* @todo unlock with mutex here */

        	 os_semphr_post(Sem_T1_T2);
         }
      }
   }

   return NULL;
}

void * ledBlink_Task(void * params)
{
   tick_t samplingTickCount;

   while(true)
   {
	 if(true == os_semphr_wait(Sem_T1_T2, OS_TICK_MAX))
	 {
		/* @todo lock with mutex here */
		samplingTickCount = g_samplingTickCount;
		/* @todo unlock with mutex here */

		if(0 < samplingTickCount)
		{
			Board_LED_Set(LED_BLUE, true);
			os_task_delay(samplingTickCount / OS_TICK_PERIOD_MS);
			Board_LED_Set(LED_BLUE, false);
		}
	 }
   }

   return NULL;
}

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
int main(void)
{
	/* Initialize board hardware */
	initHardware();

	/* Stay forever */
	os_start();
}

