	/**
	 * Directiva al ensablador que permite indicar que se encarga de buscar
	 * la instruccion mas apropiada entre thumb y thumb2
	 */
	.syntax unified

	/**
	 * .text permite indicar una seccion de codigo.
	 */
	.text

	/**
	 * .global permite definir un simbolo exportable,
	 * es decir que podemos verlo desde otros modulos (equivalente a extern).
     * Definimos la rutina como global para que sea visible desde otros modulos.
     */
	.global SysTick_Handler

	.extern getNextContent

	/**
	 * Indicamos que la siguiente subrutina debe ser ensamblada en modo thumb,
	 * entonces en las direcciones en el ultimo bit tendran el 1 para que se reconozcan como en modo thumb.
	 * Siempre hay que ponerla antes de la primer instruccion.
	 */
	.thumb_func

SysTick_Handler:
		push	{r4-r11, lr}

		mrs		r0,msp				/*	r0 = msp				*/
		bl		getNextContext		/*	getNextContext(msp)		*/

		msr		msp,r0				/*	msp = sp1				*/

		pop		{r4-r11, lr}

		bx 		lr   				/*	return	*/

