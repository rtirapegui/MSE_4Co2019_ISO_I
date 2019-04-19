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
#define OS_TASK_CREATE(name, stackSize, entry, argument, prio)				 \
		static void * entry(void *arg);										 \
		static uint32_t name##_taskStack[stackSize/4];					 	 \
		const taskDefinition_t name = {	.stackSizeBytes = stackSize, 		 \
										.entryPoint = entry,		 		 \
										.arg = argument,			 		 \
										.priority = prio,			 		 \
										.stack = name##_taskStack	 		 \
									  };

#define OS_TASKS_DECLARE(...)												 \
		static const taskDefinition_t * const g_userTaskArr[] = {__VA_ARGS__};

/*==================[external data declaration]==============================*/

/*==================[functions declaration]==================================*/
tick_t os_task_getTickCount(void);
void os_task_delay(const tick_t ticksToDelay);
void os_task_delayUntil(tick_t *const previousWakeTime, const tick_t timeIncrement);
void os_task_yield(void);

/*==================[end of file]============================================*/

#endif	/*	#ifndef _OS_TASK_H_ */
