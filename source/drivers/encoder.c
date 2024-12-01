#include "encoder.h"
#include <os.h>

static bool btnA = true;
static bool lastBtnA = true;
static bool btnB = true;
static bool lastBtnB = true;
static bool btnPush = false;
static bool lastBtnPush = false;

static pin_t encPinA;
static pin_t encPinB;
static pin_t encPinPush;

static uint16_t pressed_time = 0;

static encoder_event_t encoder_event;

static bool unread_event = false;
static bool newEvent = false;

static uint8_t irq_activation_delay = 0;
static bool irq_active = false;

static OS_TMR tmr1;
static OS_ERR os_err;

static OS_SEM enc_sem;

#define TICK_TIME 5 // ms
#define PERIODIC_CALLBACK_TICKS 2
#define READ_TIME (TICK_TIME * PERIODIC_CALLBACK_TICKS)

static void enc_periodic_callback(void *p_tmr, void *p_arg)
{
    (void)p_tmr;
    (void)p_arg;

    if (irq_activation_delay > 0)
    {
        irq_activation_delay--;
    }
    else
    {
        btnPush = !gpioRead(encPinPush);
        btnA = gpioRead(encPinA);
        btnB = gpioRead(encPinB);
    }

    if (!btnA && lastBtnA && btnB)
    {
        encoder_event.pos++;
        encoder_event.moved = true;
        encoder_event.moved_clockwise = true;
        encoder_event.type = ENC_MOVED_CW;
        newEvent = true;
    }
    if (!btnB && lastBtnB && btnA)
    {
        encoder_event.pos--;
        encoder_event.moved = true;
        encoder_event.moved_counter_clockwise = true;
        encoder_event.type = ENC_MOVED_CCW;
        newEvent = true;
    }

    if (btnPush && !lastBtnPush)    // Flanco de subida
    {
        pressed_time = 0;
    }
    else if (btnPush && lastBtnPush && pressed_time < ENC_LONG_CLICK_TIME / READ_TIME)     // Boton presionado
    {
        pressed_time++;
    }
    else if (btnPush && lastBtnPush && pressed_time == ENC_LONG_CLICK_TIME / READ_TIME)    // Long click
    {
        pressed_time++;
        encoder_event.long_click = true;
        encoder_event.type = ENC_LONG_CLICK;
        newEvent = true;
    }
    else if (!btnPush && lastBtnPush && pressed_time < ENC_LONG_CLICK_TIME / READ_TIME)    // Flanco de bajada
    {
        encoder_event.click = true;
        encoder_event.type = ENC_CLICK;
        newEvent = true;
    }

    if (newEvent)
    {
        unread_event = true;
        OSSemPost(&enc_sem, OS_OPT_POST_1, &os_err);
        newEvent = false;
    }

    lastBtnA = btnA;
    lastBtnB = btnB;
    lastBtnPush = btnPush;

    if (!irq_active && irq_activation_delay == 0)
    {
        irq_active = true;
        // turn on interrupts
        gpioIRQEnable(encPinA, GPIO_IRQ_MODE_FALLING_EDGE);
        gpioIRQEnable(encPinB, GPIO_IRQ_MODE_FALLING_EDGE);
        gpioIRQEnable(encPinPush, GPIO_IRQ_MODE_FALLING_EDGE);
    }
}

static void encoder_read_irq(void)
{
    if (irq_activation_delay == 0)
    {
        irq_activation_delay = 2;
        btnPush = !gpioRead(encPinPush);
        btnA = gpioRead(encPinA);
        btnB = gpioRead(encPinB);

        gpioIRQDisable(encPinA);
        gpioIRQDisable(encPinB);
        gpioIRQDisable(encPinPush);
        irq_active = false;
    }
}

void encoder_init(pin_t pinA, pin_t pinB, pin_t pinPush)
{
    encPinA = pinA;
    encPinB = pinB;
    encPinPush = pinPush;
    gpioMode(pinA, INPUT_PULLUP);
    gpioMode(pinB, INPUT_PULLUP);
    gpioMode(pinPush, INPUT_PULLUP);

    gpioIRQ(pinA, GPIO_IRQ_MODE_FALLING_EDGE, encoder_read_irq);
    gpioIRQ(pinB, GPIO_IRQ_MODE_FALLING_EDGE, encoder_read_irq);
    gpioIRQ(pinPush, GPIO_IRQ_MODE_FALLING_EDGE, encoder_read_irq);
    irq_active = true;

    OSTmrCreate(&tmr1, "Enc Timer", 50, PERIODIC_CALLBACK_TICKS, OS_OPT_TMR_PERIODIC, enc_periodic_callback, 0, &os_err);
    OSTmrStart(&tmr1, &os_err);

    OSSemCreate(&enc_sem, "Encoder Sem", 0u, &os_err);

    encoder_event.pos = 0;
    encoder_event.click = false;
    encoder_event.moved = false;
    encoder_event.long_click = false;
    encoder_event.moved_clockwise = false;
    encoder_event.moved_counter_clockwise = false;
    encoder_event.type = ENC_NO_EVENT;
}

OS_SEM* encoder_sem_ptr(void)
{
    return &enc_sem;
}

void encoder_set_pos(uint32_t pos)
{
    encoder_event.pos = pos;
}

bool encoder_get_event(encoder_event_t *ev)
{
    if (!unread_event)
        return false;
	ev->pos = encoder_event.pos;
	ev->click = encoder_event.click;
	ev->moved = encoder_event.moved;
	ev->long_click = encoder_event.long_click;
	ev->moved_clockwise = encoder_event.moved_clockwise;
	ev->moved_counter_clockwise = encoder_event.moved_counter_clockwise;
    ev->type = encoder_event.type;
	encoder_event.click = false;
	encoder_event.moved = false;
	encoder_event.long_click = false;
	encoder_event.moved_clockwise = false;
	encoder_event.moved_counter_clockwise = false;
    encoder_event.type = ENC_NO_EVENT;
    
    unread_event = false;
    return true;
}
