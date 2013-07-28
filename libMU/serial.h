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
#ifndef LIBMU_SERIAL_H_
#define LIBMU_SERIAL_H_

#include <stdint.h>
#include <libMU/FIFO.h>

/**
 * Serial HW operation modes
 */
typedef enum _libMU_Serial_OperationModes_t {
	LIBMU_SERIAL_NORMAL,
	LIBMU_SERIAL_INTERRUPTS,
	LIBMU_SERIAL_FREERTOS
} libMU_Serial_OperationModes_t;

/**
 * Function pointer for interrupt handler functions
 */
typedef	void (*libMU_Serial_Handler_t)(void);

/**
 * Initialize the serial port HW
 * @param baudrate	Communication speed in bits per second
 * @note			The CPU frequency must be set before calling this function
 * 					Succesive call to this function only changes the baudrate
 */
void 	libMU_Serial_Initialize(unsigned int baudrate);

/**
 * Send text to serial port
 * @param text	Pointer to text
 * @return		Number of characters sent
 * @note		A '\r' character is sent before every '\n' character
 *				This function can not ensure that the data will be sent
 * 				together without interruptions, for that purpose use
 * 				libMU_SendDataLowPriority()/libMU_SendDataHighPriority() functions.
 */
int	libMU_Serial_SendString(const char* text);

/**
 * Send data to the serial port
 * @param data	Pointer to data
 * @param len	Number of bytes to send
 * @return		Number of bytes sent
 * @note		This function can not ensure that the data will be sent
 * 				together without interruptions, for that purpose use
 * 				libMU_SendDataLowPriority()/libMU_SendDataHighPriority() functions.
 */
int	libMU_Serial_SendData(const char* data, int len);

/**
 * Get text from serial port
 * @param where	Pointer to array of characters where the received text is stored
 * @param size	Maximum size of the destination array
 * @return		Number of characters received
 */
int	libMU_Serial_GetString(char* where, int size);

/**
 * Send character to serial port
 * @param c		Character to send
 * @return		Number of characters sent
 * @note		This function blocks the execution until it sends the character
 *				This function may disable interrupts for a moment
 */
int	libMU_Serial_SendChar(const char c);

/**
 * Send character to serial port (non-blocking)
 * @param c		Character to send
 * @return		Number of characters sent
 * @note		This function does NOT block the execution
 *				This function may disable interrupts for a moment
 */
int	libMU_Serial_SendChar_NB(const char c);

/**
 * Read a character from the serial port
 * @param where	Pointer where received character is saved
 * @return		Number of characters received
 * @note		This function blocks the execution until it receives a character
 */
int	libMU_Serial_GetChar(char* where);

/**
 * Read a character from the serial port (non-blocking)
 * @param where	Pointer where received character is saved
 * @return		Number of characters received
 * @note		This function does NOT block the execution
 */
int	libMU_Serial_GetChar_NB(char* where);

/**
 * Read a character from the serial port (non-blocking)
 * @param where	Pointer where received character to saved
 * @return		Number of characters received
 * @note		This function does NOT block the execution
 */
int	libMU_Serial_GetCharFromISR_NB(char* where);

/**
 * Set interrupt handler for the serial port and enable interrupt mode
 * @param	in_buffer		Pointer to intermediate buffer for data reception
 * @param	in_buffer_len	Size of intermediate buffer for data reception
 * @note					Each communication "thread" should provide its own output queue
 * @see						libMU_Serial_SendDataThreadSafe(), libMU_Serial_SendCharThreadSafe(),
 * 							libMU_Serial_SendCharThreadSafe_NB()
 */
void libMU_Serial_SetBufferedCommunications( uint8_t* in_buffer,
											 uint32_t in_buffer_len );

/**
 * Set interrupt data reception handler for the serial port and enable interrupt mode
 * @param	function		Pointer to reception handler function
 * @note					This function is for internal use, never call this directly unless you know
 * 							what you are doing
 * 							Each communication "thread" should provide its own output queue
 * @see						libMU_Serial_SendDataThreadSafe(), libMU_Serial_SendCharThreadSafe(),
 * 							libMU_Serial_SendCharThreadSafe_NB()
 */
void	libMU_Serial_SetHandler( libMU_Serial_Handler_t function );

/**
 * Send a string to the serial line using the thread safe buffer queue
 * @param queue		Queue to use for thread safe buffering
 * @param text		Pointer to text to send
 * @return			Number of characters sent
 * @note			Each thread should use a different queue otherwise it will not work.
 * 					This function will block if the queue is not big enough to hold all data or
 * 					the transmission buffer queue is full.
 * 					This data may be delayed if high priority data is present
 */
int	libMU_Serial_SendStringTS( libMU_FIFO_t* queue, const char* text );

/**
 * Send a given number of octets to the serial line using the low priority buffer queue
 * @param queue		Queue to use for thread safe buffering
 * @param data		Pointer to data to send
 * @param len		Number of octets to send
 * @return			Number of octets sent
 * @note			Each thread should use a different queue otherwise it will not work.
 * 					This function will block if the queue is not big enough to hold all data or
 * 					the transmission buffer queue is full.
 * 					This data may be delayed if high priority data is present
 */
int	libMU_Serial_SendDataTS( libMU_FIFO_t* queue, const char* data, uint32_t len);

/**
 * Function to start sending queued buffers
 */
void libMU_Serial_StartSendingData(void);

/**
 * Check if the all buffers and data has been sent
 * @return			0 if not ended, 1 if ended, -1 if error in sending queue
 */
int libMU_Serial_EndedSending( void );

/**
 * Send the buffer
 * @param queue		Queue that holds the data to be sent
 * @param len		Number of octets to send
 * @return			Number of octets sent
 */
int libMU_Serial_SendBuffer( libMU_FIFO_t* queue, uint32_t len );

/**
 * Send the buffer (using high priority buffer queue)
 * @param queue		Queue that holds the data to be sent
 * @param len		Number of octets to send
 * @return			Number of octets sent
 */
int libMU_Serial_SendBuffer_HP( libMU_FIFO_t* queue, uint32_t len );

/**
 * Send the buffer (using high priority buffer queue)
 * @param queue		Queue that holds the data to be sent
 * @param len		Number of octets to send
 * @return			Number of octets sent
 */
int libMU_Serial_SendBuffer_HP_FromISR( libMU_FIFO_t* queue, uint32_t len );

/**
 * Return the operation mode of the serial port HW
 */
static inline libMU_Serial_OperationModes_t libMU_Serial_GetOperationMode(void)
{
	extern libMU_Serial_OperationModes_t	libMU_Serial_OperationMode;
	return libMU_Serial_OperationMode;
}

/**
 * Change the operation mode of the serial port HW
 */
static inline void libMU_Serial_SetOperationMode(libMU_Serial_OperationModes_t mode)
{
	extern libMU_Serial_OperationModes_t	libMU_Serial_OperationMode;
	libMU_Serial_OperationMode = mode;
}

#endif /*LIBMU_SERIAL_H_*/
/**
 * @}
 */

