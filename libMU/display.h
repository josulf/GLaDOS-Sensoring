/**
 * @addtogroup	libMU_Display
 * Library for graphical and text manipulation of OLED display
 * @{
 ********************************************************************
 * @author		Eñaut Muxika <emuxika at mondragon dot edu>
 * @date		2011/7/10
 * @copyright	BSDL
 ********************************************************************
 */
#ifndef LIBMU_DISPLAY_H_
#define LIBMU_DISPLAY_H_

#include <stdint.h>
#include <string.h>
#include <drivers/rit128x96x4.h>/* OLED functions */

/**
 * Constants for the OLED display
 */
enum libMU_Display_Constants_t {
	libMU_Display_RES_X			= 128,	/* Horizontal resolution of the screen */
	libMU_Display_RES_Y			= 96,	/* Vertical resolution of the screen */
	libMU_Display_CHAR_WIDTH	= 6,	/* Horizontal size of a character in pixels */
	libMU_Display_CHAR_HEIGHT	= 8,	/* Vertical size of a character in pixels */
	libMU_Display_BPP			= 4		/* Bits per pixel of the screen */
};

/**
 * Data type for memory based images
 */
typedef struct _libMU_Display_MemoryImage_t {
	uint8_t*	buffer;			/* Pointer to memory where the image is stored */
	uint16_t	width;			/* Width of the stored image (in bytes, not in pixels) */
	uint16_t	height;			/* Height of the stored image (in pixels) */
	uint32_t	size;			/* Size of the image (in bytes) */
	uint8_t*	end;			/* Pointer to memory end */
} libMU_Display_MemoryImage_t;

/**
 * Data type for memory based copy of the display to accelerate operations
 * 128 pixels width x 96 pixels height x (4 bits/pixel / 8 bits/byte ) = 6144 bytes
 */
typedef uint8_t	libMU_Display_MemoryBuffer_t
								[libMU_Display_RES_X*libMU_Display_RES_Y*libMU_Display_BPP/8];

/**
 * Dedicated Memory copy of the display screen to accelerate operations
 */
extern libMU_Display_MemoryBuffer_t	libMU_Display_ImageBuffer;
extern libMU_Display_MemoryImage_t	libMU_Display_LocalImage;

/**
 * Initialize the display HW
 */
void libMU_Display_Initialize(void);

/**
 * Clear the display
 */
static __inline void libMU_Display_Clear(void);
static __inline void libMU_Display_Clear(void)
{
	RIT128x96x4Clear();
}

/**
 * Draw a UTF8 text in the display (limited to codepoints 0-0xFF)
 * @param text		Text to show
 * @param pos_X		Horizontal position of text in pixels (lower than 128 and even)
 * @param pos_Y		Vertical position of text in pixels (lower than 96)
 * @param intensity	Color intensity (grayscale from 0 to 15)
 * @note			UTF8 support is limited to ASCII plus 4 other codepoints at the moment
 */
void libMU_Display_DrawStringUTF8(const char *text, int pos_X, int pos_Y, int intensity);

/**
 * Draw a text in the display
 * @param text		Text to show
 * @param pos_X		Horizontal position of text in pixels (lower than 128 and even)
 * @param pos_Y		Vertical position of text in pixels (lower than 96)
 * @param intensity	Color intensity (grayscale from 0 to 15)
 */
static __inline void libMU_Display_DrawString(const char *text, int pos_X, int pos_Y, int intensity);
static __inline void libMU_Display_DrawString(const char *text, int pos_X, int pos_Y, int intensity)
{
	/* RIT128x96x4StringDraw( text, pos_X, pos_Y, intensity ); */
	libMU_Display_DrawStringUTF8( text, pos_X, pos_Y, intensity );
}

/**
 * Clear a memory image
 * @param memImage	Pointer to memory image data structure
 */
static __inline void libMU_Display_MemoryImage_Clear( libMU_Display_MemoryImage_t* memImage );
static __inline void libMU_Display_MemoryImage_Clear( libMU_Display_MemoryImage_t* memImage )
{
	memset( memImage->buffer, 0, memImage->size );
}

