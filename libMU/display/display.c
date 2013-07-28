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
#include <inc/hw_types.h>		/* New data types */
#include <driverlib/sysctl.h>	/* System control functions */
#include <drivers/rit128x96x4.h>/* OLED functions */
#include <libMU/display.h>
#include <libMU/cpu.h>
#include <stdarg.h>

#define	LIBMU_ABS(x)				(((x)<0)?-(x):(x))
#define	LIBMU_DISPLAY_CHECK_BEGIN	0x2468ACE0
#define	LIBMU_DISPLAY_CHECK_END		0xFDB97531

/**
 * Data type to save a pixel position in a memory image
 */
typedef	struct _libMU_Display_MemoryImagePos_t {
	uint8_t*						pos;
	int								x;
	libMU_Display_MemoryImage_t*	mi;
} libMU_Display_MemoryImagePos_t;

/**
 * Dedicated Memory copy of the display screen to accelerate operations
 */
uint32_t						libMU_Display_ImageBuffer_begin	= LIBMU_DISPLAY_CHECK_BEGIN;
libMU_Display_MemoryBuffer_t	libMU_Display_ImageBuffer;
uint32_t						libMU_Display_ImageBuffer_end	= LIBMU_DISPLAY_CHECK_END;
libMU_Display_MemoryImage_t		libMU_Display_LocalImage;

/**
 * Initialize the display HW
 */
void 	libMU_Display_Initialize(void)
{
	/* Set processor to 50MHz */
	libMU_CPU_ClockSet(50000000);
    /* Initialize the display controller */
    RIT128x96x4Init	( 1000000 );
    /* Initialize the dedicated local memory image */
    libMU_Display_MemoryImage_InitializeLocal( libMU_Display_RES_X, libMU_Display_RES_Y );
}

/**
 * Initialize a memory image data structure
 * @param mi		Pointer to memory image data structure
 * @param buffer	Pointer to memory buffer where the image will be saved
 * @param width		Horizontal size of memory image
 * @param height	Vertical size of memory image
 * @note			The horizontal size must be even and the memory buffer must
 * 					be big enough to store the image (buffer overflow risk!!)
 */
void	libMU_Display_MemoryImage_Initialize( libMU_Display_MemoryImage_t* mi,
					uint8_t* buffer, int width, int height )
{
	if( width < 1 || width > libMU_Display_RES_X || height < 1 || height > libMU_Display_RES_Y ||
			(width & 1) ) return;
	mi->buffer = buffer;
	mi->width  = (width>>1);
	mi->height = height;
	mi->size   = (width>>1) * height;
	if( buffer == NULL )	mi->end	= mi->buffer;
	else					mi->end	= mi->buffer + mi->size;
}

/**
 * Draw a pixel in a memory image
 * @param mi		Pointer to memory image data structure
 * @param lX		Horizontal position of the pixel
 * @param lY		Vertical position of the pixel
 * @param intensity	Color intensity (grayscale from 0 to 15)
 */
void libMU_Display_MemoryImage_DrawPixel( libMU_Display_MemoryImage_t* mi, int lX, int lY, int intensity )
{
/*	if( x < 0 || y < 0 || (x>>1) >= mi->width || y >= mi->height ) return; */
    if(lX & 1)
    {
        mi->buffer[(lY * mi->width) + (lX >> 1)] =
            ((mi->buffer[(lY * mi->width) + (lX >> 1)] & 0xf0) |
             (intensity & 0xf));
    }
    else
    {
    	mi->buffer[(lY * mi->width) + (lX >> 1)] =
            ((mi->buffer[(lY * mi->width) + (lX >> 1)] & 0x0f) |
             ((intensity & 0xf) << 4));
    }
}

/**
 * Get position of a pixel in a memory image
 * @param mi		Pointer to memory image data structure
 * @param x			Horizontal position of the pixel
 * @param y			Vertical position of the pixel
 * @return			Memory image pixel position structure
 */
