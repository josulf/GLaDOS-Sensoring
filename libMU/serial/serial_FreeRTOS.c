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
#include <string.h>
#include <libMU/serial_FreeRTOS.h>
#include <libMU/FIFO.h>
#include "serial.port.h"

/**
 * Local functions
 */
void	libMU_Serial_RxHandlerFreeRTOS( void );

extern libMU_Serial_OperationModes_t 	libMU_Serial_OperationMode;
extern uint32_t							libMU_Serial_DataLost;
extern libMU_FIFO_t*					libMU_Serial_RxQueue;

xQueueHandle				libMU_Serial_RxQueueFreeRTOS = NULL;

/**
 * Read a character from the serial line
 * @param queue		Queue to use for thread safe buffering
 * @param where		Pointer to array of characters where the string is stored
 * @param size		Maximum number of characters to read
 * @return			Number of characters read
 */
int	libMU_Serial_GetStringFreeRTOS( libMU_FIFO_t* queue, char* where, int size)
{
    unsigned long ulCount = 0;
    char cChar;
    static char bLastWasCR = 0;

    /* Check for valid UART base address, and valid arguments */
	if( !libMU_Serial_Initialized() ) return 0;
    if( queue == NULL || where == NULL || size <= 0 ) return 0;

    /* Adjust the length back by 1 to leave space for the trailing null terminator */
    size--;

    /* Process characters until a newline is received */
    for(;;)
    {
        /* Read the next character from the console */
        libMU_Serial_GetCharFreeRTOS( &cChar, portMAX_DELAY );
        /* See if the backspace key was pressed */
        if(cChar == '\b')
        {
            /* If there are any characters already in the buffer, then delete */
            /* the last */
            if(ulCount)
            {
                /* Rub out the previous character */
                libMU_Serial_SendDataTS( queue, "\b \b", 3 );
                /* Decrement the number of characters in the buffer */
                ulCount--;
            }
            /* Skip ahead to read the next character */
            continue;
        }
        /* If this character is LF and last was CR, then just gobble up the
         * character because the EOL processing was taken care of with the CR */
        if((cChar == '\n') && bLastWasCR)
        {
            bLastWasCR = 0;
            continue;
        }
        /* See if a newline or escape character was received */
        if((cChar == '\r') || (cChar == '\n') || (cChar == 0x1b))
        {
            /* If the character is a CR, then it may be followed by a LF which
             * should be paired with the CR.  So remember that a CR was received */
            if(cChar == '\r')
            {
                bLastWasCR = 1;
            }
            /* Stop processing the input and end the line */
            break;
        }
        /* Process the received character as long as we are not at the end of
         * the buffer.  If the end of the buffer has been reached then all
         * additional characters are ignored until a newline is received */
        if( ulCount < size )
        {
            /* Store the character in the caller supplied buffer */
        	where[ulCount] = cChar;
            /* Increment the count of characters received */
            ulCount++;
            /* Reflect the character back to the user */
            libMU_Serial_SendDataTS( queue, &cChar, 1 );
        }
    }
    /* Add a null termination to the string */
    where[ulCount] = 0;
    /* Send a CRLF pair to the terminal to end the line */
    libMU_Serial_SendDataTS( queue, "\n", 1 );
    /* Return the count of chars in the buffer, not counting the trailing 0 */
    return(ulCount);
}

/**
 * Reads characters from the serial line until given character is received, a timeout occurs or
 * the buffer is full
 * @param	where		Pointer to array of characters where the string is stored
 * @param	size		Maximum number of characters to read
 * @param	end_char	Ending character
 * @param	timeout		Timeout value (in FreeRTOS units)
 * @return				Number of characters read, negative sign if timeout reached
 * @note				The last character in buffer is always a 0
 */
int	libMU_Serial_GetUntilChar_FreeRTOS(char* where, int size, char end_char, uint32_t timeout )
{
	unsigned long ulCount = 0; int timeleft;
	char cChar;
	portTickType	endtime;

	/* Check for valid UART base address, and valid arguments */
	if( !libMU_Serial_Initialized() ) return 0;
	if( where == NULL || size <= 0 ) return 0;

	/* Adjust the length back by 1 to leave space for the trailing null terminator */
	memset( where, 0, size ); size--;

	/* Start timeout */
	endtime = xTaskGetTickCount() + timeout;

	/* Process characters until an ending character is received, the buffer is full or a timeout happens */
	for(;;)
	{
		timeleft = endtime - xTaskGetTickCount();
		if( libMU_Serial_GetCharFreeRTOS( &cChar, timeleft ) > 0 ) {
			where[ulCount] = cChar;	ulCount++;
			if( cChar == end_char || ulCount >= size ) break;
		}else return (-ulCount);
	}
	return(ulCount);
}