/**
 * Draw a memory image on display
 * @param memImage	Pointer to memory image data structure
 * @param pos_X		Horizontal position of upper-left coordinate of image
 * @param pos_Y		Vertical position of upper-left coordinate of image
 * @note			pos_X must be even (display limitation)
 */
static __inline void libMU_Display_MemoryImage_Show(
							const libMU_Display_MemoryImage_t* memImage,
							int pos_X, int pos_Y );
static __inline void libMU_Display_MemoryImage_Show(
							const libMU_Display_MemoryImage_t* memImage,
							int pos_X, int pos_Y )
{
	RIT128x96x4ImageDraw( memImage->buffer, pos_X, pos_Y, (memImage->width << 1), memImage->height );
}

/**
 * Initialize a memory image data structure
 * @param memImage	Pointer to memory image data structure
 * @param buffer	Pointer to memory buffer where the image will be saved
 * @param width		Horizontal size of memory image
 * @param height	Vertical size of memory image
 * @note			The horizontal size must be even and the memory buffer must
 * 					be big enough to store the image (buffer overflow risk!!)
 */
void	libMU_Display_MemoryImage_Initialize( libMU_Display_MemoryImage_t* memImage,
					uint8_t* buffer, int width, int height );

/**
 * Draw a pixel in a memory image
 * @param memImage	Pointer to memory image data structure
 * @param lX		Horizontal position of the pixel
 * @param lY		Vertical position of the pixel
 * @param intensity	Color intensity (grayscale from 0 to 15)
 */
void libMU_Display_MemoryImage_DrawPixel( libMU_Display_MemoryImage_t* memImage, int lX, int lY, int intensity );

/**
 * Draw a line using the Bresenham algorithm in a memory image
 * @param memImage	Pointer to memory image data structure
 * @param lX1		Horizontal position of the start point of the line
 * @param lY1		Vertical position of the start point of the line
 * @param lX2		Horizontal position of the end point of the line
 * @param lY2		Vertical position of the end point of the line
 * @param intensity	Color intensity (grayscale from 0 to 15)
 */
void	libMU_Display_MemoryImage_DrawLine( libMU_Display_MemoryImage_t* memImage,
						int width, int height, int lX2, int lY2, int intensity );

/**
 * Draw an ellipse using the Bresenham algorithm in a memory image
 * @param memImage	Pointer to memory image data structure
 * @param lX1		Horizontal position of the upper-left corner of the ellipse
 * @param lY1		Vertical position of the upper-left corner of the ellipse
 * @param lX2		Horizontal position of the bottom-right corner of the ellipse
 * @param lY2		Vertical position of the bottom-right corner of the ellipse
 * @param intensity	Color intensity (grayscale from 0 to 15)
 */
void	libMU_Display_MemoryImage_DrawEllipse( libMU_Display_MemoryImage_t* memImage,
						int width, int height, int lX2, int lY2, int intensity );

/**
 * Initialize dedicated local memory image data structure
 * @param width		Horizontal size of memory image
 * @param height	Vertical size of memory image
 * @note			The horizontal size must be even
 */
static __inline void	libMU_Display_MemoryImage_InitializeLocal(int width, int height);
static __inline void	libMU_Display_MemoryImage_InitializeLocal(int width, int height)
{
	libMU_Display_MemoryImage_Initialize( &libMU_Display_LocalImage,
			libMU_Display_ImageBuffer, width, height );
}

/**
 * Clear dedicated local memory image
 */
static __inline void	libMU_Display_MemoryImage_ClearLocal(void);
static __inline void	libMU_Display_MemoryImage_ClearLocal(void)
{
	libMU_Display_MemoryImage_Clear( &libMU_Display_LocalImage );
}

/**
 * Draw dedicated local memory image on display
 * @param pos_X		Horizontal position of upper-left coordinate of image
 * @param pos_Y		Vertical position of upper-left coordinate of image
 * @note			pos_X must be even (display limitation)
 */