static __inline libMU_Display_MemoryImagePos_t libMU_Display_MemoryImagePosition_Get(
							libMU_Display_MemoryImage_t* mi, int x, int y )
{
	libMU_Display_MemoryImagePos_t mip;
	mip.pos = mi->buffer + (y * mi->width) + (x >> 1);
	mip.x = x; mip.mi = mi;
	return mip;
}
/**
 * Draw a pixel in a memory image using a previously calculated pixel position structure
 * @param mip		Memory image pixel position structure
 * @param intensity	Color intensity (grayscale from 0 to 15)
 */
static __inline void libMU_Display_MemoryImagePosition_DrawPixel(
							libMU_Display_MemoryImagePos_t mip, int intensity )
{
	if( mip.pos < mip.mi->buffer || mip.pos >= mip.mi->end ) return;
/*	if( x < 0 || (x>>1) >= mip.mi->width ) return; */
	if( mip.x & 1 ) {
		mip.pos[0] = (mip.pos[0] & ~0x0F) | ( intensity & 0x0F );
	}else{
		mip.pos[0] = (mip.pos[0] & ~0xF0) | ( intensity << 4 );
	}
}
/**
 * Update memory image pixel position by moving 1 pixel to the left
 * @param mip		Memory image pixel position structure
 * @return			Memory image pixel position structure (updated)
 */
static __inline libMU_Display_MemoryImagePos_t libMU_Display_MemoryImagePosition_MoveLeft(
							libMU_Display_MemoryImagePos_t mip )
{
	if( !(mip.x & 1) ) mip.pos--; mip.x--;
	return mip;
}
/**
 * Update memory image pixel position by moving 1 pixel to the right
 * @param mip		Memory image pixel position structure
 * @return			Memory image pixel position structure (updated)
 */
static __inline libMU_Display_MemoryImagePos_t libMU_Display_MemoryImagePosition_MoveRight(
							libMU_Display_MemoryImagePos_t mip )
{
	if( mip.x & 1 ) mip.pos++; mip.x++;
	return mip;
}
/**
 * Update memory image pixel position by moving 1 pixel upwards
 * @param mip		Memory image pixel position structure
 * @return			Memory image pixel position structure (updated)
 */
static __inline libMU_Display_MemoryImagePos_t libMU_Display_MemoryImagePosition_MoveUp(
							libMU_Display_MemoryImagePos_t mip )
{
	mip.pos -= mip.mi->width;
	return mip;
}
/**
 * Update memory image pixel position by moving 1 pixel downwards
 * @param mip		Memory image pixel position structure
 * @return			Memory image pixel position structure (updated)
 */
static __inline libMU_Display_MemoryImagePos_t libMU_Display_MemoryImagePosition_MoveDown(
							libMU_Display_MemoryImagePos_t mip );
