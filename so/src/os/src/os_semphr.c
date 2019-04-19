/*
 ===============================================================================
 Name        : os_semphr.c
 Author      : $(Rodrigo Tirapegui)
 Version     :
 Copyright   : $(copyright)
 Description :
===============================================================================
*/
#include "os_semphr.h"
#include "board.h"

/************************
 *	External functions	*
 ***********************/
/* From module: os_task.c */
extern void OS_CORE_TASK_PREEMPT_ENABLE(void);
extern void OS_CORE_TASK_PREEMPT_DISABLE(void);
extern void * os_core_task_getCurrentContext(void);
extern void os_core_task_unblock(void * const task);
extern void os_core_task_delay(const tick_t ticksToDelay);
extern void os_core_task_yield(void);

/********************
 *	User functions	*
 *******************/
bool os_semphr_wait(semphr_t * semphr, tick_t ticksToWait)
{
	bool ret = false;

	/* Check semaphore sanity */
	if(NULL != semphr)
	{
		OS_CORE_TASK_PREEMPT_DISABLE();	/* Disable preemption */
		{
			if(OS_SEMPHR_INVALID_TASK == semphr->task)
			{
				if(0 < semphr->value)
				{
					/* Semaphore is free */
					semphr->value--;
					ret = true;
				}
				else
				{
					/* Semaphore is taken */

					/* Set current task as waiting for semaphore to be freed */
					semphr->task = os_core_task_getCurrentContext();

					OS_CORE_TASK_PREEMPT_ENABLE();	/* Enable preemption */

					/* Delay for ticksToWait */
					os_core_task_delay(ticksToWait);

					OS_CORE_TASK_PREEMPT_DISABLE();	/* Disable preemption */

					/* Getting here means semaphore is free or timeout occured */
					semphr->task = OS_SEMPHR_INVALID_TASK;

					/* Is semaphore is free, take it */
					if(0 < semphr->value)
					{
						semphr->value--;
						ret = true;
					}
				}
			}
		}
		OS_CORE_TASK_PREEMPT_ENABLE();	/* Enable preemption */
	}

	return ret;
}
void os_semphr_post(semphr_t * semphr)
{
	if(NULL != semphr)
	{
		bool yieldFlag = false;

		OS_CORE_TASK_PREEMPT_DISABLE();	/* Disable preemption */
		{
			if(0 == semphr->value)
			{
				/* Semaphore is taken and need to be freed */

				/* Free semaphore */
				semphr->value = 1;

				/* Check if there's a task waiting for the semaphore to get freed */
				if(OS_SEMPHR_INVALID_TASK != semphr->task)
				{
					/* Unlock task blocked task */
					os_core_task_unblock(semphr->task);

					/* Set invoke scheduler flag */
					yieldFlag = true;
				}
			}
		}
		OS_CORE_TASK_PREEMPT_ENABLE();	/* Enable preemption */

		/* Invoke scheduler if flag set */
		if(true == yieldFlag)
			os_core_task_yield();
	}
}
