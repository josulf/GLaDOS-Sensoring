/**
 * @addtogroup demos
 * Test del PWM
 * @{
 ********************************************************************
 * @author		Eñaut Muxika <emuxika at mondragon dot edu>
 * @date		2011/7/10
 * @copyright	BSDL
 ********************************************************************
 */
#include <stdint.h>
#include <libMU/display.h>
#include <libMU/buttons.h>
#include <libMU/leds.h>
#include <libMU/pwm.h>
#include "check_temp.h"

void main_demo_03(void);

/**
 * Programa principal
 */
void main_demo_03(void)
{
	char		msg[50];
	uint32_t	duty_cycle = 5000;
	uint32_t 	count = 0;
	uint32_t	inc; uint32_t stepup;

	libMU_Display_Initialize();
	libMU_Button_Initialize();	
	libMU_AD_Initialize();
	libMU_PWM_InitializeOutput( LIBMU_PWM_0, 5000/*Hz*/ );
	libMU_PWM_InitializeChopperOutput( LIBMU_PWM_GRP_23, 1000/*Hz*/, 10000/*ns*/ );

	libMU_Display_DrawString	( "Kaixo!!", 0, 0, 15 );

	for(;;)	/* repite indefinidamente */
	{
		libMU_AD_StartConversion();
        if( libMU_Button_GetStatus(BUTTON_SELECT) ) {
	        libMU_Display_DrawString( "Select", 0, 8, 15 );
        }else{
	        libMU_Display_DrawString( "      ", 0, 8, 15 );        
        }
        libMU_snprintf( msg, sizeof(msg), "C:  %10u", count++ );
        libMU_Display_DrawString( msg, 0, 16, 12 ); 
        libMU_snprintf( msg, sizeof(msg), "Per:%3u.%02u%%", duty_cycle/100, duty_cycle%100 );
        libMU_Display_DrawString( msg, 0, 24, 15 );

        CheckTemperature();

        /* actualiza el periodo de manera incremental. (cuanto mas tiempo pulsado mas rápido cambia) */
        if( libMU_Button_GetStatus( (libMU_Button_Identifiers_t)(BUTTON_LEFT|BUTTON_RIGHT) ) ) {
			if( libMU_Button_Pressed( (libMU_Button_Identifiers_t)(BUTTON_LEFT|BUTTON_RIGHT) ) ) {
				inc = 1; stepup = 0;
			}else{
				stepup++;
				if( stepup > 300 )		inc = 500;
				else if( stepup > 200 )	inc = 100;
				else if( stepup > 100 )	inc = 10;
			}
			if( libMU_Button_GetStatus(BUTTON_LEFT) )	duty_cycle += inc;
			if( libMU_Button_GetStatus(BUTTON_RIGHT) )	duty_cycle -= inc;
		}
		if( duty_cycle > 10000 )	duty_cycle = 10000;
		if( duty_cycle < 1 )		duty_cycle = 1;
		libMU_PWM_SetPulseWidth( LIBMU_PWM_0, duty_cycle );
		if( duty_cycle <= 3333 ) {
			libMU_PWM_SetPulseWidth( LIBMU_PWM_2, 2500 ); /* 25% */
		}else if( duty_cycle <= 6667 ) {
			libMU_PWM_SetPulseWidth( LIBMU_PWM_2, 5000 ); /* 50% */
		}else{
			libMU_PWM_SetPulseWidth( LIBMU_PWM_2, 7500 ); /* 75% */
		}
	}
}

/**
 * @}
 */
