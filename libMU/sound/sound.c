/**
 * @addtogroup libMU_Sound
 * Library for sound management functions
 * @{
 ********************************************************************
 * @author		Eñaut Muxika <emuxika at mondragon dot edu>
 * @date		2011/7/10
 * @copyright	BSDL
 ********************************************************************
 * @note		The sound management is done using a FreeRTOS helper task
 */
#include <stdint.h>
#include <stdio.h>
#include <FreeRTOS.h>
#include <task.h>
#include <inc/hw_types.h>		/* New data types */
#include <driverlib/sysctl.h>	/* System control functions */
#include <libMU/pwm.h>
#include <libMU/cpu.h>
#include <libMU/sound.h>

/**
 * Global variables for sound control
 */
uint32_t					libMU_Sound_DutyCycle;	/* Volume duty cycle (0-500000000) */
uint8_t						libMU_Sound_Playing;	/* Playing */
uint32_t					libMU_Sound_Repeat;		/* number of repetitions */

/**
 * Local variables for sound control
 */
static uint32_t				libMU_Sound_Tempo_Mult;	/* tempo multiplication */
static uint32_t				libMU_Sound_Tempo_Div;	/* tempo division */
static libMU_Sound_Score_t	libMU_Sound_Score;

/**
 * Sound player task
 * @param	unused	Unused parameter (necesarry for FreeRTOS task prototype format)
 */
static void libMU_Sound_Task(void* unused);

/**
 * Initialize the sound library
 * @note	This library uses the FreeRTOS operating system to generate the music timings
 */
void	libMU_Sound_Initialize(void)
{
	static int initDone = 0;
	/* Set processor to 50MHz */
	libMU_CPU_ClockSet(50000000);
	libMU_PWM_InitializeOutput	( LIBMU_PWM_1, 25/*Hz*/ );
	libMU_PWM_OutputDisable		( LIBMU_PWM_1 );
	libMU_PWM_SetPulseWidth		( LIBMU_PWM_1, 0 );
	/* initialize local variables */
	libMU_Sound_DutyCycle			= 25;
	libMU_Sound_Playing			= 0;
	libMU_Sound_Repeat			= 0;
	libMU_Sound_Tempo_Mult		= portTICK_RATE_MS_INV;
	libMU_Sound_Tempo_Div		= 1;
	libMU_Sound_Score.song		= NULL;
	libMU_Sound_Score.num_notes	= 0;
	/* create task (only once)  */
	if( !initDone ) {
		xTaskCreate( libMU_Sound_Task, "Music", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY, NULL );
		initDone = 1;
	}
}

static uint32_t libMU_Interpolate_uint32(
							uint32_t val, const uint32_t val_max,
							const uint32_t* table, uint32_t table_size)
{
	int idx;
	uint32_t delta, new_val, rem;
	uint64_t tmp_val;
	if( !table || table_size < 1 ) return 0;
	table_size--;		/* Point to last element */
	delta = val_max / table_size;
	idx = (int)(val / delta);
	if( idx >= table_size ) return table[table_size];
	rem = val - idx*delta;	/* Remainder of division */
#if 1
	tmp_val = table[idx+1] - table[idx];
	tmp_val*= rem;
	tmp_val+= delta/2;
	tmp_val/= delta;
	new_val = tmp_val + table[idx];
#else
	/* Overflow risk!! */
	new_val = (table[idx+1] - table[idx]) * rem + delta/2;
	new_val/= delta;
	new_val+= table[idx];
#endif
	return new_val;
}

/**
 * Set volume
 * @param volume	Volume of the sound to be generated (in dB)
 * @note 			The volume variable is logarithmic
 */
