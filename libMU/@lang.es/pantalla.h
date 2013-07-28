/**
 * @addtogroup	libMU_Display
 * Librería para el uso sencillo de la pantalla
 * @{
 ********************************************************************
 * @author		Eñaut Muxika <emuxika at mondragon dot edu>
 * @date		2011/7/10
 * @copyright	BSDL
 ********************************************************************
 */
#ifndef PANTALLA_H_
#define PANTALLA_H_

#include <libMU/display.h>

/**
 * Constantes de la pantalla OLED
 */
enum libMU_Pantalla_Constantes_t {
	libMU_Pantalla_RES_X		= libMU_Display_RES_X,		/* Resolución horizontal de la pantalla */
	libMU_Pantalla_RES_Y		= libMU_Display_RES_Y,		/* Resolución vertical de la pantalla */
	libMU_Pantalla_CHAR_WIDTH	= libMU_Display_CHAR_WIDTH,	/* Tamaño horizontal de un caracter en pixeles */
	libMU_Pantalla_CHAR_HEIGHT	= libMU_Display_CHAR_HEIGHT,/* Tamaño vertical de un caracter en pixeles */
	libMU_Pantalla_BPP			= libMU_Display_BPP			/* Bits por pixel de la pantalla */
};

/**
 * Tipo de datos para guardar la información sobre una imagen en memoria local
 */
#if 0
	uint8_t*	buffer;	/* Puntero a la imagen en memoria */
	uint16_t	width;	/* Anchura de la imagen en memoria (en bytes, no en pixeles) */
	uint16_t	height;	/* Altura de la imagen en memoria */
	uint32_t	size;	/* Tamaño de la imagen */
	uint8_t*	end;	/* Puntero al final de la imagen en memoria */
#endif
typedef libMU_Display_MemoryImage_t libMU_Pantalla_MemoriaImagen_t;

/**
 * Tipo de datos para guardar una imagen de la pantalla en memoria local
 * para acelerar las operaciones.
 * 128 pixeles ancho x 96 pixeles alto x (4 bits/pixel / 8 bits/byte ) = 6144 bytes
 */
typedef libMU_Display_MemoryBuffer_t	libMU_Pantalla_MemoriaBuffer_t;

/**
 * Imagen de la pantalla en memoria local para acelerar las operaciones
 */
#define libMU_Pantalla_BufferImagen	libMU_Display_ImageBuffer
#define	libMU_Pantalla_ImagenLocal	libMU_Display_LocalImage

/**
 * Inicializa el HW de la pantalla
 */
static inline void libMU_Pantalla_Inicializa(void)
{
	libMU_Display_Initialize();
}

/**
 * Borrar la pantalla
 */
static inline void libMU_Pantalla_Borrar(void)
{
	libMU_Display_Clear();
}

/**
 * Dibuja un texto en pantalla
 * @param texto		Texto a mostrar
 * @param pos_X		Posicion horizontal del texto en la pantalla en pixeles (menor que 128 y par)
 * @param pos_Y		Posicion vertical del texto en la pantalla en pixeles (menor que 96)
 * @param intens	Intensidad del color (de 0 a 15)
 */
static inline void libMU_Pantalla_DibujaTexto(const char *texto, int pos_X, int pos_Y, int intens)
{
	libMU_Display_DrawString( texto, pos_X,  pos_Y, intens );
}

/**
 * Borra la imagen de memoria
 * @param memImage	Puntero a la estructura de datos de la imagen en memoria
 */
static inline void libMU_Pantalla_MemoriaImagen_Borrar( libMU_Pantalla_MemoriaImagen_t* memImage )
{
	libMU_Display_MemoryImage_Clear(memImage);
}

/**
 * Dibuja una imagen en pantalla
 * @param memImage	Puntero a la estructura de datos de la imagen en memoria
 * @param pos_X		Coordenada horizontal del punto superior izquierdo de la imagen
 * @param pos_Y		Coordenada vertical del punto superior izquierdo de la imagen
 * @note			pos_X tiene que ser par por limitaciones de pantalla
 */
static inline void libMU_Pantalla_MemoriaImagen_Dibuja(
							const libMU_Pantalla_MemoriaImagen_t* memImage,
							unsigned int pos_X,
							unsigned int pos_Y )
{
	libMU_Display_MemoryImage_Show(memImage,pos_X,pos_Y);
}

/**
 * Dibuja un pixel en la imagen local de la pantalla
 * @param memImage	Puntero a la estructura de datos de la imagen en memoria
 * @param lX		Coordenada horizontal inicial de la linea
 * @param lY		Coordenada vertical inicial de la linea
 * @param intens	Intensidad del color (de 0 a 15)
 */