static __inline libMU_Display_MemoryImagePos_t libMU_Display_MemoryImagePosition_MoveDown(
							libMU_Display_MemoryImagePos_t mip )
{
	mip.pos += mip.mi->width;
	return mip;
}

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
						int lX1, int lY1, int lX2, int lY2, int intensity )
{
    long lError, lDeltaX, lDeltaY, lYStep;
    libMU_Display_MemoryImagePos_t	mi;
    tBoolean bSteep;

    lDeltaY = lY2 - lY1;
    if( lDeltaY == 0 ) {	/* Horizontal line */
    	int intens = intensity & 0xF; intens |= (intens<<4);
    	if( lX1 > lX2 ) { lError = lX1; lX1 = lX2; lX2 = lError; }
    	mi = libMU_Display_MemoryImagePosition_Get( memImage, lX1, lY1 );

    	if( lX1 & 1 ) {
    		libMU_Display_MemoryImagePosition_DrawPixel( mi, intensity );
    		mi = libMU_Display_MemoryImagePosition_MoveRight( mi );
    		lX1++;
    	}
    	for(; lX1 < lX2; lX1+=2 ) {
    		mi.pos[0] = intens; mi.pos++;
    	}
    	if( !(lX2 & 1) ) {
    		libMU_Display_MemoryImagePosition_DrawPixel( mi, intensity );
    	}
    	return;
    }
    lDeltaX = lX2 - lX1;
    if( lDeltaX == 0 ) {	/* Vertical line */
    	if( lY1 > lY2 ) { lError = lY1; lY1 = lY2; lY2 = lError; }
    	mi = libMU_Display_MemoryImagePosition_Get( memImage, lX1, lY1 );
    	for(; lY1 <= lY2; lY1++ ) {
    		libMU_Display_MemoryImagePosition_DrawPixel( mi, intensity );
    		mi = libMU_Display_MemoryImagePosition_MoveDown( mi );
    	}
    	return;
    }
	if( lX1 > lX2 ) {
		lError = lX1; lX1 = lX2; lX2 = lError;
		lError = lY1; lY1 = lY2; lY2 = lError;
		lDeltaX = -lDeltaX; lDeltaY = -lDeltaY;
	}

    /* Determine if the line is steep.
     * A steep line has more motion in the Y direction than the X direction. */
    if( LIBMU_ABS(lDeltaY) > lDeltaX )	bSteep = true;
    else								bSteep = false;
    /* If the line is steep, then swap the X and Y coordinates */
    if(bSteep)
    {
        lError = lX1;
        lX1 = lY1;
        lY1 = lError;
        lError = lX2;
        lX2 = lY2;
        lY2 = lError;
    }
    /* If the starting X coordinate is larger than the ending X coordinate,
     * then swap the start and end coordinates  */
    if(lX1 > lX2)
    {
        lError = lX1;
        lX1 = lX2;
        lX2 = lError;
        lError = lY1;
        lY1 = lY2;
        lY2 = lError;
    }
    /* Compute the difference between the start and end coordinates in each axis */
    lDeltaX = lX2 - lX1;
    lDeltaY = (lY2 > lY1) ? (lY2 - lY1) : (lY1 - lY2);

    /* Initialize the error term to negative half the X delta */
    lError = -lDeltaX / 2;

    /* Determine the direction to step in the Y axis when required */
    if(lY1 < lY2)	lYStep = 1;
    else			lYStep = -1;

    /* Loop through all the points along the X axis of the line */
    for(; lX1 <= lX2; lX1++)
    {
        /* See if this is a steep line */
        if(bSteep)
        {
            /* Plot this point of the line, swapping the X and Y coordinates */
            libMU_Display_MemoryImage_DrawPixel( memImage, lY1, lX1, intensity );
        }
        else
        {
            /* Plot this point of the line, using the coordinates as is */
            libMU_Display_MemoryImage_DrawPixel( memImage, lX1, lY1, intensity );
        }
        /* Increment the error term by the Y delta */
        lError += lDeltaY;

        /* See if the error term is now greater than zero */
        if(lError > 0)
        {
            /* Take a step in the Y axis */
            lY1 += lYStep;

            /* Decrement the error term by the X delta */
            lError -= lDeltaX;
        }
    }
}

/**
 * Draw an ellipse using the Bresenham algorithm in a memory image
 * @param memImage	Pointer to memory image data structure
 * @param lX1		Horizontal position of the upper-left corner of the ellipse
 * @param lY1		Vertical position of the upper-left corner of the ellipse
 * @param lX2		Horizontal position of the bottom-right corner of the ellipse
 * @param lY2		Vertical position of the bottom-right corner of the ellipse
 * @param intensity	Color intensity (grayscale from 0 to 15)
 * @see				http://homepage.smc.edu/kennedy_john/belipse.pdf
 */