static __inline void	libMU_Display_MemoryImage_ShowLocal( int pos_X, int pos_Y );
static __inline void	libMU_Display_MemoryImage_ShowLocal( int pos_X, int pos_Y )
{
	RIT128x96x4ImageDraw( libMU_Display_ImageBuffer, pos_X, pos_Y,
			(libMU_Display_LocalImage.width<<1), libMU_Display_LocalImage.height );
}

/**
 * Draw a line using the Bresenham algorithm in the dedicated local memory image
 * @param lX1		Horizontal position of the start point of the line
 * @param lY1		Vertical position of the start point of the line
 * @param lX2		Horizontal position of the end point of the line
 * @param lY2		Vertical position of the end point of the line
 * @param intensity	Color intensity (grayscale from 0 to 15)
 */
static __inline void	libMU_Display_MemoryImage_DrawLineLocal(
						int lX1, int lY1, int lX2, int lY2, int intensity);
static __inline void	libMU_Display_MemoryImage_DrawLineLocal(
						int lX1, int lY1, int lX2, int lY2, int intensity)
{
	libMU_Display_MemoryImage_DrawLine( &libMU_Display_LocalImage, lX1, lY1, lX2, lY2, intensity );
}

/**
 * printf() function substitution to print text on OLED display
 * @param	format	printf() format specification
 * @param	...		Variable length argument
 * @return			Number of characters written into buf
 * @note			It has the same limitations that libMU_snprintf()
 * @see libMU_snprintf
 */
int libMU_Display_printf(const char* format, ...);

/**
 * Set line vertical position for libMU_Display_printf() function
 * @param	line	Vertical position in pixels
 */
void libMU_Display_SetLine( int line );

/**
 * Set position for libMU_Display_printf() function
 * @param	row		Vertical position in pixels
 * @param	col		Horizontal postion in pixels
 */
void libMU_Display_SetPosition( int row, int col );

/**
 * Set vertical viewport for libMU_Display_printf() function
 * @param	rowMin	Minimum vertical position in pixels
 * @param	rowMax	Maximum vertical position in pixels
 * @return			true if correctly set
 * @note
 * The specified positions must have space at least for one line of
 * height (libMU_Display_CHAR_HEIGHT - 1)
 */
int libMU_Display_SetMinMaxRow( int rowMin, int rowMax );

/**
 * Set horizontal viewport for libMU_Display_printf() function
 * @param	colMin	Minimum vertical position in pixels
 * @param	colMax	Maximum vertical position in pixels
 * @return			true if correctly set
 * @note
 * The specified positions must have space at least for one line of
 * height (libMU_Display_CHAR_WIDTH - 1)
 */
int libMU_Display_SetMinMaxCol( int colMin, int colMax );

/**
 * Set default viewport for libMU_Display_printf() function
 */
void libMU_Display_ResetViewport(void);

/**
 * Set color intensity for libMU_Display_printf() function
 * @param	color	Color intensity
 */
void libMU_Display_SetColor( int color );

/**
 * Introduce a number using the buttons and the OLED display
 * @param pNumber	Pointer where the number is stored
 * @param posX		Horizontal coordinate of position where the value is introduced
 * @param posY		Vertical coordinate of position where the value is introduced
 * @param intensity	Color intensity
 */
int libMU_Display_IntroduceUint32(uint32_t* pNumber, int posX, int posY, int intensity);

/**
 * Introduce an IP address using the buttons and the OLED display
 * @param pNumber	Pointer where the IP address is stored
 * @param posX		Horizontal coordinate of position where the value is introduced
 * @param posY		Vertical coordinate of position where the value is introduced
 * @param intensity	Color intensity
 */
int libMU_Display_IntroduceIPv4(uint32_t* pNumber, int posX, int posY, int intensity);

#endif /*LIBMU_DISPLAY_H_*/
/**
 * @}
 */
