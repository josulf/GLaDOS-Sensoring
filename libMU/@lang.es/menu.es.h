/**
 * @addtogroup	libMU_Menu
 * Librería para el manejo de menús
 * @{
 ********************************************************************
 * @author		Eñaut Muxika <emuxika at mondragon dot edu>
 * @date		2011/7/10
 * @copyright	BSDL
 ********************************************************************
 */
#ifndef LIBMU_MENU_ES_H_
#define LIBMU_MENU_ES_H_
#include <libMU/menu.h>

/**
 * Constantes de la geometria y el tamaño del menú
 */
enum _libMU_Menu_Constantes_t {
	MENU_NUMERO_MAX_ENTRADAS= MENU_MAX_ENTRIES,
	MENU_ELEM_ALTURA		= MENU_ITEM_HEIGHT,
	MENU_ELEM_ALTURA_SEP	= MENU_ITEM_HEIGHT_SEP,
	MENU_ID_PRINCIPAL		= MENU_ID_MAIN,
	MENU_ID_FIN				= MENU_ID_END
};

/**
 * Acciones del menú
 */
typedef	enum _libMU_Menu_Acciones_t {
	MENU_NADA	= MENU_NOTHING,	/* No hacer nada */
	MENU_ARRIBA = MENU_UP,		/* Seleccionar la entrada anterior del menú */
	MENU_ABAJO	= MENU_DOWN,	/* Seleccionar la entrada siguiente del menú */
	MENU_SELEC	= MENU_SEL,		/* Ejecutar función de la entrada actual o ir a submenú */
	MENU_ATRAS	= MENU_BACK,	/* Ir al menú previo */
	MENU_DIBUJAR= MENU_DRAW,	/* Redibujar el menú completo */
} libMU_Menu_Acciones_t;

/**
 * Definición de tipos de elementos del menú
 */
typedef enum _libMU_Menu_TipoElemento_t {
	MENU_ELEM_INVALIDO	= MENU_ITEM_INVALID,	/* Elemento de menú inválida */
	MENU_ELEM_SEPARADOR	= MENU_ITEM_SEPARATOR,	/* Separador (elemento vacia) */
	MENU_ELEM			= MENU_ITEM,			/* Elemento normal */
	MENU_ELEM_SUBMENU	= MENU_ITEM_SUBMENU,	/* Elemento para submenú */
} libMU_Menu_TipoElemento_t;

/**
 * Tipo de datos para guardar la función de menú o el identificador de submenú
 */
typedef union _libMU_Menu_DatosElemento_t {
	void (*fun)(void);	/* Función a llamar al seleccionar el elemento */
	int		menu_id;	/* Identificador de submenú */
} libMU_Menu_DatosElemento_t;

/**
 * Tipo de datos para guardar la información del menú
 * @note
 * Las especificaciones de menus TIENEN que estar ordenadas por menu_id
 */
struct _libMU_Menu_Elemento_t {
	int							menu_id;	/* Identificacion del menú/submenú */
											/* 0 - Menú principal */
											/* 1,2, ... - Submenus */
	libMU_Menu_TipoElemento_t	type;		/* Tipo del elemento del menú */
	const char*					text;		/* Texto del elemento del menú */
	libMU_Menu_DatosElemento_t	data;		/* Función o submenú del elemento del menú */
};
typedef libMU_Menu_Item_t libMU_Menu_Elemento_t;

/**
 * Tipo de datos para guardar el estado del menu
 */
struct _libMU_Menu_Estado_t {
	const libMU_Menu_Elemento_t*	menu;
	unsigned int					num_entries;
	unsigned int					current;
	unsigned int					cur_pos;
};
typedef libMU_Menu_Status_t libMU_Menu_Estado_t;

/**
 * Inicializa el estado del menú
 * @param	menu_estado	Puntero a los datos de estado del menú
 * @param	menu		Puntero a los elementos del del menú
 * @note
 * El puntero a los elementos del menú tiene que contener el menú y todos los submenús
 */
static inline void	libMU_Menu_Inicializa( libMU_Menu_Estado_t* menu_estado,
										   const libMU_Menu_Elemento_t* menu )
{
	libMU_Menu_Initialize(menu_estado, menu);
}

/**
 * Maneja el menú
 * @param	ms		Puntero al estado del menú
 * @param	action	Acción a realizar
 * @see		libMU_Menu_Acciones_t, libMU_Menu_Estado_t, libMU_Menu_Inicializa
 */
static inline void libMU_Menu_Maneja( libMU_Menu_Estado_t* ms, libMU_Menu_Acciones_t accion )
{
	libMU_Menu_Manage(ms,accion);
}

#endif /*LIBMU_MENU_ES_H_*/
/**
 * @}
 */
