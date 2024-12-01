#ifndef _MENU_H_
#define _MENU_H_

#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#include "menu_base.h"

/********************************************************************************
 *                              LIBERIA MENU                                    *
 * Esta libreria permite crear diferentes menues y poder controlarlos           *
 * facilmente, navegar entre sus opciones, crear sub-menues y estructuras       *
 * complejas de menues dentro de otros menues con total libertad                *
 *                                                                              *
 * Tambien permite acceder con facilidad al los datos del menu actual para poder*
 * mostrarlos por pantalla (lista de opciones, opcion seleccionada, titulo, etc)*
 *                                                                              *
 * Esta libreria NO cumple funciones de frontend, el programador decide como    *
 * conectar esta libreria con el frontend y que interfaz usara                  *
 ********************************************************************************
 */

// Estructura de datos del menu
typedef struct LIST_MENU{
    char **option_titles;   // titulos de cada opcion
    void **option_data;     // datos adicionales de cada opcion (para guardar punteros a submenues, etc)
    uint8_t *option_type; // indica si la opcion es un submenu o un callback
    uint8_t current_option; // opcion actual
    uint8_t n_options;      // cantidad de opciones
} list_menu_t; // Objeto Menu Lista


// (1) FUNCIONES DE CREACION Y CONFIGURACION DE MENUES
menu_base_t* list_menu_create(uint8_t options, char* title, menu_exit_action_t action);


void list_menu_destroy(menu_base_t *menu); 


void list_menu_set_option(menu_base_t *menu, uint8_t option_id, char* title, void* option, menu_event_type_t type);
// DESCRIPCION Y PARAMETROS: Configurar una opcion de un menu
// - [menu]:        puntero a menu_t (objeto menu)
// - [option_id]:   numero de opcion que desea configurar (entre 0 y la cant. de opciones que tiene dicho menu)
// - [title]:       titulo de la opcion (c-string)

// (3) FUNCIONES PARA NAVEGAR EN EL MENU (colocar en un callback asociado a un evento de control. ej: Teclado, Joystick)
void list_menu_go_to_option(menu_base_t* menu, uint8_t option_id); // Ir a una opcion especifica

// (4) FUNCIONES PARA MANIPULAR EL MENU EN EJECUCION ACTUAL
// bool menu_is_current_available(void);      // Retorna true si el menu actual esta activo
// void menu_force_close_current(void);       // fuerza el cierre del menu actual
// menu_t* menu_get_current(void);   // Retorna una estructura con datos del menu actual (para mostrar en una UI, etc)

#endif // _MENU_H_
