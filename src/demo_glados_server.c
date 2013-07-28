
/********************************************************************
 * @author		Ioritz Herreros Osa <ioritz.herreros@gmail.com>
 * @date		2013/07/27
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
#include "check_temp.h"

/**
 * Predefined strings on Pushing Box API and created scenarios
 */
static const char *urlPushBox = "http://api.pushingbox.com/pushingbox?devid=";	///< main URL string for Pushing Box API
static const char *eMailDevID = "vC2FCE1514030AB3";								///< Device ID for warning_email scenario
static const char *dataLogDevID = "v00C14CE1FAAD6F0";							///< Device ID for dataLog scenario

/*
 * Sensor data variables
 */
typedef struct {
	int			tilt;
	uint32_t 	temp;
	uint32_t	light;
}stData;

stData sensorData;


/*
 * Local Functions
 */
void main_demo_glados_server ();
static void Internet_ProcesamientoDePagina( char* data, unsigned short int len ) ;
static void send_data_log();
static void send_email_alarm();
static void mainTask (void *param);
static int tilt_check();
static uint32_t light_check();
static uint32_t temp_check();
static void sensor_check();



/*
 * Auxiliary Flags
 */
int flagEmailSent=0;
int flagAuxMsg=0;

//timer

uint32_t Count_20us = 0;
uint32_t contTick = 0;
int contTick2 = 0;
int contSens = 0;
int contMsg = 0;
int contMail = 0;
int flagEraseMsg=0;
int flagSens=0;
int flagMail=0;

//libMU_Timer_Stopwatch_t			time_int;

void timerTick(void *param)
{
	/* Measure interrupt execution period */
		//libMU_Stats_Update( (libMU_Stats_uint32_t*)&stat_int_period,
		//		libMU_Timer_StopwatchMeasure(&time_int) );
/*		Count_20us++;
		if( (Count_20us & 0x3FFFu) == 0 )
			contTick++;
		if(contTick > 5)
		{
			contTick2++;
			contTick=0;
			flagSens=1;
		}*/
		/* Measure interrupt execution time */
		//libMU_Stats_Update( (libMU_Stats_uint32_t*)&stat_int_exectime,
		//		libMU_Timer_StopwatchPeek(&time_int) );
/*
	if(contTick2 > 3)
	{
		contTick2 = 0;
		if(flagAuxMsg==1)
			contMsg++;
		if(flagEmailSent == 1)
		{
			contMail++;
		}
		if(flagSens==0)
		{
			contSens++;
		}
	}
	if(contMsg>2)
		flagEraseMsg=1;

	if(contSens > 20)
	{
		flagSens=1;
		contSens = 0;
	}

	if(contMail > 120)
	{
		flagEmailSent = 0;
	}
*/

}
/*
 * Component start up. From demo_09_Internet.c
 */
void main_demo_glados_server ()
{
	libMU_Display_Initialize();
	libMU_Timer_Initialize();
	libMU_Timer_SetFrequency( 50000 );
//	libMU_Timer_StopwatchStart( &time_int );
	libMU_Timer_SetHandler(timerTick, NULL );
	libMU_AD_Initialize();
	libMU_GPIO_Initialize( GPIO_PC5, GPIO_INPUT );
	libMU_LED_Initialize();
	libMU_Button_Initialize();

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
}

/*
 * Send data to google doc Form through pushingBox API
 */
static void send_data_log()
{
	char msg[200];
	char strAux[50];
	int res;
	IPAddress_t ip;

	strcpy(msg,urlPushBox);
	strcat(msg,dataLogDevID);
	sprintf(strAux,"&tilt=%d&temp=%d&light=%d",sensorData.tilt,sensorData.temp,sensorData.light);
	strcat(msg,strAux);
	libMU_Internet_DNS_resolution( "api.pushingbox.com", &ip, 10000 );
	res = libMU_Internet_GetPage (msg,Internet_ProcesamientoDePagina);
	if( !res )
		libMU_Display_DrawString( "Send ERROR          ", 0, 72, 15 );
	else
		libMU_Display_DrawString( "Data registered     ", 0, 72, 15 );
	flagAuxMsg=1;
	contMsg = 0;

}

