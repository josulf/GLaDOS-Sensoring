/**
 * @addtogroup	libMU_Stopwatch
 * Library for timer functions
 * @{
 ********************************************************************
 * @author		Eñaut Muxika <emuxika at mondragon dot edu>
 * @date		2011/7/10
 * @copyright	BSDL
 ********************************************************************
 */
#include <stdint.h>
#include <inc/hw_types.h>		/* New data types */
#include <inc/hw_memmap.h>		/* Peripheral memory map */
#include <inc/hw_timer.h>		/* Interrupt vector definitions */
#include <driverlib/sysctl.h>	/* System control functions */
#include <driverlib/timer.h>	/* Timer functions */
#include <inc/hw_ints.h>		/* Interrupt vector definitions */
#include <driverlib/interrupt.h>/* Interrupt functions */
#include <libMU/timer.h>
#include <libMU/cpu.h>

/**
 * Constants
 */
#define TIMER_MEAS	TIMER3
#define TIMER_CGEN	TIMER2

#define CONCAT(a,b)			(a ## b)
#define CONCAT3(a,b,c)		(a ## b ## c)
#define CONCAT_BASE(a)		CONCAT(a,_BASE)
#define CREATE_PERIPH(a)	CONCAT(SYSCTL_PERIPH_,a)
#define CREATE_INT(x)		CONCAT3(INT_,x,A)

#define TMEAS_BASE			CONCAT_BASE(TIMER_MEAS)
#define TCGEN_BASE			CONCAT_BASE(TIMER_CGEN)
#define TMEAS_SYSPERIPH		CREATE_PERIPH(TIMER_MEAS)
#define TCGEN_SYSPERIPH		CREATE_PERIPH(TIMER_CGEN)
#define TMEAS_INT			CREATE_INT(TIMER_MEAS)
#define TCGEN_INT			CREATE_INT(TIMER_CGEN)

#define	COUNT_UP			0	/* set to 1 if timer counts up 0 otherwise */

/**
 * Timer interrupt handler variables
 */
static libMU_Timer_Handler_t	libMU_Timer_HandlerFunction = 0;
static void*					libMU_Timer_HandlerParam = 0;
static uint32_t					libMU_Timer_Freq = 0;

/**
 * High order count of the timer (upper 32 bits of the count for the 64 bit counter)
 */
uint32_t	libMU_Timer_Count_HighOrder = 0;
uint32_t	libMU_Timer_Interrupts = 0;

/**
 * Local function prototypes
 */
void libMU_Timer_Handler_MEAS(void);
void libMU_Timer_Handler_CGEN(void);

/**
 * Enable interrupts on the timer HW
 * @param addressTimer	Memory address of the timer peripheral
 * @param flags			Interrupts that we want to enable
 * @note
 * the values for the flags can be (see driverlib/timer.h):
 * 	TIMER_TIMB_MATCH	- TimerB match interrupt
 * 	TIMER_CAPB_EVENT	- CaptureB event interrupt
 * 	TIMER_CAPB_MATCH	- CaptureB match interrupt
 * 	TIMER_TIMB_TIMEOUT	- TimerB time out interrupt
 * 	TIMER_TIMA_MATCH	- TimerA match interrupt
 * 	TIMER_RTC_MATCH		- RTC interrupt mask
 * 	TIMER_CAPA_EVENT	- CaptureA event interrupt
 * 	TIMER_CAPA_MATCH	- CaptureA match interrupt
 * 	TIMER_TIMA_TIMEOUT	- TimerA time out interrupt
 */
static __inline void libMU_Timer_IntEnable(uint32_t addressTimer, uint32_t flags)
{
    HWREG(addressTimer + TIMER_O_IMR) |= flags;
}

/**
 * Disable interrupts on the timer HW
 * @param addressTimer	Memory address of the timer peripheral
 * @param flags			Interrupts that we want to disable
 * @note
 * - The flags values are the same as in libMU_Timer_IntEnable() function
 */
static __inline void	libMU_Timer_IntDisable(uint32_t addressTimer, uint32_t flags)
{
    HWREG(addressTimer + TIMER_O_IMR) &= ~flags;
}

/**
 * Clear Interrupt on the timer HW
 * @param addressTimer	Memory address of the timer peripheral
 * @param flags			Interrupts that we want to clear
 * @note
 * - The flags values are the same as in libMU_Timer_IntEnable() function
 */
static __inline void	libMU_Timer_IntClear(uint32_t addressTimer, uint32_t flags)
{
    HWREG(addressTimer + TIMER_O_ICR) = flags;
}

/**
 * Get the interrupt status on the timer HW
 * @param addressTimer	Memory address of the timer peripheral
 * @return 				Active interrupts
 * @note
 * - The active interrupts are the same as in libMU_Timer_IntEnable() function
 */