void	libMU_Display_MemoryImage_DrawEllipse( libMU_Display_MemoryImage_t* mi,
						int lX1, int lY1, int lX2, int lY2, int intensity )
{
    long xchange, ychange, error, two_a, two_b, two_a2, two_b2, stopx, stopy;
    libMU_Display_MemoryImagePos_t	p_lu, p_lb, p_ru, p_rb;
	if( lX1 > lX2 ) { error = lX1; lX1 = lX2; lX2 = error; }
	if( lY1 > lY2 ) { error = lY1; lY1 = lY2; lY2 = error; }
    two_a = (lX2 - lX1 + 1); two_a2 = (two_a*two_a)>>1;
    two_b = (lY2 - lY1 + 1); two_b2 = (two_b*two_b)>>1;
    if( two_b2 == 0 || two_a2 == 0 ) {
    	libMU_Display_MemoryImage_DrawLine( mi, lX1, lY1, lX2, lY2, intensity );
    	return;
    }

    /* plot angles (180,225), (-180,-225), (0,45) and (0,-45) */
    p_lu = libMU_Display_MemoryImagePosition_Get( mi, lX1, (lY1 + lY2) >> 1 );
    p_ru = libMU_Display_MemoryImagePosition_Get( mi, lX2, (lY1 + lY2) >> 1 );
    p_lb = libMU_Display_MemoryImagePosition_Get( mi, lX1, (lY1 + lY2 + 1) >> 1 );
    p_rb = libMU_Display_MemoryImagePosition_Get( mi, lX2, (lY1 + lY2 + 1) >> 1 );
    xchange = (two_b2>>1) * (1 - two_a);
    ychange = (two_a2>>1);
    error = 0;
    stopx = (two_b2*two_a)>>1;
    stopy = 0;
    while( stopx >= stopy ) {
    	/* Draw points */
    	libMU_Display_MemoryImagePosition_DrawPixel	( p_lu, intensity );
    	libMU_Display_MemoryImagePosition_DrawPixel	( p_ru, intensity );
    	libMU_Display_MemoryImagePosition_DrawPixel	( p_lb, intensity );
    	libMU_Display_MemoryImagePosition_DrawPixel	( p_rb, intensity );
    	/* update positions */
    	p_lu = libMU_Display_MemoryImagePosition_MoveUp		( p_lu );
    	p_ru = libMU_Display_MemoryImagePosition_MoveUp		( p_ru );
    	p_lb = libMU_Display_MemoryImagePosition_MoveDown	( p_lb );
    	p_rb = libMU_Display_MemoryImagePosition_MoveDown	( p_rb );
    	/* update error */
    	stopy += two_a2;
    	error += ychange;
    	ychange += two_a2;
    	if( (2*error + xchange) > 0 ) {
    		/* update positions */
        	p_lu = libMU_Display_MemoryImagePosition_MoveRight	( p_lu );
        	p_lb = libMU_Display_MemoryImagePosition_MoveRight	( p_lb );
        	p_ru = libMU_Display_MemoryImagePosition_MoveLeft	( p_ru );
        	p_rb = libMU_Display_MemoryImagePosition_MoveLeft	( p_rb );
    		/* update error */
    		stopx -= two_b2;
    		error += xchange;
    		xchange += two_b2;
    	}
    }

    /* plot angles (90,135), (90,45), (-90,-135) and (-90,-45) */
    p_lu = libMU_Display_MemoryImagePosition_Get( mi, (lX1 + lX2) >> 1, lY1 );
    p_lb = libMU_Display_MemoryImagePosition_Get( mi, (lX1 + lX2) >> 1, lY2 );
    p_ru = libMU_Display_MemoryImagePosition_Get( mi, (lX1 + lX2 +1) >> 1, lY1 );
    p_rb = libMU_Display_MemoryImagePosition_Get( mi, (lX1 + lX2 +1) >> 1, lY2 );
    xchange = (two_b2>>1);
    ychange = (two_a2>>1) * (1 - two_b);
    error = 0;
    stopx = 0;
    stopy = (two_a2*two_b)>>1;
    while( stopx <= stopy ) {
    	/* Draw points */
    	libMU_Display_MemoryImagePosition_DrawPixel	( p_lu, intensity );
    	libMU_Display_MemoryImagePosition_DrawPixel	( p_ru, intensity );
    	libMU_Display_MemoryImagePosition_DrawPixel	( p_lb, intensity );
    	libMU_Display_MemoryImagePosition_DrawPixel	( p_rb, intensity );
    	/* update positions */
    	p_lu = libMU_Display_MemoryImagePosition_MoveLeft	( p_lu );
    	p_lb = libMU_Display_MemoryImagePosition_MoveLeft	( p_lb );
    	p_ru = libMU_Display_MemoryImagePosition_MoveRight	( p_ru );
    	p_rb = libMU_Display_MemoryImagePosition_MoveRight	( p_rb );
    	/* update error */
    	stopx += two_b2;
    	error += xchange;
    	xchange += two_b2;
    	if( (2*error + ychange) > 0 ) {
    		/* update positions */
        	p_lu = libMU_Display_MemoryImagePosition_MoveDown	( p_lu );
        	p_ru = libMU_Display_MemoryImagePosition_MoveDown	( p_ru );
        	p_lb = libMU_Display_MemoryImagePosition_MoveUp		( p_lb );
        	p_rb = libMU_Display_MemoryImagePosition_MoveUp		( p_rb );
    		/* update error */
    		stopy -= two_a2;
    		error += ychange;
    		ychange += two_a2;
    	}
    }
}

