/**
 * @addtogroup	libMU_Timer
 * Library for timer functions
 * @{
 ********************************************************************
 * @author		Eñaut Muxika <emuxika at mondragon dot edu>
 * @date		2011/7/10
 * @copyright	BSDL
 ********************************************************************
 */
#ifndef LIBMU_TIMER_H_
#define LIBMU_TIMER_H_

#include <stdint.h>

#define	LIBMU_TIMER_USE_64_BITS

/**
 * Constants used in timer management
 */
#define	CPU_FREQ		50000000
enum libMU_Timer_Constants {
	libMU_Timer_FREQ		= CPU_FREQ,		/* Timer input signal frequency */
	libMU_Timer_MAX_COUNT	= 0xFFFFFFFF,	/* Timer maximum count HW = (20ns * 2^31) ~ 42,95s */
	libMU_Timer_PERIOD_NS	= ((1000000000 + CPU_FREQ/2) / CPU_FREQ ),
											/* Timer clock period in ns */
};

/**
 * Data types used in timer management
 */
#ifdef LIBMU_TIMER_USE_64_BITS
typedef uint64_t libMU_Time_t;
#define	LIBMU_TIME_SIZE	2
#else
typedef uint32_t libMU_Time_t;
#define	LIBMU_TIME_SIZE	1
#endif

typedef union _libMU_Timer_Stopwatch_t {
	libMU_Time_t	time;
	uint32_t		t32[LIBMU_TIME_SIZE];	
} libMU_Timer_Stopwatch_t;

typedef void	(*libMU_Timer_Handler_t)(void *param);

/**
 * Initialize timer HW
 * @return	true if correctly initialized
 */
int libMU_Timer_Initialize(void);

/**
 * Set the timer frequency for overflow generation
 * @param freq		Frequency value
 */
void libMU_Timer_SetFrequency(uint32_t freq);

/**
 * Set the timer function called at each timer overflow
 * @param function	Pointer to function
 * @param param		Pointer to function parameters
 */
void libMU_Timer_SetHandler(libMU_Timer_Handler_t function, void* param);

/**
 * Get current time
 * @return 	Current time
 */
libMU_Time_t libMU_Timer_Current(void);

/**
 * Start time measurement
 * @param timer_sw	Pointer to variable where the initial time is stored
 */
void	libMU_Timer_StopwatchStart(libMU_Timer_Stopwatch_t* timer_sw);

/**
 * Read elapsed time since measurement start or previous measure
 * @param timer_sw	Pointer to variable where the initial time is stored
 * @return 			Return elapsed time since measurement start or previous measure
 */
libMU_Time_t	libMU_Timer_StopwatchMeasure(libMU_Timer_Stopwatch_t* timer_sw);

/**
 * Read elapsed time since measurement start or previous measure
 * @param timer_sw	Pointer to variable where the initial time is stored
 * @return 			Return elapsed time since measurement start or previous measure
 * @note			Does not change initial time
 */
libMU_Time_t	libMU_Timer_StopwatchPeek(const libMU_Timer_Stopwatch_t* timer_sw);

/**
 * Delay for a specified amount of time
 * @param delay		Delay time in us
 */
void	libMU_Timer_Delay(uint32_t delay);

/**
 * Delay for a specified amount of time from an specified starting time
 * @param ref		Pointer to starting time information
 * @param delay		Delay time in us
 */
void	libMU_Timer_DelayFrom(libMU_Time_t* ref, uint32_t delay);

/**
 * Convert time to nanoseconds
 * @param time		Time to convert (in internal units)
 * @return			Converted time in ns
 */
static __inline uint32_t	libMU_Timer_ConvertTo_ns(libMU_Time_t time)
{
	uint64_t t_ns = (uint64_t)time * libMU_Timer_PERIOD_NS;
	return (uint32_t)( t_ns );
}

/**
 * Convert time to microseconds
 * @param time		Time to convert (in internal units)
 * @return			Converted time in us
 */
static __inline uint32_t	libMU_Timer_ConvertTo_us(libMU_Time_t time)
{
	uint64_t t_ns = (uint64_t)time * libMU_Timer_PERIOD_NS;
	return (uint32_t)( t_ns / 1000U );
}

/**
 * Convert time to milliseconds
 * @param time		Time to convert (in internal units)
 * @return			Converted time in ms
 */
static __inline uint32_t	libMU_Timer_ConvertTo_ms(libMU_Time_t time)
{
	uint64_t t_ns = (uint64_t)time * libMU_Timer_PERIOD_NS;
	return (uint32_t)( t_ns / 1000000U );
}

/**
 * Convert time to internal units from microseconds
 * @param time		Time to convert
 * @return			Converted time in internal units
 */
static __inline libMU_Time_t	libMU_Timer_ConvertFrom_us(uint64_t time)
{
	libMU_Time_t t_iu = ( time*1000U ) / libMU_Timer_PERIOD_NS;
	return t_iu;
}

/**
 * Convert time to internal units from milliseconds
 * @param time		Time to convert
 * @return			Converted time in internal units
 */
static __inline libMU_Time_t	libMU_Timer_ConvertFrom_ms(uint64_t time)
{
	libMU_Time_t t_iu = ( time*1000000U ) / libMU_Timer_PERIOD_NS;
	return t_iu;
}

#endif /*LIBMU_TIMER_H_*/
/**
 * @}
 */
