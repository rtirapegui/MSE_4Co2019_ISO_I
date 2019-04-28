/*
===============================================================================
 Name        : os_task.h
 Author      : $(Rodrigo Tirapegui)
 Version     :
 Copyright   : $(copyright)
 Description :
===============================================================================
*/

#ifndef _OS_TASK_H_
#define _OS_TASK_H_

/*==================[inclusions]=============================================*/
#include <stdint.h>
#include "portmacro.h"

/*==================[typedef]================================================*/

/* Tasks prototype */
typedef void *(*task_t)(void *);

/* Tasks priority type */
typedef enum
{
	TASK_PRIORITY_IDLE
,	TASK_PRIORITY_LOW
,	TASK_PRIORITY_MEDIUM
,	TASK_PRIORITY_HIGH
} taskPriority_t;

/* Tasks definition type */
typedef struct
{
	uint32_t * const 	 stack;
	const uint32_t   	 stackSizeBytes;
	task_t const     	 entryPoint;
	void * const     	 arg;
	const taskPriority_t priority;
} taskDefinition_t;

/*==================[macros]=================================================*/
/**
 * \brief Create a new task and add it to the list of tasks that are ready to run
 *
 * \param name 		Name of the task
 * \param stackSize The size of the task stack specified as the number of
 * 					the number of bytes.
 * \param entry     Pointer to the task entry function. Tasks must be
 * 					implemented to never return (i.e. continuous loop).
 * \param argument  Pointer that will be used as the parameter for the task
 * 				    being created.
 * \param prio		The priority at which the task should run.
 *
 * \return void
 */
#define OS_TASK_CREATE(name, stackSize, entry, argument, prio)				 \
		static void * entry(void *arg);										 \
		static uint32_t name##_taskStack[stackSize/4];					 	 \
		const taskDefinition_t name = {	.stackSizeBytes = stackSize, 		 \
										.entryPoint = entry,		 		 \
										.arg = argument,			 		 \
										.priority = prio,			 		 \
										.stack = name##_taskStack	 		 \
									  };
/**
 * \brief Declares every task created by user. Should be called once in file
 * 		  consf_os_task.h to populates os scheduler.
 *
 * \param variadic	The names of the tasks created by user, separated by commas.
 *
 * \return void
 */
#define OS_TASKS_DECLARE(...)												 \
		static const taskDefinition_t * const g_userTaskArr[] = {__VA_ARGS__};

/*==================[external data declaration]==============================*/

/*==================[functions declaration]==================================*/
/**
 * \brief Gets the count of ticks since os_start() was called.
 *
 * \return tick_t Returns the current system tick.
 */
tick_t os_task_getTickCount(void);

/**
 * \brief Delay a task for a given number of ticks.
 *
 * \param ticksToDelay	The actual time that the task remains blocked depends
 * 						on the tick rate.
 * 		  				The constant OS_TICK_PERIOD_MS can be used to calculate
 * 		  				real time from the tick rate - with the resolution of one tick period.
 *
 * \return void
 */
void os_task_delay(const tick_t ticksToDelay);

/**
 * \brief Delay a task until a specified time. This function can be used by periodic
 * 		  tasks to ensure a constant execution frequency.
 *
 * 		  This function differs from os_task_delay() in one important aspect: os_task_delay()
 * 		  will cause a task to block for the specified number of ticks from the time os_task_delay()
 * 		  is called. It is therefore difficult to use os_task_delay() by itself to generate a fixed
 * 		  execution frequency as the time between a task starting to execute and that task calling
 * 		  os_task_delay() may not be fixed [the task may take a different path though the code between
 * 		  calls, or may get interrupted or preempted a different number of times each time it executes].
 *
 *		  Whereas os_task_delay() specifies a wake time relative to the time at which the function
 * 		  is called, os_task_delayUntil() specifies the absolute (exact) time at which it wishes to
 * 		  unblock.
 *
 * 	      The constant OS_TICK_PERIOD_MS can be used to calculate real time from the tick rate -
 * 	      with the resolution of one tick period.
 *
 * \param previousWake	Time Pointer to a variable that holds the time at which the task was last unblocked.
 * 						The variable must be initialized with the current time prior to its first use.
 * 						Following this the variable is automatically updated within os_task_delayUntil().
 * 						The actual time that the task remains blocked depends on the tick rate.
 * 		  				The constant OS_TICK_PERIOD_MS can be used to calculate real time from the tick rate
 * 		  				- with the resolution of one tick period.
 *
 * \param timeIncrement The cycle time period. The task will be unblocked at time *previousWakeTime + timeIncrement.
 * 						Calling os_task_delayUntil with the same timeIncrement parameter value will cause the task
 * 						to execute with a fixed interface period.
 *
 * \return void
 */
void os_task_delayUntil(tick_t *const previousWakeTime, const tick_t timeIncrement);

/**
 * \brief Invokes system context switch.
 *
 * \return void
 */
void os_task_yield(void);

/*==================[end of file]============================================*/

#endif	/*	#ifndef _OS_TASK_H_ */
