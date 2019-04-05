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
#define OS_TASKS_COUNT				    (1 + OS_USER_TASKS_COUNT)	/* Invalid task index as total tasks count */
#define IDLE_TASK_INDEX					0							/* Idle task is set as 0 index task. Do not change */
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
typedef struct taskControl_t
{
	/* Internal data */
	struct taskControl_t *next;

	/* Task data */
	uint32_t *		sp;
	taskState_t		state;
	taskPriority_t	priority;
	uint32_t		blockedCounter;
} taskControl_t;

/****************
 *	Variables	*
 ***************/
/* Stack of idle task */
static uint32_t g_idleTaskStack[IDLE_TASKS_STACK_SIZE_BYTES/4];

/* Stack pointer of idle task */
static uint32_t g_idleTaskSp;

/* Tasks control array. Is idle task + user defined tasks */
static taskControl_t g_controlTaskArr[1 + OS_USER_TASKS_COUNT] =
		{
			{NULL, &g_idleTaskSp, TASK_STATE_READY, TASK_PRIORITY_IDLE, 0 }
		};

/* Scheduler execution task list */
static taskControl_t *g_scheduleList;

/***********************
 *	External functions	*
 ***********************/
extern void SysTick_Handler(void);

/************************
 *	Private functions	*
 ***********************/
