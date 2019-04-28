/*
===============================================================================
 Name        : board.h
 Author      : $(Rodrigo Tirapegui)
 Version     :
 Copyright   : $(copyright)
 Description :
===============================================================================
*/

#ifndef __BOARD_H_
#define __BOARD_H_

/*==================[inclusiones]============================================*/
#include "chip.h"

/*==================[macros]=================================================*/

/* Board name */
#define BOARD_EDU_CIAA_4337

/*************
 * Constants *
 ************/
#define LED_1		0
#define LED_2		1
#define LED_3		2
#define LED_RED		3
#define LED_GREEN	4
#define LED_BLUE	5

#define TEC_1		0
#define TEC_2		1
#define TEC_3		2
#define TEC_4		3

/*==================[tipos de datos]===============*/

typedef enum
{
	RISING_EDGE
,	FALLING_EDGE
} btnIRQEdge_t;

/* User Button IRQ handler */
typedef void (*btn_user_irq_handler_t)(btnIRQEdge_t edge);

/*==================[declaraciones de funciones externas]====================*/
extern void Board_SystemInit(void);

/*==================[declaraciones de funciones]=============================*/
/**
 * \brief Initializes EDU-CIAA board peripherals.
 *
 * \return void
 */
void Board_Init(void);

/* LEDs control functions */
/**
 * \brief Sets EDU-CIAA LED into the desired status.
 *
 * \param ledNo	One of exiting LED indexes
 * \param On	True turns LED on. False turns LED off.
 *
 * \return void
 */
void Board_LED_Set(uint8_t ledNo, bool On);
/**
 * \brief Gets EDU-CIAA LED status.
 *
 * \param ledNo	One of exiting LED indexes
 *
 * \return void
 */
bool Board_LED_IsOn(uint8_t ledNo);
/**
 * \brief Toggles EDU-CIAA LED status.
 *
 * \param ledNo	One of exiting LED indexes
 *
 * \return void
 */
void Board_LED_Toggle(uint8_t ledNo);

/* Buttons control functions */
/**
 * \brief Registers user callback function that should be invoke when the
 * 		  corresponding button status changes.
 *
 * \param btnNo	One of the existing BUTTON indexes.
 *  \param cb	Pointer to the callback function.
 *
 * \return bool True if the callback function was successfully registered.
 * 				Otherwise false.
 */
bool Board_BUTTON_registerIRQHandler(uint8_t btnNo, btn_user_irq_handler_t cb);
/**
 * \brief Deregisters user callback function that was already registered.
 *
 * \param btnNo	One of the existing BUTTON indexes.
  *
 * \return bool True if the callback function was sucessfully registered.
 * 				Otherwise false.
 */
bool Board_BUTTON_deregisterIRQHandler(uint8_t btnNo);
/**
 * \brief Gets EDU-CIAA button status
 *
 * \param btnNo	One of the existing BUTTON indexes.
 *
 * \return bool True if the button is pressed. Otherwise false.
 */
bool Board_BUTTON_isPressed(uint8_t btnNo);
/**
 * \brief Gets EDU-CIAA buttons status as a 32-bitfield.
 *
 * \return uint32_t Bitfield where 1 indicates button pressed and 0 button released.
 */
uint32_t Board_BUTTON_GetStatus(void);

/* UART control functions */
/**
 * \brief Get a byte from EDU-CIAA USB UART.
 *
 * \param byte Pointer to were the received byte should be store.
 *
 * \return bool True if there was a byte to get. Otherwise false.
 */
bool Board_UART_ReadByte(uint8_t* byte);
/**
 * \brief Gets a number of bytes from EDU-CIAA USB UART.
 *
 * \param buff Pointer to the buffer where data should be stored.
 * \param expBytesLen Buffer size in bytes.
 * \param timeout The maximum aumount of time to wait until function returns.
 *
 * \return uint8_t The number of bytes that were copied to buffer.
 */
uint8_t Board_UART_ReadData(uint8_t* buff, const uint8_t expBytesLen, const uint32_t timeout);
/**
 * \brief Send a byte through EDU-CIAA USB UART.
 *
 * \param byte The byte to be written.
 *
 * \return bool True if the byte was sent. Otherwise false.
 */
bool Board_UART_WriteByte(const uint8_t byte);
/**
 * \brief Send a group of bytes through EDU-CIAA USB UART.
 *
 * \param buff Pointer to the buffer holding the data.
 * \param len  Number of bytes to be sent.
 *
 * \return uint8_t The number of bytes that were written.
 */
uint8_t Board_UART_WriteData(const uint8_t* buff, const uint32_t len);
/**
 * \brief Send a string through EDU-CIAA USB UART.
 *
 * \param string Pointer to the string.
 *
 * \return uint8_t The number of bytes that were written.
 */
uint8_t Board_UART_WriteString(const uint8_t* string);

#endif /* __BOARD_H_ */
