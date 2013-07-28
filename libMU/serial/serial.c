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
#include <inc/hw_types.h>			/* New data types */
#include <inc/hw_uart.h>			/* uart constants */
#include <inc/hw_memmap.h>			/* Peripheral memory map */
#include <inc/hw_ints.h>			/* interrupt constants */
#include <inc/hw_nvic.h>			/* NVIC registers */
#include <driverlib/sysctl.h>		/* System control functions */
#include <driverlib/gpio.h>			/* General Purpose IO functions */
#include <driverlib/uart.h>			/* UART functions */
#include <driverlib/interrupt.h>	/* Interrupt functions */
#include <libMU/serial.h>
#include <libMU/timer.h>
#include "BufferQueue.h"			/* Specialized library used only for the serial port */
#include "serial.port.h"

/**
 * Local function prototypes
 */
void libMU_Serial_TxHandler( void );
void libMU_Serial_HandlerFunction( void );

/**
 * Local variables
 */
uint32_t						libMU_Serial_DataLost;		/* Count of number of data bytes lost */
libMU_Serial_OperationModes_t	libMU_Serial_OperationMode;	/* Variable to signal the use of interrupts in the serial port */
libMU_Serial_Handler_t			libMU_Serial_RxHandler;		/* Handler function pointer */
libMU_FIFO_t*					libMU_Serial_RxQueue;		/* Variable to receive data from the interrupt in buffered mode */

volatile libMU_BufferQueue_t	libMU_Serial_TxQueue_LowPri;	/* Low priority tx buffer queue */
volatile libMU_BufferQueue_t	libMU_Serial_TxQueue_HighPri;	/* High priority tx buffer queue */
volatile libMU_Buffer_t			libMU_Serial_CurrentBuffer = { NULL, 0 };

int								libMU_Serial_InitializationDone = 0;

/**
 * Initialize the serial port HW
 * @param baudrate	Communication speed
 * @note			The CPU frequency must be set before calling this function
 * 					Succesive call to this function only changes the baudrate
 */
void 	libMU_Serial_Initialize(unsigned int baudrate)
{
	if( !libMU_Serial_Initialized() ) {
		/* Enable GPIO port A which is used for UART0 pins */
		SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
		SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);

		/* Select the alternate (UART) function for these pins */
		GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);

		/* Enable UART peripheral */
	    SysCtlPeripheralEnable( SYSCTL_PERIPH_UART0 );
	}
	/* Initialize the UART 0 at given baudrate */
    if( baudrate == 0 ) baudrate = 115200;	/* default baudrate */
    UARTConfigSetExpClk( UART_BASE_ADDRESS, SysCtlClockGet(), baudrate,
    	UART_CONFIG_WLEN_8 | UART_CONFIG_PAR_NONE | UART_CONFIG_STOP_ONE );
    UARTEnable( UART_BASE_ADDRESS );
    UARTFIFOEnable( UART_BASE_ADDRESS );
    UARTFIFOLevelSet( UART_BASE_ADDRESS, UART_FIFO_TX1_8, UART_FIFO_RX1_8 );

	if( !libMU_Serial_Initialized() ) {
		/* Setup initial interrupt use status */
		libMU_Serial_OperationMode = LIBMU_SERIAL_NORMAL;
		libMU_Serial_RxHandler = NULL;
		libMU_Serial_InitializationDone = 1;
	}
    libMU_Serial_DataLost = 0;
}

/**
 * Send text to serial port
 * @param text	Pointer to text
 * @return		Number of characters sent
 * @note		A '\r' character is sent before every '\n' character
 *				This function can not ensure that the data will be sent
 * 				together without interruptions, for that purpose use
 * 				libMU_SendDataLowPriority()/libMU_SendDataHighPriority() functions.
 */
