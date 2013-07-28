/**
 * @addtogroup	libMU_GPIO
 * Library to manage General Purpose Inputs and Outputs (GPIO)
 * @{
 ********************************************************************
 * @author		Eñaut Muxika <emuxika at mondragon dot edu>
 * @date		2011/7/10
 * @copyright	BSDL
 * @note			|||||||||||
 * This library may | DESTROY | the board if used incorrectly
 *******************|||||||||||**************************************
 */
#ifndef LIBMU_GPIO_H_
#define LIBMU_GPIO_H_

/**
 * Constant creation macros
 */
#define	GPIO_PORT_POS				28
#define	GPIO_PIN_MASK				( (1 << GPIO_PORT_POS ) - 1 )
#define	GPIO_CONST( port, bit )		( ( port << GPIO_PORT_POS ) | ( 1 << bit )  )
#define	GPIO_CONSTP( port )			( ( port << GPIO_PORT_POS ) | GPIO_PIN_MASK )

/**
 * Constants used in GPIO management
 */
typedef enum _libMU_GPIO_ID_t {
	/* Port A */
	GPIO_PA		= GPIO_CONSTP( 0 ),
	GPIO_PA0	= GPIO_CONST( 0, 0 ),
	GPIO_PA1	= GPIO_CONST( 0, 1 ),
	GPIO_PA2	= GPIO_CONST( 0, 2 ),
	GPIO_PA3	= GPIO_CONST( 0, 3 ),
	GPIO_PA4	= GPIO_CONST( 0, 4 ),
	GPIO_PA5	= GPIO_CONST( 0, 5 ),
	GPIO_PA6	= GPIO_CONST( 0, 6 ),
	GPIO_PA7	= GPIO_CONST( 0, 7 ),

	/* Port B */
	GPIO_PB		= GPIO_CONSTP( 1 ),
	GPIO_PB0	= GPIO_CONST( 1, 0 ),
	GPIO_PB1	= GPIO_CONST( 1, 1 ),
	GPIO_PB2	= GPIO_CONST( 1, 2 ),
	GPIO_PB3	= GPIO_CONST( 1, 3 ),
	GPIO_PB4	= GPIO_CONST( 1, 4 ),
	GPIO_PB5	= GPIO_CONST( 1, 5 ),
	GPIO_PB6	= GPIO_CONST( 1, 6 ),
	GPIO_PB7	= GPIO_CONST( 1, 7 ),

	/* Port C */
	GPIO_PC		= GPIO_CONSTP( 2 ),
	GPIO_PC0	= GPIO_CONST( 2, 0 ),
	GPIO_PC1	= GPIO_CONST( 2, 1 ),
	GPIO_PC2	= GPIO_CONST( 2, 2 ),
	GPIO_PC3	= GPIO_CONST( 2, 3 ),
	GPIO_PC4	= GPIO_CONST( 2, 4 ),
	GPIO_PC5	= GPIO_CONST( 2, 5 ),
	GPIO_PC6	= GPIO_CONST( 2, 6 ),
	GPIO_PC7	= GPIO_CONST( 2, 7 ),

	/* Port D */
	GPIO_PD		= GPIO_CONSTP( 3 ),
	GPIO_PD0	= GPIO_CONST( 3, 0 ),
	GPIO_PD1	= GPIO_CONST( 3, 1 ),
	GPIO_PD2	= GPIO_CONST( 3, 2 ),
	GPIO_PD3	= GPIO_CONST( 3, 3 ),
	GPIO_PD4	= GPIO_CONST( 3, 4 ),
	GPIO_PD5	= GPIO_CONST( 3, 5 ),
	GPIO_PD6	= GPIO_CONST( 3, 6 ),
	GPIO_PD7	= GPIO_CONST( 3, 7 ),

	/* Port E */
	GPIO_PE		= GPIO_CONSTP( 4 ),
	GPIO_PE0	= GPIO_CONST( 4, 0 ),
	GPIO_PE1	= GPIO_CONST( 4, 1 ),
	GPIO_PE2	= GPIO_CONST( 4, 2 ),
	GPIO_PE3	= GPIO_CONST( 4, 3 ),

	/* Port F */
	GPIO_PF		= GPIO_CONSTP( 5 ),
	GPIO_PF0	= GPIO_CONST( 5, 0 ),
	GPIO_PF1	= GPIO_CONST( 5, 1 ),
	GPIO_PF2	= GPIO_CONST( 5, 2 ),
	GPIO_PF3	= GPIO_CONST( 5, 3 ),

	/* Port G */
	GPIO_PG		= GPIO_CONSTP( 6 ),
	GPIO_PG0	= GPIO_CONST( 6, 0 ),
	GPIO_PG1	= GPIO_CONST( 6, 1 ),

	GPIO_PORTS	= 7
} libMU_GPIO_ID_t;

/**
 * Output type specificaton for the function libMU_GPIO_Initialize()
 * @see	libMU_GPIO_Initialize()
 */