/**
 * Rellena una figura en la imagen local de la pantalla
 * @param mi		Puntero a la estructura de datos de la imagen en memoria
 * @param x			Coordenada horizontal inicial
 * @param y			Coordenada vertical inicial
 * @param intensity	Intensidad del color (de 0 a 15)
 */
/**
 * Fill an enclosed area in a memory image
 * @param mi		Pointer to memory image data structure
 * @param x			Horizontal position of the starting point of the area to fill
 * @param y			Vertical position of the starting point of the area to fill
 * @param intensity	Color intensity (grayscale from 0 to 15)
 */
void	libMU_Display_MemoryImage_Fill( libMU_Display_MemoryImage_t* mi, int x, int y, unsigned int intensity )
{
	/* Search boundaries */
}

static uint16_t	libMU_Display_Row = 0;
static uint16_t	libMU_Display_Col = 0;
static uint16_t	libMU_Display_RowMin = 0;
static uint16_t	libMU_Display_RowMax = libMU_Display_RES_Y - libMU_Display_CHAR_HEIGHT + 1;
static uint16_t	libMU_Display_ColMin = 0;
static uint16_t	libMU_Display_ColMax = libMU_Display_RES_X - libMU_Display_CHAR_WIDTH + 1;
static uint8_t	libMU_Display_printfColor = 15;
#include <libMU/stdlib.h>

/**
 * Set vertical viewport for libMU_Display_printf() function
 * @param	rowMin	Minimum vertical position in pixels
 * @param	rowMax	Maximum vertical position in pixels
 * @return			true if correctly set
 * @note
 * The specified positions must have space at least for one line of
 * height (libMU_Display_CHAR_HEIGHT - 1)
 */
int libMU_Display_SetMinMaxRow( int rowMin, int rowMax )
{
	if( rowMin < 0 ) rowMin = 0;
	else if( rowMin > (libMU_Display_RES_Y - 1) ) rowMin = libMU_Display_RES_Y - 1;
	if( rowMax < 0 ) rowMax = 0;
	else if( rowMax > (libMU_Display_RES_Y - 1) ) rowMax = libMU_Display_RES_Y - 1;
	if( (rowMax - rowMin + 1) >= (libMU_Display_CHAR_HEIGHT - 1) ) return 0;
	libMU_Display_RowMin = rowMin;
	libMU_Display_RowMax = rowMax;
	return 1;
}

/**
 * Set horizontal viewport for libMU_Display_printf() function
 * @param	colMin	Minimum vertical position in pixels
 * @param	colMax	Maximum vertical position in pixels
 * @return			true if correctly set
 * @note
 * The specified positions must have space at least for one line of
 * height (libMU_Display_CHAR_WIDTH - 1)
 */
int libMU_Display_SetMinMaxCol( int colMin, int colMax )
{
	if( colMin < 0 ) colMin = 0;
	else if( colMin > (libMU_Display_RES_X - 1) ) colMin = libMU_Display_RES_X - 1;
	if( colMax < 0 ) colMax = 0;
	else if( colMax > (libMU_Display_RES_X - 1) ) colMax = libMU_Display_RES_X - 1;
	if( (colMax - colMin + 1) >= (libMU_Display_CHAR_WIDTH - 1) ) return 0;
	libMU_Display_ColMin = colMin;
	libMU_Display_ColMax = colMax;
	return 1;
}

/**
 * Set default viewport for libMU_Display_printf() function
 */
void libMU_Display_ResetViewport(void)
{
	libMU_Display_RowMin = 0;
	libMU_Display_RowMax = libMU_Display_RES_Y - 1;
	libMU_Display_ColMin = 0;
	libMU_Display_ColMax = libMU_Display_RES_X - 1;
}

