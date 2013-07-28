/**
 * @addtogroup	libMU_AD
 * Library for analog/digital converter functions
 * @{
 ********************************************************************
 * @author		Eñaut Muxika <emuxika at mondragon dot edu>
 * @date		2012/5/28
 * @copyright	BSDL
 ********************************************************************
 */

#ifndef LIBMU_ANALOG_DIGITAL_H_
#define LIBMU_ANALOG_DIGITAL_H_
#include <stdint.h>

/**
 * Constants to identify AD converter input channels
 */
typedef enum _libMU_AD_Channels_t {
	ADC_CHANNEL_0 = 0,
	ADC_CHANNEL_1 = 1,
	ADC_CHANNEL_2 = 2,
	ADC_CHANNEL_3 = 3,
	ADC_CHANNEL_TS = 4
} libMU_AD_Channels_t;

/**
 * Initialize the AD converter HW
 */
void 	libMU_AD_Initialize(void);

/**
 * Start the conversion of all configured channels at the same instant
 */
void	libMU_AD_StartConversion(void);

/**
 * Get the converted channel value
 * @param	channel		Select input channel ( ADC_CHANNEL_0, ADC_CHANNEL_1, ADC_CHANNEL_2, ... )
 * @return				Converted value
 * @note
 * - A value of 0 corresponds to 0 volts at the respective pin
 * - A value of 1024 corresponds to 3.0 volts at the respective pin
 * - This function may take up to 10us to get the converted value
 */
uint32_t libMU_AD_GetChannelValue(libMU_AD_Channels_t channel);

#endif/*LIBMU_ANALOG_DIGITAL_H_*/
/**
 * @}
 */
