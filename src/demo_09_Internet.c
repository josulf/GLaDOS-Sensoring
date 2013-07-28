/**
 * @addtogroup demos
 * Test de las comunicaciones Internet mediante el uso de un
 * servidor web y consulta a una página de Internet
 * @{
 ********************************************************************
 * @author		Eñaut Muxika <emuxika at mondragon dot edu>
 * @date		2011/7/10
 * @copyright	BSDL
 ********************************************************************
 */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>  
#include <ctype.h>
#include <libMU/buttons.h>
#include <libMU/display.h>
#include <libMU/internet.h>
#include <libMU/timer.h>
#include <libMU/leds.h>
#include "check_temp.h"

/**
 * Funciones locales
 */
void main_demo_09(void);
static void		ProgramaPrincipal(void *param);
static void		Internet_ProcesamientoDePagina( char* data, unsigned short int len );
static char*	Internet_ProcesamientoDeFormulario( const char* tag, int* len );

/**
 * Variables locales (datos de la página leída)
 */
static int hora;
static int minutos;
static int bytes_recibidos;

/**
 * Variables locales (paginas Web del servidor)
 */
static char respuesta_datos[100] =
	"<html><body>Tenperatura = 10.0 <br>"
	"</body></html>"
;
static char respuesta_temp[100] = "\n"
	"Esto es una prueba de temperatura\n"
	"Temperatura = 10.0 C\n"
;
static char respuesta_form[] =
	"<html><body>Tenperatura = 10.0 / LED = OFF<br>"
	"<form method=GET name=scrMsg action=cmd.html>"
	"Message:<input type=text name=msg value=\"Hey\" size=10 maxlength=10 />"
	"<input type=submit value=\"Send Screen Message\" /><br/>"
	"Position: (x=<input name=x value=\"0\" size=3 maxlength=3 />,"
	"y=<input name=y value=\"8\" size=3 maxlength=3 />)"
	"Gray intensity: <input name=col value=\"8\" size=2 maxlength=2 /><br />"
	"</form>"
	"<form method=GET name=ledMsg action=cmd.html>"
	"<input type=hidden name=led value=toggle />"
	"<input type=submit value=\"Toggle LED\" /></form>"
	"</body></html>"
;

/**
 * Programa principal
 */
void main_demo_09(void)
{
	libMU_Display_Initialize();
	libMU_Timer_Initialize();
	libMU_AD_Initialize();
	libMU_LED_Initialize();
	libMU_Button_Initialize();
		
	libMU_Display_DrawString	( "Guten morgen!!", 0, 0, 15 );

	libMU_Internet_Initialize();
	/* Crea una tarea que se ejecutara en paralelo con la tarea de comunicaciones de Internet */
	libMU_Internet_CreateTask(ProgramaPrincipal);

	/* Esta libreria es diferente, arranca un modo ejecución en paralelo
	 * para poder procesar las tramas de internet en paralelo con el programa principal
	 * y nunca debería volver */
	libMU_Internet_Start();
}

/**
 * Una vez que se inicializa internet el resto del programa sigue aqui
 */
