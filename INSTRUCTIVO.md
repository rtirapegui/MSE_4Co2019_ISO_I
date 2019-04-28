# Descripción del programa

El proyecto fue desarrollado utilizando MCUXpresso IDE v10.3.1_2233.

El proyecto consta de las carpetas:

-	bsp (Board support package): Implementa las rutinas para configurar y utilizar la EDU-CIAA (LEDs, botones y UART USB).
								 El usuario puede configurar los parámetros de funcionamiento de la UART USB desde el archivo conf_uart_usb.h.
-	os: Incluye los fuentes .c y .h del sistema operativo desarrollado durante el cursado de la materia. 
		El usuario puede configurar los parámetros de operación del OS desde el archivo conf_os.h.
		El usuario debe declarar las tareas que conforman su aplicación en el archivo conf_os_tasks.h.

La resolución del ejercicio se realiza en main.c.

**UART BAUDRATE:** 115200
