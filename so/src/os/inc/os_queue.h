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

/**
 * \brief Creates a new queue instance
 *
 * \param name Name of the queue
 * \param qLen Number of element the queue should hold
 * \param iSize Size of the individual elements of the queue
 *
 * \return void
 */
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

/**
 * \brief Declares a queue that was created outiside the current file
 *
 * \param name Name of the queue
 *
 * \return void
 */
#define OS_QUEUE_DECLARE(name)		extern queue_t * name;

/*==================[external data declaration]==============================*/

/*==================[functions declaration]==================================*/
/**
 * \brief Post an item on a queue. The item is queued by copy, not by reference.
 *
 * \param queue The handle to the queue on which the item is to be posted
 * \param sndElement A pointer to the item that is to be placed on the
 * 					 queue. The size of the items the queue will hold was
 * 					 defined when the queue was created, so this many bytes
 * 					 will be copied from sndElement into the queue storage area.
 *
 *\param ticksToWait The maximum amount of time the task should block
 * 					 waiting for space to become available on the queue,
 * 					 should it already be full.  The call will return immediately
 * 					 if this is set to 0 and the queue is full.
 * 					 The time is defined in tick periods so the constant
 * 					 OS_TICK_PERIOD_MS should be used to convert to real time
 * 					 if this is required.
 * \return bool	True if the item was sent. Otherwise false.
 */
bool os_queue_send(queue_t * queue, void * sndElement, tick_t ticksToWait);

/**
 * \brief  Receive an item from a queue. The item is received by copy so a buffer of
 * 		   adequate size must be provided. The number of bytes copied into the buffer
 * 	       was defined when the queue was created.
 *
 * \param queue The handle to the queue from which the item is to be received.
 * \param rcvElement Pointer to the buffer into which the received item will be copied.
 * \param ticksToWait The maximum amount of time the task should block waiting for an
 * 					  item to receive should the queue be empty at the time of the call.
 * 					  Will return immediately if ticksToWait is zero and the queue is empty.
 * 					  The time is defined in tick periods so the constant OS_TICK_PERIOD_MS
 * 					  should be used to convert to real time if this is required.
 *
 * \return bool True if the item was received. Otherwise false.
 */
bool os_queue_receive(queue_t * queue, void * rcvElement, tick_t ticksToWait);

/*==================[end of file]============================================*/

#endif	/*	#ifndef _OS_QUEUE_H_ */
