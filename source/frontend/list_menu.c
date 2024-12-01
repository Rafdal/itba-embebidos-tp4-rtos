#include "list_menu.h"
#include <string.h>

static menu_event_t list_menu_run(void* base);

static void list_menu_render(char* buffer, void* base);

static void list_menu_open(void* child);

// Inicializar un menu (retorna NULL en caso de error)
menu_base_t* list_menu_create(uint8_t options, char* title, menu_exit_action_t action){
    if (options == 0)
        return NULL;

    menu_base_t *base = menu_base_create(title, action);
    if (base == NULL)
        return NULL;

    list_menu_t *list_menu = (list_menu_t*) malloc(sizeof(list_menu_t)); // Asignar memoria a la estructura
    if (list_menu == NULL)
    {
        menu_base_destroy (base);
        return NULL;
    }

    // Asignar memoria a los arreglos de informacion de las opciones
    list_menu->option_titles = (char**) calloc(options, sizeof(char*));
    list_menu->option_data = (void**) calloc(options, sizeof(void*));
    list_menu->option_type = (uint8_t*) calloc(options, sizeof(uint8_t));

    // Si ocurre un error liberar todo
    if(list_menu->option_titles == NULL || list_menu->option_data == NULL || list_menu->option_type == NULL){
        free(list_menu->option_titles);
        free(list_menu->option_data);
        free(list_menu->option_type);
        free(list_menu);
        menu_base_destroy(base);
        list_menu = NULL; // (error)
        base = NULL; // (error)
    }else{
        // Asignar los datos
        list_menu->current_option = 0;
        list_menu->n_options = options;
        list_menu->on_custom_event = NULL;
        
        base->child = list_menu;
        base->run_child = list_menu_run;
        base->render_display = list_menu_render;
        base->open_menu = list_menu_open;
    }
    return base;
}

void list_menu_on_custom_event(menu_base_t *menu, void (*callback)(void*)){
    if(menu != NULL && menu->child != NULL)
    {
        list_menu_t *list_menu = (list_menu_t*) menu->child;
        list_menu->on_custom_event = callback;
    }
}

void list_menu_set_option(menu_base_t *menu, uint8_t option_id, char* title, void* option, menu_event_type_t type){
    if(menu != NULL && menu->child != NULL)
    {
        list_menu_t *list_menu = (list_menu_t*) menu->child;
        if(option_id < list_menu->n_options)
        {
            list_menu->option_titles[option_id] = title;
            list_menu->option_data[option_id] = option;
            list_menu->option_type[option_id] = type;
        }
    }
}

void list_menu_destroy(menu_base_t *menu){ // Liberar memoria de un menu lista
    if(menu != NULL && menu->child != NULL)
    {
        list_menu_t *list_menu = (list_menu_t*) menu->child;
        free(list_menu->option_titles);
        free(list_menu->option_data);
        free(list_menu->option_type);
        free(list_menu);
        menu_base_destroy(menu);
    }
}

static void list_menu_open(void* child){
    list_menu_t *list_menu = (list_menu_t*) child;
    list_menu->current_option = 0;
}

// ABRIR Y EJECUTAR UN MENU
static menu_event_t list_menu_run(void* base_ptr){
    menu_base_t *base = (menu_base_t*) base_ptr;
    list_menu_t *list_menu = (list_menu_t*) base->child;

    menu_event_t event = {MENU_EVENT_TYPE_NONE, NULL};

    if (base->state != MENU_STATE_CLOSE) // Mientras el menu no se haya cerrado o este cerrado
    {
        switch (base->state)
        {
        case MENU_STATE_IDLE: // Menu activo en espera
            /* do nothing */
            break;

        case MENU_STATE_DOWN: // Ir a la opcion inferior
            if(list_menu->current_option < list_menu->n_options-1)
                list_menu->current_option++;
            break;

        case MENU_STATE_UP: // Ir a la opcion superior
            if(list_menu->current_option >0)
                list_menu->current_option--;
            break;

        case MENU_STATE_SELECT: // Seleccionar la opcion actual
            {
                event.type = list_menu->option_type[list_menu->current_option];
                event.data = list_menu->option_data[list_menu->current_option];
            }
            break;

        case MENU_STATE_BACK: // Ir hacia atras
            base->state = MENU_STATE_CLOSE; // Cerrar el menu. La accion de salida la gestiona la clase base
            break;

        case MENU_STATE_CUSTOM: // Estado personalizado
            if(list_menu->on_custom_event != NULL)
                list_menu->on_custom_event(base_ptr);
            break;

        default:
            // printf("Menu Event Error\n");
            break;
        }
    }
    return event;
}

static void list_menu_render(char* buffer, void* base){
    menu_base_t *menu = (menu_base_t*) base;
    list_menu_t *list_menu = (list_menu_t*) menu->child;
    strcpy(buffer, list_menu->option_titles[list_menu->current_option]);
}

void list_menu_go_to_option(menu_base_t* base, uint8_t option_id)
{
    if (base != NULL && base->child != NULL)
    {
        list_menu_t *list_menu = (list_menu_t*) base->child;
        if (option_id < list_menu->n_options)
        {
            list_menu->current_option = option_id;
        }
        else
        {
            list_menu->current_option = option_id % list_menu->n_options;
        }
    }
}


// menu_base_t* menu_get_current(void)
// {
//     return menu_call_stack_current(call_stack);
// }



// bool menu_is_current_available(void){
//     return menu_is_available(menu_call_stack_current(call_stack));
// }

// void list_menu_force_close(menu_base_t *menu){
//     if(menu != NULL)
//     {
//         menu->state = MENU_STATE_CLOSE;
//     }
// }

// void list_menu_force_close_current(void){
//     menu_force_close(menu_call_stack_current(call_stack));
// }