static void ProgramaPrincipal(void *param)
{
	libMU_Timer_Stopwatch_t stopwatch; libMU_Time_t elapsed;
	enum { GET_CLOCK_IP, GET_WEBPAGE, WAIT_WEBPAGE, SHOW_CONN_STATUS } what_to_do;

	IPAddress_t ip; int res = 0;
	char msg[50];
	static const char* mensajes_ip[] = {
		"Esperando IP   ",
		"Esperando IP.  ",
		"Esperando IP.. ",
		"Esperando IP..."
	};
	static const char* mensajes_con[] = {
		"Sin conexion   ",
		"Sin conexion.  ",
		"Sin conexion.. ",
		"Sin conexion..."
	};
	what_to_do = GET_CLOCK_IP;

	/* Configuración del servidor WEB */
	libMU_Internet_Server_SetPage( "/datos.html", respuesta_datos, strlen(respuesta_datos) );  
	libMU_Internet_Server_SetPage( "/temp.txt", respuesta_temp, strlen(respuesta_temp) );
	libMU_Internet_Server_SetPage( "/form.html", respuesta_form, sizeof(respuesta_form) );
	libMU_Internet_Server_SetCommandProcessingInfo( "/cmd.html", Internet_ProcesamientoDeFormulario );
	
	for(;;) {
		switch( libMU_Internet_GetStatus() ) {
		case NETWORK_NO_CONNECTION:
			libMU_AD_StartConversion();
			res &= 3;
			libMU_Display_DrawString( mensajes_con[res++], 0, 0, 15 );
			CheckTemperature();
			what_to_do = GET_CLOCK_IP;
			libMU_Internet_Delay( 250 );
			break;

		case NETWORK_IP_MISSING:
			libMU_AD_StartConversion();
			res &= 3;
			libMU_Display_DrawString( mensajes_ip[res++], 0, 0, 15 );
			CheckTemperature();
			what_to_do = GET_CLOCK_IP;
			libMU_Internet_Delay( 250 );
			break;

		case NETWORK_IP_OK:
			ip = libMU_Internet_GetDeviceIP();
			libMU_snprintf( msg, sizeof(msg), "IP<%d.%d.%d.%d>    ",
						libMU_IP_1( ip ), libMU_IP_2( ip ),
						libMU_IP_3( ip ), libMU_IP_4( ip ) );
			libMU_Display_DrawString( msg, 0, 0, 12 );
#if defined(USE_WIFI)
			ip = libMU_Internet_GetWiFiRouterIP();
			if( ip != 0 ) {
				libMU_snprintf( msg, sizeof(msg), "WF<%d.%d.%d.%d>    ",
							libMU_IP_1( ip ), libMU_IP_2( ip ),
							libMU_IP_3( ip ), libMU_IP_4( ip ) );
				libMU_Display_DrawString( msg, 0, 0, 15 );
			}
#endif
			switch( what_to_do ) {
			case GET_CLOCK_IP:
				libMU_Internet_DNS_resolution( "www.worldtimeserver.com", &ip, 10000 );
				libMU_Display_DrawString( "worldtimeserver.com", 0, 8, 12 );
				libMU_snprintf( msg, sizeof(msg), "  <%d.%d.%d.%d>",
							libMU_IP_1( ip ), libMU_IP_2( ip ),
							libMU_IP_3( ip ), libMU_IP_4( ip ) );
				libMU_Display_DrawString( msg, 0, 16, 15 );
				bytes_recibidos = 0;
				libMU_Timer_StopwatchStart(&stopwatch);
				res = libMU_Internet_GetPage (
							"http://www.worldtimeserver.com/mobile/mobile_time_in_ES.aspx?city=Madrid",
							Internet_ProcesamientoDePagina	/* Función llamada por cada trozo de página recibida */
						);
				if( !res ) {
					libMU_Display_DrawString( "Error de conexion", 0, 24, 15 );
					what_to_do = SHOW_CONN_STATUS;
				}else{
					what_to_do = WAIT_WEBPAGE;
				}
				break;
			case WAIT_WEBPAGE:
				libMU_AD_StartConversion();
				libMU_snprintf( msg, sizeof(msg), "Bytes rcvd:% 6d", bytes_recibidos );
				libMU_Display_DrawString( msg, 0, 24, 15 );
				CheckTemperature();
				if( libMU_Internet_IsPageReadingFinished() ) {
					elapsed = libMU_Timer_StopwatchMeasure(&stopwatch);
					libMU_snprintf( msg, sizeof(msg), "Hora = %d:%02d (%ums)", hora, minutos,
							libMU_Timer_ConvertTo_ms(elapsed) );
					libMU_Display_DrawString( msg, 0, 32, 15 );
					what_to_do = SHOW_CONN_STATUS;
				}
				break;
			case SHOW_CONN_STATUS:
			default:
				libMU_AD_StartConversion();
				libMU_Internet_Delay( 100 );
				/* Mostrar */
				libMU_snprintf( msg, sizeof(msg), "R/S:%d/%d",
					libMU_Internet_GetNumberOfFramesReceived(),
					libMU_Internet_GetNumberOfFramesSent() );
				libMU_Display_DrawString( msg, 0, 40, 15 );
				char* m = CheckTemperature();
				/* update info */
				strncpy( &respuesta_datos[26], m, 4 );
				strncpy( &respuesta_form[26], m, 4 );
				strncpy( &respuesta_temp[49], m, 4 );
				break;
			}
			libMU_Internet_Delay( 100 );
			break;

		case NETWORK_ERROR:
		default:
			libMU_Display_DrawString( "Error!!!   ", 0, 8, 15 );
			what_to_do = GET_CLOCK_IP;
			libMU_Internet_Delay( 250 );
			break;
		}		
	}
}

