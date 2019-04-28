/*
===============================================================================
 Name        : os_vector.h
 Author      : $(Rodrigo Tirapegui)
 Version     :
 Copyright   : $(copyright)
 Description :
===============================================================================
*/

#ifndef _OS_VECTOR_H_
#define _OS_VECTOR_H_

/*==================[inclusions]=============================================*/
#include <stdint.h>
#include "chip.h"

/*==================[typedef]================================================*/

/* User IRQ handler */
typedef void (*user_irq_handler_t)(void);

/*==================[macros]=================================================*/

/*==================[external data declaration]==============================*/

/*==================[functions declaration]==================================*/
bool os_vector_attach_irq(LPC43XX_IRQn_Type irq, user_irq_handler_t cb);
bool os_vector_detach_irq(LPC43XX_IRQn_Type irq);

/*==================[end of file]============================================*/

#endif	/*	#ifndef _OS_VECTOR_H_ */
