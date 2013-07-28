/**
 * @addtogroup	libMU_Temperatura
 * Librería para el uso sencillo del sensor de temperatura analógico
 * @{
 ********************************************************************
 * @author		Eñaut Muxika <emuxika at mondragon dot edu>
 * @date		2011/7/10
 * @copyright	BSDL
 * @note		Necesita la libreria libMU_AD (analogico digital) para funcionar
 ********************************************************************
 */
#ifndef TEMPERATURA_H_
#define TEMPERATURA_H_
#include <libMU/temperature.h>

/**
 * @return	Valor de la temperatura
 * @note
 * - Un valor 0 corresponde a 0ºC
 * - Un valor 100 corresponde a 10ºC
 * - Un valor -100 corresponde a -10ºC
 */
static inline int libMU_Temperatura_Convertir(unsigned long valor)
{
	return libMU_Temperature_Convert(valor);
}

/**
 * @return	Valor de la temperatura
 * @note
 * - Un valor 0 corresponde a 0ºC
 * - Un valor 100 corresponde a 10ºC
 * - Un valor -100 corresponde a -10ºC
 * - El sensor de temperatura mide la temperatura interna del procesador!!
 */
static inline int libMU_Temperatura_Obtener()
{
	return libMU_Temperature_Get();
}

#endif /*TEMPERATURA_H_*/
/**
 * @}
 */
