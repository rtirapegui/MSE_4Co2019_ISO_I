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
typedef void *(*task_t)(void *);

/* Tasks priority list */
typedef enum
{
	TASK_PRIORITY_IDLE
,	TASK_PRIORITY_LOW
,	TASK_PRIORITY_MEDIUM
,	TASK_PRIORITY_HIGH
} taskPriority_t;

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
		static uint32_t stackThread_##name[stackSize/4];					 \
		const taskDefinition_t name = {	.stackSizeBytes = stackSize, 		 \
										.entryPoint = entry,		 		 \
										.arg = argument,			 		 \
										.priority = prio,			 		 \
										.stack = stackThread_##name	 		 \
									  };

#define OS_TASKS_DECLARE(...)												 \
		static const taskDefinition_t * const g_userTaskArr[] = {__VA_ARGS__};

/*==================[external data declaration]==============================*/

/*==================[external functions declaration]=========================*/
void os_start(void);
void os_delay(uint32_t milliseconds);

/*==================[end of file]============================================*/

#endif	/*	#ifndef _OS_H_ */
