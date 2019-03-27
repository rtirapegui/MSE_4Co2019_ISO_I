/*
 ===============================================================================
 Name        : os.c
 Author      : $(Rodrigo Tirapegui)
 Version     :
 Copyright   : $(copyright)
 Description :
===============================================================================
*/

#include <string.h>
#include "conf_os.h"
#include "os.h"
#include "board.h"

/****************
 *	Constants	*
 * *************/
#define EXC_RETURN						0xFFFFFFF9					/* Exception return direction */
#define INVALID_TASK_INDEX				(1 + OS_USER_TASKS_COUNT)	/* Invalid task index as total tasks count */
#define IDLE_TASK_INDEX					0							/* Idle task is set as 0 index task */
#define IDLE_TASKS_STACK_SIZE_BYTES		256							/* Idle task size in bytes */
#define IDLE_TASK_ARGUMENT				0x00000000					/* Idle task received argument */

/************
 *	Types	*
 ***********/
/* Tasks states list */
typedef enum
{
	TASK_STATE_READY
,	TASK_STATE_RUNNING
,	TASK_STATE_BLOCKED
} taskState_t;

/* Internal control struct for os tasks */
typedef struct
{
	uint32_t *	sp;
	taskState_t	state;
	uint32_t	blockedCounter;
} taskControl_t;

/****************
 *	Variables	*
 ***************/
/* Current task index */
static uint32_t g_currentTaskIndex = INVALID_TASK_INDEX;

/* Stack of idle task */
static uint32_t g_idleTaskStack[IDLE_TASKS_STACK_SIZE_BYTES/4];

/* Stack pointer of idle task */
static uint32_t g_idleTaskSp;

/* Tasks control array. Is idle task + user defined tasks */
static taskControl_t g_controlTaskArr[1 + OS_USER_TASKS_COUNT] =
		{
			{&g_idleTaskSp, TASK_STATE_READY, 0 }
		};

/***********************
 *	External functions	*
 ***********************/
extern void SysTick_Handler(void);

/************************
 *	Private functions	*
 ***********************/
static void returnHookTask(void *ret_val)
{
   while(1)
   {
      __WFI();
   }
}

static void * idleTask(void *ret_val)
{
   while(1)
   {
      __WFI();
   }

   return NULL;
}

static void initTaskStack(uint32_t stack[],			/*	Pointer to RAM stack section	*/
 	   	  	  	  	  	  uint32_t stackSizeBytes,	/*	RAM stack section size			*/
						  uint32_t *sp,				/*	Stack pointer					*/
						  task_t entryPoint,		/*	Pointer to task function		*/
						  void *arg)				/*	Pointer to task argument		*/
{
	memset(stack, 0, stackSizeBytes);							/*	Set stack section with zeros	*/
	stack[stackSizeBytes/4 - 1] = 1 << 24;           			/*	xPSR, T = 1  */
	stack[stackSizeBytes/4 - 2] = (uint32_t) entryPoint; 		/*	PC	*/
	stack[stackSizeBytes/4 - 3] = (uint32_t) returnHookTask;  	/*	LR	*/
	stack[stackSizeBytes/4 - 8] = (uint32_t) arg;         		/*	R0	*/
	stack[stackSizeBytes/4 - 9] = EXC_RETURN;         			/*	LR	*/

	*sp = (uint32_t) &(stack[stackSizeBytes/4 - 17]);    		/* Consider the other 8 registers	*/
}

/************************
 *	Kernel functions	*
 ***********************/
void os_updateBlockedCounter(void)
{
	/* Update blocked tasks counters */
	for(uint32_t i = 0;i < INVALID_TASK_INDEX;i++)
	{
		if(TASK_STATE_BLOCKED == g_controlTaskArr[i].state)
		{
			if(g_controlTaskArr[i].blockedCounter)
			{
				--g_controlTaskArr[i].blockedCounter;

				/* If blocked task counter equals 0, set task state to READY */
				if(0 == g_controlTaskArr[i].blockedCounter)
					g_controlTaskArr[i].state = TASK_STATE_READY;
			}
		}
	}
}

