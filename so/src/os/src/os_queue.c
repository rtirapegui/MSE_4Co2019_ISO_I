/*
 ===============================================================================
 Name        : os_queue.c
 Author      : $(Rodrigo Tirapegui)
 Version     :
 Copyright   : $(copyright)
 Description :
===============================================================================
*/
#include <string.h>
#include "os_queue.h"

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

/*********************
 * Private functions *
 ********************/
static inline uint32_t getNext(uint32_t cur_offset, uint32_t queueLen)
{
   return (cur_offset == (queueLen - 1) ? 0 : cur_offset + 1);
}
static inline uint32_t queueGetNextWrite(const queue_t * queue)
{
   return getNext(queue->writeItemOffset, queue->queueLen);
}
static inline uint32_t queueGetNextRead(const queue_t * queue)
{
   return getNext(queue->readItemOffset, queue->queueLen);
}
static inline bool queueIsFull(const queue_t * queue)
{
   return (queue->readItemOffset == queueGetNextWrite(queue));
}
static inline bool queueIsEmpty(const queue_t * queue)
{
   return (queue->readItemOffset == queue->writeItemOffset);
}
static inline void queuePut(queue_t * queue, const void *data)
{
   memcpy(&queue->buffer[queue->writeItemOffset*queue->itemSize], data, queue->itemSize);
   queue->writeItemOffset = queueGetNextWrite(queue);
}
static inline void queueGet(queue_t * queue, void *data)
{
   memcpy(data, &queue->buffer[queue->readItemOffset*queue->itemSize], queue->itemSize);
   queue->readItemOffset =  queueGetNextRead(queue);
}

/********************
 *	User functions	*
 *******************/
bool os_queue_send(queue_t * queue, void * sndElement, tick_t ticksToWait)
{
	bool ret = false;

	/* Check queue sanity */
	if(NULL != queue)
	{
		bool yieldFlag = false;

		OS_CORE_TASK_PREEMPT_DISABLE();	/* Disable preemption */
		{
			/* Just one task could be waiting to insert element in queue at a time */
			if(OS_QUEUE_INVALID_TASK == queue->sndWaitingTask)
			{
				bool queueFullFlag = false;

				/* Check if queue is full */
				if (true == queueIsFull(queue))
				{
					/* Queue is full */

					/* Set current task as waiting for queue to have free space */
					queue->sndWaitingTask = os_core_task_getCurrentContext();

					OS_CORE_TASK_PREEMPT_ENABLE();	/* Enable preemption */

					/* Delay for ticksToWait */
					os_core_task_delay(ticksToWait);

					OS_CORE_TASK_PREEMPT_DISABLE();	/* Disable preemption */

					/* Getting here means queue has free space or timeout occured */
					queue->sndWaitingTask = OS_QUEUE_INVALID_TASK;

					/* Check again if queue keeps being full */
					if (true == queueIsFull(queue))
						queueFullFlag = true;
				}

				/* Check if queue has free space */
				if (false == queueFullFlag)
				{
					/* Insert element into queue */
					queuePut(queue, sndElement);

					/* Set return value */
					ret = true;

					/* Generate event to receive waiting task */
					if(OS_QUEUE_INVALID_TASK != queue->rcvWaitingTask)
					{
						/* Unlock task blocked task */
						os_core_task_unblock(queue->rcvWaitingTask);

						/* Set invoke scheduler flag */
						yieldFlag = true;
					}
				}
			}
		}
		OS_CORE_TASK_PREEMPT_ENABLE();	/* Enable preemption */

		/* Invoke scheduler if flag set */
		if(true == yieldFlag)
			os_core_task_yield();
	}

	return ret;
}
bool os_queue_receive(queue_t * queue, void * rcvElement, tick_t ticksToWait)
{
	bool ret = false;

	/* Check queue sanity */
	if(NULL != queue)
	{
		bool yieldFlag = false;

		OS_CORE_TASK_PREEMPT_DISABLE();	/* Disable preemption */
		{
			/* Just one task could be waiting to get element from queue at a time */
			if(OS_QUEUE_INVALID_TASK == queue->rcvWaitingTask)
			{
				bool queueEmptyFlag = false;

				/* Check if queue is empty */
				if (true == queueIsEmpty(queue))
				{
					/* Queue is empty */

					/* Set current task as waiting for queue to have one item to get */
					queue->rcvWaitingTask = os_core_task_getCurrentContext();

					OS_CORE_TASK_PREEMPT_ENABLE();	/* Enable preemption */

					/* Delay for ticksToWait */
					os_core_task_delay(ticksToWait);

					OS_CORE_TASK_PREEMPT_DISABLE();	/* Disable preemption */

					/* Getting here means queue has one item inside or timeout occured */
					queue->rcvWaitingTask = OS_QUEUE_INVALID_TASK;

					/* Check again if queue keeps being empty */
					if (true == queueIsEmpty(queue))
						queueEmptyFlag = true;
				}

				/* Check if queue has at least one element inside */
				if (false == queueEmptyFlag)
				{
					/* Insert element into queue */
					queueGet(queue, rcvElement);

					/* Set return value */
					ret = true;

					/* Generate event to receive waiting task */
					if(OS_QUEUE_INVALID_TASK != queue->sndWaitingTask)
					{
						/* Unlock task blocked task */
						os_core_task_unblock(queue->sndWaitingTask);

						/* Set invoke scheduler flag */
						yieldFlag = true;
					}
				}
			}
		}
		OS_CORE_TASK_PREEMPT_ENABLE();	/* Enable preemption */

		/* Invoke scheduler if flag set */
		if(true == yieldFlag)
			os_core_task_yield();
	}

	return ret;
}