static __inline uint32_t libMU_Timer_IntGetStatus(uint32_t addressTimer)
{
    return (uint32_t)HWREG(addressTimer + TIMER_O_MIS);
}

/**
 * Get the timer count value
 * @param addressTimer	Memory address of the timer peripheral
 * @return 				Count value
 */
static __inline uint32_t libMU_Timer_GetCount(uint32_t addressTimer)
{
    return (uint32_t)HWREG(addressTimer + TIMER_O_TAR);
}

/**
 * Set the timer reload value
 * @param addressTimer	Memory address of the timer peripheral
 * @param value			Reload value
 */
static __inline void	libMU_Timer_SetReload(uint32_t addressTimer, uint32_t value)
{
    HWREG(addressTimer + TIMER_O_TAILR) = value;
}

/**
 * Enable the timer
 * @param addressTimer	Memory address of the timer peripheral
 * @note 				Only TIMER_A is managed
 */
static __inline void	libMU_Timer_Enable(uint32_t addressTimer)
{
    HWREG(addressTimer + TIMER_O_CTL) |= TIMER_CTL_TAEN;
}

/**
 * Disable the timer
 * @param addressTimer	Memory address of the timer peripheral
 * @note 				Only TIMER_A is managed
 */
static __inline void	libMU_Timer_Disable(uint32_t addressTimer)
{
    HWREG(addressTimer + TIMER_O_CTL) &=~TIMER_CTL_TAEN;
}

/**
 * Initialize timer HW
 * @return	true if correctly initialized
 */
int	libMU_Timer_Initialize(void)
{	
	/* Set CPU frequency to 50MHz */
	libMU_CPU_ClockSet(50000000);
	/* Initialize TIMER_MEAS to measure times */
	SysCtlPeripheralEnable	( TMEAS_SYSPERIPH );	/* Enable TIMER_MEAS */
	/* Set Timer 1 as a 32 bit periodic timer with maximum count & clear interrupt signal */
    TimerControlTrigger		( TMEAS_BASE, TIMER_BOTH, false );	/* Disable ADC trigger */
    TimerConfigure			( TMEAS_BASE, TIMER_CFG_32_BIT_PER );
    /* Setup interrupt system */
    libMU_Timer_IntDisable	( TMEAS_BASE, TIMER_TIMA_TIMEOUT );
    libMU_Timer_IntClear	( TMEAS_BASE, TIMER_TIMA_TIMEOUT );
    libMU_Timer_IntEnable	( TMEAS_BASE, TIMER_TIMA_TIMEOUT );
    IntRegister				( TMEAS_INT, libMU_Timer_Handler_MEAS );
    IntEnable				( TMEAS_INT );
    libMU_Timer_SetReload	( TMEAS_BASE, libMU_Timer_MAX_COUNT );
    libMU_Timer_Enable		( TMEAS_BASE );

	/* Initialize TIMER_CGEN to generate interrupts */
	SysCtlPeripheralEnable	( TCGEN_SYSPERIPH );	/* Enable TIMER_CGEN */
	/* Set Timer 1 as a 32 bit periodic timer with maximum count & clear interrupt signal */
    TimerControlTrigger		( TCGEN_BASE, TIMER_BOTH, false );	/* Disable ADC trigger */
    TimerConfigure			( TCGEN_BASE, TIMER_CFG_32_BIT_PER );
    /* Setup interrupt system */
    libMU_Timer_IntDisable	( TCGEN_BASE, TIMER_TIMA_TIMEOUT );
    libMU_Timer_IntClear	( TCGEN_BASE, TIMER_TIMA_TIMEOUT );
    libMU_Timer_IntEnable	( TCGEN_BASE, TIMER_TIMA_TIMEOUT );
    IntRegister				( TCGEN_INT, libMU_Timer_Handler_CGEN );
    IntEnable				( TCGEN_INT );

    /* Global enable */
    libMU_CPU_InterruptEnable();
    return 1;
}

/**
 * Get current time
 * @return 	Current time
 */
libMU_Time_t libMU_Timer_Current(void)
{
	libMU_Timer_Stopwatch_t time;
	time.t32[0] = libMU_Timer_GetCount( TMEAS_BASE );
#if COUNT_UP == 0
	time.t32[0] = ~time.t32[0];
#endif
#ifdef LIBMU_TIMER_USE_64_BITS
	time.t32[1] = libMU_Timer_Count_HighOrder;
#endif
	return time.time;
}

/**
 * Start time measurement
 * @param timer_sw	Pointer to variable where the initial time is stored
 */
void	libMU_Timer_StopwatchStart(libMU_Timer_Stopwatch_t* time_sw)
{
	time_sw->t32[0] = libMU_Timer_GetCount( TMEAS_BASE );
#if COUNT_UP == 0
	time_sw->t32[0] = ~time_sw->t32[0];
#endif
#ifdef LIBMU_TIMER_USE_64_BITS
	time_sw->t32[1] = libMU_Timer_Count_HighOrder;
#endif
}

