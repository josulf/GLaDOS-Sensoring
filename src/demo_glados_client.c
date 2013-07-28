/********************************************************************
 * @author		Josu Lopez <josu.lopez.fernandez@gmail.com>
 * @date		2013/07/28
 * @copyright	BSDL
 ********************************************************************
 */

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <libMU/analog_digital.h>
#include <libMU/buttons.h>
#include <libMU/display.h>
#include <libMU/internet.h>
#include <libMU/timer.h>
#include <libMU/leds.h>
#include <libMU/gpio.h>

void main_demo_glados_client();
static void Internet_ProcesamientoDePagina( char* data, unsigned short int len ) ;
static void mainTask (void *param);

/*
 * Component start up. From demo_09_Internet.c
 */
void main_demo_glados_client()
{
	libMU_Display_Initialize();

	libMU_Display_DrawString("GLaDOSS Client - EK21", 0, 0, 15);
	libMU_Display_DrawString("=====================", 0, 8, 6);
	libMU_Display_DrawString("=====================", 0, 80, 6);

	libMU_Internet_Initialize();
	/* Crea una tarea que se ejecutara en paralelo con la tarea de comunicaciones de Internet */
	libMU_Internet_CreateTask(mainTask);

	/* Esta libreria es diferente, arranca un modo ejecución en paralelo
	 * para poder procesar las tramas de internet en paralelo con el programa principal
	 * y nunca debería volver */
	libMU_Internet_Start();
}

/*
 * GetPage generated task
 */
static void Internet_ProcesamientoDePagina( char* data, unsigned short int len )
{
	char *tilt_s, *temp_s, *light_s;
	int tilt=0u;
	float temp=0u, light=0u;
	char msg[50];

	// Hay que buscar la segunda coma
	if (len < 100)
	{
		tilt_s = strstr(data, ",") + 1u;
		temp_s = strstr(tilt_s, ",") + 1u;
		light_s = strstr(temp_s, ",") + 1u;

		tilt = atoi(tilt_s);
		temp = atof(temp_s);
		light = atof(light_s);

		libMU_snprintf(msg, sizeof(msg), "Tilt: %d", tilt);
		libMU_Display_DrawString(msg, 0, 16, 15);
		libMU_snprintf(msg, sizeof(msg), "Temp: %.1f C", temp);
		libMU_Display_DrawString(msg, 0, 24, 15);
		libMU_snprintf(msg, sizeof(msg), "Light: %.1f %%", light);
		libMU_Display_DrawString(msg, 0, 32, 15);
	}
}

/*
 * Get data from google spreadsheet
 */
static void get_current_data(void)
{
	int res;
	IPAddress_t ip;

	libMU_Internet_DNS_resolution("trastero.fregona.biz", &ip, 10000);

	res = libMU_Internet_GetPage("http://trastero.fregona.biz/glados/current.csv", Internet_ProcesamientoDePagina);
	if (!res)
	{
		libMU_Display_DrawString("Retreive ERROR    ", 0, 88, 15);
	}

	libMU_Internet_Delay(3000);
}

/*
 * Main task running http get commands and data mangement
 */
static void mainTask (void *param)
{
	IPAddress_t ip;
	char msg[50];

	while(1)
	{
		switch( libMU_Internet_GetStatus() )
		{
			case NETWORK_NO_CONNECTION:
				libMU_Display_DrawString("NO connection!", 0, 88, 15 );
				libMU_Internet_Delay( 250 );
				break;
			case NETWORK_IP_MISSING:
				libMU_Display_DrawString("IP missing!", 0, 88, 12 );
				libMU_Internet_Delay( 250 );
				libMU_snprintf( msg, sizeof(msg), "IP<%d.%d.%d.%d>    ",
														libMU_IP_1( ip ), libMU_IP_2( ip ),
														libMU_IP_3( ip ), libMU_IP_4( ip ) );
				break;

			case NETWORK_IP_OK:
				ip = libMU_Internet_GetDeviceIP();
				libMU_snprintf(msg, sizeof(msg), "IP<%d.%d.%d.%d>    ",
							libMU_IP_1( ip ), libMU_IP_2( ip ),
							libMU_IP_3( ip ), libMU_IP_4( ip ) );
				libMU_Display_DrawString( msg, 0, 88, 12 );

				get_current_data();

				libMU_Internet_Delay( 100 );
				break;

			case NETWORK_ERROR:
			default:
				libMU_Display_DrawString( "Network ERROR", 0, 88, 12 );
				libMU_Internet_Delay( 250 );
				break;
		}
	}
}
