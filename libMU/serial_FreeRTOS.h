/**
 * @addtogroup libMU_Serial
 * Library for easy access to the serial port
 * @{
 ********************************************************************
 * @author		Eñaut Muxika <emuxika at mondragon dot edu>
 * @date		2011/7/10
 * @copyright	BSDL
 ********************************************************************
 */
#ifndef LIBMU_SERIAL_FREERTOS_H_
#define LIBMU_SERIAL_FREERTOS_H_

#include <stdint.h>
#include <libMU/serial.h>
#include <FreeRTOS.h>
#include <task.h>
#include <queue.h>

/**
 * Reads a character from the serial port
 * @param where		Pointer where the received character is stored
 * @param timeout	Maximum time to wait for character (in FreeRTOS ticks)
 * @return			Number of characters read
 * @note			This function blocks the execution until it receives a character
 */
int	libMU_Serial_GetCharFreeRTOS(char* where, uint32_t timeout );

/**
 * Reads a character from the serial port without blocking the execution
 * @param where		Pointer where the received character is stored
 * @return			Number of characters read
 * @note			This function does NOT block the execution
 */
int	libMU_Serial_GetCharFreeRTOS_NB(char* where);

/**
 * Read a character from the serial port
 * @param queue		Queue to use for thread safe buffering
 * @param where		Pointer to array of characters where the string is stored
 * @param size		Maximum number of characters to read
 * @return			Number of characters read
 */
int	libMU_Serial_GetStringFreeRTOS( libMU_FIFO_t* queue, char* where, int size );

/**
 * Reads characters from the serial port until given character is received, a timeout occurs or
 * the buffer is full
 * @param	where		Pointer to array of characters where the string is stored
 * @param	size		Maximum number of characters to read
 * @param	end_char	Ending character
 * @param	timeout		Timeout value (in ms)
 * @return				Number of characters read
 * @note				The last character in buffer is always a 0
 */
int	libMU_Serial_GetUntilChar_FreeRTOS(char* where, int size, char end_char, uint32_t timeout );

/**
 * Set interrupt handler for the serial port and enable interrupt mode
 * @param	rxQueueSize		Size of the reception queue to allocate in FreeRTOS
 * @return					true if correctly setup
 */
int	libMU_Serial_SetupFreeRTOSHandler( uint32_t rxQueueSize );

/**
 * Restore FreeRTOS reception handler
 */
void libMU_Serial_RestoreRxHandlerFreeRTOS(void);

/**
 * Restore FreeRTOS reception handler
 * @param RxHandler		Reception Handler function called by ISR
 */
void libMU_Serial_SetRxHandlerFreeRTOS( libMU_Serial_Handler_t RxHandler );

#endif/*LIBMU_SERIAL_FREERTOS_H_*/
/**
 * @}
 */