static inline void libMU_Pantalla_MemoriaImagen_DibujaPixel( libMU_Pantalla_MemoriaImagen_t* memImage, int lX, int lY, int intens )
{
	libMU_Display_MemoryImage_DrawPixel( memImage, lX, lY, intens );
}

/**
 * Establece el tamaño de la imagen en memoria local
 * @param memImage	Puntero a la estructura de datos de la imagen en memoria
 * @param buffer	Puntero a la memoria donde se guardará la imagen en memoria local
 * @param anchura	Dimensión horizontal de la imagen en memoria
 * @param altura	Dimensión vertical de la imagen en memoria
 * @note			La dimensión horizontal tiene que ser par y el tamaño del
 * 					buffer tiene que se suficiente para poder guardar la imagen
 */
static inline void	libMU_Pantalla_MemoriaImagen_Inicializa( libMU_Pantalla_MemoriaImagen_t* memImage,
					uint8_t* buffer, int anchura, int altura )
{
	libMU_Display_MemoryImage_Initialize( memImage, buffer, anchura, altura );
}

/**
 * Dibuja una linea usando el algoritmo de Bresenham en la imagen local de la pantalla
 * @param memImage	Puntero a la estructura de datos de la imagen en memoria
 * @param lX1		Coordenada horizontal inicial de la linea
 * @param lY1		Coordenada vertical inicial de la linea
 * @param lX2		Coordenada horizontal final de la linea
 * @param lY2		Coordenada vertical final de la linea
 * @param intensity	Intensidad del color (de 0 a 15)
 */
static inline void	libMU_Pantalla_MemoriaImagen_DibujaLinea( libMU_Pantalla_MemoriaImagen_t* memImage,
						int lX1, int lY1, int lX2, int lY2, unsigned int intens )
{
	libMU_Display_MemoryImage_DrawLine(memImage, lX1, lY1, lX2, lY2, intens);
}

/**
 * Dibuja una elipse usando el algoritmo de Bresenham en la imagen local de la pantalla
 * @param memImage	Puntero a la estructura de datos de la imagen en memoria
 * @param lX1		coordenada horizontal inicial
 * @param lY1		coordenada vertical inicial
 * @param lX2		coordenada horizontal final
 * @param lY2		coordenada vertical final
 * @param intens	intensidad del color (de 0 a 15)
 */
static inline void	libMU_Pantalla_MemoriaImagen_DibujaElipse( libMU_Pantalla_MemoriaImagen_t* memImage,
						int lX1, int lY1, int lX2, int lY2, unsigned int intens )
{
	libMU_Display_MemoryImage_DrawEllipse(memImage, lX1, lY1, lX2, lY2, intens);
}

/**
 * Establece el tamaño de la imagen en memoria local
 * @param anchura	Dimensión horizontal de la imagen en memoria
 * @param altura	Dimensión vertical de la imagen en memoria
 * @note			La dimensión horizontal tiene que ser par
 */
static inline void	libMU_Pantalla_MemoriaImagen_InicializaLocal(int anchura, int altura)
{
	libMU_Display_MemoryImage_InitializeLocal(anchura, altura);
}

/**
 * Borrar la imagen local de la pantalla
 */
static inline void	libMU_Pantalla_MemoriaImagen_BorrarLocal(void)
{
	libMU_Display_MemoryImage_ClearLocal();
}

/**
 * Dibuja la imagen de memoria local en pantalla
 * @param pos_X		coordenada horizontal del punto superior izquierdo de la imagen
 * @param pos_Y		coordenada vertical del punto superior izquierdo de la imagen
 * @note			pos_X tiene que ser par por limitaciones de pantalla
 */
static inline void	libMU_Pantalla_MemoriaImagen_DibujaLocal(
							unsigned int pos_X,
							unsigned int pos_Y )
{
	libMU_Display_MemoryImage_ShowLocal(pos_X, pos_Y);
}

/**
 * Dibuja una linea usando el algoritmo de Bresenham en la imagen local de la pantalla
 * @param lX1		coordenada horizontal inicial de la linea
 * @param lY1		coordenada vertical inicial de la linea
 * @param lX2		coordenada horizontal final de la linea
 * @param lY2		coordenada vertical final de la linea
 * @param intens	intensidad del color (de 0 a 15)
 */
static inline void	libMU_Pantalla_MemoriaImagen_DibujaLineaLocal(
						int lX1, int lY1, int lX2, int lY2, unsigned int intens)
{
	libMU_Display_MemoryImage_DrawLineLocal( lX1, lY1, lX2, lY2, intens );
}

#endif /*PANTALLA_H_*/
/**
 * @}
 */