uint32_t os_getNextContext(uint32_t currentSp)
{
   uint32_t nextSp;

   if(INVALID_TASK_INDEX == g_currentTaskIndex)
   {
	   /* Set currentTaskIndex as IDLE_TASK_INDEX */
	   g_currentTaskIndex = IDLE_TASK_INDEX;

	   /* Set Idle task state as RUNNING */
	   g_controlTaskArr[IDLE_TASK_INDEX].state = TASK_STATE_RUNNING;

	   /* Set Idle task as current task */
	   nextSp = *g_controlTaskArr[IDLE_TASK_INDEX].sp;
   }
   else if(INVALID_TASK_INDEX > g_currentTaskIndex)
   {
	   uint32_t loopCnt = (1 + OS_USER_TASKS_COUNT);

	   /* Save current task context */
	   *g_controlTaskArr[g_currentTaskIndex].sp = currentSp;

	   /* If running, set current task state as READY */
	   if(TASK_STATE_RUNNING == g_controlTaskArr[g_currentTaskIndex].state)
		   g_controlTaskArr[g_currentTaskIndex].state = TASK_STATE_READY;

	   /* Find next READY_STATE task */
	   while(loopCnt && --loopCnt)
	   {
		   /* Increment current task index */
		   g_currentTaskIndex = (g_currentTaskIndex + 1)%INVALID_TASK_INDEX;

		   /* Find next ready task */
		   if(TASK_STATE_READY == g_controlTaskArr[g_currentTaskIndex].state)
		   {
			   nextSp = *g_controlTaskArr[g_currentTaskIndex].sp;
			   break;
		   }
	   }
   }

   return nextSp;
}

/************************
 *	Public functions	*
 ***********************/
void os_start(void)
{
	/* Initialize Idle task control data */
	initTaskStack(g_idleTaskStack, IDLE_TASKS_STACK_SIZE_BYTES, &g_idleTaskSp, idleTask, IDLE_TASK_ARGUMENT);

	/* Update system core clock rate */
	SystemCoreClockUpdate();

	/* Initializes the System Timer and its interrupt, and starts the System Tick Timer */
	SysTick_Config(SystemCoreClock / 1000);

	while(1)
	{
		__WFI();
	}
}

void os_initTaskStack(uint32_t stack[],			/*	Pointer to RAM stack section	*/
               	   	  uint32_t stackSizeBytes,	/*	RAM stack section size			*/
					  uint32_t *sp,				/*	Stack pointer					*/
					  task_t entryPoint,		/*	Pointer to task function		*/
					  void *arg)				/*	Pointer to task argument		*/
{
   static uint32_t userTaskIndex = 0;

   if(userTaskIndex < OS_USER_TASKS_COUNT)
   {
	   /* Initialize user task stack */
	   initTaskStack(stack, stackSizeBytes, sp, entryPoint, arg);

	   /* Increment user task index */
	   userTaskIndex++;

	   /* Save user task stack pointer in control array */
	   g_controlTaskArr[userTaskIndex].sp = sp;

	   /* Set user task state as OS_TASK_STATE_READY */
	   g_controlTaskArr[userTaskIndex].state = TASK_STATE_READY;
   }
}

void os_delay(uint32_t milliseconds)
{
	/* Validate current task is a valid task */
	if((0 != milliseconds) && (INVALID_TASK_INDEX > g_currentTaskIndex))
	{
		/* Set task state as blocked */
		g_controlTaskArr[g_currentTaskIndex].state = TASK_STATE_BLOCKED;

		/* Set task blocked counter */
		g_controlTaskArr[g_currentTaskIndex].blockedCounter = milliseconds;

		/* Wait until blocked counter equals 0 */
		while(0 < g_controlTaskArr[g_currentTaskIndex].blockedCounter)
		{
			__WFI();
		}
	}
}
