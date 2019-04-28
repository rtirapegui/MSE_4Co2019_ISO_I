/*
===============================================================================
 Name        : conf_os_tasks.h
 Author      : $(Rodrigo Tirapegui)
 Version     :
 Copyright   : $(copyright)
 Description :
===============================================================================
*/

#ifndef _CONF_OS_TASKS_H_
#define _CONF_OS_TASKS_H_

#include "os_task.h"

/* Declare below user tasks using OS_TASKS_DECLARE macro */

// @todo.rtirapegui.8.4.2019	Buscar la manera de generar los externs desde dentro de OS_TASKS_DECLARE
extern taskDefinition_t Processing_Task_name[];
extern taskDefinition_t LOG_Task_name[];
extern taskDefinition_t LED_Task_name[];

OS_TASKS_DECLARE(Processing_Task_name,
				 LOG_Task_name,
				 LED_Task_name);

#endif	/*	#ifndef _CONF_OS_TASKS_H_ */
