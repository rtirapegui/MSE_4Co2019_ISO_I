/*
===============================================================================
 Name        : os_queue.h
 Author      : $(Rodrigo Tirapegui)
 Version     :
 Copyright   : $(copyright)
 Description :
===============================================================================
*/

#ifndef _OS_QUEUE_H_
#define _OS_QUEUE_H_

/*==================[inclusions]=============================================*/
#include <stdint.h>
#include <stdbool.h>
#include "portmacro.h"

/*==================[typedef]================================================*/

/* Queue type */
typedef struct
{
	volatile uint32_t 	writeItemOffset;
	volatile uint32_t 	readItemOffset;
	const uint32_t 		itemSize;
	const uint32_t 		queueLen;
	uint8_t * 			buffer;
	void * 				sndWaitingTask;
	void * 				rcvWaitingTask;
} queue_t;

/*==================[macros]=================================================*/
#define OS_QUEUE_INVALID_TASK	0

#define OS_QUEUE_CREATE(name, qLen, iSize)										\
		static uint8_t name##_queueBuff[(1 + qLen)*iSize];						\
		static queue_t name##_queue = {											\
										.writeItemOffset = 0,					\
										.readItemOffset = 0,					\
										.itemSize = iSize,						\
										.queueLen = 1 + qLen,					\
										.buffer = name##_queueBuff,				\
										.sndWaitingTask = OS_QUEUE_INVALID_TASK,\
										.rcvWaitingTask = OS_QUEUE_INVALID_TASK	\
									};											\
		queue_t * name = &name##_queue;

#define OS_QUEUE_DECLARE(name)		extern queue_t * name;

/*==================[external data declaration]==============================*/

/*==================[functions declaration]==================================*/
bool os_queue_send(queue_t * queue, void * sndElement, tick_t ticksToWait);
bool os_queue_receive(queue_t * queue, void * rcvElement, tick_t ticksToWait);

/*==================[end of file]============================================*/

#endif	/*	#ifndef _OS_QUEUE_H_ */
