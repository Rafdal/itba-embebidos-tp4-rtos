#include "input_menu.h"
#include "menu_base.h"
#include <string.h>

static menu_event_t input_menu_run(void* base);
static void input_menu_render(char* buffer, void* base);
static void input_menu_open(void* child);

enum INPUT_MENU_OPTION {
    INPUT_MENU_OPTION_CONFIRM,
    INPUT_MENU_OPTION_CANCEL,
    INPUT_MENU_OPTION_EDIT,
};

static const char confirm_text[] = "Accept";
static const char cancel_text[] = "Cancel";
static const char edit_text[] = "Edit";

static const char* const input_menu_option_titles[] = {confirm_text, cancel_text, edit_text};

typedef enum INPUT_MENU_MODE {
    INPUT_MENU_MODE_NAVIGATION, // Navegar entre los caracteres
    INPUT_MENU_MODE_EDIT_CHAR,  // Editar un caracter
    INPUT_MENU_MODE_CONFIRMATION, // Confirmar el valor
} input_menu_mode_t;

static void input_menu_increment(input_menu_t *input_menu);
static void input_menu_decrement(input_menu_t *input_menu);

// Inicializar un menu (retorna NULL en caso de error)
menu_base_t* input_menu_create(char* title, uint8_t max_len, input_menu_type_t type, void (*on_data_input)(char*)){
    menu_base_t *base = menu_base_create(title, MENU_EXIT_ACTION_JUST_EXIT);
    if (base == NULL)
        return NULL;

    input_menu_t *input_menu = (input_menu_t*) malloc(sizeof(input_menu_t)); // Asignar memoria a la estructura
    if (input_menu == NULL)
    {
        menu_base_destroy (base);
        return NULL;
    }

    input_menu->max_len = max_len;
    input_menu->cursor = 0;
    input_menu->win_offset = 0;
    input_menu->win_size = DISPLAY_SIZE;
    input_menu->option_index = 0;
    input_menu->mode = INPUT_MENU_MODE_NAVIGATION;
    input_menu->type = type;
    input_menu->data = (char*) malloc(max_len * sizeof(char));
    input_menu->on_data_input = on_data_input;
    input_menu->on_open = NULL;

    if(input_menu->data == NULL){
        free(input_menu);
        menu_base_destroy(base);
        input_menu = NULL; // (error)
        base = NULL; // (error)
    }else{
        for (uint8_t i = 0; i < max_len; i++)
        {
            input_menu->data[i] = '_';
        }

        base->child = input_menu;
        base->run_child = input_menu_run;
        base->render_display = input_menu_render;
        base->open_menu = input_menu_open;
        base->slide = false;
    }
    return base;
}

void input_menu_on_open(menu_base_t *menu, void (*on_open)(void*)){
    if(menu != NULL && menu->child != NULL){
        input_menu_t *input_menu = (input_menu_t*) menu->child;
        input_menu->on_open = on_open;
    }
}

void input_menu_destroy(menu_base_t *menu){
    if(menu != NULL && menu->child != NULL){
        input_menu_t *input_menu = (input_menu_t*) menu->child;
        free(input_menu->data);
        free(input_menu);
        menu_base_destroy(menu);
    }
}

static void input_menu_open(void* child){
    input_menu_t *input_menu = (input_menu_t*) child;
    input_menu->cursor = 0;
    input_menu->win_offset = 0;
    input_menu->mode = INPUT_MENU_MODE_NAVIGATION;
    if (input_menu->on_open == NULL)
    {
        for (uint8_t i = 0; i < input_menu->max_len; i++)
        {
            input_menu->data[i] = '_';
        }
    }
    else
    {
        input_menu->on_open(child);
    }
}

void input_menu_clear(menu_base_t *menu){
    if(menu != NULL && menu->child != NULL){
        input_menu_t *input_menu = (input_menu_t*) menu->child;
        for (uint8_t i = 0; i < input_menu->max_len; i++)
        {
            input_menu->data[i] = '_';
        }
        input_menu->cursor = 0;
        input_menu->win_offset = 0;
    }
}

void input_menu_set_value(menu_base_t *menu, char* source){
    if(menu != NULL && menu->child != NULL){
        input_menu_t *input_menu = (input_menu_t*) menu->child;
        uint8_t len = strlen(source);
        if(len > input_menu->max_len)
            len = input_menu->max_len;
        uint8_t i;
        for (i = 0; i < len; i++)
        {
            input_menu->data[i] = source[i];
        }
        for (; i < input_menu->max_len; i++)
        {
            input_menu->data[i] = '_';
        }
        input_menu->cursor = 0;
        input_menu->win_offset = 0;
    }
}

void input_menu_get_value(menu_base_t *menu, char* dest){
    if(menu != NULL && menu->child != NULL){
        input_menu_t *input_menu = (input_menu_t*) menu->child;
        for (uint8_t i = 0; i < input_menu->max_len; i++)
        {
            dest[i] = input_menu->data[i];
        }
        dest[input_menu->max_len] = '\0';
    }
}

static void input_menu_render(char* buffer, void* base){
    menu_base_t *base_menu = (menu_base_t*) base;
    input_menu_t *input_menu = (input_menu_t*) base_menu->child;
    if (input_menu->mode == INPUT_MENU_MODE_CONFIRMATION)
    {
        strcpy(buffer, input_menu_option_titles[input_menu->option_index]);
        base_menu->cursor = 0;
        base_menu->slide = true;
    }
    else
    {
        for (uint8_t i = 0; i < input_menu->win_size; i++)
        {
            char c = input_menu->data[i + input_menu->win_offset];
            if (c == '_')
                buffer[i] = '_';
            else
            {
                if (input_menu->type == INPUT_MENU_TYPE_PASSWORD && (input_menu->cursor != i + input_menu->win_offset))
                {
                    buffer[i] = '-';
                }
                else
                {
                    buffer[i] = c;
                }
            }
        }
        buffer[input_menu->win_size] = '\0';
        base_menu->cursor = input_menu->cursor - input_menu->win_offset;
        base_menu->slide = false;
    }
}

