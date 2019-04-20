/*
===============================================================================
 Name        : os_semphr.h
 Author      : $(Rodrigo Tirapegui)
 Version     :
 Copyright   : $(copyright)
 Description :
===============================================================================
*/

#ifndef _OS_SEMPHR_H_
#define _OS_SEMPHR_H_

/*==================[inclusions]=============================================*/
#include <stdint.h>
#include <stdbool.h>
#include "portmacro.h"

/*==================[typedef]================================================*/

/* Semaphore type */
typedef struct
{
	uint32_t value;
	void *	 waitingTask;
} semphr_t;

/*==================[macros]=================================================*/
#define OS_SEMPHR_INVALID_TASK	0

#define OS_SEMPHR_CREATE(name)														\
		static semphr_t name##_semphr = {											\
											.value = 0,								\
											.waitingTask = OS_SEMPHR_INVALID_TASK	\
								 	 	 };											\
		semphr_t * name = &name##_semphr;

#define OS_SEMPHR_DECLARE(name)		extern semphr_t * name;

/*==================[external data declaration]==============================*/

/*==================[functions declaration]==================================*/
bool os_semphr_wait(semphr_t * semphr, tick_t ticksToWait);
void os_semphr_post(semphr_t * semphr);

/*==================[end of file]============================================*/

#endif	/*	#ifndef _OS_SEMPHR_H_ */
