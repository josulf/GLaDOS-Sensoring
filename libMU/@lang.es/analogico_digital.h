/**
 * @addtogroup	libMU_AD
 * libreria para el uso sencillo de los convertidores AD
 * @{
 ********************************************************************
 * @author		Eñaut Muxika <emuxika at mondragon dot edu>
 * @date		2012/5/28
 * @copyright	BSDL
 ********************************************************************
 */

#ifndef ANALOGICO_DIGITAL_H_
#define ANALOGICO_DIGITAL_H_

#include <libMU/analog_digital.h>

/**
 * Constantes usadas en la conversion analógico/digital
 */
enum libMU_AD_Constantes {
	ADC_CANAL_0 = ADC_CHANNEL_0,
	ADC_CANAL_1 = ADC_CHANNEL_1,
	ADC_CANAL_2 = ADC_CHANNEL_2,
	ADC_CANAL_3 = ADC_CHANNEL_3,
	ADC_CANAL_TS = ADC_CHANNEL_TS
};

/**
 * Inicializa el HW de la conversion analogico digital
 */
static inline void 	libMU_AD_Inicializa(void)
{
	return libMU_AD_Initialize();
}

/**
 * Inicia la conversión de todos los canales a la vez
 */
static inline void	libMU_AD_EmpiezaAConvertir(void)
{
	return libMU_AD_StartConversion();
}

/**
 * Obtiene el valor convertido del canal
 * @param	canal	Valor de canal ( ADC_CANAL_0, ADC_CANAL_1, ADC_CANAL_2, ADC_CANAL_3, ADC_CANAL_TS )
 * @return			Valor convertido
 * NOTAS:
 * - Un valor 0 corresponde a 0 voltios en el pin correspondiente
 * - Un valor 1024 corresponde a 3.0 voltions en el pin correspondiente
 * - Esta funcion tarda hasta 10us en devolver el resultado
 */
static inline unsigned long libMU_AD_ObtenValorCanal(int canal)
{
	return libMU_AD_GetChannelValue(canal);
}

#endif /*ANALOGICO_DIGITAL_H_*/
/**
 * @}
 */
