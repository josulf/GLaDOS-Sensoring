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
#include <libMU/display.h>
#include <libMU/buttons.h>
#include <libMU/analog_digital.h>

#include <libMU/FreeRTOS.h>
#include <task.h>

#include "check_temp.h"

/**
 * Count milliseconds
 */
static uint32_t	Count_1ms;

/**
 * Prototypes
 */
void main_demo_07(void);
static void T1_Display(void* unused);
static void T2_Counter(void* unused);
extern void libMU_FreeRTOS_SetupInterruptVectors(void);

/**
 * Programa principal
 */
void main_demo_07(void)
{
	/* Initialize HW */
	libMU_Display_Initialize();
	libMU_AD_Initialize();
	libMU_Button_Initialize();
	libMU_Display_DrawString( "HW init done!", 0, 0, 12 );	

	/* Create tasks */
	xTaskCreate( T1_Display, "T1_Disp",  1024, NULL, tskIDLE_PRIORITY, NULL );
	xTaskCreate( T2_Counter, "T1_Count", 1024, NULL, tskIDLE_PRIORITY, NULL ); 

	/* Start FreeRTOS scheduler */
	libMU_FreeRTOS_SetupInterruptVectors();
	vTaskStartScheduler();
	
	/* Should never reach this point */
	libMU_Display_DrawString("Error           ", 0, 0, 15);
	for(;;) {}
}

static void T1_Display(void* unused)
{
	uint32_t	sec = 0, min = 0, hour = 0, days = 0;
	portTickType xLast = xTaskGetTickCount();
	for(;;) {
		vTaskDelayUntil( &xLast, 1000 * portTICK_RATE_MS_INV );	/* 1000 milliseconds */
		sec++;
		if( sec >= 60 ) {
			sec = 0; min++;
			if( min >= 60 ) {
				min = 0; hour++;
				if( hour >= 24 ) { hour++; days++; }
			}
		}
		libMU_Display_SetLine(8);
		libMU_Display_SetColor(12);
		libMU_Display_printf( "% 3u% 2u:%02u:%02u\n", days, hour, min, sec );
		libMU_Display_printf( "ms:% 10u", Count_1ms );
		CheckTemperature();
	}	
}

static void T2_Counter(void* unused)
{
	portTickType xLast = xTaskGetTickCount();
	for(;;) {
		vTaskDelayUntil( &xLast, 10 * portTICK_RATE_MS_INV );	/* 10 milliseconds */
		Count_1ms+=10;
		/* Mostrar temperatura */
		libMU_AD_StartConversion();
	}	
}
/**
 * @}
 */