/**
 * Set line vertical position for libMU_Display_printf() function
 * @param	line	Vertical position in pixels
 */
void libMU_Display_SetLine( int row )
{
	if( row < libMU_Display_RowMin ) row = libMU_Display_RowMin;
	else if( row > libMU_Display_RowMax ) row = libMU_Display_RowMax;
	libMU_Display_Row = (uint16_t)row;
}

/**
 * Set position for libMU_Display_printf() function
 * @param	row		Vertical position in pixels
 * @param	col		Horizontal postion in pixels
 */
void libMU_Display_SetPosition( int row, int col )
{
	if( row < libMU_Display_RowMin ) row = libMU_Display_RowMin;
	else if( row > libMU_Display_RowMax ) row = libMU_Display_RowMax;
	if( col < libMU_Display_ColMin ) col = libMU_Display_ColMin;
	else if( col > libMU_Display_ColMax ) col = libMU_Display_ColMax;
	libMU_Display_Row = row;
	libMU_Display_Col = col;
}

/**
 * Set color intensity for libMU_Display_printf() function
 * @param	color	Color intensity
 */
void libMU_Display_SetColor( int color )
{
	libMU_Display_printfColor = (uint8_t)color;
}

/**
 * printf() function substitution to print text on OLED display
 * @param	format	printf() format specification
 * @param	...		Variable length argument
 * @return			Number of characters written into buf
 * @note			It has the same limitations that libMU_snprintf()
 * @see libMU_snprintf
 */
int libMU_Display_printf(const char* format, ...)
{
	static int libMU_Display_Initialized = 0;
	char msg[libMU_Display_RES_X/libMU_Display_CHAR_WIDTH * 2];
	char msgformat[sizeof(msg)];
	extern va_list libMU_vsnprintf_vls_out;
	va_list vls; char saveChar; int totalchar = 0;
	int nchar, i, col, charsInDisplay, leftMostChar;
	if( format == NULL ) return 0;
	if( !libMU_Display_Initialized ) {
		libMU_Display_Initialize();
		libMU_Display_Initialized = 1;
	}
	if( libMU_Display_Row < libMU_Display_RowMin )
		libMU_Display_Row = libMU_Display_RowMin;
	if( libMU_Display_Col < libMU_Display_ColMin )
		libMU_Display_Col = libMU_Display_ColMin;
	va_start(vls,format);
	while( *format ) {
		/* Obtain format for a single line */
		i = 0;
		while( *format != 0 && *format != '\n' && *format != '\r' ) {
			if( i < sizeof(msgformat) - 1 ) {
				msgformat[i] = *format; i++;
			}else break;
			format++;
		}
		msgformat[i] = 0;
		/* Display line */
		nchar = libMU_vsnprintf( msg, sizeof(msg), msgformat, vls );
		vls   = libMU_vsnprintf_vls_out;
		totalchar += nchar;
		leftMostChar = 0;
		while( nchar > 0 ) {
			col = libMU_Display_Col + nchar * libMU_Display_CHAR_WIDTH;
			if( col > libMU_Display_ColMax ) col = libMU_Display_ColMax;
			charsInDisplay = (col - libMU_Display_Col)/libMU_Display_CHAR_WIDTH;
			saveChar = msg[leftMostChar + charsInDisplay];
			msg[leftMostChar + charsInDisplay] = 0;
			libMU_Display_DrawString( &msg[leftMostChar], libMU_Display_Col,
					libMU_Display_Row, libMU_Display_printfColor );
			msg[leftMostChar + charsInDisplay] = saveChar;
			nchar -= charsInDisplay; leftMostChar += charsInDisplay;
			if( nchar > 0 ) {
				libMU_Display_Col = libMU_Display_ColMin;
				libMU_Display_Row += libMU_Display_CHAR_HEIGHT;
				if( libMU_Display_Row > libMU_Display_RowMax )
					libMU_Display_Row = libMU_Display_RowMin;
			}else{
				libMU_Display_Col += charsInDisplay * libMU_Display_CHAR_WIDTH;
			}
		}
		if( *format == '\n' ) {
			libMU_Display_Col = libMU_Display_ColMin;
			libMU_Display_Row += libMU_Display_CHAR_HEIGHT;
			if( libMU_Display_Row > libMU_Display_RowMax )
				libMU_Display_Row = libMU_Display_RowMin;
			format++;
		}else if( *format == '\r' ) {
			libMU_Display_Col = libMU_Display_ColMin;
			format++;
		}
	}
	va_end(vls);
	return nchar;
}

