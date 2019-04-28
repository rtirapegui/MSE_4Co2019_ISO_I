/*
===============================================================================
 Name        : board.c
 Author      : $(Rodrigo Tirapegui)
 Version     :
 Copyright   : $(copyright)
 Description :
===============================================================================
*/

#include <string.h>
#include "board.h"
#include "uart_usb.h"
#include "os.h"

/*************
 * Constants *
 *************/

/* System configuration variables used by chip driver */
const uint32_t ExtRateIn = 0;
const uint32_t OscRateIn = 12000000;

/* EDU-CIAA-NXP button defines */
#define TEC1_PRESSED        0x01
#define TEC2_PRESSED        0x02
#define TEC3_PRESSED        0x04
#define TEC4_PRESSED        0x08

/*********
 * Types *
 ********/
typedef struct
{
	uint8_t port;
	uint8_t pin;
} io_port_t;

/* LEDs io_port_t definitions */
static const io_port_t g_gpioLEDBits[] = {
											{0,14},	/* LED_1 	 */
											{1,11},	/* LED_2 	 */
											{1,12},	/* LED_3 	 */
											{5,0},	/* LED_RED   */
											{5,1},	/* LED_GREEN */
											{5,2}	/* LED_BLUE  */
										};

/* BUTTONs io_port_t definitions */
static const io_port_t g_gpioBtnBits[] = {
											{0,4},	/* TEC_1 */
											{0,8},	/* TEC_2 */
											{0,9},	/* TEC_3 */
											{1,9}	/* TEC_4 */
										 };

/* BUTTONs pressed id's */
static const uint8_t g_gpioBtnIDs[] = {
										TEC1_PRESSED,
										TEC2_PRESSED,
										TEC3_PRESSED,
										TEC4_PRESSED
									  };

/**************
 *  Variables *
 *************/
static btn_user_irq_handler_t g_btnIRQUserCb[sizeof(g_gpioBtnBits)/sizeof(g_gpioBtnBits[0])];

/**********************
 *  Private functions *
 *********************/
static btnIRQEdge_t getEdgeType(uint8_t irqChannel)
{
   /* If interrupt was because a rising edge */
   if ( Chip_PININT_GetRiseStates(LPC_GPIO_PIN_INT) & PININTCH(irqChannel) )
   {
	  /* Clear rise edge irq */
	  Chip_PININT_ClearRiseStates(LPC_GPIO_PIN_INT,PININTCH(irqChannel));

	  return RISING_EDGE;
   }
   /* If not, interrupt was because a falling edge */
   else
   {
	  /* Clear falling edge irq */
	  Chip_PININT_ClearFallStates(LPC_GPIO_PIN_INT,PININTCH(irqChannel));

	  return FALLING_EDGE;
   }
}
static void serveIRQ(uint8_t irqChannel)
{
	if(NULL != g_btnIRQUserCb[irqChannel])
		g_btnIRQUserCb[irqChannel] (getEdgeType(irqChannel));

	/* Clear interrupt flag for irqChannel */
	Chip_PININT_ClearIntStatus(LPC_GPIO_PIN_INT,PININTCH(irqChannel));
}

/*****************
 * IRQ functions *
 ****************/
static void BOARD_TEC1_IRQHandler(void)
{
	serveIRQ(0);
}
static void BOARD_TEC2_IRQHandler(void)
{
	serveIRQ(1);
}
static void BOARD_TEC3_IRQHandler(void)
{
	serveIRQ(2);
}
static void BOARD_TEC4_IRQHandler(void)
{
	serveIRQ(3);
}

/*********************
 *  Public functions *
 ********************/

/********************
 *  LEDs functions	*
 *******************/
