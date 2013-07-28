/**
 * @addtogroup	libMU_CPU
 * CPU management library
 * @{
 ********************************************************************
 * @author		Eñaut Muxika <emuxika at mondragon dot edu>
 * @date		2013/5/28
 * @copyright	BSDL
 ********************************************************************
 * @note		The CPU management consists of:
 * 				- CPU clock management
 * 				- Global interrupt enable/disable
 * 				- CPU sleep mode enter
 */
#include <inc/hw_types.h>
#include <libMU/cpu.h>
#include <driverlib/sysctl.h>	/* System control functions */

/**
 * Global variables
 */
uint32_t libMU_CPU_Clock_Desired;
uint32_t libMU_CPU_Clock;

/**
 * Set CPU clock frequency
 * @param freq	Desired clock frequency
 * @return		true if correctly set
 * @note 		Some clock frequencies may not be available
 * 				In those cases the closest frequency is chosen
 */
uint32_t	libMU_CPU_ClockSet(uint32_t freq)
{
	if( freq == libMU_CPU_Clock_Desired ) return 1;
	SysCtlClockSet	( SYSCTL_SYSDIV_4 | SYSCTL_USE_PLL | SYSCTL_OSC_MAIN | SYSCTL_XTAL_8MHZ );
	libMU_CPU_Clock_Desired = freq;
	libMU_CPU_Clock = SysCtlClockGet();
	return 1;
}

/**
 * @}
 */
