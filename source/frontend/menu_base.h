#ifndef MENU_BASE_H
#define MENU_BASE_H

#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

// Estados del menu
typedef enum MENU_STATE {
    MENU_STATE_IDLE,    // Menu activo en espera
    MENU_STATE_CLOSE,   // Cerrar menu | menu cerrado
    MENU_STATE_UP,      // Ir a la opcion superior (estado de transicion)
    MENU_STATE_DOWN,    // Ir a la opcion inferior (estado de transicion)
    MENU_STATE_BACK,    // Ir hacia atras (estado de transicion)
    MENU_STATE_SELECT,  // Seleccionar opcion (estado de transicion)
    MENU_STATE_CUSTOM,  // Estado personalizado (para menus con comportamiento especial)
    MENU_STATE_DESTROY, // Destruir menu
} menu_state_t;

// Acciones del menu al ejecutar menu_go_back()
typedef enum MENU_EXIT_ACTION {
    MENU_EXIT_ACTION_DONT_CLOSE, // No hacer nada al ir hacia atras (no salir)
    MENU_EXIT_ACTION_JUST_EXIT,  // Salir del menu al ir hacia atras
    MENU_EXIT_ACTION_CALLBACK,   // Ejecutar un callback al ir hacia atras
    MENU_EXIT_ACTION_DESTROY,    // Destruir el menu al ir hacia atras
} menu_exit_action_t;

typedef void (*void_callback_t)(void); // Callback de una opcion de menu

typedef enum MENU_EVENT_TYPE {
    MENU_EVENT_TYPE_NONE,
    MENU_EVENT_TYPE_CLOSE,
    MENU_EVENT_TYPE_OPEN_MENU,
    MENU_EVENT_TYPE_CALLBACK,
    MENU_EVENT_TYPE_CUSTOM,
    MENU_EVENT_TYPE_DESTROY,
} menu_event_type_t;

typedef struct MENU_EVENT {
    menu_event_type_t type;
    void* data;
} menu_event_t;

// Estructura de datos del menu
typedef struct {
    char *title;                                // Titulo del menu
    menu_state_t state;                         // evento (estado) actual

    menu_exit_action_t exit_action;             // accion de salida
    void_callback_t on_exit_callback;           // callback de salida

    bool slide;                                 // desplazamiento de menu
    uint8_t cursor;                             // cursor del menu

    void (*open_menu)(void*);                   // funcion para abrir el menu
    void (*render_display)(char*, void*);       // funcion para renderizar el menu
    void *child;                                // clase aniadida al menu
    void *data;                                 // datos adicionales del menu
    menu_event_t (*run_child)(void*);           // funcion para ejecutar procesos de la clase aniadida
} menu_base_t; // Objeto Menu

void menu_base_render(char* buffer, menu_base_t* menu);

menu_base_t* menu_base_create(char* title, menu_exit_action_t action);

void menu_base_open(menu_base_t* menu); // Abre el menu, setea todos los valores a los iniciales

menu_event_t menu_base_run(menu_base_t* menu);

void menu_base_destroy(menu_base_t* menu);

#endif // MENU_BASE_H
