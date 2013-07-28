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
#ifndef LIBMU_CPU_H_
#define LIBMU_CPU_H_

#include <stdint.h>
#include <inc/hw_types.h>
#include <driverlib/interrupt.h>
#include <driverlib/cpu.h>

/**
 * Set CPU clock frequency
 * @param freq	Desired clock frequency
 * @return		true if correctly set
 * @note 		Some clock frequencies may not be available
 * 				In those cases the closest frequency is chosen
 */
uint32_t	libMU_CPU_ClockSet(uint32_t freq);

/**
 * Get CPU clock frequency
 * @return		CPU clock frequency
 */
static __inline uint32_t	libMU_CPU_ClockGet(void);
static __inline uint32_t	libMU_CPU_ClockGet(void)
{
	extern uint32_t libMU_CPU_Clock;
	return libMU_CPU_Clock;
}

/**
 * Enable global interrupts
 * @return	true if previously enabled
 */
static __inline uint32_t libMU_CPU_InterruptEnable();
static __inline uint32_t libMU_CPU_InterruptEnable()
{
	return !IntMasterEnable();
}
/**
 * Disable global interrupts
 * @return	true if previously enabled
 */
static __inline uint32_t libMU_CPU_InterruptDisable();
static __inline uint32_t libMU_CPU_InterruptDisable()
{
	return !IntMasterDisable();
}

/**
 * Enter sleep mode
 * @param deep	Enable deep sleep mode
 */
static __inline void libMU_CPU_Sleep(uint32_t deep);
static __inline void libMU_CPU_Sleep(uint32_t deep)
{
	CPUwfi();
}

#endif/*LIBMU_CPU_H_*/
/**
 * @}
 */
