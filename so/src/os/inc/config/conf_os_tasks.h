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
extern taskDefinition_t Tarea_1[];
extern taskDefinition_t Tarea_2[];
extern taskDefinition_t Tarea_3[];

OS_TASKS_DECLARE(Tarea_1,
				 Tarea_2,
				 Tarea_3);

#endif	/*	#ifndef _CONF_OS_TASKS_H_ */
