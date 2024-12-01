#ifndef MESSAGE_BOX_H
#define MESSAGE_BOX_H

#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#include "menu_base.h"

menu_base_t* message_box(const char *message, void* on_click, menu_event_type_t type);

#endif // MESSAGE_BOX_H