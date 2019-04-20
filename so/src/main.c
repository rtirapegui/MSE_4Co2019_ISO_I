/*
===============================================================================
 Name        : main.c
 Author      : $(Rodrigo Tirapegui)
 Version     :
 Copyright   : $(copyright)
 Description :
===============================================================================
*/
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include "board.h"
#include "os.h"

/****************
 *	Constants	*
 * *************/
#define STACK_SIZE_BYTES   2048

/**********************
 *	Application Tasks *
 *********************/

/*
 * Ejercicio 7
 *
 * No se cuenta con la implementacion de mutex, por lo que se utilizara un
 * semaforo binario en su lugar.
 * Al colocar la prioridad de todas la tareas en el mismo valor, no se necesita
 * el concepto de priority inheritance; con lo que un semaforo binario resulta ser
 * suficiente.
 * Además, como al crearlo se inicia tomado, se libera en el main del programa.
 */

OS_TASK_CREATE(Tarea_1, STACK_SIZE_BYTES, HumiditySensor1_Task, (void *) 0, TASK_PRIORITY_IDLE + 1);
OS_TASK_CREATE(Tarea_2, STACK_SIZE_BYTES, HumiditySensor2_Task, (void *) 0, TASK_PRIORITY_IDLE + 1);
OS_TASK_CREATE(Tarea_3, STACK_SIZE_BYTES, HumiditySensor3_Task, (void *) 0, TASK_PRIORITY_IDLE + 1);
OS_TASK_CREATE(Tarea_4, STACK_SIZE_BYTES, TemperatureSensor1_Task, (void *) 0, TASK_PRIORITY_IDLE + 1);
OS_TASK_CREATE(Tarea_5, STACK_SIZE_BYTES, TemperatureSensor2_Task, (void *) 0, TASK_PRIORITY_IDLE + 1);
OS_TASK_CREATE(Tarea_6, STACK_SIZE_BYTES, TemperatureSensor3_Task, (void *) 0, TASK_PRIORITY_IDLE + 1);
OS_TASK_CREATE(Tarea_7, STACK_SIZE_BYTES, TemperatureSensor4_Task, (void *) 0, TASK_PRIORITY_IDLE + 1);

OS_SEMPHR_CREATE(Sem_UART);

/*************
 * Constants *
 ************/
#define MSG_SIZE_MAX			64
#define GREENHOUSE_COUNT_MAX	20
#define VALUE_MAX				100

enum sensor_t
{
	HUMIDITY_SENSOR,
	TEMPERATURE_SENSOR
};
/************************
 *	Private functions	*
 ***********************/
static void initHardware(void)
{
	// Set up and initialize all required blocks and
    // functions related to the board hardware
	Board_Init();
}
static uint32_t formatVariadicString(uint8_t *buff, uint32_t buffSize, const uint8_t *format, ...)
{
	va_list arg;
	int32_t size;

	va_start(arg, format);
	size = vsnprintf((char *) buff, (size_t) buffSize, (const char *) format, arg);
	va_end(arg);

	if(0 > size)
		size = 0;

	return (uint32_t) size;
}
static void getTime(uint8_t *hour, uint8_t *minute, uint8_t *second)
{
	tick_t currTick = (os_task_getTickCount() * OS_TICK_PERIOD_MS);	// In ms

	currTick /= 1000;	// In seconds

	if(second)
		*second = currTick%60;

	currTick /= 60;		// In minutes

	if(minute)
		*minute = currTick%60;

	currTick /= 60;		// In hours

	if(hour)
		*hour = currTick%24;
}
static void sendMeasurement(uint32_t sensorNo, enum sensor_t sensorTypeIndex)
{
	tick_t delayTicks;
	uint8_t msg[MSG_SIZE_MAX];
	uint8_t hour, minute, second;
	char * sensorType[] = {
							"HUM",
							"TEMP"
						   };

	char * unit[] = {
						"%%",
						"T"
					};

	if(true == os_semphr_wait(Sem_UART, 0))
	{
		// Get current Time
		getTime(&hour, &minute, &second);

		delayTicks = (rand() % 50);

		if(5 > delayTicks)
			delayTicks += 5;

		os_task_delay(delayTicks / OS_TICK_PERIOD_MS);

		// Format and send string
		if(0 < formatVariadicString(msg, sizeof(msg), (const uint8_t *)"[%2u:%2u:%2u][INVERNADERO:%3u][%s:%u][%2u%s]\r\n", hour,
																														  minute,
																														  second,
																														  rand() % GREENHOUSE_COUNT_MAX,
																														  sensorType[sensorTypeIndex],
																														  sensorNo,
																														  rand() % VALUE_MAX,
																														  unit))
		{
			Board_UART_WriteString(msg);
		}

		// Free semaphore
		os_semphr_post(Sem_UART);
	}
}

uint8_t buffMsg[100];

/**********
 *	Tasks *
 *********/
void * HumiditySensor1_Task(void *arg)
{
	const uint32_t sensorNo = 1;

	while(true)
	{
		sendMeasurement(sensorNo, HUMIDITY_SENSOR);
	}

	return NULL;
}
void * HumiditySensor2_Task(void *arg)
{
	const uint32_t sensorNo = 2;

	while(true)
	{
		sendMeasurement(sensorNo, HUMIDITY_SENSOR);
	}

	return NULL;
}
void * HumiditySensor3_Task(void *arg)
{
	const uint32_t sensorNo = 3;

	while(true)
	{
		sendMeasurement(sensorNo, HUMIDITY_SENSOR);
	}

	return NULL;
}
void * TemperatureSensor1_Task(void *arg)
{
	const uint32_t sensorNo = 1;

	while(true)
	{
		sendMeasurement(sensorNo, TEMPERATURE_SENSOR);
	}

	return NULL;
}
void * TemperatureSensor2_Task(void *arg)
{
	const uint32_t sensorNo = 2;

	while(true)
	{
		sendMeasurement(sensorNo, TEMPERATURE_SENSOR);
	}

	return NULL;
}
void * TemperatureSensor3_Task(void *arg)
{
	const uint32_t sensorNo = 3;

	while(true)
	{
		sendMeasurement(sensorNo, TEMPERATURE_SENSOR);
	}

	return NULL;
}
void * TemperatureSensor4_Task(void *arg)
{
	const uint32_t sensorNo = 4;

	while(true)
	{
		sendMeasurement(sensorNo, TEMPERATURE_SENSOR);
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

	/* Free semaphore for the first time */
	os_semphr_post(Sem_UART);

	/* Stay forever */
	os_start();
}

