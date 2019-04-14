/*
===============================================================================
 Name        : os.h
 Author      : $(Rodrigo Tirapegui)
 Version     :
 Copyright   : $(copyright)
 Description :
===============================================================================
*/

#ifndef _OS_H_
#define _OS_H_

#include <stdint.h>

/*==================[inclusions]=============================================*/

/*==================[typedef]================================================*/
/* Tasks prototype */
typedef void *(*task_t)(void *);

/* Tick type. Should be unsigned! */
typedef uint32_t tick_t;

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
	uint32_t * 	   stack;
	uint32_t   	   stackSizeBytes;
	task_t 	   	   entryPoint;
	void *	   	   arg;
	taskPriority_t priority;
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

/*==================[external functions declaration]=========================*/
void os_start(void);
void os_taskDelay(const tick_t ticksToDelay);
void os_taskDelayUntil(tick_t *const previousWakeTime, const tick_t timeIncrement);
tick_t os_getTickCount(void);
void os_taskYield(void);

/*==================[end of file]============================================*/

#endif	/*	#ifndef _OS_H_ */
