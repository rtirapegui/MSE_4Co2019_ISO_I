/*
===============================================================================
 Name        : main.c
 Author      : $(Rodrigo Tirapegui)
 Version     :
 Copyright   : $(copyright)
 Description :
===============================================================================
*/
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include "board.h"
#include "os.h"

/****************************
 *	Application Constants	*
 * *************************/

/* IRQ queue size */
#define IRQ_QUEUE_SIZE	10

/* Button defines */
#define TEC_INDEX_0		0
#define TEC_INDEX_1     1
#define TEC_INVALID     0xFF

/* LED defines */
#define LED_INDEX_0			LED_GREEN
#define LED_INDEX_1     	LED_RED
#define LED_INDEX_2     	LED_1
#define LED_INDEX_3     	LED_BLUE
#define LED_INDEX_INVALID	0xFF

#define LED_INDEX_0_COLOR	"Led Verde"
#define LED_INDEX_1_COLOR   "Led Rojo"
#define LED_INDEX_2_COLOR   "Led Amarillo"
#define LED_INDEX_3_COLOR   "Led Azul"
#define LED_COLOR_INVALID	NULL

/* Processing state machine */
typedef enum
{
   SM_WAITING_FIRST_FALLING_EDGE
,  SM_WAITING_SECOND_FALLING_EDGE
,  SM_WAITING_FIRST_RISING_EDGE
,  SM_WAITING_SECOND_RISING_EDGE
} sm_states_t;

/* Tasks stack size */
#define TASK_STACK_SIZE_BYTES   1024

/************************
 *	Application Types	*
 * *********************/

/* IRQ data type */
typedef struct
{
   uint8_t  tecIndex;
   bool     eventByFallingEdge;
   tick_t	eventTickstamp;
} IRQQueue_data_t;

/* LOG data type */
typedef struct
{
   uint8_t *ledColor;
   uint32_t ledOnIntervalMs;
   uint32_t tecFallingEdgeIntervalMs;
   uint32_t tecRaisingEdgeIntervalMs;
} LOG_data_t;

/* LED data type */
typedef struct
{
   uint8_t  ledIndex;
   uint32_t ledOnIntervalMs;
} LED_data_t;

/* Buttons state machine control data */
typedef struct
{
	tick_t  B0_fallingEdge;
	tick_t  B1_fallingEdge;
	bool    t1DoneFlag;
	tick_t	B0_risingEdge;
	tick_t  B1_risingEdge;
	bool    t2DoneFlag;
} sm_control_t;

/********************************
 *	Application Tasks creation	*
 *******************************/
/* Create task to process button pressed information */
OS_TASK_CREATE(Processing_Task_name, TASK_STACK_SIZE_BYTES, Processing_Task, (void *) 0, TASK_PRIORITY_IDLE + 2);

/* Create task to print measurements through UART when processing task indicates */
OS_TASK_CREATE(LOG_Task_name, TASK_STACK_SIZE_BYTES, LOG_Task, (void *) 0, TASK_PRIORITY_IDLE + 1);

/* Create task to blink LED when processing task indicates  */
OS_TASK_CREATE(LED_Task_name, TASK_STACK_SIZE_BYTES, LED_Task, (void *) 0, TASK_PRIORITY_IDLE + 1);

/********************************
 *	Application Queues creation	*
 *******************************/
/* Create queue to send IRQQueue_data_t packets between IRQ and Processing Task */
OS_QUEUE_CREATE(IRQ_queue, IRQ_QUEUE_SIZE, sizeof(IRQQueue_data_t));

/************************************
 *	Application Semaphores creation	*
 ***********************************/
/* Create semaphore to synchronize Processing and LOG tasks */
OS_SEMPHR_CREATE(Processing_LOG_semphr);

/* Create semaphore to synchronize data between Processing and LOG tasks */
OS_SEMPHR_CREATE(LOG_data_mutex);

/* Create semaphore to synchronize Processing and LED tasks */
OS_SEMPHR_CREATE(Processing_LED_semphr);

/* Create semaphore to synchronize data between Processing and LED tasks */
OS_SEMPHR_CREATE(LED_data_mutex);

/****************************
 *	Application Variables	*
 ***************************/
/* Variable to hold LOG data */
static LOG_data_t g_LOGData;

/* Variable to hold LED data */
static LED_data_t g_LEDData;

/************************
 *	Private functions	*
 ***********************/