static void updateBlockedCounter(void)
{
	/* Update blocked tasks counters */
	for(uint32_t i = 0;i < OS_TASKS_COUNT;i++)
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
void os_schedule(void)
{
	/* Instruction Synchronization Barrier: make sure every
	 * pending instruction in the pipeline is executed
	 */
	__ISB();

	/* Data Synchronization Barrier: make sure every memory
	 * access is done
	 */
	__DSB();

	/* Activate PendSV to make context switch */
	SCB->ICSR |= SCB_ICSR_PENDSVSET_Msk;
}
void SysTick_Handler(void)
{
	updateBlockedCounter();
	os_schedule();
}
uint32_t os_getNextContext(uint32_t currentSp)
{
	/* Check scheduler list status */
	if(NULL == g_scheduleList)
	{
	   /* Initialize scheduler list */
	   for(uint32_t taskIndex = 0;(taskIndex + 1) < OS_TASKS_COUNT;taskIndex++)
	   {
		   g_controlTaskArr[taskIndex].next = &g_controlTaskArr[taskIndex + 1];
	   }

	   /* Set head list to IDLE task */
	   g_scheduleList = &g_controlTaskArr[IDLE_TASK_INDEX];
	}
	else
	{
	   /* Save current task context */
	   *g_scheduleList->sp = currentSp;

	   /* If running, set current task state as READY */
	   if(TASK_STATE_RUNNING == g_scheduleList->state)
		   g_scheduleList->state = TASK_STATE_READY;
	}

	#define ORDER_NEXT_BEFORE_CURRENT_TASK	0
	#define ORDER_NEXT_AFTER_CURRENT_TASK	1

	/*****************************************************************
	 * Next task to be executed is always the first element pointed by
	 * g_scheduleList (head of list).
	 *
	 * Schedule List is reordered as follows:
	 *
	 * - Set a pointer to current and next task and decide if is needed
	 * to invert their order or just leave them the way they are by
	 * analyzing:
	 *
	 * - Tasks states
	 * - Tasks priorities
	 * - Tasks blocking counter
	 *
	 * Using a schedule list reduce OS footprint
	 *****************************************************************/

	uint32_t loopCnt = 0;
	bool keepLooping = false;

	do
	{
		taskControl_t *nextSchTask = NULL;
		taskControl_t *prevSchTask = NULL;

		/* Reorder scheduler list */
		for(taskControl_t *currSchTask = g_scheduleList;currSchTask;)
		{
			/* Set pointer to next element in list */
			nextSchTask = currSchTask->next;

			/* If there's no next element, break */
			if(NULL == nextSchTask)
				break;

			/*******************************************************************
			 * Decide if current and next task order should be switched or not *
			 ******************************************************************/

			/* By default leave list as it's ordered now */
			uint8_t schTaskOrder = ORDER_NEXT_AFTER_CURRENT_TASK;

			/* Evaluate states */
			if(currSchTask->state != nextSchTask->state)
			{	// States are different

				/* If next element state is READY, switch elements position */
				if(TASK_STATE_READY == nextSchTask->state)
					schTaskOrder = ORDER_NEXT_BEFORE_CURRENT_TASK;
			}
			else
			{	// States are equal
				if(TASK_STATE_READY == currSchTask->state)
				{	// States are both READY

					/* Evaluate priority */
					// Switch elements position if either:
					// * next element priority is higher
					// * priorities are the same and is the first loop (Round robin style)
					if( (currSchTask->priority < nextSchTask->priority) ||
						((currSchTask->priority == nextSchTask->priority) && (0 == loopCnt)) )
					{
						schTaskOrder = ORDER_NEXT_BEFORE_CURRENT_TASK;
					}
				}
				else if(TASK_STATE_BLOCKED == currSchTask->state)
				{	// States are both BLOCKED

					/* Evaluate blockedCounter */
					if(currSchTask->blockedCounter != nextSchTask->blockedCounter)
					{	// Block counters differ

						/* If next element counter is smaller than current element, switch elements position */
						if(currSchTask->blockedCounter > nextSchTask->blockedCounter)
							schTaskOrder = ORDER_NEXT_BEFORE_CURRENT_TASK;
					}
					else
					{	// Block counters are equal

						/* Evaluate priority */
						// Switch elements position if either:
						// * next element priority is higher
						// * priorities are the same and is the first loop (Round robin style)
						if( (currSchTask->priority < nextSchTask->priority) ||
						   ((currSchTask->priority == nextSchTask->priority) && (0 == loopCnt)) )
						{
						   schTaskOrder = ORDER_NEXT_BEFORE_CURRENT_TASK;
						}
					}
				}
			}

			/* Reorder tasks elements */
			if(ORDER_NEXT_BEFORE_CURRENT_TASK == schTaskOrder)
			{
				currSchTask->next = nextSchTask->next;
				nextSchTask->next = currSchTask;

				if(NULL != prevSchTask)
				{
					prevSchTask->next = nextSchTask;
				}
				else
				{
					g_scheduleList = nextSchTask;
				}

				prevSchTask = nextSchTask;

				/* Tasks order was switched, so keepLooping */
				keepLooping = true;
			}
			else
			{
				prevSchTask = currSchTask;
				currSchTask = currSchTask->next;
			}
		}

		/* If list order was not altered, keepLooping == false */
	} while(keepLooping && (++loopCnt < (OS_TASKS_COUNT - 1)));

	return *g_scheduleList->sp;
}

/************************
 *	Public functions	*
 ***********************/
void os_start(void)
{
	/* Initialize Idle task control data */
	initTaskStack(g_idleTaskStack, IDLE_TASKS_STACK_SIZE_BYTES, &g_idleTaskSp, idleTask, IDLE_TASK_ARGUMENT);

	/* Initialize execution tasks list */
	g_scheduleList = NULL;

	/* Configure PendSV interrupt priority */
	NVIC_SetPriority(PendSV_IRQn, (1 << __NVIC_PRIO_BITS) - 1);

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
					  void *arg,				/*	Pointer to task argument		*/
					  taskPriority_t priority)	/*  Task priority 					*/
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

	   /* */
	   g_controlTaskArr[userTaskIndex].priority = priority;
   }
}
void os_delay(uint32_t milliseconds)
{
	/* Validate current task is a valid task */
	if((0 != milliseconds) && (NULL != g_scheduleList))
	{
		/* Set task state as blocked */
		g_scheduleList->state = TASK_STATE_BLOCKED;

		/* Set task blocked counter */
		g_scheduleList->blockedCounter = milliseconds;

		/* Invoke scheduler */
		os_schedule();
	}
}
