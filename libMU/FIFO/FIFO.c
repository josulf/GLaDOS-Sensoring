/**
 * @addtogroup	libMU_FIFO
 * Library to create and use FIFO queues
 * @{
 ********************************************************************
 * @author		Eñaut Muxika <emuxika at mondragon dot edu>
 * @date		2012/6/05
 * @copyright	BSDL
 ********************************************************************
 */

#include <libMU/FIFO.h>

/**
 * Initialize FIFO queue
 * @param	buffer		Pointer to FIFO queue memory buffer
 * @param	size		Size of the FIFO queue memory buffer
 * @param	unit_size	Size of each element stored in the memory buffer
 * @return				NULL if buffer size is not big enough
 * @note				The FIFO management data is also stored in the buffer.
 * 						Therefore, the buffer has to be big enough to hold this data
 * 						plus another 2 elements at least
 */
libMU_FIFO_t*	libMU_FIFO_Initialize( uint8_t* buffer, uint32_t size, uint32_t unit_size )
{
	libMU_FIFO_t* fifo; uint32_t unused;
	if( buffer == NULL ) 				return NULL;
	if( unit_size < 1 )					return NULL;
	if( size < sizeof(libMU_FIFO_t) )	return NULL;
	size -= sizeof(libMU_FIFO_t);
	unused= (size % unit_size);			/* Calculate wasted space */
	size -= unused;						/* Remove unused elements */
	/* Make sure we have at least space for 2 elements */
	if( size < (unit_size<<1) )			return NULL;
	fifo = (libMU_FIFO_t*)buffer;
	fifo->size			= size;
	fifo->next_write 	= 0;
	fifo->next_read		= 0;
	fifo->count			= 0;
	fifo->unit_size		= unit_size;
	return fifo;
}

/**
 * @}
 */