/**
 * Función para el tratamiento de datos descargados de Internet.
 * @param	data	Puntero a los datos descargados
 * @param	len		Tamaño de los datos descargados
 * @note
 * Debido a limitaciones de memoria, la página descargada no se almacena y hay
 * que procesarla según llegan los datos.
 * Esta función se llama varias veces por cada página excepto para aquellas
 * páginas que tengan un tamaño menor de 1kB (aprox.).
 * @see libMU_Internet_GetPage()
 */
static void Internet_ProcesamientoDePagina( char* data, unsigned short int len ) 
{
	bytes_recibidos += len;
	data[len-1] = 0;
	
	/* Find specific string in the received packet */
	char *loc = strstr(data, "\"font7\"" );
	if( loc == NULL ) return;
	
	loc += 7; char *end = data + len;
	/* Skip everything until we find digits */
	while( !isdigit( *loc ) && loc < end ) loc++;
	/* Convert hours to number */
	hora = atoi( loc );
	/* Skip digits  */
	loc++; while( isdigit( *loc ) && loc < end ) loc++;
	/* check if ':' present */
	if( loc[0] != ':' ) { hora = 0; return; } 
	loc++;
	/* Convert minutes to number */
	minutos = atoi( loc );
}

/**
 * Función para el tratamiento de formularios con metodo (GET)
 * @param	cmd_param	Parametros del comando GET (empezando desde el caracter '?')
 * @param	len			Puntero a un entero donde se guarda la longitud de datos de la respuesta
 * @return				Puntero al texto con el que tiene que responder el servidor web
 * @note
 * - La longitud máxima del comando GET es de 100 bytes
 * - Si se devuelve NULL el servidor no responde nada
 * @see libMU_Internet_Server_SetCommandProcessingInfo()
 */
static char* Internet_ProcesamientoDeFormulario( const char* cmd_param, int* len )
{
	static int calls = 0;
	char msg[50]; int x = 0, y = 0, col = 15; int found;

	/* Debug info */
	calls++;	

	/* Incorrectly formated cmd_param */
	if( cmd_param == NULL || len == NULL ) return NULL;

	/* Process cmd_param (NOTE: Case is NOT ignored)*/
	/* x= */
	found = libMU_Internet_GetFormIntegerValue( cmd_param, "x=", &x );
	/* y= */
	found |= libMU_Internet_GetFormIntegerValue( cmd_param, "y=", &y );
	/* col= */
	found |= libMU_Internet_GetFormIntegerValue( cmd_param, "col=", &col );
	/* msg= */
	found |= libMU_Internet_GetFormStringValue( cmd_param, "msg=", msg, sizeof(msg) );
	/* show msg */
	if( found )	{
		libMU_Display_DrawString( msg, x, y, col );
	}
	/* LED toggle */
	found = libMU_Internet_GetFormStringValue( cmd_param, "led=", msg, sizeof(msg) );
	if( found && strcmp( msg, "toggle" )==0 ) {
		libMU_LED_Toggle( LED_1 );
	}
	*len = sizeof(respuesta_form);
	return respuesta_form;
}

/**
 * @}
 */