int	libMU_Serial_SendString(const char* text)
{
	uint32_t	len, i;
    /* Check for valid UART base address, and valid arguments */
	if( !libMU_Serial_Initialized() ) return 0;
	if( text == NULL ) return 0;
	len = strlen(text);
    /* Send the characters */
    for(i = 0; i < len; i++)
    {
        /* If the character to the UART is \n, then add a \r before it so that */
        /* \n is translated to \r\n in the output */
        if(text[i] == '\n')
        {
            libMU_Serial_SendChar('\r');
        }
        /* Send the character to the UART output */
        libMU_Serial_SendChar( text[i] );
    }
    /* Return the number of characters written */
    return(i);
}

/**
 * Send data to the serial port
 * @param data	Pointer to data
 * @param len	Number of bytes to send
 * @return		Number of bytes sent
 * @note		This function can not ensure that the data will be sent
 * 				together without interruptions, for that purpose use
 * 				libMU_SendDataLowPriority()/libMU_SendDataHighPriority() functions.
 */
int	libMU_Serial_SendData(const char* data, int len)
{
    int i;
    /* Check for valid UART base address, and valid arguments */
	if( !libMU_Serial_Initialized() ) return 0;
    if( data == NULL ) return 0;

    /* Send the characters */
    for(i = 0; i < len; i++)
    {
        /* Send the character to the UART output */
        libMU_Serial_SendChar( data[i] );
    }
    /* Return the number of characters written */
    return(i);
}

/**
 * Get text from serial port
 * @param where	Pointer where the received text is saved
 * @param size	Maximum size of the destination pointer
 * @return		Number of characters received
 */