#define USE_BUTTON_AND_OLED_FOR_INPUT
#if defined(USE_BUTTON_AND_OLED_FOR_INPUT)
#include <libMU/buttons.h>

typedef enum _libMU_Display_Input_t {
	NONE	= 0,
	ACCEPT	= BUTTON_SELECT,
	UP		= BUTTON_UP,
	DOWN	= BUTTON_DOWN,
	LEFT	= BUTTON_LEFT,
	RIGHT	= BUTTON_RIGHT,
	CANCEL	= 0x80,
} libMU_Display_Input_t;

const char libMU_Display_ValidCharsDec[] = " 0123456789-.e";
const char libMU_Display_ValidCharsHex[] = " 0123456789abcdef";
#define LIBMU_NUMCHARS_UINT		11
#define LIBMU_NUMCHARS_HEX		17
#define LIBMU_NUMCHARS_REAL		14
#define LINESIZE 				(libMU_Display_CHAR_WIDTH-2)
#define MEMORYSIZE				(libMU_Display_CHAR_WIDTH*libMU_Display_BPP/8)
#define COUNT_LIMIT				100000

/**
 * Introduce a number using the buttons and the OLED display
 * @param pNumber	Pointer where the number is stored
 * @param posX		Horizontal coordinate of position where the value is introduced
 * @param posY		Vertical coordinate of position where the value is introduced
 * @param intensity	Color intensity
 * @return			true if correctly introduced, false otherwise
 */
int libMU_Display_IntroduceUint32(uint32_t* pNumber, int posX, int posY, int intensity)
{
	char text[11];	/* 10 digits and ending character for uint32_t */
	int pos = 0; int charpos = 0; uint32_t count = 0;
	libMU_Display_Input_t input = NONE;

	if( pNumber == NULL ) return 0;
	libMU_snprintf( text, sizeof(text), "% 10u", pNumber[0] );

	/* Prepare underline */
	uint8_t	lineBuffer[ MEMORYSIZE ];
	libMU_Display_MemoryImage_t lineImage =
			{ lineBuffer, MEMORYSIZE, 1, MEMORYSIZE, lineBuffer + MEMORYSIZE };
	libMU_Display_MemoryImage_DrawLine( &lineImage, 0,0, LINESIZE,0, intensity );

	for(;;) {
		libMU_Display_DrawString( text, posX, posY, intensity );
		count++;
		if( count < COUNT_LIMIT/2 ) {
			libMU_Display_MemoryImage_Show( &lineImage,
					posX + pos * libMU_Display_CHAR_WIDTH,
					posY + libMU_Display_CHAR_HEIGHT - 1 );
		}
		if( count >= COUNT_LIMIT ) count = 0;
		input = (libMU_Display_Input_t)libMU_Button_Pressed( BUTTON_ALL );
		switch( input ) {
		case CANCEL:
			return 0;
		case UP:
			if( charpos >= (LIBMU_NUMCHARS_UINT-1) ) break;
			charpos++;
			text[pos] = libMU_Display_ValidCharsDec[charpos];
			break;
		case DOWN:
			if( charpos <= 0 ) break;
			charpos--;
			text[pos] = libMU_Display_ValidCharsDec[charpos];
			break;
		case LEFT:
			if( pos <= 0 ) break;
			pos--;
			charpos = (int)strchr( libMU_Display_ValidCharsDec, text[pos] );
			if( charpos ) charpos -= (int)libMU_Display_ValidCharsDec;
			else input = CANCEL;
			break;
		case RIGHT:
			if( pos >= (sizeof(text)-1) ) break;
			pos++;
			charpos = (int)strchr( libMU_Display_ValidCharsDec, text[pos] );
			if( charpos ) charpos -= (int)libMU_Display_ValidCharsDec;
			else input = CANCEL;
			break;
		case ACCEPT:
			*pNumber = libMU_atoul( text );
			return 1;
		}
	}
}

