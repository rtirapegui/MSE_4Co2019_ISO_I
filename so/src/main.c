/*
===============================================================================
 Name        : main.c
 Author      : $(Rodrigo Tirapegui)
 Version     :
 Copyright   : $(copyright)
 Description :
===============================================================================
*/

#include <stdio.h>
#include <stdlib.h>
#include "chip.h"
#include "board.h"
#include "os.h"

/****************
 *	Constants	*
 * *************/
#define STACK_SIZE_BYTES   	512
#define BUFFER_SIZE_BYTES	128

/**********
 *	Types *
 *********/

/**********************
 *	Application Tasks *
 *********************/

/*
 * Ejercicio 2
 */

/**************
 * Variables *
 *************/
static int16_t g_Buffer_A[BUFFER_SIZE_BYTES];
static int16_t g_Buffer_B[BUFFER_SIZE_BYTES];
static int32_t g_Buffer_Resultado[BUFFER_SIZE_BYTES];

static int16_t * g_feedBuffer;
static int16_t * g_operateBuffer;

OS_TASK_CREATE(Tarea_1, STACK_SIZE_BYTES, ISRSimulation_Task, NULL, TASK_PRIORITY_IDLE + 2);
OS_TASK_CREATE(Tarea_2, STACK_SIZE_BYTES, FeedBuffer_Task, (void *) &g_feedBuffer, TASK_PRIORITY_IDLE + 1);
OS_TASK_CREATE(Tarea_3, STACK_SIZE_BYTES, OperateBuffer_Task, (void *) &g_operateBuffer, TASK_PRIORITY_IDLE + 1);

OS_SEMPHR_CREATE(Sem_ISR_Feed_Task);
OS_SEMPHR_CREATE(Sem_ISR_Operate_Task);

/************************
 *	Private functions	*
 ***********************/
static void initHardware(void)
{
	// Set up and initialize all required blocks and
    // functions related to the board hardware
	Board_Init();
}

/**********
 * Tasks *
 *********/
static void * ISRSimulation_Task(void *arg)
{
	#define TICK_INCREMENT	6	// In ms
	tick_t startTick = os_task_getTickCount();

	bool feedBuffA_OperateBuffB_Flag = true;

	while(true)
	{
		// Toggle buffer to feed and operate
		if(true == feedBuffA_OperateBuffB_Flag)
		{
			g_feedBuffer = g_Buffer_A;
			g_operateBuffer = g_Buffer_B;
		}
		else
		{
			g_feedBuffer = g_Buffer_B;
			g_operateBuffer = g_Buffer_A;
		}

		// Toggle buffers
		feedBuffA_OperateBuffB_Flag = !feedBuffA_OperateBuffB_Flag;

		// Post feed task
		os_semphr_post(Sem_ISR_Feed_Task);

		// Post operate task
		os_semphr_post(Sem_ISR_Operate_Task);

		// Wait until timeout
		os_task_delayUntil(&startTick, TICK_INCREMENT / OS_TICK_PERIOD_MS);
	}

	return NULL;
}

static void * FeedBuffer_Task(void *arg)
{
	while(true)
	{
		if(true == os_semphr_wait(Sem_ISR_Feed_Task, OS_TICK_MAX))
		{
			int16_t * buffer = (int16_t *)(*(uint32_t *)(arg));

			for(uint32_t buffIndex = 0;buffIndex < BUFFER_SIZE_BYTES;buffIndex++)
			{
				// Feed buffer
				buffer[buffIndex] = (int16_t) (rand() & 0xFFFF);
			}
		}
	}

	return NULL;
}

static void * OperateBuffer_Task(void *arg)
{
	while(true)
	{
		if(true == os_semphr_wait(Sem_ISR_Operate_Task, OS_TICK_MAX))
		{
			int16_t * buffer = (int16_t *)(*(uint32_t *)(arg));

			for(uint32_t buffIndex = 0;buffIndex < BUFFER_SIZE_BYTES;buffIndex++)
			{
				// Operate buffer buffer
				g_Buffer_Resultado[buffIndex] = ((int32_t) buffer[buffIndex] * rand());
			}
		}
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