/**
 * Reads a character from the serial line
 * @param where		Pointer where the received character is stored
 * @param timeout	Maximum time to wait for character (in FreeRTOS ticks)
 * @return			Number of characters read
 * @note			This function blocks the execution until it receives a character
 */
int	libMU_Serial_GetCharFreeRTOS(char* where, uint32_t timeout )
{
	//if( where == NULL ) return 0;	/* Sanity check */
	if( libMU_Serial_OperationMode == LIBMU_SERIAL_FREERTOS ) {
		if( xQueueReceive( libMU_Serial_RxQueueFreeRTOS, where, timeout ) != pdTRUE ) return 0;
		return 1;
	}
	while( libMU_Serial_GetChar_NB(where) == 0 );
	return 1;
}

/**
 * Reads a character from the serial line without blocking the execution
 * @param where		Pointer where the received character is stored
 * @return			Number of characters read
 * @note			This function does NOT block the execution
 */
int	libMU_Serial_GetCharFreeRTOS_NB(char* where)
{
	//if( where == NULL ) return 0;	/* Sanity check */
	if( libMU_Serial_OperationMode == LIBMU_SERIAL_FREERTOS ) {
		if( xQueueReceive( libMU_Serial_RxQueueFreeRTOS, where, 0 ) != pdTRUE )	return 0;
		return 1;
	}
	return libMU_Serial_GetChar_NB(where);
}

/**
 * Set interrupt handler for the serial port and enable interrupt mode
 * @param	rxQueueSize		Size of the reception queue to allocate in FreeRTOS
 * @return					true if correctly setup
 */
#include <driverlib/uart.h>
int	libMU_Serial_SetupFreeRTOSHandler( uint32_t rxQueueSize )
{
	if( !libMU_Serial_Initialized() ) return 0;
	if( rxQueueSize < 128 ) rxQueueSize = 128;
	libMU_Serial_RxQueueFreeRTOS = xQueueCreate( rxQueueSize, sizeof(char) );
	if( libMU_Serial_RxQueueFreeRTOS == NULL ) return 0;
	libMU_Serial_SetHandler( libMU_Serial_RxHandlerFreeRTOS );
	if( libMU_Serial_GetOperationMode() != LIBMU_SERIAL_INTERRUPTS ) return 0;
	libMU_Serial_SetOperationMode( LIBMU_SERIAL_FREERTOS );
	return 1;
}

/**
 * Restore FreeRTOS reception handler
 * @param RxHandler		Reception Handler function called by ISR
 */
void libMU_Serial_SetRxHandlerFreeRTOS( libMU_Serial_Handler_t RxHandler )
{
	extern libMU_Serial_Handler_t libMU_Serial_RxHandler;
	if( libMU_Serial_GetOperationMode() != LIBMU_SERIAL_FREERTOS ) return;
	libMU_Serial_RxHandler = RxHandler;
}

/**
 * Restore FreeRTOS reception handler
 */
void libMU_Serial_RestoreRxHandlerFreeRTOS(void)
{
	extern libMU_Serial_Handler_t libMU_Serial_RxHandler;
	if( libMU_Serial_GetOperationMode() != LIBMU_SERIAL_FREERTOS ) return;
	libMU_Serial_RxHandler = libMU_Serial_RxHandlerFreeRTOS;
}

/**
 * Interrupt service routine handler for serial port
 */
void	libMU_Serial_RxHandlerFreeRTOS( void )
{
	portBASE_TYPE woken; char c;
	while( !libMU_Serial_IsRxFifoEmpty() ) {
		c = libMU_Serial_GetCharHW();
		if( xQueueSendFromISR( libMU_Serial_RxQueueFreeRTOS, &c, &woken ) == pdFALSE ) {
			libMU_Serial_DataLost++;
		}
	}
}

/**
 * @}
 */
