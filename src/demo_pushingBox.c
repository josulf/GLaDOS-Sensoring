
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
static const char *dataLogDevID = "v2F47507140177A3";							///< Device ID for dataLog scenario

/*
 * Sensor data variables
 */
//int
//int data1=0;
//int data2=1;
typedef struct {
	int			tilt;
	uint32_t 	temp;
	uint32_t	light;
}stData;

stData sensorData;

int flagEmailSent=0;

void main_demo_pushingBox ();
static void Internet_ProcesamientoDePagina( char* data, unsigned short int len ) ;
static void send_data_log(unsigned int d1,unsigned int d2);
static void send_email_alarm();
static void mainTask (void *param);
static int tilt_check();
static uint32_t light_check();
static uint32_t temp_check();

/*
 * Component start up. From demo_09_Internet.c
 */
void main_demo_pushingBox ()
{
	libMU_Display_Initialize();
//	libMU_Timer_Initialize();
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
	//libMU_Display_DrawString("Data registered.", 0, 85, 15 );
}

/*
 * Send data to google doc Form through pushingBox API
 */
static void send_data_log(unsigned int d1,unsigned int d2)
{
	char msg[200];
	char strAux[20];
	int res;
	IPAddress_t ip;

	strcpy(msg,urlPushBox);
	strcat(msg,dataLogDevID);
	sprintf(strAux,"&data1=%d&data2=%d",d1,d2);
	strcat(msg,strAux);
	libMU_Internet_DNS_resolution( "api.pushingbox.com", &ip, 10000 );
	res = libMU_Internet_GetPage (msg,Internet_ProcesamientoDePagina);
	if( !res )
		libMU_Display_DrawString( "Send ERROR", 0, 85, 15 );
	else
		libMU_Display_DrawString( "Data registered", 0, 85, 15 );

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
		libMU_Display_DrawString( "Send ERROR", 0, 85, 15 );
	else
		libMU_Display_DrawString( "E-mail send OK", 0, 85, 15 );
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
		libMU_AD_StartConversion();
		sensorData.tilt = tilt_check();
		sensorData.light = light_check();
		sensorData.temp = temp_check();
		switch( libMU_Internet_GetStatus() )
		{
			case NETWORK_NO_CONNECTION:
				libMU_Display_DrawString("NO connection!", 0, 0, 15 );
				libMU_Internet_Delay( 250 );
				break;
			case NETWORK_IP_MISSING:
				libMU_Display_DrawString("IP missing!", 0, 0, 12 );
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
				libMU_Display_DrawString( msg, 0, 0, 12 );

				if(flagEmailSent == 0)
				{
					if(sensorData.tilt)
					{
						send_email_alarm();
						flagEmailSent = 1;
					}
				}

				if(libMU_Button_Pressed(BUTTON_SELECT))
				{
					flagEmailSent = 0;
				}
				//send_data_log(data1, data2);
				libMU_Internet_Delay( 100 );
				break;

			case NETWORK_ERROR:
			default:
				libMU_Display_DrawString( "Network ERROR", 0, 0, 12 );
				libMU_Internet_Delay( 250 );
				break;
		}
	}
}

/*
 * Sensor check function
 */
static int tilt_check()
{
	if(libMU_GPIO_GetStatus(GPIO_PC5))
	{
		libMU_Display_DrawString("Tilt = 1", 0, 20, 15 );
		return 1;
	}
	libMU_Display_DrawString("Tilt = 0", 0, 20, 15 );
	return 0;
}

static uint32_t light_check()
{
	uint32_t value;
	char str[25];
	value = libMU_AD_GetChannelValue(ADC_CHANNEL_0);
	sprintf(str,"Light = %d",value);
	libMU_Display_DrawString(str, 0, 35, 15 );
	return value;
}

static uint32_t temp_check()
{
	uint32_t value;
	char str[25];
	value = libMU_AD_GetChannelValue(ADC_CHANNEL_1);
	sprintf(str,"Temp = %d",value);
	libMU_Display_DrawString(str, 0, 50, 15 );
	return value;
}
