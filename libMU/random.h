/**
 * @addtogroup	libMU_Random
 * Random number generation library
 * @{
 ********************************************************************
 * @author		Eñaut Muxika <emuxika at mondragon dot edu>
 * @date		2012/5/28
 * @copyright	BSDL
 ********************************************************************
 */
#ifndef _RANDOM_H_
#define _RANDOM_H_

#include <stdint.h>
/**
 * Add 8 bit value to the entropy pool.
 * @param ulEntropy	New entropy value
 * @note 			The pool consists of 64 bytes
 */
void	libMU_Random_AddEntropy(uint8_t ulEntropy);

/**
 * Initialize the random number generator
 */
void 	libMU_Random_Initialize(void);

/**
 * @return	Random number
 */
uint32_t libMU_Random_GetValue(void);

#endif /*_RANDOM_H_*/
/**
 * @}
 */
