/**
 * @addtogroup	libMU_TS27010
 * Librería para el protocol de multiplexacion 3GPP TS27.010 v10.0.0
 * crea puertos virtuales COM usando una linea serie
 * @{
 ********************************************************************
 * @author		Eñaut Muxika <emuxika at mondragon dot edu>
 * @date		2012/6/05
 * @copyright	BSDL
 ********************************************************************
 */
#ifndef TS27010_H_
#define TS27010_H_

#include <stdint.h>
#include <FreeRTOS.h>
#include <queue.h>

/**
 * Initialize the TS 27.010 protocol variables/functions/task
 * @param	baudrate		Connection speed of the multiplexed serial communication in (bits/s)
 * @param	rxQueueSize		Size of the memory used for the global input buffer (allocated by FreeRTOS)
 * @return					true if initialization is correct
 * @note					The FreeRTOS scheduler has to be running for this function to work
 */
int		libMU_TS27010_Initialize( uint32_t baudrate, uint32_t rxQueueSize );

/**
 * Establish a Data Link Channel (Virtual COM port) with the other multiplexer device
 * @param queue			Queue to use for thread safe buffering
 * @param dlci			Data Link Channel Identifier (Virtual COM port)
 * @param rxQueueSize	Size of the memory used for the channel input buffer (allocated by FreeRTOS)
 * @return				true if channel is correctly established
 */
int		libMU_TS27010_Connect( libMU_FIFO_t* queue, uint8_t dlci, int rxQueueSize );

/**
 * Check if a Data Link Channel (Virtual COM port) is established with the other multiplexer device
 * @param dlci			Data Link Channel Identifier (Virtual COM port)
 * @return				true if channel is correctly established
 */
uint8_t		libMU_TS27010_Connected( uint8_t dlci );

/**
 * Send data to the specified data link channel
 * @param queue		Queue to use for thread safe buffering
 * @param dlci		Data Link Channel Identifier (Virtual COM port)
 * @param data		Pointer to data to send
 * @param len		Number of octets to send
 * @return			Number of octets sent
 * @note			Each thread should use a different queue otherwise it will not work.
 * 					This function will block if the queue is not big enough to hold all data or
 * 					the transmission buffer queue is full.
 */
int		libMU_TS27010_Send( libMU_FIFO_t* queue, int dlci, const uint8_t* data, uint32_t len );

/**
 * Function to read data from the channel until a newline character is received or
 * the data storage is full
 * @param	dlci	Data Link Channel Identifier (Virtual COM port)
 * @param	data	Pointer where the received data is stored
 * @param	size	Maximum size of the storage area
 * @return			number of characters read
 * @note			This function blocks the execution if there is not enough data
 */
int		libMU_TS27010_GetLine( int dlci, uint8_t* data, uint32_t size );

/**
 * Function to read exactly the given number of data from the channel
 * @param	dlci	Data Link Channel Identifier (Virtual COM port)
 * @param	data	Pointer where the received data is stored
 * @param	len		Number of characters to read
 * @param	timeout	Timeout to finish receiving data (in FreeRTOS units)
 * @return			number of characters read
 * @note			This function blocks the execution if there is not enough data
 */
int		libMU_TS27010_GetData( int dlci, uint8_t* data, uint32_t size, uint32_t timeout );

/**
 * Obtain the reception queue for the given Data Link Channel (Virtual COM port)
 * @param dlci			Data Link Channel Identifier (Virtual COM port)
 * @return				Reception queue handle for the virtual port
 */
xQueueHandle	libMU_TS27010_GetQueue( uint8_t dlci );

#endif /*TS27010_H_*/
/**
 * @}
 */
