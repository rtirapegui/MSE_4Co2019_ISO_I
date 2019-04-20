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
extern taskDefinition_t Tarea_4[];
extern taskDefinition_t Tarea_5[];
extern taskDefinition_t Tarea_6[];
extern taskDefinition_t Tarea_7[];

OS_TASKS_DECLARE(Tarea_1,
				 Tarea_2,
				 Tarea_3,
				 Tarea_4,
				 Tarea_5,
				 Tarea_6,
				 Tarea_7);

#endif	/*	#ifndef _CONF_OS_TASKS_H_ */