void	libMU_Sound_SetVolume(uint32_t volume)
{
#define NELEM(x)	(sizeof(x)/sizeof(x[0]))
	/* Table to convert dB to power */
	const uint32_t volume_to_power[] = {
			100,		/*  0 dB */
			316,		/* 10 dB */
			1000,	 	/* 20 dB */
			3162, 		/* 30 dB */
			10000,		/* 40 dB */
			31623,		/* 50 dB */
			100000,		/* 60 dB */
			316228,		/* 70 dB */
			1000000,	/* 80 dB */
			3162278,	/* 90 dB */
			10000000	/*100 dB */
	};
	/* Power (p) & duty cycle (d) relation function
	 * p = (1 - cos(2*pi*d) ) / 2
	 * d = acos(1 - 2*p)/2/pi
	 * @see PWM_calc_harm.mws
	 */
	const uint32_t power_to_duty_cycle[21] = {
			0,          71783147, 102416382, 126591656, 147583618,
			166666667, 184505060, 201506658, 217952892, 234057860,
			250000000,
			265942140, 282047108, 298493342, 315494940, 333333333,
			352416382, 373408345, 397583618, 428216853, 500000000
	};
	uint32_t val;
	if( volume == 0 ) {
		libMU_Sound_DutyCycle = 0; return;
	}
	/* Convert from dB to power amplitude */
	val = libMU_Interpolate_uint32(
				volume, 100,
				volume_to_power, NELEM(volume_to_power));
	/* Convert from power to duty cycle */
	val = libMU_Interpolate_uint32(
				val, 10000000,
				power_to_duty_cycle, NELEM(power_to_duty_cycle));
	/* Update volume */
	libMU_Sound_DutyCycle = val;
}

/**
 * Set the song to be played
 * @param	notes		Pointer to note frequencies and durations array
 * @param	num_notes	Number of notes in the previous score
 */
void	libMU_Sound_SetSong( libMU_Sound_Note_t* notes, uint32_t num_notes )
{
	if( notes == NULL || num_notes == 0 ) {
		notes = NULL; num_notes = 0;
	}
	libMU_Sound_Score.song = notes;
	libMU_Sound_Score.num_notes = num_notes;
}

/**
 * Sound player task
 * @param	unused	Unused parameter (necessary for FreeRTOS task prototype format)
 */
static void libMU_Sound_Task(void* unused)
{
	static portTickType	last;
	uint32_t			note;
	uint32_t			time = 20 * portTICK_RATE_MS_INV;
	unsigned int		cur_note = 0;
	unsigned int		prv_playing;

	for(;;)	/* repite indefinidamente */
	{
		if( libMU_Sound_Playing && !prv_playing ) {
			cur_note = 0;	/* We start playing again */
			libMU_PWM_OutputEnable( LIBMU_PWM_1 );	/* Enable output */
		}
		if( !libMU_Sound_Playing && prv_playing ) {
			libMU_PWM_OutputDisable( LIBMU_PWM_1 );	/* Disable output */
		}
		prv_playing = libMU_Sound_Playing;
		if( libMU_Sound_Score.song == NULL ) libMU_Sound_Playing = 0;
		if( libMU_Sound_Playing ) {
			note = libMU_Sound_Score.song[cur_note].note;
			time = libMU_Sound_Score.song[cur_note].time;
			if( libMU_Sound_Tempo_Mult> 1 ) time *= libMU_Sound_Tempo_Mult;
			if( libMU_Sound_Tempo_Div > 1 ) time /= libMU_Sound_Tempo_Div;
			if( note && libMU_Sound_DutyCycle ) {
				libMU_PWM_SetFrequency		( LIBMU_PWM_1, note );
				libMU_PWM_SetPulseWidthPPB	( LIBMU_PWM_1, libMU_Sound_DutyCycle );
			}else{
				libMU_PWM_SetFrequency	( LIBMU_PWM_1, 25/*Hz*/ );
				libMU_PWM_SetPulseWidth	( LIBMU_PWM_1, 0 );
			}
			cur_note++;
			if( cur_note >= libMU_Sound_Score.num_notes ) {
				if( libMU_Sound_Repeat > 0 )	{	/* repeat if necessary */
					cur_note = 0;
					libMU_Sound_Repeat--;
				}else{
					libMU_Sound_Playing = 0;		/* stop playing */
				}
			}
		}else{
			libMU_PWM_SetFrequency	( LIBMU_PWM_1, 25/*Hz*/ );
			libMU_PWM_SetPulseWidth	( LIBMU_PWM_1, 0 );
			time = 20 * portTICK_RATE_MS_INV;
		}
		vTaskDelayUntil( &last, time );
	}
}

/**
 * @}
 */
