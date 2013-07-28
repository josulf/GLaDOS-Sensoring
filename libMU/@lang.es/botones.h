/**
 * @addtogroup	libMU_Button
 * Libreria para el uso sencillo de los botones
 * @{
 ********************************************************************
 * @author		Eñaut Muxika <emuxika at mondragon dot edu>
 * @date		2011/7/10
 * @copyright	BSDL
 ********************************************************************
 */
#ifndef BOTONES_H_
#define BOTONES_H_
#include <libMU/buttons.h>

/**
 * Constantes usadas en la gestion de botones
 */
typedef enum _libMU_Boton_Identificador_t {
	BOTON_ARRIBA	= BUTTON_UP,
	BOTON_ABAJO		= BUTTON_DOWN,
	BOTON_IZQUIERDA = BUTTON_LEFT,
	BOTON_DERECHA	= BUTTON_RIGHT,
	BOTON_SELECT	= BUTTON_SELECT,

	BOTON_TODOS		= BUTTON_ALL
} libMU_Boton_Identificador_t;

/**
 * Inicializa el HW de los botones de la placa
 */
void 	libMU_Boton_Inicializa(void);

/**
 * Lee el estado de los botones de la placa
 */
static inline libMU_Button_t	libMU_Boton_ObtenEstadoCompleto(void)
{
	return libMU_Button_GetFullStatus();
}

/**
 * Comprueba si el boton o los botones correspondientes estan pulsado
 * @param	botones	Código del boton o los botones a comprobar
 * @return			false si no pulsado, true si pulsado
 */
static inline unsigned int	libMU_Boton_ObtenEstado( libMU_Boton_Identificador_t botones )
{
	return libMU_Button_GetStatus( botones );
}

/**
 * Comprueba si se acaba de pulsar el boton correspondiente
 * @param	botones	Código del boton o los botones a comprobar
 * @return			true si se ha pulsado, si no false
 * @note
 * - Se detecta el flanco de subida en la señal correspondiente
 */
static inline unsigned int	libMU_Boton_Pulsado( libMU_Boton_Identificador_t botones )
{
	return libMU_Button_Pressed(botones);
}

/**
 * Comprueba si se acaba de soltar el boton correspondiente
 * @param	botones	Código del boton o los botones a comprobar
 * @return			true si se ha soltado, si no false
 * @note
 * - Se detecta el flanco de bajada en la señal correspondiente
 */
static inline unsigned int	libMU_Boton_Soltado( libMU_Boton_Identificador_t botones )
{
	return libMU_Button_Depressed(botones);
}

#endif /*BOTONES_H_*/
/**
 * @}
 */
