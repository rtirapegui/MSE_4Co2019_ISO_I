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

/**
 * \brief Creates a new semaphore instance
 *
 * \param name Name of the semaphore
 *
 * \return void
 */
#define OS_SEMPHR_CREATE(name)														\
		static semphr_t name##_semphr = {											\
											.value = 0,								\
											.waitingTask = OS_SEMPHR_INVALID_TASK	\
								 	 	 };											\
		semphr_t * name = &name##_semphr;

/**
 * \brief Declares a semaphore that was created outiside the current file
 *
 * \param name Name of the semaphore
 *
 * \return void
 */
#define OS_SEMPHR_DECLARE(name)		extern semphr_t * name;

/*==================[external data declaration]==============================*/

/*==================[functions declaration]==================================*/
/**
 * \brief Obtains a semaphore. The semaphore must have previously been create
 *
 * \param semphr 	 A handle to the semaphore being taken - created when
 * 					 the semaphore was created
 *
 *\param ticksToWait The time in ticks to wait for the semaphore to become
 * 					 available. The macro OS_TICK_PERIOD_MS can be used to convert
 * 					 this to a real time.
 * 					 A block time of zero can be used to poll the semaphore.
 * 					 A block time of OS_TICK_MAX can be used to block indefinitely.
 *
 * \return bool True if the semaphore was taken. Otherwise false.
 */
bool os_semphr_wait(semphr_t * semphr, tick_t ticksToWait);

/**
 * \brief Obtains a semaphore. The semaphore must have previously been create
 *
 * \param semphr 	 A handle to the semaphore being released - created when
 * 					 the semaphore was created
 *
 * \return void
 */
void os_semphr_post(semphr_t * semphr);

/*==================[end of file]============================================*/

#endif	/*	#ifndef _OS_SEMPHR_H_ */
