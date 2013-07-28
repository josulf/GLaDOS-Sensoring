/*
 * check_temp.h
 *
 *  Created on: 2012 ira 6
 *      Author: emuxika
 */

#ifndef TEST_TEMP_H_
#define CHECK_TEMP_H_

#include <string.h>
#include <libMU/analog_digital.h>
#include <libMU/temperature.h>
#include <libMU/stdlib.h>

static __inline char* CheckTemperature(void);
static __inline char* CheckTemperature(void)
{
	static char msg[10];
	/* Mostrar temperatura */
	int Temp = libMU_Temperature_Get();
	libMU_snprintf	( msg, sizeof(msg), "% 2u.%u", Temp/10, Temp%10 );
	libMU_Display_DrawString( msg,
			libMU_Display_RES_X - libMU_Display_CHAR_WIDTH * strlen(msg),
			libMU_Display_RES_Y - libMU_Display_CHAR_HEIGHT + 1, 8 );
	return msg;
}

static __inline char* CheckTemperatureXY(const int X, const int Y);
static __inline char* CheckTemperatureXY(const int X, const int Y)
{
	static char msg[10];
	/* Mostrar temperatura */
	int Temp = libMU_Temperature_Get();
	libMU_snprintf( msg, sizeof(msg), "% 2u.%u", Temp/10, Temp%10 );
	libMU_Display_DrawString( msg, X, Y, 8 );
	return msg;
}

#endif /* TEST_TEMP_H_ */
