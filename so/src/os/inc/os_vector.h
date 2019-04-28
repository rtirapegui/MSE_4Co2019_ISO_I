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
/**
 * \brief Registers a user callback function related to one of LPC18xx/43xx
 * 		  Specific Interrupt Numbers.
 *
 * \param irq One of LPC18xx/43xx Specific Interrupt Number. Should be >= 0.
 * \param cb  A pointer to the callback function the os should invoke que an irq is generated.
 *
 * \return bool	True if user callback was successfully registered. Otherwise false.
 */
bool os_vector_attach_irq(LPC43XX_IRQn_Type irq, user_irq_handler_t cb);
/**
 * \brief Deregisters a user callback function related to one of LPC18xx/43xx
 * 		  Specific Interrupt Numbers.
 *
 * \param irq One of LPC18xx/43xx Specific Interrupt Number. Should be >= 0.
 *
 * \return bool	True if user callback was successfully deregistered. Otherwise false.
 */
bool os_vector_detach_irq(LPC43XX_IRQn_Type irq);

/*==================[end of file]============================================*/

#endif	/*	#ifndef _OS_VECTOR_H_ */
