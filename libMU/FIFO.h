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
#ifndef LIBMU_FIFO_H_
#define LIBMU_FIFO_H_

#include <stdint.h>
#include <string.h>

typedef struct _libMU_FIFO_t {
	uint32_t	size;			/* Maximum number of elements in FIFO queue */
	uint32_t	unit_size;		/* Size of one element in FIFO queue */
	uint32_t	next_write;		/* Index to next free space */
	uint32_t	next_read;		/* Index to next element to read */
	uint32_t	count;			/* Number of elements in FIFO queue */
	uint8_t		buffer[0];		/* FIFO queue data pointer */
} libMU_FIFO_t;

/**
 * Buffer element pointing to data queue and number of elements
 */
typedef struct _libMU_Buffer_t {
	libMU_FIFO_t*	queue;		/* Pointer to the data FIFO queue */
	uint32_t		left;		/* Remaining data in buffer */
} libMU_Buffer_t;

/**
 * Initialize FIFO queue
 * @param	buffer		Pointer to FIFO queue memory buffer
 * @param	size		Size of the FIFO queue memory buffer
 * @param	unit_size	Size of each element stored in the memory buffer
 * @return				NULL if buffer size is not big enough, address of the FIFO otherwise
 * @note				The FIFO management data is also stored in the buffer.
 * 						Therefore, the buffer has to be big enough to hold this data
 */
libMU_FIFO_t*	libMU_FIFO_Initialize( uint8_t* buffer, uint32_t size, uint32_t unit_size );

/**
 * @param fifo	FIFO queue pointer
 * @return		The number of elements in FIFO queue
 */
static __inline uint32_t	libMU_FIFO_Count( const libMU_FIFO_t* queue );
static __inline uint32_t	libMU_FIFO_Count( const libMU_FIFO_t* queue )
{
	return queue->count;
}

/**
 * @param fifo	FIFO queue pointer
 * @return		true if FIFO queue is empty
 */
static __inline uint32_t	libMU_FIFO_IsEmpty( const libMU_FIFO_t* queue );
static __inline uint32_t	libMU_FIFO_IsEmpty( const libMU_FIFO_t* queue )
{
	return (queue->count == 0);
}

/**
 * @param fifo	FIFO queue pointer
 * @return		true if FIFO queue is full
 */
static __inline uint32_t	libMU_FIFO_IsFull( const libMU_FIFO_t* queue );
static __inline uint32_t	libMU_FIFO_IsFull( const libMU_FIFO_t* queue )
{
	return ( (queue->count * queue->unit_size ) == queue->size );
}

/**
 * @return	Size of FIFO queue (in number of elements)
 */
static __inline uint32_t	libMU_FIFO_Size( const libMU_FIFO_t* queue );
static __inline uint32_t	libMU_FIFO_Size( const libMU_FIFO_t* queue )
{
	return ( queue->size / queue->unit_size );
}

/**
 * @return	Number of remaining space left (in number of elements)
 */
static __inline uint32_t	libMU_FIFO_SpaceLeft( const libMU_FIFO_t* queue );
static __inline uint32_t	libMU_FIFO_SpaceLeft( const libMU_FIFO_t* queue )
{
	return ( queue->size / queue->unit_size - queue-> count );
}

/**
 * Put character in FIFO
 * @param fifo	FIFO queue pointer
 * @param c		Charater to put in FIFO queue
 * @note		There is no check to see if the FIFO queue is full
 */
static __inline void	libMU_FIFO_PutChar( libMU_FIFO_t* queue, const uint8_t c );
static __inline void	libMU_FIFO_PutChar( libMU_FIFO_t* queue, const uint8_t c )
{
	queue->buffer[ queue->next_write ] = c;
	queue->count++;
	queue->next_write++;
	if( queue->next_write >= queue->size ) queue->next_write = 0u;
}

/**
 * Read character from FIFO queue
 * @param fifo	FIFO queue pointer
 * @return		Value read from FIFO queue
 * @note		There is no check to see if the FIFO queue is empty
 */
static __inline uint8_t	libMU_FIFO_GetChar( libMU_FIFO_t* queue );
static __inline uint8_t	libMU_FIFO_GetChar( libMU_FIFO_t* queue )
{
	uint8_t c = queue->buffer[ queue->next_read ];
	queue->count--;
	queue->next_read++;
	if( queue->next_read >= queue->size ) queue->next_read = 0u;
	return c;
}

/**
 * Put a data element into FIFO queue
 * @param	queue	Pointer to FIFO queue
 * @param	data	Pointer to data to put into FIFO queue
 */
static __inline void	libMU_FIFO_PutData( libMU_FIFO_t* queue, const void* data );
static __inline void	libMU_FIFO_PutData( libMU_FIFO_t* queue, const void* data )
{
	memcpy( &queue->buffer[ queue->next_write ], data, queue->unit_size );
	queue->count++;
	queue->next_write += queue->unit_size;
	if( queue->next_write >= queue->size ) queue->next_write = 0u;
}

/**
 * Get a data element from FIFO queue
 * @param	queue	Pointer to FIFO queue
 * @param	data	Pointer to where to put data from FIFO queue
 */
static __inline void libMU_FIFO_GetData( libMU_FIFO_t* queue, void* data );
static __inline void libMU_FIFO_GetData( libMU_FIFO_t* queue, void* data )
{
	memcpy( data, &queue->buffer[ queue->next_read ], queue->unit_size );
	queue->count--;
	queue->next_read += queue->unit_size;
	if( queue->next_read >= queue->size ) queue->next_read = 0u;
}

/**
 * Remove all element from FIFO queue
 * @param	queue	Pointer to FIFO queue
 */
static __inline void libMU_FIFO_Reset( libMU_FIFO_t* queue );
static __inline void libMU_FIFO_Reset( libMU_FIFO_t* queue )
{
	queue->count = queue->next_read = queue->next_write = 0u;
}

#if 0
/**
 * Peek a data element from FIFO queue
 * @param	queue	Pointer to FIFO queue
 */
static __inline void libMU_FIFO_PeekData( const libMU_FIFO_t* queue, void* data );
static __inline void libMU_FIFO_PeekData( const libMU_FIFO_t* queue, void* data )
{
	memcpy( data, &queue->buffer[ queue->next_read ], queue->unit_size );
}

/**
 * Get a data element pointer from FIFO queue
 * @param	queue	Pointer to FIFO queue
 * @return			Pointer to FIFO data
 */
static __inline void*	libMU_FIFO_GetDataPtr( const libMU_FIFO_t* queue );
static __inline void*	libMU_FIFO_GetDataPtr( const libMU_FIFO_t* queue )
{
	return (void*)(&queue->buffer[ queue->next_read ]);
}

/**
 * Remove a data element from FIFO queue
 * @param	queue	Pointer to FIFO queue
 */
static __inline void libMU_FIFO_Remove( libMU_FIFO_t* queue );
static __inline void libMU_FIFO_Remove( libMU_FIFO_t* queue )
{
	queue->count--;
	queue->next_read += queue->unit_size;
	if( queue->next_read >= queue->size ) queue->next_read = 0u;
}
#endif

#endif /*LIBMU_FIFO_H_*/
/**
 * @}
 */
