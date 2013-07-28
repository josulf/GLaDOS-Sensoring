/**
 * @addtogroup	libMU_FreeRTOS
 * FreeRTOS header include library
 * @{
 ********************************************************************
 * @author		Eñaut Muxika <emuxika at mondragon dot edu>
 * @date		2013/4/11
 * @copyright	BSDL
 ********************************************************************
 */
#ifndef LIBMU_FREERTOS_H_
#define LIBMU_FREERTOS_H_

#include <FreeRTOS.h>

/**
 * Setup the required interrupt vectors for FreeRTOS
 * @note	This function should be called before vStartScheduler()
 */
void libMU_FreeRTOS_SetupInterruptVectors(void);

#endif/*LIBMU_FREERTOS_H_*/