static void Board_LED_Init()
{
   /* LEDs EDU-CIAA-NXP */
   Chip_SCU_PinMux(2,0,MD_PUP|MD_EZI,FUNC4);  /* GPIO5[0], LED0R */
   Chip_SCU_PinMux(2,1,MD_PUP|MD_EZI,FUNC4);  /* GPIO5[1], LED0G */
   Chip_SCU_PinMux(2,2,MD_PUP|MD_EZI,FUNC4);  /* GPIO5[2], LED0B */
   Chip_SCU_PinMux(2,10,MD_PUP|MD_EZI,FUNC0); /* GPIO0[14], LED1 */
   Chip_SCU_PinMux(2,11,MD_PUP|MD_EZI,FUNC0); /* GPIO1[11], LED2 */
   Chip_SCU_PinMux(2,12,MD_PUP|MD_EZI,FUNC0); /* GPIO1[12], LED3 */

   Chip_GPIO_SetDir(LPC_GPIO_PORT, 5,(1<<0)|(1<<1)|(1<<2),1);
   Chip_GPIO_SetDir(LPC_GPIO_PORT, 0,(1<<14),1);
   Chip_GPIO_SetDir(LPC_GPIO_PORT, 1,(1<<11)|(1<<12),1);

   Chip_GPIO_ClearValue(LPC_GPIO_PORT, 5,(1<<0)|(1<<1)|(1<<2));
   Chip_GPIO_ClearValue(LPC_GPIO_PORT, 0,(1<<14));
   Chip_GPIO_ClearValue(LPC_GPIO_PORT, 1,(1<<11)|(1<<12));
}
void Board_LED_Set(uint8_t ledNo, bool On)
{
	if (ledNo < (sizeof(g_gpioLEDBits) / sizeof(io_port_t)))
		Chip_GPIO_SetPinState(LPC_GPIO_PORT, g_gpioLEDBits[ledNo].port, g_gpioLEDBits[ledNo].pin, On);
}
bool Board_LED_IsOn(uint8_t ledNo)
{
	if (ledNo < (sizeof(g_gpioLEDBits) / sizeof(io_port_t)))
		return (bool) Chip_GPIO_GetPinState(LPC_GPIO_PORT, g_gpioLEDBits[ledNo].port, g_gpioLEDBits[ledNo].pin);

	return false;
}
void Board_LED_Toggle(uint8_t ledNo)
{
	Board_LED_Set(ledNo, !Board_LED_IsOn(ledNo));
}

/*********************
 * BUTTONs functions *
 * ******************/
