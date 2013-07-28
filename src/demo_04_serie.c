/**
 * @addtogroup demos
 * Test de la linea serie
 * @{
 ********************************************************************
 * @author		Eñaut Muxika <emuxika at mondragon dot edu>
 * @date		2011/7/10
 * @copyright	BSDL
 ********************************************************************
 */
#include <stdint.h>
#include <stdio.h>
#include <libMU/serial.h>
#include <libMU/display.h>
#include <libMU/leds.h>
#include "check_temp.h"

void main_demo_04(void);

/**
 * Programa principal
 */
void main_demo_04(void)
{
	char msg[50]; int pos = 8;
	
	libMU_Display_Initialize();
	libMU_Serial_Initialize(115200);
	libMU_AD_Initialize();
	libMU_LED_Initialize();
	libMU_LED_Off(LED_1);
		
	libMU_Display_DrawString( "Guten morgen, bidali!!\r\n", 0, 0, 15 );
	libMU_Serial_SendString	( "Guten morgen, bidali!!\r\n" );

	while(1)	/* repite indefinidamente */
	{        			
		libMU_AD_StartConversion();
		CheckTemperature();

		libMU_LED_Toggle(LED_1);

		/* enviar texto */
		libMU_Serial_SendString( "\r\nEnvia algo por la linea serie!!\r\n" );
        
        /* Leer texto */
        libMU_Serial_GetString( msg, sizeof(msg)-1 );
        
        libMU_Serial_SendString( "\r\nHas enviado :\r\n" );
        libMU_Serial_SendString( msg );
        /* Show received msg on display (erase line first) */
        libMU_Display_DrawString( "                      ", 0, pos, 12 );
        libMU_Display_DrawString( msg, 0, pos, 12 );
        /* Update position */
        pos += libMU_Display_CHAR_HEIGHT;
        if( pos >= libMU_Display_RES_Y ) pos = 0;
	}
}
/**
 * @}
 */
