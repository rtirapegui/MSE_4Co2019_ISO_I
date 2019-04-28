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

/*==================[inclusions]=============================================*/
#include <stdint.h>
#include "portmacro.h"
#include "conf_os.h"

/* OS modules */
#include "os_task.h"
#include "os_semphr.h"
#include "os_queue.h"
#include "os_vector.h"

/*==================[typedef]================================================*/

/*==================[macros]=================================================*/

/*==================[external data declaration]==============================*/

/*==================[functions declaration]==================================*/
/**
 * \brief Starts the real time kernel tick processing. After calling the kernel
 * 		  has control over which tasks are executed and when.
 *
 * \return void
 */
void os_start(void);

/*==================[end of file]============================================*/

#endif	/*	#ifndef _OS_H_ */
