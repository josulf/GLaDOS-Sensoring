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
#include <inc/hw_types.h>		/* New data types */
#include <inc/hw_memmap.h>		/* Peripheral memory map */
#include <driverlib/sysctl.h>	/* System control functions */
#include <driverlib/gpio.h>		/* General Purpose IO functions */
#include <libMU/buttons.h>

/**
 * Variable where the button state is saved to detect press and depress situations
 */
libMU_Button_t libMU_ButtonState;

/**
 * Initialize the GPIO HW connected to buttons
 */
void 	libMU_Button_Initialize(void)
{
	/* 6.- Configure buttons
	 * select PF1, up PE0, down PE1/PWM4, left PE2/PWM5, right PE3 */
    SysCtlPeripheralEnable	( SYSCTL_PERIPH_GPIOE );/* Enable port for button pins & assign pins */
    GPIODirModeSet	( GPIO_PORTE_BASE, 
    				  GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3,
    				  GPIO_DIR_MODE_IN );
    GPIOPadConfigSet( GPIO_PORTE_BASE,
    				  GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3,
    				  GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPU );
    SysCtlPeripheralEnable	( SYSCTL_PERIPH_GPIOF );/* Enable port for button pins & assign pins */
    GPIODirModeSet	( GPIO_PORTF_BASE, GPIO_PIN_1, GPIO_DIR_MODE_IN );
    GPIOPadConfigSet( GPIO_PORTF_BASE, GPIO_PIN_1,
    				  GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPU );
    libMU_ButtonState = 0;
}

/**
 * @return The Button status
 * @note A value of 1 in the corresponding bit means the button is pressed
 * and a value of 0 means depressed
 */
libMU_Button_t	libMU_Button_GetFullStatus(void)
{
	libMU_Button_t status;
	status = GPIOPinRead( GPIO_PORTF_BASE, GPIO_PIN_1 ) << 3;
	status|= GPIOPinRead( GPIO_PORTE_BASE, GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3 );
	return ( (~status) & 0x1F );
}

/**
 * @}
 */