#include <libMU/internet.h>
/**
 * Introduce an IP address using the buttons and the OLED display
 * @param pNumber	Pointer where the IP address is stored
 * @param posX		Horizontal coordinate of position where the value is introduced
 * @param posY		Vertical coordinate of position where the value is introduced
 * @param intensity	Color intensity
 */
int libMU_Display_IntroduceIPv4(uint32_t* pNumber, int posX, int posY, int intensity)
{
	char text[16];	/* 12 digits + 3 dots and ending character */
	int pos = 0; int charpos = 0; uint32_t count = 0; uint32_t num, numpos = 0;
	libMU_Display_Input_t input = NONE;

	if( pNumber == NULL ) return 0;
	libMU_snprintf( text, sizeof(text), "% 3u.% 3u.% 3u.% 3u",
					pNumber[0] & 0xFF,		  (pNumber[0] >> 8) & 0xFF,
				   (pNumber[0] >> 16) & 0xFF, (pNumber[0] >>24) & 0xFF );

	/* Prepare underline */
	uint8_t	lineBuffer[ MEMORYSIZE ];
	libMU_Display_MemoryImage_t lineImage =
			{ lineBuffer, MEMORYSIZE, 1, MEMORYSIZE, lineBuffer + MEMORYSIZE };
	libMU_Display_MemoryImage_DrawLine( &lineImage, 0,0, LINESIZE,0, intensity );

	for(;;) {
		libMU_Display_DrawString( text, posX, posY, intensity );
		count++;
		if( count < COUNT_LIMIT/2) {
			libMU_Display_MemoryImage_Show( &lineImage,
					posX + pos * libMU_Display_CHAR_WIDTH,
					posY + libMU_Display_CHAR_HEIGHT - 1 );
		}
		if( count >= COUNT_LIMIT ) count = 0;
		input = (libMU_Display_Input_t)libMU_Button_Pressed( BUTTON_ALL );
		switch( input ) {
		case CANCEL:
			return 0;
		case UP:
			if( charpos >= (LIBMU_NUMCHARS_UINT-1) && num >= 0xFF) break;
			charpos++;
			text[pos] = libMU_Display_ValidCharsDec[charpos];
			break;
		case DOWN:
			if( charpos <= 0 && num == 0 ) break;
			charpos--;
			text[pos] = libMU_Display_ValidCharsDec[charpos];
			break;
		case LEFT:
			if( pos <= 0 ) break;
			pos--;
			if( pos == 3 || pos == 7 || pos == 11 ) { pos--; numpos = pos - 2; }
			charpos = (int)strchr( libMU_Display_ValidCharsDec, text[pos] );
			if( charpos ) charpos -= (int)libMU_Display_ValidCharsDec;
			else input = CANCEL;
			break;
		case RIGHT:
			if( pos >= (sizeof(text)-1) ) break;
			pos++;
			if( pos == 3 || pos == 7 || pos == 11 ) { pos++; numpos = pos; }
			charpos = (int)strchr( libMU_Display_ValidCharsDec, text[pos] );
			if( charpos ) charpos -= (int)libMU_Display_ValidCharsDec;
			else input = CANCEL;
			break;
		case ACCEPT:
			libMU_IP_Initialize( pNumber,
					libMU_atoul( &text[0] ), libMU_atoul( &text[4] ),
					libMU_atoul( &text[8] ), libMU_atoul( &text[12]) );
			return 1;
		}
		/* Update number format */
		num = libMU_atoul(&text[numpos]);
		if( num > 0xFF ) {
			text[numpos] = '2'; text[numpos+1] = '5'; text[numpos+2] = '5';
		}else if( num < 10 ) {
			text[numpos] = ' '; text[numpos+1] = ' ';
			text[numpos+2] = '0' + num;
		}else if( num < 100 ){
			text[numpos] = ' ';
			text[numpos+1] = '0' + num/10; text[numpos+2] = '0' + num % 10;
		}
	}
}

#endif

/**
 * @}
 */
