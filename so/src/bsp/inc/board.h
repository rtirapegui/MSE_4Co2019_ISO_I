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

#include "chip.h"

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

/*********
 * Types *
 ********/

typedef enum
{
	RISING_EDGE
,	FALLING_EDGE
} btnIRQEdge_t;

/* User Button IRQ handler */
typedef void (*btn_user_irq_handler_t)(btnIRQEdge_t edge);

/**********************
 * External functions *
 *********************/
extern void Board_SystemInit(void);

/********************
 * Public functions *
 *******************/
void Board_Init(void);

/* LEDs control functions */
void Board_LED_Set(uint8_t ledNo, bool On);
bool Board_LED_IsOn(uint8_t ledNo);
void Board_LED_Toggle(uint8_t ledNo);

/* Buttons control functions */
void Board_BUTTON_registerIRQHandler(uint8_t btnNo, btn_user_irq_handler_t cb);
void Board_BUTTON_deregisterIRQHandler(uint8_t btnNo);
bool Board_BUTTON_isPressed(uint8_t btnNo);
uint32_t Board_BUTTON_GetStatus(void);

/* UART control functions */
bool Board_UART_ReadByte(uint8_t* byte);
uint8_t Board_UART_ReadData(uint8_t* buff, const uint8_t expBytesLen, const uint32_t timeout);
bool Board_UART_WriteByte(const uint8_t byte);
uint8_t Board_UART_WriteData(const uint8_t* buff, const uint32_t len);
uint8_t Board_UART_WriteString(const uint8_t* string);

#endif /* __BOARD_H_ */
