/**
 * @addtogroup	libMU_LED
 * Librería para el uso sencillo de los indicadores LED
 * @{
 ********************************************************************
 * @author		Eñaut Muxika <emuxika at mondragon dot edu>
 * @date		2011/7/10
 * @copyright	BSDL
 ********************************************************************
 */
#ifndef LIBMU_LED_ES_H_
#define LIBMU_LED_ES_H_
#include <libMU/leds.h>

/**
 * Constantes usadas en la identificación de los leds
 */
typedef enum _libMU_LED_Identificadores_t {
	LED_uno	= LED_1,	/* Status LED: Port F, pin 0 */
	LED_dos	= LED_2,	/* Ethernet LED: Port F, pin 2 */
	LED_tres= LED_3		/* Ethernet LED: Port F, pin 3 */
} libMU_LED_Identificadores_t;

/**
 * Inicializa el HW de los leds de la placa
 */
static inline void	libMU_LED_Inicializa(void)
{
	libMU_LED_Initialize();
}

/**
 * Inicializa el HW de los leds de la placa (del interface ethernet LED_2 y LED_3)
 */
static inline void	libMU_LED_InicializaEth(void)
{
	libMU_LED_InitializeEth();
}

/**
 * Enciende la luz LED correspondiente
 * @param	led		Código del LED a encender
 */
static inline void	libMU_LED_Enciende(libMU_LED_Identifiers_t led)
{
	libMU_LED_On(led);
}

/**
 * Apaga la luz LED correspondiente
 * @param	led		Código del LED a apagar
 */
static inline void	libMU_LED_Apaga(libMU_LED_Identifiers_t led)
{
	libMU_LED_Off(led);
}

/**
 * Conmuta la luz LED correspondiente
 * @param	led		Código del LED a conmutar
 */
static inline void	libMU_LED_Conmuta(libMU_LED_Identifiers_t led)
{
	libMU_LED_Toggle(led);
}

#endif /*LIBMU_LED_ES_H_*/
/**
 * @}
 */
