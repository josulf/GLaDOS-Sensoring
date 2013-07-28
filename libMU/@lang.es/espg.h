/**
 * @addtogroup	libMU_ESPG
 * Libreria para el uso sencillo de entradas/salidas de proposito general
 * @{
 ********************************************************************
 * @author		Eñaut Muxika <emuxika at mondragon dot edu>
 * @date		2011/7/10
 * @copyright	BSDL
 * @note
 * Esta librería puede provocar la DESTRUCCIÓN de la placa si se usa de manera incorrecta
 ********************************************************************
 */
#ifndef LIBMU_ESGP_H_
#define LIBMU_ESGP_H_

/**
 * Constantes usadas en la gestion de E/S
 */
typedef enum _libMU_ESPG_ID_t {
	/* Port A */
	ESPG_PA		= GPIO_PA,
	ESPG_PA0	= GPIO_PA0,
	ESPG_PA1	= GPIO_PA1,
	ESPG_PA2	= GPIO_PA2,
	ESPG_PA3	= GPIO_PA3,
	ESPG_PA4	= GPIO_PA4,
	ESPG_PA5	= GPIO_PA5,
	ESPG_PA6	= GPIO_PA6,
	ESPG_PA7	= GPIO_PA7,

	/* Port B */
	ESPG_PB		= GPIO_PB,
	ESPG_PB0	= GPIO_PB0,
	ESPG_PB1	= GPIO_PB1,
	ESPG_PB2	= GPIO_PB2,
	ESPG_PB3	= GPIO_PB3,
	ESPG_PB4	= GPIO_PB4,
	ESPG_PB5	= GPIO_PB5,
	ESPG_PB6	= GPIO_PB6,
	ESPG_PB7	= GPIO_PB7,

	/* Port C */
	ESPG_PC		= GPIO_PC,
	ESPG_PC0	= GPIO_PC0,
	ESPG_PC1	= GPIO_PC1,
	ESPG_PC2	= GPIO_PC2,
	ESPG_PC3	= GPIO_PC3,
	ESPG_PC4	= GPIO_PC4,
	ESPG_PC5	= GPIO_PC5,
	ESPG_PC6	= GPIO_PC6,
	ESPG_PC7	= GPIO_PC7,

	/* Port D */
	ESPG_PD		= GPIO_PD,
	ESPG_PD0	= GPIO_PD0,
	ESPG_PD1	= GPIO_PD1,
	ESPG_PD2	= GPIO_PD2,
	ESPG_PD3	= GPIO_PD3,
	ESPG_PD4	= GPIO_PD4,
	ESPG_PD5	= GPIO_PD5,
	ESPG_PD6	= GPIO_PD6,
	ESPG_PD7	= GPIO_PD7,

	/* Port E */
	ESPG_PE		= GPIO_PE,
	ESPG_PE0	= GPIO_PE0,
	ESPG_PE1	= GPIO_PE1,
	ESPG_PE2	= GPIO_PE2,
	ESPG_PE3	= GPIO_PE3,

	/* Port F */
	ESPG_PF		= GPIO_PF,
	ESPG_PF0	= GPIO_PF0,
	ESPG_PF1	= GPIO_PF1,
	ESPG_PF2	= GPIO_PF2,
	ESPG_PF3	= GPIO_PF3,

	/* Port G */
	ESPG_PG		= GPIO_PG,
	ESPG_PG0	= GPIO_PG0,
	ESPG_PG1	= GPIO_PG1,

	ESPG_PORTS	= 7
} libMU_ESPG_ID_t;

/**
 * Specificación del tipo de salidas para la función libMU_GPIO_Initialize()
 * @see	libMU_GPIO_Initialize()
 */
typedef enum _libMU_ESPG_Tipo_t {
	ESPG_ENTRADA		= GPIO_INPUT,				/* Entrada */
	ESPG_SALIDA			= GPIO_OUPUT,				/* Salida estandar */
	ESPG_TIPO_STD		= GPIO_TYPE_STD,			/* E/S estándar */
	ESPG_TIPO_PULLUP	= GPIO_TYPE_WEAK_PULLUP,	/* E/S con resistencia pullup debil */
	ESPG_TIPO_PULLDOWN	= GPIO_TYPE_WEAK_PULLDOWN,	/* E/S con resistencia pulldown debil */

	ESPG_MASCARA_IO		= GPIO_MASK_INOUT,
	ESPG_MASCARA_TIPO	= GPIO_MASK_TYPE
} libMU_ESPG_Tipo_t;

/**
 * Inicializa los pines de un puerto de la placa como entradas o salidas
 * @param	port_and_pin_ID	Port and pin identifications
 * @param	tipo			Set pins as output if true, otherwise as inputs
 */
