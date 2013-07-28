/**
 * @addtogroup	libMU_Menu
 * Library for menu management
 * @{
 ********************************************************************
 * @author		Eñaut Muxika <emuxika at mondragon dot edu>
 * @date		2011/7/10
 * @copyright	BSDL
 ********************************************************************
 */
#ifndef LIBMU_MENU_H_
#define LIBMU_MENU_H_

#include <stdio.h>
#include <libMU/display.h>

/**
 * Menu geometry and size constants
 */
enum _libMU_Menu_Constants_t {
	MENU_MAX_ENTRIES		= 500,
	MENU_ITEM_HEIGHT		= libMU_Display_CHAR_HEIGHT,
	MENU_ITEM_HEIGHT_SEP	= libMU_Display_CHAR_HEIGHT/2,
	MENU_ID_MAIN			= 0,
	MENU_ID_END				= -1
};

/**
 * Menu actions
 */
typedef	enum _libMU_Menu_Actions_t {
	MENU_NOTHING = 0,	/* Do nothing */
	MENU_UP,		/* Select previous menu entry */
	MENU_DOWN,		/* Select next menu entry */
	MENU_SEL,		/* Execute the function of the current entry or enter submenu */
	MENU_BACK,		/* Go to previous menu */
	MENU_DRAW,		/* Redraw the full menu */
} libMU_Menu_Actions_t;

/**
 * Menu item type identification constants
 */
typedef enum _libMU_Menu_ItemType_t {
	MENU_ITEM_INVALID = 0,	/* Invalid menu item */
	MENU_ITEM_SEPARATOR,	/* Menu separator (empty item) */
	MENU_ITEM,				/* Menu item */
	MENU_ITEM_SUBMENU,		/* Sub menu selection item */
} libMU_Menu_ItemType_t;

/**
 * Data type for menu functions or submenu identification
 */
typedef union _libMU_Menu_ItemData_t {
	void (*fun)(void);	/* Item function to be called when selected */
	int		menu_id;	/* Submenu id to display when selected */
} libMU_Menu_ItemData_t;

/**
 * Data type for menu item specification
 * @note
 * The menu items MUST be ordered by menu_id
 */
typedef struct _libMU_Menu_Item_t {
	int						menu_id;	/* Menu/submenu identifier */
										/* 0 - Main Menu */
										/* 1,2, ... - Submenus */
	libMU_Menu_ItemType_t	type;		/* Menu item type */
	const char*				text;		/* Menu item text */
	libMU_Menu_ItemData_t	data;		/* Menu item function or submenu */
} libMU_Menu_Item_t;

/**
 * Tipo de dato para la información del estado del menú
 */
typedef struct _libMU_Menu_Status_t {
	const libMU_Menu_Item_t*	menu;
	unsigned int				num_entries;
	unsigned int				current;
	unsigned int				cur_pos;
} libMU_Menu_Status_t;

/**
 * Initialize the menu status variable
 * @param menu_status	Pointer to menu status variable
 * @param menu			Pointer to menu specification (entry array)
 * @note
 * The pointer to the menu must have the main menu and all submenus
 */
static __inline void libMU_Menu_Initialize( libMU_Menu_Status_t* menu_status,
											const libMU_Menu_Item_t* menu )
{
	if( menu_status == NULL ) return;
	menu_status->menu = menu;
	menu_status->num_entries = 0;
	menu_status->current = 0;	/* Initial entry to display */
	menu_status->cur_pos = 0;
}

/**
 * Menu management function
 * @param ms		Pointer to menu status data
 * @param action	Action to perform
 * @note
 * The menu status MUST be previously initialized
 * @see		libMU_Menu_Actions_t, libMU_Menu_Status_t,  libMU_Menu_Initialize
 */
void libMU_Menu_Manage( libMU_Menu_Status_t* ms, libMU_Menu_Actions_t action );

#endif /*LIBMU_MENU_H_*/
/**
 * @}
 */
