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

/*==================[macros]=================================================*/

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

/*==================[external data declaration]==============================*/

/*==================[external functions declaration]=========================*/
void os_start(void);
void os_initTaskStack(uint32_t stack[], uint32_t stackSizeBytes, uint32_t *sp, task_t entryPoint, void *arg, taskPriority_t priority);
void os_delay(uint32_t milliseconds);

/*==================[end of file]============================================*/

#endif	/*	#ifndef _OS_H_ */
