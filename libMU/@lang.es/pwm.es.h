/**
 * @addtogroup	libMU_PWM
 * Librería para el manejo de las salidas de modulación de anchura de pulsos PWM
 * @{
 ********************************************************************
 * @author		Eñaut Muxika <emuxika at mondragon dot edu>
 * @date		2011/7/10
 * @copyright	BSDL
 ********************************************************************
 */
#ifndef _LIBMU_PWM_ES_H_
#define _LIBMU_PWM_ES_H_
#include <libMU/pwm.h>

/**
 * Identificadores de las salidas PWM
 */
#define LIBMU_PWM_0_		LIBMU_PWM_0
#define LIBMU_PWM_1_		LIBMU_PWM_1
#define LIBMU_PWM_2_		LIBMU_PWM_2
#define LIBMU_PWM_3_		LIBMU_PWM_3
#define LIBMU_PWM_4_		LIBMU_PWM_4
#define LIBMU_PWM_5_		LIBMU_PWM_5
#define	LIBMU_PWM_NUMERO	LIBMU_PWM_NUMBER
				/* Grupos de salidas PWM para control de chopper con tiempos muertos */
#define	LIBMU_PWM_GRP_01_	LIBMU_PWM_GRP_01
#define	LIBMU_PWM_GRP_23_	LIBMU_PWM_GRP_23
#define	LIBMU_PWM_GRP_45_	LIBMU_PWM_GRP_45

/**
 * Configura una salida PWM
 * @param idSalida	Identificador de la salida PWM
 * @param freq		Frecuencia PWM deseada
 * @return			1 si hay error, 0 si se configura bién
 * @note
 * Cuidado con las siguientes salidas:
 * - PWM0 está conectado al diodo LED
 * - PWM1 está conectado al zumbador
 * - PWM4 está conectado a la entrada del boton arriba (riesgo de cortocircuito!!!)
 * - PWM5 está conectado a la entrada del boton abajo (riesgo de cortocircuito!!!)
 */
static inline int	libMU_PWM_InicializaSalida( libMU_PWM_Constants_t idSalida, uint32_t freq )
{
	return libMU_PWM_InitializeOutput(idSalida,freq);
}

/**
 * Configura un grupo de salidas PWM para control de un chopper con tiempos muertos
 * @param idGrupo		Identificador del grupo PWM
 * @param freq			Frecuencia PWM deseada
 * @param tiempomuerto	Tiempo muerto requerido en ns
 * @return				1 si hay error, 0 si se configura bién
 * @note
 * Cuidado con las siguientes salidas:
 * - LIBMU_PWM_GRP_01 están conectados al LED y al zumbador
 * - LIBMU_PWM_GRP_45 están conectados a los botones arriba/abajo (riesgo de cortocircuito!!!)
 */
static inline int	libMU_PWM_InicializaSalidasChopper( libMU_PWM_Constants_t idGrupo, uint32_t freq, uint32_t tiempomuerto )
{
	return libMU_PWM_InitializeChopperOutput(idGrupo,freq,tiempomuerto);
}

/**
 * Fijar la anchura del pulso PWM
 * @param idSalida	Identificador de salida PWM
 * @param anchura	En centésimas de porcentaje : 0 -> 0.00%, 10000 -> 100.00%, 4567 -> 45.67%
 * @note
 * - La resolución real es 100*Fpwm/Fcpu (en porcentaje)
 * - Si la frecuencia del PWM está por encima de 400kHz esta función no sirve,
 *   usar la función libMU_PWM_SetPulseWidthPPM() en ese caso
 * @see libMU_PWM_SetPulseWidthPPM
 */
static inline void	libMU_PWM_FijarAnchuraPulso( libMU_PWM_Constants_t idSalida, uint32_t anchura )
{
	libMU_PWM_SetPulseWidth(idSalida,anchura);
}

/**
 * Fijar la anchura del pulso PWM
 * @param idSalida	Identificador de salida PWM
 * @param anchura	En partes por mil millones (PPB) : 0 -> 0%, 1000000000 -> 100%, 456789012 -> 45.6789012%
 * @note
 * - La resolución real es 100*Fpwm/Fcpu (en porcentaje)
 */
static inline void	libMU_PWM_FijarAnchuraPulsoPPB( libMU_PWM_Constants_t idSalida, uint32_t anchura )
{
	libMU_PWM_SetPulseWidthPPM(idSalida,anchura);
}

/**
 * Fijar la frecuencia de PWM
 * @param idSalida	Identificador de salida PWM
 * @param freq		en Hz
 * @note
 * La resolución en frecuencia depende de la frecuencia del procesador
 */
static inline void	libMU_PWM_SetFrequency( libMU_PWM_Constants_t idSalida, uint32_t freq )
{
	libMU_PWM_SetFrequency(idSalida,freq);
}

/**
 * Obtener el periodo del PWM en unidades internas (ticks de reloj)
 * @param idSalida	Identificador de salida PWM
 * @return			Periodo del generador PWM
 */
static inline uint32_t	libMU_PWM_ObtenPeriodo( libMU_PWM_Constants_t idSalida )
{
	return libMU_PWM_GetPeriodIU(idSalida);
}

/**
 * Obtener la anchura del pulso PWM en unidades internas (ticks de reloj)
 * @param idSalida	Identificador de salida PWM
 * @return			Anchura del pulso PWM
 */
static inline uint32_t	libMU_PWM_ObtenAnchuraPulso( libMU_PWM_Constants_t idSalida )
{
	return libMU_PWM_GetPulseWidthIU(idSalida);
}

/**
 * Deshabilita la salida PWM
 * @param idSalida	Identificador de salida PWM
 */
static inline void	libMU_PWM_OutputDisable( libMU_PWM_Constants_t idSalida )
{
	libMU_PWM_OutputDisable(idSalida);
}
/**
 * Habilita la salida PWM
 * @param idSalida	Identificador de salida PWM
 */
static inline void	libMU_PWM_OutputEnable( libMU_PWM_Constants_t idSalida )
{
	libMU_PWM_OutputEnable(idSalida);
}

#endif /*_LIBMU_PWM_ES_H_*/
/**
 * @}
 */
