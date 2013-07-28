/**
 * @addtogroup demos
 * Test de los botones y LEDs
 * @{
 ********************************************************************
 * @author		Eñaut Muxika <emuxika at mondragon dot edu>
 * @date		2011/7/10
 * @copyright	BSDL
 ********************************************************************
 */
#include <stdint.h>
#include <stdio.h>
#include <libMU/FreeRTOS.h>
#include <task.h>
#include <libMU/display.h>
#include <libMU/buttons.h>
#include <libMU/leds.h>
#include <libMU/pwm.h>
#include <libMU/sound.h>
#include "check_temp.h"

/**
 * Frère Jacques
 */
libMU_Sound_Note_t	frere_jacques[] = {
		/* 4/4 */
		{ DO,	400 },
		{ RE,	400 },
		{ MI,	400 },
		{ DO,	400 },

		{ DO,	400 },
		{ RE,	400 },
		{ MI,	400 },
		{ DO,	400 },

		{ MI,	400 },
		{ FA,	400 },
		{ SOL,	800 },

		{ MI,	400 },
		{ FA,	400 },
		{ SOL,	800 },

		{ SOL,	200 },
		{ LA,	200 },
		{ SOL,	200 },
		{ FA,	200 },
		{ MI,	400 },
		{ DO,	400 },

		{ SOL,	200 },
		{ LA,	200 },
		{ SOL,	200 },
		{ FA,	200 },
		{ MI,	400 },
		{ DO,	400 },

		{ RE,	400 },
		{ SOL,	400 },
		{ DO,	800 },

		{ RE,	400 },
		{ SOL,	400 },
		{ DO,	800 },
};

/**
 * Input processing and display task
 */
void main_demo_08(void);
void Task_Control(void* unused);

/**
 * Programa principal
 */
void main_demo_08(void)
{
	libMU_Display_Initialize();
	libMU_Button_Initialize();
	libMU_AD_Initialize();
	libMU_Sound_Initialize();
	libMU_Sound_SetSong( frere_jacques, sizeof(frere_jacques)/sizeof(libMU_Sound_Note_t) );

	libMU_Display_DrawString	( "Kaixo!!", 0, 0, 15 );

	xTaskCreate( Task_Control, "Control", 1024, NULL, tskIDLE_PRIORITY, NULL );
	libMU_FreeRTOS_SetupInterruptVectors();
	vTaskStartScheduler();
	for(;;) { /* never here */ }
}

void Task_Control(void* unused)
{
	portTickType	last;
	uint32_t 		col = 1, count, up;
	uint32_t 		volume = 40, volume_old;
	libMU_Display_DrawString( "SEL: Play", 0, 89, 12 );
	libMU_Display_DrawString( "Up/Down: Volume", 0, 81, 12 );
	libMU_Sound_SetVolume(volume);
	volume_old = volume;

	for(;;) {
		vTaskDelayUntil( &last, 100 * portTICK_RATE_MS_INV );

		libMU_AD_StartConversion();
		CheckTemperature();

		if( libMU_Button_Pressed( BUTTON_SELECT ) ) {
			if( !libMU_Sound_Playing )	libMU_Sound_Playing = 1;
			else						libMU_Sound_Repeat++;
		}
		if( volume_old != volume ) {
			volume_old = volume;
			libMU_Sound_SetVolume(volume);
		}
		if( libMU_Button_Pressed( BUTTON_UP ) && volume < 100 )	volume = volume_old + 3;
		if( libMU_Button_Pressed( BUTTON_DOWN ) && volume > 0 )	volume = volume_old - 3;
		if( libMU_Sound_Playing )
			libMU_Display_DrawString("playing", 64, 0, col );
		else
			libMU_Display_DrawString("       ", 64, 0, 0 );
		count++;
		if( count >= 3 ) {
			count = 0;
			if( up ) {
				if( col < 15 ) col += 2; else up = 0;
			}else{
				if( col > 1  ) col -= 2; else up = 1;
			}
		}
		libMU_Display_SetLine(8);
		libMU_Display_printf( "Volume: %3u\n", volume );
		libMU_Display_printf( "Repeat: %3u", libMU_Sound_Repeat );
	}
}

/**
 * @}
 */
