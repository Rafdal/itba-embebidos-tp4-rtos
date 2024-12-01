#ifndef INPUT_MENU_H
#define INPUT_MENU_H

#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#include "menu_base.h"

typedef enum INPUT_MENU_TYPE {
    INPUT_MENU_TYPE_NUM,
    INPUT_MENU_TYPE_PASSWORD,
    INPUT_MENU_TYPE_TEXT,
} input_menu_type_t;

typedef struct NUM_MENU {
    uint8_t max_len;
    uint8_t cursor;
    uint8_t option_index;
    uint8_t win_offset;
    uint8_t win_size;
    uint8_t mode;
    input_menu_type_t type;
    void (*on_data_input)(char*);
    void (*on_open)(void*);
    char *data;
} input_menu_t;

#ifndef DISPLAY_SIZE
#define DISPLAY_SIZE 4
#endif

menu_base_t* input_menu_create(char *title, uint8_t max_len, input_menu_type_t type, void (*on_data_input)(char*));
void input_menu_destroy(menu_base_t *menu);

void input_menu_on_open(menu_base_t *menu, void (*on_open)(void*));

void input_menu_set_value(menu_base_t *menu, char* source);
void input_menu_get_value(menu_base_t *menu, char* dest);

#endif // INPUT_MENU_H