int	libMU_Serial_GetString(char* where, int size)
{
    unsigned long ulCount = 0;
    char cChar;
    static char bLastWasCR = 0;

    /* Check for valid UART base address, and valid arguments */
	if( !libMU_Serial_Initialized() ) return 0;
    if( where == NULL || size <= 0 ) return 0;

    /* Adjust the length back by 1 to leave space for the trailing null terminator */
    size--;

    /* Process characters until a newline is received */
    for(;;)
    {
        /* Read the next character from the console */
        libMU_Serial_GetChar( &cChar );
        /* See if the backspace key was pressed */
        if(cChar == '\b')
        {
            /* If there are any characters already in the buffer, then delete */
            /* the last */
            if(ulCount)
            {
                /* Rub out the previous character */
                libMU_Serial_SendData( "\b \b", 3 );
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
             * should be paired with the CR.  So remember that a CR was
             * received */
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
            libMU_Serial_SendChar( cChar );
        }
    }
    /* Add a null termination to the string */
    where[ulCount] = 0;
    /* Send a CRLF pair to the terminal to end the line */
    libMU_Serial_SendData( "\n", 1 );
    /* Return the count of chars in the buffer, not counting the trailing 0 */
    return(ulCount);
}

/**
 * Function to start sending queued buffers
 */
void libMU_Serial_StartSendingData(void)
{
	uint32_t status = libMU_CPU_InterruptDisable();
	volatile int i;
	i++; i++; /* Small delay to make sure UART interrupts do not interfere */
    if( libMU_Serial_IsTxFifoEmpty() ) {
		libMU_Serial_TxHandler();
    }
	if( status ) libMU_CPU_InterruptEnable();
}

/**
 * Check if the all buffers and data has been sent
 * @return		0 if not ended, 1 if ended, -1 if error in sending queue
 */
int libMU_Serial_EndedSending( void )
{
	if( libMU_BufferQueue_Count( libMU_Serial_TxQueue_LowPri ) > 0 ) return 0;
	if( libMU_BufferQueue_Count( libMU_Serial_TxQueue_HighPri ) > 0 ) return 0;
	if( libMU_Serial_CurrentBuffer.left > 0 ) {
		/* Error */
		if( libMU_FIFO_Count( libMU_Serial_CurrentBuffer.queue ) == 0 )
			return -1;
		else
			return 0;
	}
	return 1;
}

/**
 * Send a string to the serial portusing the thread safe buffer queue
 * @param queue		Queue to use for thread safe buffering
 * @param text		Pointer to text to send
 * @return			Number of characters sent
 * @note			Each thread should use a different queue otherwise it will not work.
 * 					This function will block if the queue is not big enough to hold all data or
 * 					the transmission buffer queue is full.
 * 					This data may be delayed if high priority data is present
 */
int	libMU_Serial_SendStringTS( libMU_FIFO_t* queue, const char* text )
{
	return libMU_Serial_SendDataTS( queue, text, strlen(text) );
}

/**
 * Send the buffer
 * @param queue		Queue that holds the data to be sent
 * @param len		Number of octets to send
 * @return			Number of octets sent
 */
int libMU_Serial_SendBuffer( libMU_FIFO_t* queue, uint32_t len )
{
	libMU_Buffer_t bd;
	if( queue == NULL || len == 0 ) return 0;
    /* queue buffer */
    while( libMU_BufferQueue_IsFull( libMU_Serial_TxQueue_LowPri ) ) {}
    int intStatus = libMU_CPU_InterruptDisable();
    bd.queue = queue; bd.left = len;
    libMU_BufferQueue_Put( libMU_Serial_TxQueue_LowPri, bd );
    if( intStatus ) libMU_CPU_InterruptEnable();
    return len;
}

/**
 * Send the buffer (using high priority buffer queue)
 * @param queue		Queue that holds the data to be sent
 * @param len		Number of octets to send
 * @return			Number of octets sent
 */
int libMU_Serial_SendBuffer_HP( libMU_FIFO_t* queue, uint32_t len )
{
	libMU_Buffer_t bd;
	if( queue == NULL || len == 0 ) return 0;
    /* queue buffer */
    while( libMU_BufferQueue_IsFull( libMU_Serial_TxQueue_HighPri ) ) {}
    int intStatus = libMU_CPU_InterruptDisable();
	bd.queue = queue; bd.left = len;
    libMU_BufferQueue_Put( libMU_Serial_TxQueue_HighPri, bd );
    if( intStatus ) libMU_CPU_InterruptEnable();
    return len;
}

/**
 * Send the buffer (using high priority buffer queue)
 * @param queue		Queue that holds the data to be sent
 * @param len		Number of octets to send
 * @return			Number of octets sent
 * @note			This function does not block the execution
 */
int libMU_Serial_SendBuffer_HP_FromISR( libMU_FIFO_t* queue, uint32_t len )
{
	libMU_Buffer_t bd;
    /* queue buffer */
    if( libMU_BufferQueue_IsFull( libMU_Serial_TxQueue_HighPri ) ) return 0;
	bd.queue = queue; bd.left = len;
    libMU_BufferQueue_Put( libMU_Serial_TxQueue_HighPri, bd );
    return len;
}

/**
 * Send a given number of octets to the serial portusing the thread safe buffer queue
 * @param queue		Queue to use for thread safe buffering
 * @param data		Pointer to data to send
 * @param len		Number of octets to send
 * @return			Number of octets sent
 * @note			Each thread should use a different queue otherwise it will not work.
 * 					This function will block if the queue is not big enough to hold all data or
 * 					the transmission buffer queue is full.
 * 					This data may be delayed if high priority data is present
 */
int	libMU_Serial_SendDataTS( libMU_FIFO_t* queue, const char* data, uint32_t len)
{
    uint32_t i, block_len, block_end;
     /* Check for valid UART base address, and valid arguments */
	if( !libMU_Serial_Initialized() ) return 0;
    if( queue == NULL || data == NULL || len == 0 ) return 0;
    i = 0; block_len = libMU_FIFO_Size( queue );
    /* If the queue is not big enough wait until there is space to send another block of data */
    do{
    	if( block_len > ( len - i ) ) block_len = len - i;
    	while( libMU_FIFO_SpaceLeft( queue ) < block_len ) {}			/* Wait for block_len spaces in queue */
    	block_end = i + block_len;
    	for(; i < block_end; i++ ) {
    		libMU_FIFO_PutChar( queue, data[i] );
    	}
        /* queue buffer */
        libMU_Serial_SendBuffer( queue, block_len );
    }while( i < len );
    /* Start sending data */
    libMU_Serial_StartSendingData();
    /* Return the number of characters written */
    return i;
}

/**
 * Send a given number of octets to the serial portusing the thread safe buffer queue (high priority)
 * @param queue		Queue to use for thread safe buffering
 * @param data		Pointer to data to send
 * @param len		Number of octets to send
 * @return			Number of octets sent
 * @note			Each thread should use a different queue otherwise it will not work.
 * 					This function will block if the queue is not big enough to hold all data or
 * 					the transmission buffer queue is full.
 * 					In the case that the queue can not hold the data the information will be fragmented
 * 					when sending
 * 					This data may delay other low priority data
 * 					WARNING: Do not use this function unless you know what you are doing
 */
int	libMU_Serial_SendDataTS_HP( libMU_FIFO_t* queue, const char* data, uint32_t len)
{
    uint32_t i, block_len, block_end;
     /* Check for valid UART base address, and valid arguments */
	if( !libMU_Serial_Initialized() ) return 0;
    if( queue == NULL || data == NULL || len == 0 ) return 0;
    i = 0; block_len = libMU_FIFO_Size( queue );
    /* If the queue is not big enough wait until there is space to send another block of data */
    do{
    	if( block_len > ( len - i ) ) block_len = len - i;
    	while( libMU_FIFO_SpaceLeft( queue ) < block_len ) {}			/* Wait for block_len spaces in queue */
    	block_end = i + block_len;
    	for(; i < block_end; i++ ) {
    		libMU_FIFO_PutChar( queue, data[i] );
    	}
        /* queue buffer */
        libMU_Serial_SendBuffer_HP( queue, block_len );
    }while( i < len );
    /* Start sending data */
    libMU_Serial_StartSendingData();
    /* Return the number of characters written */
    return i;
}


/**
 * Send a given number of octets to the serial portusing the thread safe buffer queue (high priority)
 * @param queue		Queue to use for thread safe buffering
 * @param data		Pointer to data to send
 * @param len		Number of octets to send
 * @return			Number of octets sent
 * @note			Each thread should use a different queue otherwise it will not work
 * 					If the queue is not big enough to hold all data or
 * 					the transmission buffer queue is full this function will not send anything.
 * 					This data may delay other low priority data
 * 					WARNING: Do not use this function unless you know what you are doing
 */
int	libMU_Serial_SendDataTS_HP_FromISR( libMU_FIFO_t* queue, const char* data, uint32_t len)
{
    libMU_Buffer_t	bd; int i;
    /* Check for valid UART initialization, and valid arguments */
	if( !libMU_Serial_Initialized() ) return 0;
    if( queue == NULL || data == NULL || len == 0 ) return 0;
    /* Check if enough space in queue */
    if( libMU_FIFO_SpaceLeft( queue ) < len ) return 0;
    if( libMU_BufferQueue_IsFull( libMU_Serial_TxQueue_HighPri ) ) return 0;

    /* Copy data to queue */
    bd.queue = queue; bd.left = len;
    for( i = 0; i < len; i++ ) {
    	libMU_FIFO_PutChar( queue, data[i] );
    }
    /* queue buffer */
    libMU_BufferQueue_Put( libMU_Serial_TxQueue_HighPri, bd );
    /* Start sending data */
	libMU_Serial_StartSendingData();
    /* Return the number of characters written */
    return i;
}

/**
 * Send character to serial port
 * @param c		Character to send
 * @return		Number of characters sent
 * @note		This function blocks the execution
 */
int	libMU_Serial_SendChar(const char c)
{
	if( libMU_Serial_OperationMode == LIBMU_SERIAL_FREERTOS ) return 0;
	while( libMU_Serial_SendChar_NB(c) == 0 );
	return 1;
}

/**
 * Send character to serial port (non-blocking)
 * @param c		Character to send
 * @return		Number of characters sent
 * @note		This function does NOT block the execution
 */
int	libMU_Serial_SendChar_NB(const char c)
{
	switch( libMU_Serial_OperationMode ) {
	case LIBMU_SERIAL_INTERRUPTS:
	case LIBMU_SERIAL_NORMAL:
		if( libMU_Serial_IsTxFifoFull() ) return 0;
		libMU_Serial_SendCharHW( c );
		return 1;
	default:
		return 0;
	}
}

/**
 * Read a character from the serial port
 * @param where	Pointer where received character to saved
 * @return		Number of characters received
 * @note		This function blocks the execution
 */
int	libMU_Serial_GetChar(char* donde)
{
	if( libMU_Serial_OperationMode == LIBMU_SERIAL_FREERTOS ) return 0;
	while( libMU_Serial_GetChar_NB(donde) == 0 );
	return 1;
}

/**
 * Read a character from the serial port (non-blocking)
 * @param where	Pointer where received character to saved
 * @return		Number of characters received
 * @note		This function does NOT block the execution
 */
int	libMU_Serial_GetChar_NB(char* where)
{
	uint32_t mask = libMU_Serial_IntDisable(UART_INT_RX);
	switch( libMU_Serial_OperationMode ) {
	case LIBMU_SERIAL_INTERRUPTS:
		if( !libMU_FIFO_IsEmpty( libMU_Serial_RxQueue ) ) {
			*where = libMU_FIFO_GetChar( libMU_Serial_RxQueue );
			libMU_Serial_IntRestore(mask);
			return 1;
		}
	case LIBMU_SERIAL_NORMAL:
		if( !libMU_Serial_IsRxFifoEmpty() ) {
			*where = libMU_Serial_GetCharHW();
			libMU_Serial_IntRestore(mask);
			return 1;
		}
	}
	libMU_Serial_IntRestore(mask);
	return 0;
}

/**
 * Set interrupt handler for the serial port and enable interrupt mode
 * @param function			Pointer to reception handler function
 */
void	libMU_Serial_SetHandler( libMU_Serial_Handler_t function )
{
	if( !libMU_Serial_Initialized() ) return;
	libMU_BufferQueue_Reset( libMU_Serial_TxQueue_HighPri );
	libMU_BufferQueue_Reset( libMU_Serial_TxQueue_LowPri );
	UARTIntRegister	( UART_BASE_ADDRESS, libMU_Serial_HandlerFunction );
	libMU_Serial_IntClear( UART_INT_RX|UART_INT_TX );
	libMU_Serial_IntEnable( UART_INT_RX|UART_INT_TX );
	libMU_Serial_OperationMode = LIBMU_SERIAL_INTERRUPTS;
	libMU_Serial_RxHandler = function;
	libMU_CPU_InterruptEnable();
}

/**
 * Set interrupt handler for the serial port and enable interrupt mode
 * @param	in_buffer		Pointer to intermediate buffer for data reception
 * @param	in_buffer_len	Size of intermediate buffer for data reception
 * @note					Each communication "thread" should provide its own output queue
 * @see						libMU_Serial_SendDataThreadSafe(), libMU_Serial_SendCharThreadSafe(),
 * 							libMU_Serial_SendCharThreadSafe_NB()
 */
void libMU_Serial_SetBufferedCommunications( uint8_t* in_buffer,
											 uint32_t in_buffer_len )
{
	if( !libMU_Serial_Initialized() ) return;
	if( in_buffer == NULL  || in_buffer_len < 1 ) return;
	libMU_Serial_RxQueue = libMU_FIFO_Initialize( in_buffer, in_buffer_len, sizeof(uint8_t) );
	if( libMU_Serial_RxQueue == NULL ) return;
	libMU_BufferQueue_Reset( libMU_Serial_TxQueue_HighPri );
	libMU_BufferQueue_Reset( libMU_Serial_TxQueue_LowPri );
	UARTIntRegister( UART_BASE_ADDRESS, libMU_Serial_HandlerFunction );
	libMU_Serial_IntClear( UART_INT_RX|UART_INT_TX );
	libMU_Serial_IntEnable( UART_INT_RX|UART_INT_TX );
	libMU_Serial_OperationMode = LIBMU_SERIAL_INTERRUPTS;
	libMU_Serial_RxHandler = NULL;
	libMU_CPU_InterruptEnable();
}

/**
 * Set interrupt handler for the serial port and enable interrupt mode
 * @param out_buffer		Pointer to intermediate buffer for data transmission
 * @param out_buffer_len	Size of intermediate buffer for data transmission
 * @param in_buffer			Pointer to intermediate buffer for data reception
 * @param in_buffer_len		Size of intermediate buffer for data reception
 */
#if 0
void	libMU_Serial_SetBufferedTransmission(
				uint8_t* out_buffer, uint32_t out_buffer_len,
				uint8_t* in_buffer,	 uint32_t in_buffer_len )
{
	if( out_buffer == NULL || out_buffer_len < 1 ||
		in_buffer == NULL  || in_buffer_len < 1 ) return;
	libMU_Serial_TxQueue = libMU_FIFO_Initialize( out_buffer, out_buffer_len, sizeof(uint8_t) );
	if( libMU_Serial_TxQueue == NULL ) return;
	libMU_Serial_RxQueue = libMU_FIFO_Initialize( in_buffer, in_buffer_len, sizeof(uint8_t) );
	if( libMU_Serial_RxQueue == NULL ) return;
	UARTIntRegister	( UART_BASE_ADDRESS, libMU_Serial_HandlerFunction );
	libMU_Serial_IntClear( UART_INT_RX|UART_INT_TX );
	libMU_Serial_IntEnable( UART_INT_RX|UART_INT_TX );
	libMU_Serial_OperationMode = LIBMU_SERIAL_INTERRUPTS;
	libMU_Serial_RxHandler = NULL;
	libMU_Interrupt_Enable();
}
#endif

/**
 * Transmission handler for the serial port.
 * @note	This function is also called to start sending information
 * 			This function should be called with interrupts disabled
 * @see		libMU_Serial_StartSendingData()
 */
void	libMU_Serial_TxHandler( void )
{
	uint8_t c;
	do{
		if( libMU_Serial_CurrentBuffer.left == 0 ) {
			if( libMU_BufferQueue_Count( libMU_Serial_TxQueue_HighPri ) > 0 ) {
				/* Get new buffer from high priority buffer queue */
				libMU_BufferQueue_Get( libMU_Serial_TxQueue_HighPri, libMU_Serial_CurrentBuffer );
			}else if( libMU_BufferQueue_Count( libMU_Serial_TxQueue_LowPri ) > 0 ) {
				/* Get new buffer from low priority buffer queue */
				libMU_BufferQueue_Get( libMU_Serial_TxQueue_LowPri, libMU_Serial_CurrentBuffer );
			}else{
				/* No new buffer: return */
				return;
			}
		}
		c = libMU_FIFO_GetChar( libMU_Serial_CurrentBuffer.queue ); /* Get new char from buffer */
		libMU_Serial_CurrentBuffer.left--;
		libMU_Serial_SendCharHW( c );
	}while( !libMU_Serial_IsTxFifoFull() );
}

/**
 * Interrupt service routine handler for serial port
 */
/*interrupt*/
void	libMU_Serial_HandlerFunction( void )
{
	uint32_t	Status = libMU_Serial_IntStatus();
	if( Status & LIBMU_SERIAL_DATA_RECEIVED ) {
		libMU_Serial_IntClear( UART_INT_RX );
		if( libMU_Serial_RxHandler != NULL ) {
			if( !libMU_Serial_IsRxFifoEmpty() )
				libMU_Serial_RxHandler();
		}else{
			while( !libMU_Serial_IsRxFifoEmpty() ) {
				char c = libMU_Serial_GetCharHW();
				if(!libMU_FIFO_IsFull ( libMU_Serial_RxQueue ) )
					libMU_FIFO_PutChar( libMU_Serial_RxQueue, c );
				else
					libMU_Serial_DataLost++;
			}
		}
	}
	if( Status & LIBMU_SERIAL_DATA_NEEDED ) {
		libMU_Serial_IntClear( UART_INT_TX );
		libMU_Serial_TxHandler();
	}
}

/**
 * @}
 */
