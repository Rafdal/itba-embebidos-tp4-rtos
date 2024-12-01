#include "menu_base.h"


menu_base_t* menu_base_create(char* title, menu_exit_action_t action)
{
    menu_base_t* menu_base = (menu_base_t*)malloc(sizeof(menu_base_t));

    if (menu_base != NULL)
    {
        menu_base->title = title;
        menu_base->state = MENU_STATE_IDLE;
        menu_base->child = NULL;
        menu_base->data = NULL;
        menu_base->render_display = NULL;
        menu_base->open_menu = NULL;
        menu_base->slide = true; // default slide
        menu_base->cursor = 0;

        menu_base->exit_action = action;
        menu_base->on_exit_callback = NULL;
        menu_base->run_child = NULL;
    }
    else
    {
        free(menu_base);
        menu_base = NULL;
    }
    return menu_base;
}

void menu_base_render(char* buffer, menu_base_t* menu)
{
    if (menu != NULL)
    {
        if (menu->render_display != NULL)
            menu->render_display(buffer, menu);
    }
}

void menu_base_open(menu_base_t* menu)
{
    if (menu != NULL)
    {
        if (menu->open_menu != NULL)
            menu->open_menu(menu->child);
    }
}

menu_event_t menu_base_run(menu_base_t* menu)
{
    menu_event_t event = {MENU_EVENT_TYPE_NONE, NULL};
    if (menu != NULL)
    {
        if (menu->run_child != NULL)
            event = menu->run_child(menu);      

        switch (menu->state)
        {
        case MENU_STATE_CLOSE:
            {
                switch (menu->exit_action)
                {
                case MENU_EXIT_ACTION_CALLBACK:
                    if (menu->on_exit_callback != NULL)
                        menu->on_exit_callback();
                    break;

                case MENU_EXIT_ACTION_DONT_CLOSE:
                    menu->state = MENU_STATE_IDLE;
                    break; // do nothing

                case MENU_EXIT_ACTION_JUST_EXIT:
                    event.type = MENU_EVENT_TYPE_CLOSE;
                    break;
                default:
                    menu->state = MENU_STATE_IDLE;
                    break;
                }
            }
            break;
        case MENU_STATE_IDLE:
        case MENU_STATE_UP:
        case MENU_STATE_DOWN:
        case MENU_STATE_BACK:
        case MENU_STATE_SELECT:
        case MENU_STATE_CUSTOM:
        default:
            break;
        }

        if (menu->state != MENU_STATE_IDLE && menu->state != MENU_STATE_CLOSE)
        {
            menu->state = MENU_STATE_IDLE;
        }
    }
    return event;
}

void menu_base_destroy(menu_base_t* menu)
{
    if (menu != NULL)
    {
        free(menu);
    }
}
