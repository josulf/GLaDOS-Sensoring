/**
 * @addtogroup demos
 * Test del uso del sistema operativo FreeRTOS
 * @{
 ********************************************************************
 * @author		Eñaut Muxika <emuxika at mondragon dot edu>
 * @date		2011/7/10
 * @copyright	BSDL
 ********************************************************************
 */
#include <stdint.h>
#include <stdio.h>
#include <libMU/display.h>
#include <libMU/delay.h>

void main_demo_06c(void);

/**
 * Programa principal
 */
void main_demo_06c(void)
{
	int i;
	libMU_Display_Initialize();
	libMU_Display_DrawStringUTF8( "Eñaut Ño!", 16, 40, 12 );
	libMU_Display_DrawStringUTF8( "40°C ± ÿ", 16, 48, 12 );

	for( i=0; i <= libMU_Display_CHAR_WIDTH; i++ ) {
		libMU_Display_DrawStringUTF8("\x01\x02", -i,                       24+i*8, 12 );
		libMU_Display_DrawStringUTF8("\x01\x02", libMU_Display_RES_X-12+i, 24+i*8, 12 );
	}
	for( i=0; i <= libMU_Display_CHAR_HEIGHT; i++ ) {
		libMU_Display_DrawStringUTF8("\x02", 32+i*6, -i,                      12 );
		libMU_Display_DrawStringUTF8("\x02", 32+i*6, libMU_Display_RES_Y-8+i, 12 );
	}
	for(;;) {
	}	
}

/**
 * @}
 */
