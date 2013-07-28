/**
 * @addtogroup	libMU_Delay
 * Delay function library
 * @{
 ********************************************************************
 * @author		Eñaut Muxika <emuxika at mondragon dot edu>
 * @date		2011/7/10
 * @copyright	BSDL
 ********************************************************************
 */
#ifndef DELAY_H_
#define DELAY_H_

#include <stdint.h>

/**
 * @param	repetitions		number of times the delay is repeated
 * @note
 * Each repetition takes approximately 3 clock cycles
 * (that is, 60ns at 50MHz)
 */
extern void SysCtlDelay( unsigned long repetitions );
static __inline void libMU_Delay(uint32_t repetitions);
static __inline void libMU_Delay(uint32_t repetitions)
{
	SysCtlDelay( (unsigned long)repetitions );
}

#endif /*DELAY_H_*/
/**
 * @}
 */
