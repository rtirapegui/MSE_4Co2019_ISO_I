	/**
	 * Assembly directive to look for the better instruction (thumb and thumb2)
	 */
	.syntax unified

	/**
	 * .text Indicate code section
	 */
	.text

	/**
	 * .global Define exportable symbol (extern equivalent).
     */
	.global PendSV_Handler

	.extern os_core_task_getNextContent

	/**
	 * Indicate the next subroutine must be assembled in thumb mode, so las bit instructions will have
	 * 1 to be recognized as a thumb instruction.
	 */
	.thumb_func

PendSV_Handler:

		cpsid   i							/* Disable interrupts					*/

		tst 	lr,0x10
		it		eq
		vpusheq	{s16-s31}

		push	{r4-r11, lr}

		mrs		r0,msp						/*	r0 = msp							*/
		bl		os_core_task_getNextContext	/*	os_core_task_getNextContext(msp)	*/

		msr		msp,r0						/*	msp = sp1							*/

		pop		{r4-r11, lr}

		tst 	lr,0x10
		it 		eq
		vpopeq	{s16-s31}

		cpsie   i							/* Enable interrupts					*/

		bx 		lr   						/*	return								*/

