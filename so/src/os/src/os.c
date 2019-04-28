/*
 ===============================================================================
 Name        : os.c
 Author      : $(Rodrigo Tirapegui)
 Version     :
 Copyright   : $(copyright)
 Description :
===============================================================================
*/
#include "conf_os.h"
#include "os.h"
#include "chip.h"

/************************
 *	External functions	*
 ***********************/
/* From module: os_task.c */
extern void os_core_task_init(void);

/********************
 *	User functions	*
 *******************/
void os_start(void)
{
	/* Initialize system tasks */
	os_core_task_init();

	/* Configure PendSV interrupt priority */
	NVIC_SetPriority(PendSV_IRQn, (1 << __NVIC_PRIO_BITS) - 1);

	/* Update system core clock rate */
	SystemCoreClockUpdate();

	/* Initializes the System Timer and its interrupt and
	 * starts the System Tick Timer at user rate in milliseconds */
	SysTick_Config((SystemCoreClock / 1000)*OS_TICK_PERIOD_MS);

	while(1)
	{
		__WFI();
	}
}