/**
 * Read elapsed time since measurement start or previous measure
 * @param timer_sw	Pointer to variable where the initial time is stored
 * @return 			Return elapsed time since measurement start or previous measure
 */
libMU_Time_t	libMU_Timer_StopwatchMeasure(libMU_Timer_Stopwatch_t* time_sw)
{
	libMU_Timer_Stopwatch_t current, diff;
	current.t32[0] = libMU_Timer_GetCount( TMEAS_BASE );
#if COUNT_UP == 0
	current.t32[0] = ~current.t32[0];
#endif
#ifdef LIBMU_TIMER_USE_64_BITS
	current.t32[1] = libMU_Timer_Count_HighOrder;
#endif
	diff.time = current.time - time_sw->time;
	time_sw->time = current.time;
	return diff.time;
}

/**
 * Read elapsed time since measurement start or previous measure
 * @param timer_sw	Pointer to variable where the initial time is stored
 * @return 			Return elapsed time since measurement start or previous measure
 * @note			Does not change initial time
 */
libMU_Time_t	libMU_Timer_StopwatchPeek(const libMU_Timer_Stopwatch_t* time_sw)
{
	libMU_Timer_Stopwatch_t current, diff;
	current.t32[0] = libMU_Timer_GetCount( TMEAS_BASE );
#if COUNT_UP == 0
	current.t32[0] = ~current.t32[0];
#endif
#ifdef LIBMU_TIMER_USE_64_BITS
	current.t32[1] = libMU_Timer_Count_HighOrder;
#endif
	diff.time = current.time - time_sw->time;
	return diff.time;
}

/**
 * Delay for a specified amount of time
 * @param delay		Delay time in us
 */
void	libMU_Timer_Delay(uint32_t delay)
{
	libMU_Timer_Stopwatch_t ref;
	libMU_Timer_StopwatchStart( &ref );
	delay = libMU_Timer_ConvertFrom_us( delay );
	while( libMU_Timer_StopwatchPeek( &ref ) < delay ) {};
}

/**
 * Delay for a specified amount of time from an specified starting time
 * @param ref		Pointer to starting time information
 * @param delay		Delay time in us
 */
void	libMU_Timer_DelayFrom(libMU_Time_t* prevTime, uint32_t delay)
{
	libMU_Time_t newTime = *prevTime;
	newTime += libMU_Timer_ConvertFrom_us( delay );
	while( libMU_Timer_Current() < newTime ) {};
	*prevTime = newTime;
}

/**
 * Set the timer frequency for overflow generation
 * @param freq		Frequency value
 */
void libMU_Timer_SetFrequency(uint32_t freq)
{
	uint32_t count;
	libMU_Timer_Freq = freq;
	if( freq == 0 ) {
		libMU_Timer_Disable( TCGEN_BASE );
		return;
	}
	count = (libMU_Timer_FREQ + (freq>>1) ) / freq;
	libMU_Timer_SetReload( TCGEN_BASE, count - 1 );
	if( libMU_Timer_HandlerFunction )
		libMU_Timer_Enable( TCGEN_BASE );
	else
		libMU_Timer_Disable( TCGEN_BASE );
}

/**
 * Set the timer function called at each timer overflow
 * @param function	Pointer to function
 * @param param		Pointer to function parameters
 */
void libMU_Timer_SetHandler( libMU_Timer_Handler_t function, void* param )
{
	libMU_Timer_HandlerFunction = function;
	libMU_Timer_HandlerParam = param;
	if( libMU_Timer_HandlerFunction && libMU_Timer_Freq > 0 )
		libMU_Timer_Enable( TCGEN_BASE );
	else
		libMU_Timer_Disable( TCGEN_BASE );
}

/*
######################################################################################
######################################################################################
######################################################################################
*/

/**
 * Called on timer overflow
 */
/*interrupt*/
void libMU_Timer_Handler_MEAS(void)
{
	uint32_t status = libMU_Timer_IntGetStatus(TMEAS_BASE);
	if( status & TIMER_TIMA_TIMEOUT ) {
		libMU_Timer_Count_HighOrder++;
	}
	libMU_Timer_IntClear(TMEAS_BASE, status);
}

/**
 * Called on timer overflow
 */
/*interrupt*/
void libMU_Timer_Handler_CGEN(void)
{
	uint32_t status = libMU_Timer_IntGetStatus(TCGEN_BASE);
	if( status & TIMER_TIMA_TIMEOUT ) {
		libMU_Timer_Interrupts++;
		if( libMU_Timer_HandlerFunction ) {
			libMU_Timer_HandlerFunction(libMU_Timer_HandlerParam);
		}
	}
	libMU_Timer_IntClear(TCGEN_BASE, status);
}

/**
 * @}
 */
