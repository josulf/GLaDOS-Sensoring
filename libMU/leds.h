/**
 * @addtogroup	libMU_LED
 * Library for LED management functions
 * @{
 ********************************************************************
 * @author		Eñaut Muxika <emuxika at mondragon dot edu>
 * @date		2011/7/10
 * @copyright	BSDL
 ********************************************************************
 */
#ifndef LED_H_
#define LED_H_

/**
 * Constants used for LED identification
 */
typedef enum _libMU_LED_Identifiers_t {
	LED_1	= 0x01,			/* Status LED: Port F, pin 0 */
	LED_2	= 0x04,			/* Ethernet LED: Port F, pin 2 */
	LED_3 	= 0x08,			/* Ethernet LED: Port F, pin 3 */

	LED_ALL =  0x08 | 0x4 | 0x01
} libMU_LED_Identifiers_t;

/**
 * Initialize LED HW in the board
 */
void	libMU_LED_Initialize(void);

/**
 * Inicializa LED HW in the board (Ethernet interface leds: LED_2 and LED_3)
 */
void	libMU_LED_InitializeEth(void);

/**
 * Switch on the LED
 * @param led	LED identifier code
 * @see libMU_LED_Identifiers_t
 */
void	libMU_LED_On(libMU_LED_Identifiers_t led);

/**
 * Switch off the LED
 * @param led	LED identifier code
 * @see libMU_LED_Identifiers_t
 */
void	libMU_LED_Off(libMU_LED_Identifiers_t led);

/**
 * Toggle the LED
 * @param led	LED identifier code
 * @see libMU_LED_Identifiers_t
 */
void	libMU_LED_Toggle(libMU_LED_Identifiers_t led);

#endif /*LED_H_*/
/**
 * @}
 */