static inline void 	libMU_ESPG_Inicializa( libMU_ESPG_ID_t port_and_pins, libMU_ESPG_Tipo_t tipo )
{
	libMU_GPIO_Initialize(port_and_pins, tipo)
}

/**
 * Lee el estado de los pines de la placa
 * @param	puerto	Código del puerto del cual se quiere obtener el estado
 * @return			Devuelve el estado del puerto especificado
 */
static inline libMU_ESPG_t	libMU_ESPG_ObtenEstado( libMU_ESPG_ID_t puerto )
{
	return libMU_GPIO_GetPortStatus(puerto);
}

/**
 * Cambia el estado de los pines de la placa
 * @param	puerto	Código del puerto del cual se quiere cambiar el estado
 * @param	valor	Nuevo estado del puerto
 * @note			Solo se cambian realmente aquellos pines que sean salidas
 */
static inline void	libMU_ESPG_FijarEstado( libMU_ESPG_ID_t puerto, unsigned int valor )
{
	libMU_GPIO_SetPortStatus(puerto,valor);
}

/**
 * Activar pines (poner a "1")
 * @param	pines	Pin a activar
 * @note			Sólo se cambian los pines configurados como salida
 */
static inline void	libMU_ESPG_ActivaPin( libMU_ESPG_ID_t pines  )
{
	libMU_GPIO_SetPinsToOne(pines)
}

/**
 * Desactivar pines (poner a "0")
 * @param	pines	Pin a activar
 * @note			Sólo se cambian los pines configurados como salida
 */
static inline void	libMU_ESPG_DesactivaPin( libMU_ESPG_ID_t pines )
{
	libMU_GPIO_SetPinsToZero(pines);
}

/**
 * Conmutar pines
 * @param	pines	Pines a conmutar
 * @note			Sólo se cambian los pines configurados como salida
 */
static inline void	libMU_ESPG_ConmutaPin( libMU_ESPG_ID_t pines  )
{
	libMU_GPIO_TogglePins(pines)
}

/**
 * Devuelve la máscara de bits para el pin correspondiente
 * @param	pin		Código del pin a del cual se quiere obtener la máscara
 * @return			Máscara de bits
 */
static inline unsigned int	libMU_GPIO_GetBitMask( libMU_ESPG_ID_t pin )
{
	return ( pin & GPIO_PIN_MASK );
}

/**
 * Comprueba si el pin correspondiente esta activo
 * @param	pin		Código del pin a comprobar
 * @return			Estado digital en la entrada del pin
 */
static inline unsigned int	libMU_GPIO_GetStatus( libMU_ESPG_ID_t pin )
{
	return ( libMU_GPIO_GetPortStatus(pin) & libMU_GPIO_GetBitMask(pin) );
}

/**
 * Comprueba si se ha detectado un flanco de subida en el pin correspondiente
 * @param	pin		Código del pin a comprobar
 * @return			true si se detecta un flanco de subida en el pin
 */
static inline unsigned int	libMU_GPIO_GetStatusRisingEdge( libMU_ESPG_ID_t pin )
{
	libMU_GPIO_t b_prv = libMU_GPIO_PortStatus[ pin >> GPIO_PORT_POS ] & libMU_GPIO_GetBitMask(pin);
	libMU_GPIO_t b = libMU_GPIO_GetPortStatus( pin ) & libMU_GPIO_GetBitMask(pin);
	libMU_GPIO_PortStatus[ pin >> GPIO_PORT_POS ] = ( libMU_GPIO_PortStatus[ pin >> GPIO_PORT_POS ] & ~libMU_GPIO_GetBitMask(pin) ) | b;
	return ( b & ~b_prv );
}

/**
 * Comprueba si se ha detectado un flanco de bajada en el pin correspondiente
 * @param	pin		Código del pin a comprobar
 * @return			true si se detecta un flanco de bajada en el pin
 */
static inline unsigned int	libMU_GPIO_GetStatusFallingEdge( libMU_ESPG_ID_t pin )
{
	libMU_GPIO_t b_prv = libMU_GPIO_PortStatus[ pin >> GPIO_PORT_POS ] & libMU_GPIO_GetBitMask( pin );
	libMU_GPIO_t b = libMU_GPIO_GetPortStatus( pin ) & libMU_GPIO_GetBitMask( pin );
	libMU_GPIO_PortStatus[ pin >> GPIO_PORT_POS ] =
			( libMU_GPIO_PortStatus[ pin >> GPIO_PORT_POS ] & ~libMU_GPIO_GetBitMask( pin ) ) | b;
	return ( ~b & b_prv );
}

#endif /*LIBMU_ESGP_H_*/
/**
 * @}
 */
