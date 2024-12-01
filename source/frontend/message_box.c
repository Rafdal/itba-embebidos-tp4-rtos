#include "message_box.h"

#include "list_menu.h"

menu_base_t* message_box(const char *message, void* on_click, menu_event_type_t type)
{
    menu_base_t *list_menu = list_menu_create(1, "Message Box", MENU_EXIT_ACTION_JUST_EXIT);
    if (list_menu != NULL)
    {
        list_menu_set_option(list_menu, 0, message, on_click, type);
    }
    return list_menu;
}

void message_box_destroy(menu_base_t *menu)
{
    list_menu_destroy(menu);
}