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
#include <libMU/display.h>
#include <libMU/buttons.h>
#include <libMU/leds.h>
#include <libMU/timer.h>
#include <libMU/delay.h>
#include <libMU/stdlib.h>
#include <libMU/stats.h>

extern uint32_t					libMU_Timer_Count_HighOrder;
static uint32_t					Count_20us;
volatile libMU_Stats_uint32_t	stat_int_period;
volatile libMU_Stats_uint32_t	stat_int_exectime;
volatile libMU_Stats_uint32_t	stat_prog_period;
volatile libMU_Stats_uint32_t	stat_prog_exectime;
libMU_Timer_Stopwatch_t			time_int;

void main_demo_05(void);
void timerfunc(void *param);
void show_stats( const char* desc,
		volatile libMU_Stats_uint32_t* stats,
		double factor );

void timerfunc(void *param)
{
	/* Measure interrupt execution period */
	libMU_Stats_Update( (libMU_Stats_uint32_t*)&stat_int_period,
			libMU_Timer_StopwatchMeasure(&time_int) );
	Count_20us++;
	if( (Count_20us & 0x3FFFu) == 0 )
		libMU_LED_Toggle( LED_1 );
	/* Measure interrupt execution time */
	libMU_Stats_Update( (libMU_Stats_uint32_t*)&stat_int_exectime,
			libMU_Timer_StopwatchPeek(&time_int) );
}

/**
 * Main program
 */
void main_demo_05(void)
{
	libMU_Timer_Stopwatch_t time_prog;
	libMU_Time_t			time_delay;
	double					factor_us, factor_ms;

	factor_ms = (double)libMU_Timer_ConvertTo_ns(1) * 1e-6;
	factor_us = (double)libMU_Timer_ConvertTo_ns(1) * 1e-3;

	libMU_Stats_Initialize(&stat_int_period);
	libMU_Stats_Initialize(&stat_int_exectime);
	libMU_Stats_Initialize(&stat_prog_period);
	libMU_Stats_Initialize(&stat_prog_exectime);

	libMU_Display_Initialize();
	libMU_Display_DrawString( "Test 2013/4/11", 0, 0, 12 );
	libMU_Button_Initialize();
	libMU_LED_Initialize();
	libMU_LED_Off	( LED_ALL );
	libMU_LED_On	( LED_1 );
	if( !libMU_Timer_Initialize() ) {
		for(;;);
	}
	libMU_Timer_SetFrequency( 50000 );
	libMU_Timer_StopwatchStart( &time_int );
	libMU_Timer_SetHandler( timerfunc, NULL );

	libMU_Display_DrawString( "## Main Loop ##", 0, 0, 15 );
	libMU_Timer_StopwatchStart( &time_prog );
	time_delay = libMU_Timer_Current();
	for(;;)	/* Repeat forever */
	{
		/* Measure program execution period */
		libMU_Stats_Update( (libMU_Stats_uint32_t*)&stat_prog_period,
				libMU_Timer_StopwatchMeasure(&time_prog) );

		libMU_Display_SetLine( 8 );
		libMU_Display_SetColor( 15 );

		/* Interrupt count (time measurement and dedicated interrupts) */
		libMU_Display_printf( "C: % 10u\n", Count_20us );
		libMU_Display_printf( "HO:% 10u\n", libMU_Timer_Count_HighOrder );

		/* Interrupt execution period */
		show_stats( "Int Per±StdDev(us)\n", &stat_int_period, factor_us );
		/* Interrupt execution period */
		show_stats( "Int Exec±StdDev(us)\n", &stat_int_exectime, factor_us );
		/* Main program execution period */
		show_stats( "Prog Per±StdDev(ms)\n", &stat_prog_period, factor_ms );
#if 0
		/* Main program execution time */
		show_stats( "ProgExec±StdDev(ms)\n", &stat_prog_exectime, factor_ms );
#endif
		/* Measure program execution time */
		libMU_Stats_Update( (libMU_Stats_uint32_t*)&stat_prog_exectime,
				libMU_Timer_StopwatchPeek(&time_prog) );

		/* Reset statistics */
		if( libMU_Button_Pressed( BUTTON_SELECT ) ) {
			libMU_Stats_Initialize(&stat_int_period);
			libMU_Stats_Initialize(&stat_int_exectime);
			libMU_Stats_Initialize(&stat_prog_period);
			libMU_Stats_Initialize(&stat_prog_exectime);
		}
		/* Delay */
		libMU_Timer_DelayFrom( &time_delay, 200000 );
	}
}

void show_stats( const char* desc,
		volatile libMU_Stats_uint32_t* stats,
		double factor )
{
	libMU_Display_SetColor( 8 );
	libMU_Display_printf( desc );
	libMU_Display_SetColor( 13 );
	libMU_Display_printf( "% 8.3f±% 11.6f\n",
			(double)libMU_Stats_GetMeanValue(stats) * factor,
			libMU_Stats_GetStdDev(stats) * factor );
	libMU_Display_printf( "% 8.3f,% 8.3f\n",
			(double)libMU_Stats_GetMinValue(stats) * factor,
			(double)libMU_Stats_GetMaxValue(stats) * factor );
}
/**
 * @}
 */
