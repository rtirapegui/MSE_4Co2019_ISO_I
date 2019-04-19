/*
===============================================================================
 Name        : board_sysinit.c
 Author      : $(Rodrigo Tirapegui)
 Version     :
 Copyright   : $(copyright)
 Description :
===============================================================================
*/

#include "board.h"

/* The System initialization code is called prior to the application and
   initializes the board for run-time operation. */

/*********
 * Types *
 ********/

/* SPIFI high speed pin mode setup */
STATIC const PINMUX_GRP_T spifipinmuxing[] =
{
	{0x3, 3,  (SCU_PINIO_FAST | SCU_MODE_FUNC3)},	/* SPIFI CLK */
	{0x3, 4,  (SCU_PINIO_FAST | SCU_MODE_FUNC3)},	/* SPIFI D3 */
	{0x3, 5,  (SCU_PINIO_FAST | SCU_MODE_FUNC3)},	/* SPIFI D2 */
	{0x3, 6,  (SCU_PINIO_FAST | SCU_MODE_FUNC3)},	/* SPIFI D1 */
	{0x3, 7,  (SCU_PINIO_FAST | SCU_MODE_FUNC3)},	/* SPIFI D0 */
	{0x3, 8,  (SCU_PINIO_FAST | SCU_MODE_FUNC3)}	/* SPIFI CS/SSEL */
};

STATIC const PINMUX_GRP_T pinmuxing[] =
{
	/* RMII pin group */
	{0x7, 7, MD_EHS | MD_PLN | MD_EZI | MD_ZI |FUNC6},
	{0x1 ,17 , MD_EHS | MD_PLN | MD_EZI | MD_ZI| FUNC3},  // ENET_MDIO: P1_17 -> FUNC3
	{0x1 ,18 , MD_EHS | MD_PLN | MD_EZI | MD_ZI| FUNC3},  // ENET_TXD0: P1_18 -> FUNC3
	{0x1 ,20 , MD_EHS | MD_PLN | MD_EZI | MD_ZI| FUNC3},  // ENET_TXD1: P1_20 -> FUNC3
	{0x1 ,19 , MD_EHS | MD_PLN | MD_EZI | MD_ZI| FUNC0},  // ENET_REF: P1_19 -> FUNC0 (default)
	{0x0 ,1 , MD_EHS | MD_PLN | MD_EZI | MD_ZI| FUNC6},   // ENET_TX_EN: P0_1 -> FUNC6
	{0x1 ,15 ,MD_EHS | MD_PLN | MD_EZI | MD_ZI| FUNC3},   // ENET_RXD0: P1_15 -> FUNC3
	{0x0 ,0 , MD_EHS | MD_PLN | MD_EZI | MD_ZI| FUNC2},   // ENET_RXD1: P0_0 -> FUNC2
	{0x1 ,16 ,MD_EHS | MD_PLN | MD_EZI | MD_ZI| FUNC7}
};

/*********************
 * Private functions *
 ********************/
/* Sets up system pin muxing */
static void Board_SetupMuxing(void)
{
	/* Setup system level pin muxing */
	Chip_SCU_SetPinMuxing(pinmuxing, sizeof(pinmuxing) / sizeof(PINMUX_GRP_T));

	/* SPIFI pin setup is done prior to setting up system clocking */
	Chip_SCU_SetPinMuxing(spifipinmuxing, sizeof(spifipinmuxing) / sizeof(PINMUX_GRP_T));
}

/********************
 * Public functions *
 *******************/
/* Set up and initialize hardware prior to call to main */
void Board_SystemInit(void)
{
	/* Setup system clocking and memory. This is done early to allow the
	   application and tools to clear memory and use scatter loading to
	   external memory. */
	Board_SetupMuxing();
	Chip_SetupXtalClocking();
}
