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
#include <libMU/analog_digital.h>

/**
 * @param value	Value to convert (using the internal temperature sensor representation)
 * @return 		Temperature value
 * @note
 * - The temperature sensor measures the internal CPU temperature!!
 * - A value of 0 corresponds to 0 C
 * - A value of 100 corresponds to 10 C
 * - A value of -100 corresponds to -10 C
 */
int libMU_Temperature_Convert(unsigned long value)
{
	int temp = (int)value;
    temp = 1475 - (temp * 2250) / 1023;	/* From stellaris datasheet info */
	return temp;
}	

/**
 * Read temperature value from internal temperature sensor
 * connected to the A/D input
 * @return 		Temperature value
 * @note
 * - The temperature sensor measures the internal CPU temperature!!
 * - A value of 0 corresponds to 0 C
 * - A value of 100 corresponds to 10 C
 * - A value of -100 corresponds to -10 C
 * - We need to start the AD conversion by using the libMU_AD_StartConversion() function
 *   EACH time we need to measure the temperature
 * @see libMU_AD_StartConversion
 */
int libMU_Temperature_Get()
{
	return libMU_Temperature_Convert( libMU_AD_GetChannelValue( ADC_CHANNEL_TS ) );
}	

/**
 * @}
 */