static void initHardware(void)
{
	// Set up and initialize all required blocks and
    // functions related to the board hardware
	Board_Init();
}
static void serveIRQ(uint8_t tecIndex, btnIRQEdge_t edge)
{
	IRQQueue_data_t IRQDataElement;

	/* Store TEC index */
	IRQDataElement.tecIndex = tecIndex;

	/* Store event tickstamp */
	IRQDataElement.eventTickstamp = os_task_getTickCount();

	/* Save edge event type */
	IRQDataElement.eventByFallingEdge = (FALLING_EDGE == edge ? true : false);

	/* Store IRQ data into IRQ queue */
	os_queue_send(IRQ_queue, &IRQDataElement, 0);
}
static void TEC1_IRQ_Handler(btnIRQEdge_t edge)
{
	serveIRQ(TEC_INDEX_0, edge);
}
static void TEC2_IRQ_Handler(btnIRQEdge_t edge)
{
	serveIRQ(TEC_INDEX_1, edge);
}
static void runButtonsStateMachine(sm_states_t * const state, sm_control_t * const tickstamps, const IRQQueue_data_t * const irqData)
{
	/* Validate parameters sanity */
	if(state && tickstamps && irqData)
	{
		/* Evaluate Processing State Machine */
		switch(*state)
		{
		   case SM_WAITING_FIRST_FALLING_EDGE:
		   {
				/* Evaluate IRQElementData and advance state machine to its next state */

				if(true == irqData->eventByFallingEdge)
				{  /* Falling edge received */

					if(TEC_INDEX_0 == irqData->tecIndex)
					{
						tickstamps->B0_fallingEdge = irqData->eventTickstamp;
					}
					else if(TEC_INDEX_1 == irqData->tecIndex)
					{
						tickstamps->B1_fallingEdge = irqData->eventTickstamp;
					}

					/* Advance state machine */
					*state = SM_WAITING_SECOND_FALLING_EDGE;
				}
		   } break;
		   case SM_WAITING_SECOND_FALLING_EDGE:
		   {
			    if(true == irqData->eventByFallingEdge)
				{  /* Falling edge received */

					if(TEC_INDEX_0 == irqData->tecIndex)
					{
						tickstamps->B0_fallingEdge = irqData->eventTickstamp;
					}
					else if(TEC_INDEX_1 == irqData->tecIndex)
					{
						tickstamps->B1_fallingEdge = irqData->eventTickstamp;
					}

					/* Flag that falling edges measurement was done */
					tickstamps->t1DoneFlag = true;

					/* Advance state machine */
					*state = SM_WAITING_FIRST_RISING_EDGE;
				}
			    else
			    {	/* Rising edge received */

			    	/* Reset state machine */
			    	*state = SM_WAITING_FIRST_FALLING_EDGE;
			    }
		   } break;
		   case SM_WAITING_FIRST_RISING_EDGE:
		   {
			    if(false == irqData->eventByFallingEdge)
				{  /* Rising edge received */

					if(TEC_INDEX_0 == irqData->tecIndex)
					{
						tickstamps->B0_risingEdge = irqData->eventTickstamp;
					}
					else if(TEC_INDEX_1 == irqData->tecIndex)
					{
						tickstamps->B1_risingEdge = irqData->eventTickstamp;
					}

					/* Advance state machine */
					*state = SM_WAITING_SECOND_RISING_EDGE;
				}
				else
				{	/* Falling edge received */

					/* Reset state machine */
					*state = SM_WAITING_FIRST_FALLING_EDGE;
				}
		   } break;
		   case SM_WAITING_SECOND_RISING_EDGE:
		   {
			    if(false == irqData->eventByFallingEdge)
				{  /* Rising edge received */

					if(TEC_INDEX_0 == irqData->tecIndex)
					{
						tickstamps->B0_risingEdge = irqData->eventTickstamp;
					}
					else if(TEC_INDEX_1 == irqData->tecIndex)
					{
						tickstamps->B1_risingEdge = irqData->eventTickstamp;
					}

					/* Flag that rising edges measurement was done */
					tickstamps->t2DoneFlag = true;
				}

				/* Reset state machine */
				*state = SM_WAITING_FIRST_FALLING_EDGE;
		   } break;
		   default:
		   {
			  *state = SM_WAITING_FIRST_FALLING_EDGE;
		   } break;
		}
	}
}
static bool processStateMachineResults(sm_control_t * const tickstamps, uint32_t *t1, uint32_t *t2, uint8_t *firstPressTecIndex, uint8_t *firstReleaseTecIndex)
{
	#define DEBOUNCE  40 	// In ms

	/* Check parameters sanity */
	if(tickstamps)
	{
		uint8_t _firstPressTecIndex, _firstReleaseTecIndex;
		uint32_t _t1, _t2;

		/* Check if both buttons were pressed and released in a valid combination */
		if((true == tickstamps->t1DoneFlag) && (true == tickstamps->t2DoneFlag))
		{
			/* Determine t1 interval */
			if(tickstamps->B1_fallingEdge > tickstamps->B0_fallingEdge)
			{
				_t1 = (tickstamps->B1_fallingEdge - tickstamps->B0_fallingEdge);

				_firstPressTecIndex = TEC_INDEX_0;
			}
			else
			{
				_t1 = (tickstamps->B0_fallingEdge - tickstamps->B1_fallingEdge);

				_firstPressTecIndex = TEC_INDEX_1;
			}

			/* Debounce logic for t1 */
			if(DEBOUNCE / OS_TICK_PERIOD_MS < _t1)
			{
				/* Measurement is valid, so save it */
				_t1 *= OS_TICK_PERIOD_MS;  // In ms

				/* Store t1 in ms */
				if(t1)
					*t1 = _t1;
			}

			/* Determine t2 interval */
			if(tickstamps->B1_risingEdge > tickstamps->B0_risingEdge)
			{
				_t2 = (tickstamps->B1_risingEdge - tickstamps->B0_risingEdge);

				_firstReleaseTecIndex = TEC_INDEX_0;
			}
			else
			{
				_t2 = (tickstamps->B0_risingEdge - tickstamps->B1_risingEdge);

				_firstReleaseTecIndex = TEC_INDEX_1;
			}

			/* Debounce logic for t2 */
			if(DEBOUNCE / OS_TICK_PERIOD_MS < _t2)
			{
				/* Measurement is valid, so save it */
				_t2 *= OS_TICK_PERIOD_MS;	// In ms

				/* Strore t2 in ms */
				if(t2)
					*t2 = _t2;
			}

			/* Clear falling edge measurement flag */
			tickstamps->t1DoneFlag = false;

			/* Clear rising edge measurement flag */
			tickstamps->t2DoneFlag = false;

			/* Store first press tec */
			if(firstPressTecIndex)
				*firstPressTecIndex = _firstPressTecIndex;

			/* Store first release tec */
			if(firstReleaseTecIndex)
				*firstReleaseTecIndex = _firstReleaseTecIndex;

			return true;
		}
	}

	return false;
}
static void getLEDIndexAndColor(const uint8_t firstPressTecIndex, const uint8_t firstReleaseTecIndex, uint8_t * const ledIndex, uint8_t ** ledColor)
{
	uint8_t _ledIndex = LED_INDEX_INVALID;
	uint8_t *_color = LED_COLOR_INVALID;

	/* Get LED index to turn on and string indicating LED color */
	if(TEC_INDEX_0 == firstPressTecIndex)
	{
		if(TEC_INDEX_0 == firstReleaseTecIndex)
		{
			_ledIndex = LED_INDEX_0;
			_color = (uint8_t *) LED_INDEX_0_COLOR;
		}
		else
		{
			_ledIndex = LED_INDEX_1;
			_color = (uint8_t *) LED_INDEX_1_COLOR;
		}
	}
	else
	{
		if(TEC_INDEX_0 == firstReleaseTecIndex)
		{
			_ledIndex = LED_INDEX_2;
			_color = (uint8_t *) LED_INDEX_2_COLOR;
		}
		else
		{
			_ledIndex = LED_INDEX_3;
			_color = (uint8_t *) LED_INDEX_3_COLOR;
		}
	}

	if(ledIndex)
		*ledIndex = _ledIndex;

	if(ledColor)
		*ledColor = _color;
}
static uint32_t formatVariadicString(uint8_t *buf, uint32_t bufSize, const char * format, ...)
{
   int32_t size;
   va_list arg;

   va_start(arg, format);
   size = vsnprintf((char *)buf, (size_t) bufSize, (const char *) format, arg);
   va_end(arg);

   if(size < 0)
      size = 0;

   return (uint32_t) size;
}

