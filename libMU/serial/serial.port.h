/**
 * @addtogroup libMU_Serial
 * Library for easy access to the serial port
 * @{
 ********************************************************************
 * @author		Eñaut Muxika <emuxika at mondragon dot edu>
 * @date		2011/7/10
 * @copyright	BSDL
 ********************************************************************
 * @note		This file contains the low level inline functions
 * 				that need to be ported between different architectures
 */
#include <inc/hw_types.h>			/* New data types */
#include <inc/hw_uart.h>			/* uart constants */
#include <inc/hw_memmap.h>			/* Peripheral memory map */
#include <libMU/cpu.h>

/**
 * Base address of UART device to use
 */
#define	UART_BASE_ADDRESS			UART0_BASE
#define LIBMU_SERIAL_DATA_RECEIVED	UART_INT_RX
#define LIBMU_SERIAL_DATA_NEEDED	UART_INT_TX

/**
 * Check if the serial peripheral is intialized
 */
static __inline int	libMU_Serial_Initialized(void)
{
	extern int 	libMU_Serial_InitializationDone;
	return libMU_Serial_InitializationDone;
}

/**
 * Checks if the transmission FIFO queue is full
 * @return 		true if queue full
 */
static __inline int	libMU_Serial_IsTxFifoFull(void)
{
	/* Check if TX FIFO is full */
	return  ( HWREG( UART_BASE_ADDRESS + UART_O_FR ) & UART_FR_TXFF );
}

/**
 * Checks if the transmission FIFO queue is empty
 * @return 		true if queue empty
 */
static __inline int	libMU_Serial_IsTxFifoEmpty(void)
{
	/* Check if TX FIFO is full */
	return  ( HWREG( UART_BASE_ADDRESS + UART_O_FR ) & UART_FR_TXFE );
}

/**
 * Checks if the reception FIFO queue is full
 * @return 		true if queue full
 */
static __inline int	libMU_Serial_IsRxFifoFull(void)
{
	/* Check if RX FIFO is empty */
	return  ( HWREG( UART_BASE_ADDRESS + UART_O_FR ) & UART_FR_RXFF );
}

/**
 * Checks if the reception FIFO queue is empty
 * @return 		true if queue empty
 */
static __inline int	libMU_Serial_IsRxFifoEmpty(void)
{
	/* Check if RX FIFO is empty */
	return  ( HWREG( UART_BASE_ADDRESS + UART_O_FR ) & UART_FR_RXFE );
}

/**
 * Sends a character to the UART HW (low level access)
 * @param c		Character to send
 */
static __inline void libMU_Serial_SendCharHW(const char c)
{
	HWREG( UART_BASE_ADDRESS + UART_O_DR ) = c;
}

/**
 * Reads a character from the UART HW (low level access)
 * @return		Read character
 */
static __inline uint8_t	libMU_Serial_GetCharHW(void)
{
	return HWREG( UART_BASE_ADDRESS + UART_O_DR );
}

/**
 * Disables all the serial port interrupts
 */
static __inline uint32_t libMU_Serial_IntDisableAll(void)
{
	uint32_t mask = HWREG(UART_BASE_ADDRESS + UART_O_IM);
	HWREG(UART_BASE_ADDRESS + UART_O_IM) = 0;
	return mask;
}
/**
 * Disables the serial port interrupts
 */
static __inline uint32_t libMU_Serial_IntDisable(uint32_t flags)
{
	uint32_t mask = HWREG(UART_BASE_ADDRESS + UART_O_IM);
	HWREG(UART_BASE_ADDRESS + UART_O_IM) &= ~flags;
	return mask;
}
/**
 * Enables the serial port interrupts
 */
static __inline void libMU_Serial_IntEnable(uint32_t flags)
{
	HWREG(UART_BASE_ADDRESS + UART_O_IM) |= flags;
}
/**
 * Restores the serial port interrupts
 */
static __inline void libMU_Serial_IntRestore(uint32_t mask)
{
	HWREG(UART_BASE_ADDRESS + UART_O_IM) = mask;
}
/**
 * Obtains the serial port interrupt status
 */
static __inline uint32_t libMU_Serial_IntStatus(void)
{
	return HWREG(UART_BASE_ADDRESS + UART_O_RIS);
}
/**
 * Clears the serial port interrupt flag
 */
static __inline void libMU_Serial_IntClear(uint32_t flag)
{
	HWREG(UART_BASE_ADDRESS + UART_O_ICR) = flag;
}

/**
 * @}
 */
