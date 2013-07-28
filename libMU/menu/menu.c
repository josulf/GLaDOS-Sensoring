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
#include <libMU/menu.h>
#include <libMU/display.h>

/**
 * Static function prototypes
 */
static void libMU_Menu_Draw	( libMU_Menu_Status_t* ms );
static void libMU_Menu_Up	( libMU_Menu_Status_t* ms );
static void libMU_Menu_Down	( libMU_Menu_Status_t* ms );
static void libMU_Menu_Back	( libMU_Menu_Status_t* ms );
static void libMU_Menu_Select( libMU_Menu_Status_t* ms );

/**
 * Menu management function
 * @param ms		Pointer to menu status data
 * @param action	Action to perform
 * @note
 * The menu status MUST be previously initialized
 * @see		libMU_Menu_Actions_t, libMU_Menu_Status_t,  libMU_Menu_Initialize
 */
void libMU_Menu_Manage( libMU_Menu_Status_t* ms, libMU_Menu_Actions_t action )
{
	if( ms == NULL ) return;
	switch( action ) {
	case MENU_DRAW:		libMU_Menu_Draw	( ms );	break;
	case MENU_UP:		libMU_Menu_Up	( ms );	break;
	case MENU_DOWN:		libMU_Menu_Down	( ms );	break;
	case MENU_BACK:		libMU_Menu_Back	( ms );	break;
	case MENU_SEL:		libMU_Menu_Select(ms );	break;
	default:			break;
	}
}

static void libMU_Menu_Draw( libMU_Menu_Status_t* ms )
{
	int start, end, pos, i;
	const libMU_Menu_Item_t* menu = ms->menu;
	if( !menu ) return;
	int cur_menu = menu[ ms->current ].menu_id;
	/* Draw menu text */
	pos = 0;
	/* Count the number of entries of the menu */
	ms->num_entries = 0;
	/* Search first entry */
	for( i = ms->current; i >= 0; i-- ) {
		if( menu[i].menu_id != cur_menu ) break;
		start = i;
	}
	/* Search last entry */
	for( i = ms->current + 1; i < MENU_MAX_ENTRIES; i++ ) {
		if( menu[i].menu_id != cur_menu ) break;
	}
	end = i;
	ms->num_entries = end - start;
	libMU_Display_Clear();
	libMU_Display_DrawString( "Menu", 0, pos, 15 );
	pos += MENU_ITEM_HEIGHT;
	for( i = start; i < end; i++ ) {
		const char* text = menu[i].text;
		if( ms->current == i ) {
			ms->cur_pos = pos;
			libMU_Display_DrawString( ">", 0, pos, 15 );
		}
		if( menu[i].type == MENU_ITEM_SEPARATOR ) {
			pos += MENU_ITEM_HEIGHT_SEP;
		}else if( menu[i].type == MENU_ITEM || menu[i].type == MENU_ITEM_SUBMENU ) {
			if( text ) {
				libMU_Display_DrawString( text , libMU_Display_CHAR_WIDTH, pos, 12 );
			}
			pos += MENU_ITEM_HEIGHT;
		}else{
			break;
		}
		if( pos >= libMU_Display_RES_Y ) break;
	}
}

static void libMU_Menu_Up( libMU_Menu_Status_t* ms )
{
	int pos, i, new_cur;
	const libMU_Menu_Item_t* menu = ms->menu;
	if( !menu ) return;
	int cur_menu = menu[ ms->current ].menu_id;
	/* Search for previous menu entry */
	new_cur = ms->current; pos = ms->cur_pos;
	for( i = ms->current - 1; i >= 0 && menu[i].menu_id == cur_menu; i-- ) {
		if( menu[i].type == MENU_ITEM_INVALID ) break;
		if( menu[i].type == MENU_ITEM_SEPARATOR) {
			pos -= MENU_ITEM_HEIGHT_SEP;
			continue;
		}
		pos -= MENU_ITEM_HEIGHT;
		new_cur = i;
		break;
	}
	/* Update if new entry found */
	if( ms->current!= new_cur ) {
		libMU_Display_DrawString( " ", 0, ms->cur_pos, 15 );
		libMU_Display_DrawString( ">", 0, pos, 15 );
		ms->current = new_cur;
		ms->cur_pos = pos;
	}
}

static void libMU_Menu_Down( libMU_Menu_Status_t* ms )
{
	int pos, i, new_cur;
	const libMU_Menu_Item_t* menu = ms->menu;
	if( !menu ) return;
	int cur_menu = menu[ ms->current ].menu_id;
	/* Search for next menu entry */
	new_cur = ms->current; pos = ms->cur_pos + MENU_ITEM_HEIGHT;
	for( i = 1 + ms->current; i < MENU_MAX_ENTRIES && menu[i].menu_id == cur_menu; i++ ) {
		if( menu[i].type == MENU_ITEM_INVALID ) break;
		if( menu[i].type == MENU_ITEM_SEPARATOR) {
			pos += MENU_ITEM_HEIGHT_SEP;
			continue;
		}
		new_cur = i;
		break;
	}
	/* Update if new entry found */
	if( ms->current!= new_cur ) {
		libMU_Display_DrawString( " ", 0, ms->cur_pos, 15 );
		libMU_Display_DrawString( ">", 0, pos, 15 );
		ms->current = new_cur;
		ms->cur_pos = pos;
	}
}

static void libMU_Menu_Back( libMU_Menu_Status_t* ms )
{
	int i;
	const libMU_Menu_Item_t* menu = ms->menu;
	if( !menu ) return;
	int cur_menu = menu[ ms->current ].menu_id;
	if( cur_menu == MENU_ID_MAIN ) return;
	/* Search parent menu */
	/* Works if a menu can only be reached by using a single path */
	for( i = 0; i < MENU_MAX_ENTRIES; i++ ) {
		if( menu[i].menu_id == MENU_ID_END ) break;
		if( menu[i].type == MENU_ITEM_INVALID ) break;
		if( menu[i].type == MENU_ITEM_SUBMENU && menu[i].data.menu_id == cur_menu ) {
			ms->current = i;
			libMU_Menu_Draw( ms );
			break;
		}
	}
}

static void libMU_Menu_Select( libMU_Menu_Status_t* ms )
{
	const libMU_Menu_Item_t* menu = ms->menu;
	if( !menu ) return;
	if( menu[ms->current].type == MENU_ITEM ) {
		if( menu[ms->current].data.fun != NULL ) {
			menu[ms->current].data.fun();
			libMU_Menu_Draw( ms );
		}
		return;
	}
	if ( menu[ms->current].type == MENU_ITEM_SUBMENU ) {
		/* Search for first entry of the new submenu */
		int i, new_menu = menu[ms->current].data.menu_id;
		for( i = 0; i < MENU_MAX_ENTRIES; i++ ) {
			if( menu[i].menu_id == MENU_ID_END ) break;
			if( menu[i].type == MENU_ITEM_INVALID ) break;
			if( menu[i].menu_id == new_menu ) {
				ms->current = i;
				libMU_Menu_Draw( ms );
				break;
			}
		}
	}
}

/**
 * @}
 */
