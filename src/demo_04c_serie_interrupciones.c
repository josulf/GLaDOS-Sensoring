/**
 * @addtogroup demos
 * Test del protocolo XON/XOFF
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
#include <libMU/leds.h>
#include <libMU/delay.h>
#include <libMU/buttons.h>
#include <libMU/display.h>
#include "check_temp.h"

void main_demo_04c(void);

uint8_t	TxBuf[1024];
uint8_t	RxBuf[1024];

/**
 * Programa principal
 */
void main_demo_04c(void)
{
	char msg[50]; int pos = 8;
	
	libMU_Display_Initialize();
	libMU_LED_Initialize();
	libMU_AD_Initialize();
	libMU_Serial_Initialize(115200);

	libMU_Display_DrawString( "Guten morgen, bidali!!\r\n", 0, 0, 15 );
	libMU_Serial_SendString	( "Guten morgen, bidali!! (NO INT)\n" );
	libMU_Serial_SetBufferedCommunications( RxBuf, sizeof(RxBuf) );
	libMU_Serial_SendString	( "Guten morgen, bidali!! (INT)\n" );

	while(1)	/* repite indefinidamente */
	{
		libMU_AD_StartConversion();
		CheckTemperature();

        libMU_Serial_SendString( "\n\nEnvia algo por la linea serie : " );

        /* Leer texto */
        libMU_Serial_GetString( msg, sizeof(msg)-1 );

        libMU_Serial_SendString( "\nHas enviado : " );
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
