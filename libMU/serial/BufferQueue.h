/**
 * @addtogroup	libMU_BufferQueue
 * Specialized library to manage buffer queues
 * @{
 ********************************************************************
 * @author		Eñaut Muxika <emuxika at mondragon dot edu>
 * @date		2012/6/05
 * @copyright	BSDL
 ********************************************************************
 */
#ifndef LIBMU_BUFFERQUEUE_H_
#define LIBMU_BUFFERQUEUE_H_

#include <stdint.h>
#include <string.h>
#include <libMU/FIFO.h>

#define	BUFFER_QUEUE_BITS		5							/* 2^5 = 32 elements in buffer queue */
#define	BUFFER_QUEUE_SIZE		(1<<BUFFER_QUEUE_BITS)
#define BUFFER_QUEUE_MASK		(BUFFER_QUEUE_SIZE-1)

typedef struct _libMU_BufferQueue_t {
	uint8_t			count;						/* Number of elements in buffer queue */
	uint8_t			next_write;					/* Index to next free space */
	uint8_t			next_read;					/* Index to next element to read */
	libMU_Buffer_t	buffer[BUFFER_QUEUE_SIZE];	/* buffer queue storage */
} libMU_BufferQueue_t;

/**
 * @return	The number of elements in buffer queue
 */
#define	libMU_BufferQueue_Count( queue )		(queue).count

/**
 * @return	If buffer queue is empty
 */
#define	libMU_BufferQueue_IsEmpty( queue )		((queue).count == 0)

/**
 * @return	If buffer queue is full
 */
#define	libMU_BufferQueue_IsFull( queue )		((queue).count == BUFFER_QUEUE_SIZE)

/**
 * @return	Size of buffer queue (in number of elements)
 */
#define	libMU_BufferQueue_Size( queue )			BUFFER_QUEUE_SIZE

/**
 * @return	Number of remaining space left (in number of elements)
 */
#define	libMU_BufferQueue_SpaceLeft( queue )	(BUFFER_QUEUE_SIZE - (queue).count)

/**
 * Put a data element into buffer queue
 * @param	queue	Pointer to buffer queue
 * @param	data	Pointer to data to put into buffer queue
 */
#define	libMU_BufferQueue_Put( queue, data )	{											\
					(queue).buffer[ (queue).next_write ] = (data);							\
					(queue).count++;														\
					(queue).next_write = (((queue).next_write + 1) & BUFFER_QUEUE_MASK);	\
				}

/**
 * Get a data element from buffer queue
 * @param	queue	Pointer to buffer queue
 * @param	data	Pointer to where to put data from buffer queue
 */
#define	libMU_BufferQueue_Get( queue, data )	{											\
					(data) = (queue).buffer[ (queue).next_read ];							\
					(queue).count--;														\
					(queue).next_read = (((queue).next_read + 1) & BUFFER_QUEUE_MASK);		\
				}

/**
 * Remove all elements from buffer queue
 * @param	queue	Pointer to buffer queue
 */
#define	libMU_BufferQueue_Reset( queue )	{ (queue).count = (queue).next_read = (queue).next_write = 0; }

#if 0
/**
 * Peek a data element from buffer queue
 * @param	queue	Pointer to buffer queue
 */
#define	libMU_BufferQueue_Peek( queue )			(queue).buffer[ (queue).next_read ]

/**
 * Remove a data element from buffer queue
 * @param	queue	Pointer to buffer queue
 */
#define	libMU_BufferQueue_Remove( queue )	{												\
					(queue).count--;														\
					(queue).next_read = (((queue).next_read + 1) & BUFFER_QUEUE_MASK);		\
				}

#endif

#endif /*LIBMU_BUFFERQUEUE_H_*/
/**
 * @}
 */
