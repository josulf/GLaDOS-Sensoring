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
#include <libMU/serial_FreeRTOS.h>
#include <libMU/buttons.h>
#include <libMU/leds.h>
#include <libMU/delay.h>
#include <libMU/timer.h>
#include <libMU/stdlib.h>
#include <libMU/random.h>
#include <libMU/FreeRTOS.h>
#include <task.h>

#define	SC(x)	((signed char *)(x))

static int		rtosStarted = 0;
#define	DEBUG 1
#if DEBUG
#include <semphr.h>
#include <libMU/display.h>
static xSemaphoreHandle	DbgPrintBusy = NULL;
static void	DbgPrintWin( int bottom, int top, const char* msg ) __attribute((weak));
static void	DbgPrintWin( int bottom, int top, const char* msg )
{
	static int pos = 0;
	if( rtosStarted ) {
		if( DbgPrintBusy == NULL ) {
			DbgPrintBusy = xSemaphoreCreateMutex();
			if( DbgPrintBusy == NULL ) return;
		}
		xSemaphoreTake( DbgPrintBusy, portMAX_DELAY );
	}
	if( bottom < 0 || bottom > ( libMU_Display_RES_Y-libMU_Display_CHAR_HEIGHT + 1) )
		bottom = 0;
	if( top < 0 || top > (libMU_Display_RES_Y-libMU_Display_CHAR_HEIGHT + 1) || top < bottom )
		top = libMU_Display_RES_Y-libMU_Display_CHAR_HEIGHT + 1;
	if( pos < bottom || pos > top ) pos = bottom;
	libMU_Display_DrawString( "                     ", 0, pos, 15 );
	libMU_Display_DrawString( msg, 0, pos, 15 );
	pos += libMU_Display_CHAR_HEIGHT;
	if( rtosStarted ) {
		xSemaphoreGive( DbgPrintBusy );
	}
}
#define DbgPrint(x)		DbgPrintWin(-1,-1,x)
#define DbgPrintM(x)	DbgPrintWin(48,80,x)
#else
#define DbgPrint(x)
#define	print(x)
#endif

/**
 * Prototypes
 */
void main_demo_07b(void);
static void T1_Display		(void* unused);
static void T2_Counter		(void* unused);
static void T3_Interference	(void* unused);

/**
 * Local vars
 */
volatile static uint32_t	Count_1ms;
volatile static uint32_t	fast;

/**
 * Programa principal
 */
void main_demo_07b(void)
{
	/* We CANNOT generate any interrupt until the FreeRTOS scheduler starts working!!
	 * The FreeRTOS _interrupt_handler() depends on the FreeRTOS being running */
	libMU_Display_Initialize();
	libMU_Serial_Initialize( 115200 );
	libMU_Button_Initialize();
	libMU_LED_Initialize();
	libMU_LED_InitializeEth();
	libMU_LED_On	( LED_1 );

	libMU_Serial_SendString	( "FreeRTOS 2012/6/14!\n" );
	DbgPrint("<M>:Start\r\n");

	/* Create tasks */
	xTaskCreate( T1_Display,	  SC("T1_Disp"),  1024, 					NULL, tskIDLE_PRIORITY,			NULL );
	xTaskCreate( T2_Counter,	  SC("T2_Count"), configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY,			NULL );
	xTaskCreate( T3_Interference, SC("T2_Inter"), 1024,						NULL, configMAX_PRIORITIES - 1, NULL );

	/* Start FreeRTOS scheduler */
	libMU_Serial_SetupFreeRTOSHandler( 1024 );
	libMU_FreeRTOS_SetupInterruptVectors();
	vTaskStartScheduler();

	/* Should never reach this point */
	DbgPrint( "Error\n" );
	for(;;) {}
}

static void T1_Display(void* unused)
{
	static uint8_t	TxBuffer1[512];
	char msg[50]; volatile int i;
	libMU_FIFO_t*	queue;
	rtosStarted = 1;
	DbgPrint("<T1>:Start\r\n");
	queue = libMU_FIFO_Initialize( TxBuffer1, sizeof(TxBuffer1), sizeof(char) );
	for(;;) {
		libMU_snprintf( msg, sizeof(msg), "Count=% 8dms\r\n", (int)Count_1ms );
		libMU_Serial_SendStringTS	( queue, msg );
		msg[0] = 0;

		libMU_LED_Toggle			( LED_1 );
		libMU_Serial_SendStringTS	( queue, "Seems to be working\r\n" );
		libMU_Serial_SendStringTS	( queue, "Input a string to check: " );
/*		libMU_Serial_GetUntilChar_FreeRTOS( msg, sizeof(msg), '\n', 5000 ); */
		/* There is a problem reading the serial port using FreeRTOS.
		 * The reception interrupt only happens when 2 characters are received
		 * and therefore the following function show something only when
		 * 2 characters are received */
		libMU_Serial_GetStringFreeRTOS( queue, msg, sizeof(msg) );
		libMU_Serial_SendStringTS	( queue, "\r" );
		DbgPrintM( msg );
		fast = 1;
		for( i = 0; i < 50; i++ ) {
			libMU_Serial_SendStringTS	( queue, "You wrote: " );
			libMU_Serial_SendStringTS	( queue, msg );
			libMU_Serial_SendStringTS	( queue, "\r\n" );
		}
		fast = 0;
	}
}

static void T2_Counter(void* unused)
{
	portTickType xLast = xTaskGetTickCount(); Count_1ms = 0;
	char msg[22];
	rtosStarted = 1;
	DbgPrint("<T2>:Start\r\n");
	for(;;) {
		vTaskDelayUntil( &xLast, 10 * portTICK_RATE_MS_INV );	/* 10 milliseconds */
		Count_1ms+=10;
		if( ( Count_1ms % 500 ) == 0 ) {
			libMU_LED_Toggle( LED_2 );
			libMU_snprintf( msg, sizeof(msg), "C=% 9dms", Count_1ms );
			DbgPrintM( msg );
		}
	}
}

static void T3_Interference	(void* unused)
{
	static uint8_t	TxBuffer2[512];
	uint32_t	delay; char msg[50];
	libMU_FIFO_t*	queue; fast = 0;
	rtosStarted = 1;
	DbgPrint("<T3>:Start\r\n");
	queue = libMU_FIFO_Initialize( TxBuffer2, sizeof(TxBuffer2), sizeof(char) );
	libMU_Random_Initialize();
	for(;;) {
		if( fast )
			delay = ( libMU_Random_GetValue() >> (32-4) ) + 1;	/* 1 - 16 ms in 1ms units */
		else
			delay = ( libMU_Random_GetValue() >> (32-4) ) * 10 + 10;	/* 10 - 160 ms in 10ms units */
		libMU_snprintf( msg, sizeof(msg), "Hello in % 3dms\r\n", delay );
		libMU_Serial_SendStringTS( queue, msg );
		vTaskDelay( delay * 100 * portTICK_RATE_MS_INV );
	}
}

/**
 * @}
 */