/********************
 *	Tasks functions	*
 *******************/
static void * Processing_Task(void *arg)
{
	IRQQueue_data_t IRQDataElement;
	sm_states_t	 smState = SM_WAITING_FIRST_FALLING_EDGE;
	sm_control_t smControlData = {
									.B0_fallingEdge = 0,
									.B1_fallingEdge = 0,
									.t1DoneFlag = false,
									.B0_risingEdge = 0,
									.B1_risingEdge = 0,
									.t2DoneFlag = false
								 };
	uint8_t *ledColor = NULL;
	uint8_t ledIndex, firstPressTecIndex, firstReleaseIndex;
	uint32_t t1 = 0, t2 = 0;

	while(true)
	{
		/* Wait until button pressed/release event is received */
		if(true == os_queue_receive(IRQ_queue, &IRQDataElement, OS_TICK_MAX))
		{
			/* Capture buttons press and release tickstamps */
			runButtonsStateMachine(&smState, &smControlData, &IRQDataElement);

			/* Evaluate if both buttons were released in a valid combination */
			if(true == processStateMachineResults(&smControlData, &t1, &t2, &firstPressTecIndex, &firstReleaseIndex))
			{
				uint32_t ledOnIntervalMs = t1 + t2;

				/* Verify LED needs to be turned on */
				if(0 < ledOnIntervalMs)
				{
					/* Get LED index to be turned on */
					getLEDIndexAndColor(firstPressTecIndex, firstReleaseIndex, &ledIndex, &ledColor);

					if((LED_INDEX_INVALID != ledIndex) && (LED_COLOR_INVALID != ledColor))
					{
						/* Try to get access to g_LEDData */
						if(true == os_semphr_wait(LED_data_mutex, 0))
						{
							/* Complete LED data structure */
							g_LEDData.ledIndex = ledIndex;
							g_LEDData.ledOnIntervalMs = ledOnIntervalMs;

							/* Release access to g_LEDData */
							os_semphr_post(LED_data_mutex);

							/* Give LED_Task semaphore */
							os_semphr_post(Processing_LED_semphr);
						}

						/* Try to get access to g_LOGData */
						if(true == os_semphr_wait(LOG_data_mutex, 0))
						{
							/* Complete LOG data structure */
							g_LOGData.ledColor = ledColor;
							g_LOGData.ledOnIntervalMs = ledOnIntervalMs;
							g_LOGData.tecFallingEdgeIntervalMs = t1;
							g_LOGData.tecRaisingEdgeIntervalMs = t2;

							/* Release access to g_LOGData */
							os_semphr_post(LOG_data_mutex);

							/* Give LOG_Task semaphore */
							os_semphr_post(Processing_LOG_semphr);
						}
					}
				}
			}
		}
	}

	return NULL;
}
static void * LOG_Task(void *arg)
{
	#define MSG_SIZE	256

	while(true)
	{
		/* Wait until Processing task free semaphore */
		if(true == os_semphr_wait(Processing_LOG_semphr, OS_TICK_MAX))
		{
			LOG_data_t LOGData = {
									.ledColor = LED_COLOR_INVALID,
									.ledOnIntervalMs = 0,
									.tecFallingEdgeIntervalMs = 0,
									.tecRaisingEdgeIntervalMs = 0
								 };

			/* Try get access to g_LOGData */
			if(true == os_semphr_wait(LOG_data_mutex, 0))
			{
				/* Save LED index and ON interval */
				LOGData = g_LOGData;

				/* Release access to g_LOGData */
				os_semphr_post(LOG_data_mutex);
			}

			if(LED_COLOR_INVALID != LOGData.ledColor)
			{
				static uint8_t msg[MSG_SIZE];

				/* Clear message buffer */
				memset(msg, 0, sizeof(msg));

				if(0 < formatVariadicString(msg, sizeof(msg), "%s encendido:\n\r"
															  "\t Tiempo encendido: %u ms\n\r"
															  "\t Tiempo entre flancos descendentes: %u ms\n\r"
															  "\t Tiempo entre flancos ascendentes: %u ms\n\r", (const char *) LOGData.ledColor,
																											   LOGData.ledOnIntervalMs,
																											   LOGData.tecFallingEdgeIntervalMs,
																											   LOGData.tecRaisingEdgeIntervalMs))
				{
					/* Send message through UART */
					Board_UART_WriteString(msg);
				}
			}
		}
	}

	return NULL;
}
static void * LED_Task(void *arg)
{
	while(true)
	{
		/* Wait until Processing task free semaphore */
		if(true == os_semphr_wait(Processing_LED_semphr, OS_TICK_MAX))
		{
			LED_data_t LEDData = {
									.ledIndex = LED_INDEX_INVALID,
									.ledOnIntervalMs = 0
								 };

			/* Try get access to g_LEDData */
			if(true == os_semphr_wait(LED_data_mutex, 0))
			{
				/* Save LED index and ON interval */
				LEDData = g_LEDData;

				/* Release access to g_LEDData */
				os_semphr_post(LED_data_mutex);
			}

			/* Validate LEDData sanity */
			if((LED_INDEX_INVALID != LEDData.ledIndex) && (0 != LEDData.ledOnIntervalMs))
			{
				Board_LED_Set(LEDData.ledIndex, true);
				os_task_delay(LEDData.ledOnIntervalMs / OS_TICK_PERIOD_MS);
				Board_LED_Set(LEDData.ledIndex, false);
			}
		}
	}

	return NULL;
}

/************************
 *	Public functions	*
 ***********************/
int main(void)
{
	/* Initialize board hardware */
	initHardware();

	/* Mutexes start released */
	os_semphr_post(LOG_data_mutex);

	/* Mutexes start released */
	os_semphr_post(LED_data_mutex);

	/* Register TEC 1 interrupt handler */
	Board_BUTTON_registerIRQHandler(TEC_1, TEC1_IRQ_Handler);

	/* Register TEC 2 interrupt handler */
	Board_BUTTON_registerIRQHandler(TEC_2, TEC2_IRQ_Handler);

	/* Stay forever */
	os_start();
}
