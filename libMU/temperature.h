/**
 * @addtogroup	libMU_Temperature
 * Library for temperature measurement functions
 * @{
 ********************************************************************
 * @author		Eñaut Muxika <emuxika at mondragon dot edu>
 * @date		2011/7/10
 * @copyright	BSDL
 * @note		Necesita la libreria libMU_AD (analogico digital) para funcionar
 ********************************************************************
 */
#ifndef TEMPERATURE_H_
#define TEMPERATURE_H_

/**
 * @param value	Value to convert (using the internal temperature sensor representation)
 * @return 		Temperature value
 * @note
 * - The temperature sensor measures the internal CPU temperature!!
 * - A value of 0 corresponds to 0 C
 * - A value of 100 corresponds to 10 C
 * - A value of -100 corresponds to -10 C
 */
int libMU_Temperature_Convert(unsigned long value);

/**
 * Read temperature value from internal temperature sensor
 * connected to the A/D input
 * @return 		Temperature value
 * @note
 * - The temperature sensor measures the internal CPU temperature!!
 * - A value of 0 corresponds to 0 C
 * - A value of 100 corresponds to 10 C
 * - A value of -100 corresponds to -10 C
 */
int libMU_Temperature_Get();

#endif /*TEMPERATURE_H_*/
/**
 * @}
 */