typedef enum _libMU_GPIO_Type_t {
	GPIO_INPUT				= 0x00,	/* Input */
	GPIO_OUTPUT				= 0x01,	/* Standard output */
	GPIO_TYPE_STD			= 0x00,	/* Standard I/O */
	GPIO_TYPE_WEAK_PULLUP	= 0x10,	/* I/O with weak pull-up resistor */
	GPIO_TYPE_WEAK_PULLDOWN	= 0x20,	/* I/O with weak pull-down resistor */

	GPIO_MASK_INOUT			= 0x0F,
	GPIO_MASK_TYPE			= 0xF0
} libMU_GPIO_Type_t;

#undef GPIO_CONST

/**
 * Data type to store the I/O status
 */
typedef unsigned int libMU_GPIO_t;

/**
 * Variable where the status of each port is saved (for rising and falling edge detection)
 */
extern	libMU_GPIO_t	libMU_GPIO_PortStatus[GPIO_PORTS];

/**
 * Initialize the board pins as inputs or outputs
 * @param	port_and_pin_ID	Port and pin identifications
 * @param	type			Specify the pin types
 * @note					Only one port can be initialized at a time
 * @see 	libMU_GPIO_Type_t
 */
void 	libMU_GPIO_Initialize( libMU_GPIO_ID_t port_and_pins, libMU_GPIO_Type_t tipo );

/**
 * Read the status of the pins in a port
 * @param	port	Port identifier
 * @return			Status of the specified port
 */
libMU_GPIO_t	libMU_GPIO_GetPortStatus( libMU_GPIO_ID_t port );

/**
 * Set the status of the pins in a port
 * @param	port	Port identifier
 * @param	value	New state of the port
 * @note			Only output pins are changed
 */
void	libMU_GPIO_SetPortStatus( libMU_GPIO_ID_t port, unsigned int value );

/**
 * Set the status of the output pins to one
 * @param	pins	Code of the port to write
 * @note			Only output pins are changed
 */
void	libMU_GPIO_SetPinsToOne( libMU_GPIO_ID_t pins  );

/**
 * Set the status of the output pins to zero
 * @param	pins	Code of the port to write
 * @note			Only output pins are changed
 */
void	libMU_GPIO_SetPinsToZero( libMU_GPIO_ID_t pins  );

/**
 * Toggle the status of the output pins
 * @param	pins	Code of the port to write
 * @note			Only output pins are changed
 */
void	libMU_GPIO_TogglePins( libMU_GPIO_ID_t pins  );

/**
 * get the bit mask for the specified pin identifier
 * @param	pin		Pin identifier
 * @return			Bit mask
 */
static __inline unsigned int	libMU_GPIO_GetBitMask( libMU_GPIO_ID_t pin )
{
	return ( pin & GPIO_PIN_MASK );
}

/**
 * Check the status of a pin
 * @param	pin		Pin identifier
 * @return			Pin status, true if active, false otherwise
 */
static __inline unsigned int	libMU_GPIO_GetStatus( libMU_GPIO_ID_t pin )
{
	return ( libMU_GPIO_GetPortStatus(pin) & libMU_GPIO_GetBitMask(pin) );
}

/**
 * Check if rising edge has been detected in a pin
 * @param	pin		Pin identifier
 * @return			true if a rising edge is detected
 */
static __inline unsigned int	libMU_GPIO_GetStatusRisingEdge( libMU_GPIO_ID_t pin )
{
	libMU_GPIO_t b_prv = libMU_GPIO_PortStatus[ pin >> GPIO_PORT_POS ] & libMU_GPIO_GetBitMask(pin);
	libMU_GPIO_t b = libMU_GPIO_GetPortStatus( pin ) & libMU_GPIO_GetBitMask(pin);
	libMU_GPIO_PortStatus[ pin >> GPIO_PORT_POS ] =
			( libMU_GPIO_PortStatus[ pin >> GPIO_PORT_POS ] & ~libMU_GPIO_GetBitMask(pin) ) | b;
	return ( b & ~b_prv );
}

/**
 * Check if falling edge has been detected in a pin
 * @param	pin		Pin identifier
 * @return			true if a falling edge is detected
 */
static __inline unsigned int	libMU_GPIO_GetStatusFallingEdge( libMU_GPIO_ID_t pin )
{
	libMU_GPIO_t b_prv = libMU_GPIO_PortStatus[ pin >> GPIO_PORT_POS ] & libMU_GPIO_GetBitMask( pin );
	libMU_GPIO_t b = libMU_GPIO_GetPortStatus( pin ) & libMU_GPIO_GetBitMask( pin );
	libMU_GPIO_PortStatus[ pin >> GPIO_PORT_POS ] =
			( libMU_GPIO_PortStatus[ pin >> GPIO_PORT_POS ] & ~libMU_GPIO_GetBitMask( pin ) ) | b;
	return ( ~b & b_prv );
}

#endif /*LIBMU_GPIO_H_*/
/**
 * @}
 */
