#ifndef __ENCODER_H__
#define __ENCODER_H__

#include "gpio.h"
#include <stdbool.h>
#include <stdint.h>
#include <os.h>

#define ENC_LONG_CLICK_TIME 1500

typedef enum {
    ENC_NO_EVENT,
    ENC_MOVED_CW,
    ENC_MOVED_CCW,
    ENC_CLICK,
    ENC_LONG_CLICK,
} encoder_event_type_t;

typedef struct encoder_event
{
    bool click;     // True if the encoder was clicked
    bool long_click; // True if the encoder was long clicked (more than 1 second)
    bool moved;     // True if the encoder was moved (position changed)
    bool moved_clockwise; // True if the encoder was moved clockwise
    bool moved_counter_clockwise; // True if the encoder was moved counter clockwise
    encoder_event_type_t type; // Type of the event
    uint32_t pos;    // Position of the encoder
} encoder_event_t;


void encoder_init(pin_t pinA, pin_t pinB, pin_t pinPush);
OS_SEM* encoder_sem_ptr(void);
void encoder_set_pos(uint32_t pos);
bool encoder_get_event(encoder_event_t* ev); // Returns true if there is a new event

#endif // __ENCODER_H__