static void Board_BUTTON_Init(void)
{
	/* EDU-CIAA-NXP buttons */
	PINMUX_GRP_T pin_config[] = {
			{1, 0, MD_PUP|MD_EZI|FUNC0},	/* TEC1 -> P1_0. Input glitch filter enabled. */
			{1, 1, MD_PUP|MD_EZI|FUNC0},	/* TEC2 -> P1_1. Input glitch filter enabled. */
			{1, 2, MD_PUP|MD_EZI|FUNC0},	/* TEC3 -> P1_2. Input glitch filter enabled. */
			{1, 6, MD_PUP|MD_EZI|FUNC0} 	/* TEC4 -> P1_6. Input glitch filter enabled. */
	};

	Chip_SCU_SetPinMuxing(pin_config, (sizeof(pin_config) / sizeof(PINMUX_GRP_T)));

	for (uint8_t i = 0; i < (sizeof(g_gpioBtnBits) / sizeof(io_port_t)); i++)
	{
		Chip_GPIO_SetPinDIRInput(LPC_GPIO_PORT, g_gpioBtnBits[i].port, g_gpioBtnBits[i].pin);
	}

	/* IRQs */
	Chip_PININT_Init(LPC_GPIO_PIN_INT);
	Chip_SCU_GPIOIntPinSel(0, 0, 4);	/* TEC_1 */
	Chip_SCU_GPIOIntPinSel(1, 0, 8);	/* TEC_2 */
	Chip_SCU_GPIOIntPinSel(2, 0, 9);	/* TEC_3 */
	Chip_SCU_GPIOIntPinSel(3, 1, 9);	/* TEC_4 */
	Chip_PININT_EnableIntLow(LPC_GPIO_PIN_INT, PININTCH0 | PININTCH1 | PININTCH2 | PININTCH3);		/* Falling edge interrupt */
	Chip_PININT_EnableIntHigh(LPC_GPIO_PIN_INT, PININTCH0 | PININTCH1 | PININTCH2 | PININTCH3);		/* Rising edge interrupt */
	Chip_PININT_SetPinModeEdge(LPC_GPIO_PIN_INT, PININTCH0 | PININTCH1 | PININTCH2 | PININTCH3);	/* Configure pin as edge sentitive pin */

	/* Register buttons handlers with OS API */
	os_vector_attach_irq(PIN_INT0_IRQn, BOARD_TEC1_IRQHandler);
	os_vector_attach_irq(PIN_INT1_IRQn, BOARD_TEC2_IRQHandler);
	os_vector_attach_irq(PIN_INT2_IRQn, BOARD_TEC3_IRQHandler);
	os_vector_attach_irq(PIN_INT3_IRQn, BOARD_TEC4_IRQHandler);
}
bool Board_BUTTON_registerIRQHandler(uint8_t btnNo, btn_user_irq_handler_t cb)
{
	bool rst = false;

	/* Check if user callback is not already registered */
	if(NULL == g_btnIRQUserCb[btnNo])
	{
		/* Register user callback */
		g_btnIRQUserCb[btnNo] = cb;
		rst = true;
	}

	return rst;
}
bool Board_BUTTON_deregisterIRQHandler(uint8_t btnNo)
{
	bool rst = false;

	/* Check if user callback is not already deregistered */
	if(NULL != g_btnIRQUserCb[btnNo])
	{
		/* Deregister user callback */
		g_btnIRQUserCb[btnNo] = NULL;
		rst = true;
	}

	return rst;
}
bool Board_BUTTON_isPressed(uint8_t btnNo)
{
	if (btnNo < (sizeof(g_gpioBtnBits) / sizeof(io_port_t)))
		return (bool) !Chip_GPIO_GetPinState(LPC_GPIO_PORT, g_gpioBtnBits[btnNo].port, g_gpioBtnBits[btnNo].pin);

	return false;
}
uint32_t Board_BUTTON_GetStatus(void)
{
	uint8_t ret = 0;

	for (uint8_t i = 0; i < (sizeof(g_gpioBtnBits) / sizeof(io_port_t)); i++)
	{
		if (Chip_GPIO_GetPinState(LPC_GPIO_PORT, g_gpioBtnBits[i].port, g_gpioBtnBits[i].pin) == 0)
		{
			ret |= g_gpioBtnIDs[i];
		}
	}

	return ret;
}

/******************
 * UART functions *
 *****************/
static void Board_UART_Init(void)
{
	/* Initialize UART usb */
	uartUsbInit();
}
bool Board_UART_ReadByte(uint8_t* byte)
{
	return uartUsbReadByte(byte);
}
uint8_t Board_UART_ReadData(uint8_t* buff, const uint8_t expBytesLen, const uint32_t timeout)
{
	return uartUsbReadData(buff, expBytesLen, timeout);
}
bool Board_UART_WriteByte(const uint8_t byte)
{
	return uartUsbWriteByte(byte);
}
uint8_t Board_UART_WriteData(const uint8_t* buff, const uint32_t len)
{
	return uartUsbWriteData(buff, len);
}
uint8_t Board_UART_WriteString(const uint8_t* string)
{
	return uartUsbWriteString(string);
}

/* Set up and initialize all required blocks
 * and functions related to the board hardware */
void Board_Init(void)
{
	/* Initializes GPIO */
	Chip_GPIO_Init(LPC_GPIO_PORT);

	/* Initialize LEDs */
	Board_LED_Init();

	/* Initialize Buttons */
	Board_BUTTON_Init();

	/* Initialize UART usb */
	Board_UART_Init();
}
