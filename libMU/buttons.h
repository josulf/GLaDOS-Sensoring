/**
 * @addtogroup	libMU_Button
 * Library to manipulate buttons
 * @{
 ********************************************************************
 * @author		Eñaut Muxika <emuxika at mondragon dot edu>
 * @date		2011/7/10
 * @copyright	BSDL
 ********************************************************************
 */
#ifndef LIBMU_BUTTONS_H_
#define LIBMU_BUTTONS_H_
#include <stdint.h>

/**
 * Constants used for button functions
 */
typedef enum _libMU_Button_Identifiers_t {
	BUTTON_UP		= 0x01,
	BUTTON_DOWN		= 0x02,
	BUTTON_LEFT 	= 0x04,
	BUTTON_RIGHT	= 0x08,
	BUTTON_SELECT	= 0x10,

	BUTTON_ALL		= 0x1F,
#if 0
	BOTON_ARRIBA	= BUTTON_UP,
	BOTON_ABAJO		= BUTTON_DOWN,
	BOTON_IZQUIERDA = BUTTON_LEFT,
	BOTON_DERECHA	= BUTTON_RIGHT,
	BOTON_SELECT	= BUTTON_SELECT,
#endif
	BUTTON_END		= -1,
} libMU_Button_Identifiers_t;

/**
 * Data type to store the button status
 */
typedef uint32_t libMU_Button_t;

/**
 * Variable where the button state is saved to detect press and depress situations
 */
extern	libMU_Button_t	libMU_ButtonState;

/**
 * Initialize the GPIO HW connected to buttons
 */
void 	libMU_Button_Initialize(void);

/**
 * @return The Button status
 * @note A value of 1 in the corresponding bit means the button is pressed
 * and a value of 0 means depressed
 */
libMU_Button_t	libMU_Button_GetFullStatus(void);

/**
 * Test if given buttons are pressed
 * @param	button	Identification code for one or more buttons
 * @return			false if depressed, true if pressed
 */
static __inline uint32_t libMU_Button_GetStatus( const libMU_Button_Identifiers_t button );
static __inline uint32_t libMU_Button_GetStatus( const libMU_Button_Identifiers_t button )
{
	return ( libMU_Button_GetFullStatus() & button );
}

/**
 * Test if given buttons have been just pressed
 * @param	button	Identification code for one or more buttons
 * @return			true if just pressed
 * @note
 * - This is performed by detecting rising edges in the corresponding pins
 */
static __inline uint32_t libMU_Button_Pressed( const libMU_Button_Identifiers_t button );
static __inline uint32_t libMU_Button_Pressed( const libMU_Button_Identifiers_t button )
{
	libMU_Button_t b_prv = libMU_ButtonState & button;
	libMU_Button_t b = libMU_Button_GetFullStatus() & button;
	/* Update the button state for given buttons */
	libMU_ButtonState = ( libMU_ButtonState & ((libMU_Button_t)~button) ) | b;
	return ( b & ~b_prv );
}

/**
 * Test if given buttons have been just depressed
 * @param	button	Identification code for one or more buttons
 * @return			true if just depressed
 * @note
 * - This is performed by detecting falling edges in the corresponding pins
 */
static __inline uint32_t	libMU_Button_Depressed( const libMU_Button_Identifiers_t button );
static __inline uint32_t	libMU_Button_Depressed( const libMU_Button_Identifiers_t button )
{
	libMU_Button_t b_prv = libMU_ButtonState & button;
	libMU_Button_t b = libMU_Button_GetFullStatus() & button;
	/* Update the button state for given buttons */
	libMU_ButtonState = ( libMU_ButtonState & ((libMU_Button_t)~button) ) | b;
	return ( ~b & b_prv );
}

#endif /*LIBMU_BUTTONS_H_*/
/**
 * @}
 */