static menu_event_t input_menu_run(void* base_ptr)
{
    menu_base_t *base = (menu_base_t*) base_ptr;
    input_menu_t *input_menu = (input_menu_t*) base->child;

    menu_event_t event = {MENU_EVENT_TYPE_NONE, NULL};

    if (base->state != MENU_STATE_CLOSE)
    {
        switch (base->state)
        {
            case MENU_STATE_IDLE:
                break;
            case MENU_STATE_DOWN:
                input_menu_increment(input_menu);
                break;

            case MENU_STATE_UP:
                input_menu_decrement(input_menu);
                break;

            case MENU_STATE_SELECT:
                if (input_menu->mode == INPUT_MENU_MODE_CONFIRMATION)
                {
                    switch (input_menu->option_index)
                    {
                        case INPUT_MENU_OPTION_CONFIRM:
                            event.type = MENU_EVENT_TYPE_CLOSE;
                            if (input_menu->on_data_input != NULL)
                                input_menu->on_data_input(input_menu->data);
                            break;
                        case INPUT_MENU_OPTION_CANCEL:
                            event.type = MENU_EVENT_TYPE_CLOSE;
                            break;
                        case INPUT_MENU_OPTION_EDIT:
                            input_menu->mode = INPUT_MENU_MODE_NAVIGATION;
                            break;
                        default:
                            break;
                    }
                }
                else if (input_menu->mode == INPUT_MENU_MODE_NAVIGATION)
                {
                    input_menu->mode = INPUT_MENU_MODE_EDIT_CHAR;
                }
                else if (input_menu->mode == INPUT_MENU_MODE_EDIT_CHAR)
                {
                    input_menu->mode = INPUT_MENU_MODE_NAVIGATION;
                }
                break;

            case MENU_STATE_BACK: // Long press -> ask for confirmation
                base->state = MENU_STATE_IDLE;
                input_menu->mode = INPUT_MENU_MODE_CONFIRMATION;
                input_menu->option_index = 0;
                break;

            case MENU_STATE_CUSTOM:
                base->state = MENU_STATE_IDLE;
                input_menu->mode = INPUT_MENU_MODE_NAVIGATION;
                input_menu_set_value(base, (char*)base->data);
                break;

            default:
                break;

        }
    }
    return event;
}


static void input_menu_increment(input_menu_t *input_menu)
{
    if (input_menu->mode == INPUT_MENU_MODE_CONFIRMATION)
    {
        if (input_menu->option_index < 2)
            input_menu->option_index++;
    }
    else if (input_menu->mode == INPUT_MENU_MODE_NAVIGATION)
    {
        if (input_menu->cursor < input_menu->max_len - 1)
        {
            input_menu->cursor++;
            if (input_menu->cursor - input_menu->win_offset >= input_menu->win_size)
            {
                input_menu->win_offset = input_menu->cursor - input_menu->win_size + 1;
            }
        }
    }
    else if (input_menu->mode == INPUT_MENU_MODE_EDIT_CHAR)
    {
        switch (input_menu->type)
        {
            case INPUT_MENU_TYPE_PASSWORD:
            case INPUT_MENU_TYPE_NUM:
                if (input_menu->data[input_menu->cursor] == '_')
                    input_menu->data[input_menu->cursor] = '0';
                else if (input_menu->data[input_menu->cursor] == '9')
                    input_menu->data[input_menu->cursor] = '_';
                else
                    input_menu->data[input_menu->cursor]++;
                break;
            case INPUT_MENU_TYPE_TEXT:
                if (input_menu->data[input_menu->cursor] == '_')
                    input_menu->data[input_menu->cursor] = 'A';
                else if (input_menu->data[input_menu->cursor] == 'Z')
                    input_menu->data[input_menu->cursor] = '_';
                else
                    input_menu->data[input_menu->cursor]++;
                break;
            default:
                break;

        }
    }
}


static void input_menu_decrement(input_menu_t *input_menu)
{
    if (input_menu->mode == INPUT_MENU_MODE_CONFIRMATION)
    {
        if (input_menu->option_index > 0)
            input_menu->option_index--;
    }
    else if (input_menu->mode == INPUT_MENU_MODE_NAVIGATION)
    {
        if (input_menu->cursor > 0)
        {
            input_menu->cursor--;
            if (input_menu->cursor < input_menu->win_offset)
            {
                input_menu->win_offset = input_menu->cursor;
            }
        }
    }
    else if (input_menu->mode == INPUT_MENU_MODE_EDIT_CHAR)
    {
        switch (input_menu->type)
        {
            case INPUT_MENU_TYPE_PASSWORD:
            case INPUT_MENU_TYPE_NUM:
                if (input_menu->data[input_menu->cursor] == '_')
                    input_menu->data[input_menu->cursor] = '9';
                else if (input_menu->data[input_menu->cursor] == '0')
                    input_menu->data[input_menu->cursor] = '_';
                else
                    input_menu->data[input_menu->cursor]--;
                break;
            case INPUT_MENU_TYPE_TEXT:
                if (input_menu->data[input_menu->cursor] == '_')
                    input_menu->data[input_menu->cursor] = 'Z';
                else if (input_menu->data[input_menu->cursor] == 'A')
                    input_menu->data[input_menu->cursor] = '_';
                else
                    input_menu->data[input_menu->cursor]--;
                break;
            default:
                break;

        }
    }
}