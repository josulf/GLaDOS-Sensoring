/**
 * @addtogroup	libMU_Aleatorio
 * Librería para la generación de números aleatorios
 * @{
 ********************************************************************
 * @author		Eñaut Muxika <emuxika at mondragon dot edu>
 * @date		2012/5/28
 * @copyright	BSDL
 ********************************************************************
 */
#ifndef _ALEATORIO_H_
#define _ALEATORIO_H_

#include <libMU/random.h>

/**
 * Añade valores entrópicos al conjunto
 * @param	ulEntropy	New entropy value
 */
static inline void	libMU_Aleatorio_AnadeEntropia(unsigned long ulEntropia)
{
	libMU_Random_AddEntropy(ulEntropia);
}

/**
 * Inicializa el generador de números aleatorios
 */
static inline void 	libMU_Aleatorio_Inicializa(void)
{
	libMU_Random_Initialize();
}

/**
 * @return	Número aleatorio
 */
unsigned long libMU_Aleatorio_ObtenValor(void)
{
	return libMU_Random_GetValue();
}

#endif /*_ALEATORIO_H_*/
/**
 * @}
 */
