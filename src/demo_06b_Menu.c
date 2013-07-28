/**
 * @addtogroup demos
 * Test del uso del sistema operativo FreeRTOS
 * @{
 ********************************************************************
 * @author		Eñaut Muxika <emuxika at mondragon dot edu>
 * @date		2011/7/10
 * @copyright	BSDL
 ********************************************************************
 */
#include <stdint.h>
#include <libMU/display.h>
#include <libMU/stdlib.h>
#include <libMU/buttons.h>
#include <libMU/menu.h>
#include <libMU/delay.h>
#include "check_temp.h"

/**
 * Count milliseconds
 */
static uint32_t	Count_1ms		= 0xFF;
static uint32_t	Count_Display;

/**
 * Prototypes
 */
void Show_display_count(void);
void Show_sensor_count(void);
void main_demo_06b(void);

/**
 * Menu definitions
 */
enum menus_t {
	MAIN_MENU = MENU_ID_MAIN,
	SUBMENU_1,
};
const libMU_Menu_Item_t menu_entries[] = {
	/* Main menu */
	{ MAIN_MENU, MENU_ITEM, "Display count", { .fun = Show_display_count } },
	{ MAIN_MENU, MENU_ITEM, "Sensor count", { .fun = Show_sensor_count } },
	{ MAIN_MENU, MENU_ITEM_SEPARATOR },
	{ MAIN_MENU, MENU_ITEM_SUBMENU, "Submenu", { .menu_id = SUBMENU_1 } },
	{ MAIN_MENU, MENU_ITEM, "Do Nothing", { .fun = NULL } },

	/* Submenu 1 */
	{ SUBMENU_1, MENU_ITEM, "SM Display count", { .fun = Show_display_count } },
	{ SUBMENU_1, MENU_ITEM_SEPARATOR },
	{ SUBMENU_1, MENU_ITEM, "SM Do Nothing", { .fun = NULL } },
	{ SUBMENU_1, MENU_ITEM, "SM Sensor count", { .fun = Show_sensor_count } },

	/* End menu specification */
	{ MENU_ID_END }
};

/**
 * Programa principal
 */
void main_demo_06b(void)
{
	/* Initialize HW */
	libMU_Menu_Status_t		menu_stat;
	libMU_Menu_Actions_t	action;

	libMU_Display_Initialize();
	libMU_AD_Initialize();
	libMU_Display_DrawString( "HW init done!", 0, 0, 12 );	
	Count_Display = 0;
	libMU_Button_Initialize();
	libMU_Menu_Initialize( &menu_stat, menu_entries );
	libMU_Menu_Manage( &menu_stat, MENU_DRAW );
	for(;;) {
		libMU_AD_StartConversion();
		libMU_Delay( 1000000 );	/* 60 milliseconds */
		CheckTemperature();
		Count_Display++;
		uint32_t act = libMU_Button_Pressed( BUTTON_ALL );
		action = MENU_NOTHING;
		if( act & BUTTON_UP   )	action = MENU_UP;
		if( act & BUTTON_SELECT) action = MENU_UP;
		if( act & BUTTON_DOWN )	action = MENU_DOWN;
		if( act & BUTTON_LEFT )	action = MENU_BACK;
		if( act & BUTTON_RIGHT)	action = MENU_SEL;
		libMU_Menu_Manage( &menu_stat, action );
	}	
}

void Show_display_count(void)
{
	char msg[50];
	libMU_snprintf( msg, sizeof(msg), "Cnt<Disp>=%d", Count_Display );
	libMU_Display_DrawString( msg, 0, 88, 15 );
	while( !libMU_Button_Pressed( BUTTON_LEFT ) ) {}
}

void Show_sensor_count(void)
{
	char msg[50];
	libMU_snprintf( msg, sizeof(msg), "Cnt<Sens>=%d", Count_1ms );
	libMU_Display_DrawString( msg, 0, 88, 15 );
	while( !libMU_Button_Pressed( BUTTON_LEFT ) ) {}
}
/**
 * @}
 */
