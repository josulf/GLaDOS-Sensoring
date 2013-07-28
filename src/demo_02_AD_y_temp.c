/**
 * @addtogroup demos
 * Test del HW analogico digital, temperatura y pantalla:
 * @{
 ********************************************************************
 * @author		Eñaut Muxika <emuxika at mondragon dot edu>
 * @date		2011/7/10
 * @copyright	BSDL
 ********************************************************************
 */
#include <stdint.h>
#include <libMU/analog_digital.h>
#include <libMU/temperature.h>
#include <libMU/display.h>
#include <libMU/delay.h>

void main_demo_02(void);

/**
 * Programa principal
 */
void main_demo_02(void)
{
	uint32_t	ADC_Values[4];
	int 		Temp; 
	int			count = 0;

	libMU_Display_Initialize();	
	libMU_AD_Initialize();			

	libMU_Display_DrawString	( "Kaixo!!", 0, 0, 15 );

	while(1)	/* repite indefinidamente */
	{
		libMU_AD_StartConversion();
		count++;
		ADC_Values[0] = libMU_AD_GetChannelValue( ADC_CHANNEL_0 );
		ADC_Values[1] = libMU_AD_GetChannelValue( ADC_CHANNEL_1 );
		ADC_Values[2] = libMU_AD_GetChannelValue( ADC_CHANNEL_2 );
		ADC_Values[3] = libMU_AD_GetChannelValue( ADC_CHANNEL_3 );
		Temp          = libMU_Temperature_Get();

		/* Mostrar resultados */
		libMU_Display_SetLine(8);
		libMU_Display_SetColor(15);
		libMU_Display_printf( "Temp = % 3d.%d C\n", Temp/10, Temp%10 );
		libMU_Display_printf( "ADC0 = %.2f V\n", (double)ADC_Values[0]*(3.0/1024.0) );
		libMU_Display_printf( "ADC1 = %.2f V\n", (double)ADC_Values[1]*(3.0/1024.0) );
		libMU_Display_printf( "ADC2 = %.2f V\n", (double)ADC_Values[2]*(3.0/1024.0) );
		libMU_Display_printf( "ADC3 = %.2f V\n", (double)ADC_Values[3]*(3.0/1024.0) );
		libMU_Display_SetColor(8);
		libMU_Display_printf( "\nNum = % 8d", count );

		/* Retardo ~ 15000000 x 60ns = 900ms */
		libMU_Delay( 15000000u );
	}
}
/**
 * @}
 */