/*
 * Send warning email through pushingBox API
 */
static void send_email_alarm()
{
	char msg[200];
	int res;
	IPAddress_t ip;

	strcpy(msg,urlPushBox);
	strcat(msg,eMailDevID);
	libMU_Internet_DNS_resolution( "api.pushingbox.com", &ip, 10000 );
	res = libMU_Internet_GetPage (msg,Internet_ProcesamientoDePagina);
	if( !res )
		libMU_Display_DrawString( "Send ERROR          ", 0, 72, 15 );
	else
		libMU_Display_DrawString( "E-mail send OK      ", 0, 72, 15 );
	flagAuxMsg=1;
	contMsg = 0;
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
		libMU_Display_DrawString("GLaDOS server    EE21", 0, 0, 15 );
		libMU_Display_DrawString("=====================", 0, 8, 6);
		libMU_Display_DrawString("=====================", 0, 80, 6);

		/*if(flagEraseMsg == 1)
		{
			flagEraseMsg = 0;
			flagAuxMsg = 0;
			libMU_Display_DrawString( "                    ", 0, 85, 15 );
		}*/

		sensor_check();

		if(libMU_Button_GetStatus(BUTTON_DOWN) )
		{
			libMU_Display_DrawString("                     ", 0, 72, 15);
		}

		switch( libMU_Internet_GetStatus() )
		{
			case NETWORK_NO_CONNECTION:
				libMU_Display_DrawString("NO connection!       ", 0, 88, 12 );
				libMU_Internet_Delay( 250 );
				break;
			case NETWORK_IP_MISSING:
				libMU_Display_DrawString("IP missing!          ", 0, 88, 12 );
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

			/*	if(flagSens==1)
				{
					flagSens = 0;
					send_data_log();
				}*/

				if(flagEmailSent == 0)
				{
					if(sensorData.tilt)
					{
						send_email_alarm();
						flagEmailSent = 1;
						contMail = 0;
					}
				}
				if(libMU_Button_GetStatus(BUTTON_UP) )
				{
					send_data_log();
				}

				if(libMU_Button_GetStatus(BUTTON_SELECT) )
				{
					flagEmailSent = 0;
				}
				libMU_Internet_Delay( 100 );
				break;

			case NETWORK_ERROR:
			default:
				libMU_Display_DrawString( "Network ERROR       ", 0, 88, 12 );
				libMU_Internet_Delay( 250 );
				break;
		}
	}
}

/*
 * Tilt detector status
 * @return device status 1 OK, 0 UPSIDE DOWN
 */
static int tilt_check()
{
	int ret = 0;
	char str[50];

	ret = !libMU_GPIO_GetStatus(GPIO_PC5);

	sprintf(str,"Tilt = %d   ",ret);
	if(flagEmailSent)
		strcat(str,"(ALARM ON)");
	else
		strcat(str,"          ");
	libMU_Display_DrawString(str, 0, 20, 15 );
	return ret;

}

/*
 * LDR sensor status
 * @return LDR ADC value
 */
static uint32_t light_check()
{
	uint32_t value;
	char str[50];
	value = libMU_AD_GetChannelValue(ADC_CHANNEL_0);
	sprintf(str,"Light = %d       ",value);
	libMU_Display_DrawString(str, 0, 30, 15 );
	return value;
}

/*
 * NTC sensor status
 * @return NTC ADC value
 */
static uint32_t temp_check()
{
	uint32_t value;
	char str[50];
	value = libMU_AD_GetChannelValue(ADC_CHANNEL_1);
	sprintf(str,"Temp = %d       ",value);
	libMU_Display_DrawString(str, 0, 40, 15 );
	return value;
}

/*
 * Whole sensor system check
 */
static void sensor_check()
{
	libMU_AD_StartConversion();
	sensorData.tilt = tilt_check();
	sensorData.light = light_check();
	sensorData.temp = temp_check();
}
