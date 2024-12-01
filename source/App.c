#include "App.h"

#include <os.h>
#include "drivers/gpio.h"
#include "config.h"
#include "drivers/encoder.h"
#include "serial/serial.h"
#include "frontend/menu_base.h"
#include "frontend/list_menu.h"
#include "frontend/call_stack.h"
#include "frontend/input_menu.h"
#include "frontend/message_box.h"

#include "backend/user_table.h"

// #include "frontend/message_box.h"
// #include "frontend/num_menu.h"

#include "drivers/uart.h"
#include "drivers/lector.h"
#include "drivers/display.h"

/* Task 3 */
#define TASK_MENU_STK_SIZE			1024u
#define TASK_MENU_STK_SIZE_LIMIT	(TASK_MENU_STK_SIZE / 2u)
#define TASK_MENU_PRIO              3u


static OS_TCB TaskMenuTCB;
static CPU_STK TaskMenuStk[TASK_MENU_STK_SIZE];

static OS_SEM semLectorAvailable;

static OS_PEND_DATA event_pend_data[2];

#define MENU_CALL_STACK_SIZE 8
static call_stack_t *call_stack = NULL;

static menu_base_t* user_not_found_msg = NULL;
static menu_base_t* door_open_msg = NULL;
static menu_base_t* access_denied_msg = NULL;

#define MAX_USERS 15
static char buffer[20];
static int id_input_data;
static int pass_input_data;
#define PASS_SIZE 4


void confirm_access_callback(void)
{
    index_t user_idx = user_table_find(id_input_data);
    if (user_idx < 0)
    {
        call_stack_pop(call_stack);
        call_stack_push(call_stack, user_not_found_msg);
        return;
    }
    user_data_t* user = user_table_get_user(user_idx);
    if (user != NULL && user->pin == pass_input_data)
    {
        call_stack_pop(call_stack);
        call_stack_push(call_stack, door_open_msg);
    }
    else
    {
        call_stack_pop(call_stack);
        call_stack_push(call_stack, access_denied_msg);
    }
}

void enter_id_callback(char* data)
{
    id_input_data = string_to_integer(data, 8);
}

void enter_pass_callback(char* data)
{
    pass_input_data = string_to_integer(data, PASS_SIZE);
}

void update_display(char* text, bool slide_enable, uint8_t cursor)
{
    dispBlank();
    dispEnableSlider(slide_enable);
    setCursor(0);
    setWinOffset(0);
    dispWord(text);
    if (slide_enable)
        disp_reset_slider();
    else
    	setCursor(cursor);
}


static void App_Menu_Task(void *p_arg) {
    (void)p_arg;
    OS_ERR os_err;

    char display_buffer[20];

    menu_base_t* enter_id = input_menu_create("ID Input", 8, INPUT_MENU_TYPE_NUM, enter_id_callback);
    menu_base_t* enter_pass = input_menu_create("Pass Input", PASS_SIZE, INPUT_MENU_TYPE_PASSWORD, enter_pass_callback);

    menu_base_t* access_panel = list_menu_create(4, "Access Panel", MENU_EXIT_ACTION_JUST_EXIT);
    list_menu_set_option(access_panel, 0, "0-Enter ID", enter_id, MENU_EVENT_TYPE_OPEN_MENU);
    list_menu_set_option(access_panel, 1, "1-Enter Pass", enter_pass, MENU_EVENT_TYPE_OPEN_MENU);
    list_menu_set_option(access_panel, 2, "2-Confirm", confirm_access_callback, MENU_EVENT_TYPE_CALLBACK);
    list_menu_set_option(access_panel, 3, "3-Cancel", NULL, MENU_EVENT_TYPE_CLOSE);

    menu_base_t* root_menu = list_menu_create(2, "Main Menu", MENU_EXIT_ACTION_DONT_CLOSE);
    list_menu_set_option(root_menu, 0, "Access", access_panel, MENU_EVENT_TYPE_OPEN_MENU);
    list_menu_set_option(root_menu, 1, "Setup Panel", NULL, MENU_EVENT_TYPE_NONE);

    call_stack = call_stack_init(MENU_CALL_STACK_SIZE, root_menu);
    menu_base_open(root_menu); // Abre el menu, setea todos los valores a los iniciales
    while (1) {
        menu_base_t* current_menu = (menu_base_t*)call_stack_current(call_stack);

        menu_base_render(display_buffer, current_menu);
        update_display(display_buffer, current_menu->slide, current_menu->cursor);

        OSPendMulti(event_pend_data, 2, 0, OS_OPT_PEND_BLOCKING, &os_err);
        if (event_pend_data[0].RdyObjPtr != NULL)
        {
            encoder_event_t ev;
            encoder_get_event(&ev);
            switch (ev.type)
            {
                case ENC_MOVED_CW:
                    current_menu->state = MENU_STATE_DOWN;
                    break;
                case ENC_MOVED_CCW:
                    current_menu->state = MENU_STATE_UP;
                    break;
                case ENC_CLICK:
                    current_menu->state = MENU_STATE_SELECT;
                    break;
                case ENC_LONG_CLICK:
                    current_menu->state = MENU_STATE_BACK;
                    break;
                default:
                    break;
            }
        }
        if (event_pend_data[1].RdyObjPtr != NULL)
        {
            int num = 98765432;
            integer_to_string(num, buffer, 8);
            current_menu->data = buffer;
            current_menu->state = MENU_STATE_CUSTOM;
        }

        menu_event_t menu_ev = menu_base_run(current_menu);
        switch (menu_ev.type)
        {
        case MENU_EVENT_TYPE_CLOSE:
            call_stack_pop(call_stack);
            break;
        case MENU_EVENT_TYPE_OPEN_MENU:
        	menu_base_open((menu_base_t*)menu_ev.data);
            call_stack_push(call_stack, menu_ev.data);
            break;
        case MENU_EVENT_TYPE_CALLBACK:
            void_callback_t callback = (void_callback_t)menu_ev.data;
            callback();
            break;
        default:
            break;
        }
    }
}


void App_Start(void *p_arg) {
    (void)p_arg;
    OS_ERR os_err;

    CPU_Init();

    /* (optional) Compute CPU capacity with no task running */
    // OSStatTaskCPUUsageInit(&os_err);

    // CPU_IntDisMeasMaxCurReset();

    encoder_init(RCHA_PIN, RCHB_PIN, RSW_PIN);
    serial_init(SERIAL_BAUDRATE);
    dispInit();

    OSSemCreate(&semLectorAvailable, "sem lector", 0, &os_err);
    lector_Init(&semLectorAvailable);

    user_table_init(MAX_USERS);

    user_data_t user1 = {
        .id = 99,
        .pin = 123,
        .current_floor = 1
    };
    user_data_t user2 = {
        .id = 100,
        .pin = 456,
        .current_floor = 1
    };
    user_data_t user3 = {
        .id = 101,
        .pin = 789,
        .current_floor = 2
    };
    user_data_t user4 = {
        .id = 102,
        .pin = 321,
        .current_floor = 2
    };
    user_data_t user5 = {
        .id = 103,
        .pin = 654,
        .current_floor = 3
    };
    user_data_t user6 = {
        .id = 104,
        .pin = 222,
        .current_floor = 3
    };
    user_table_add(user1);
    user_table_add(user2);
    user_table_add(user3);
    user_table_add(user4);
    user_table_add(user5);
    user_table_add(user6);

    event_pend_data[0].PendObjPtr = (OS_PEND_OBJ *)encoder_sem_ptr();
    event_pend_data[1].PendObjPtr = (OS_PEND_OBJ *)&semLectorAvailable;

    user_not_found_msg = message_box("  User not found", NULL, MENU_EVENT_TYPE_CLOSE);
    door_open_msg = message_box("  Door Open", NULL, MENU_EVENT_TYPE_CLOSE);
    access_denied_msg = message_box("  Denied", NULL, MENU_EVENT_TYPE_CLOSE);

    /* Create TaskMenu */
    OSTaskCreate(&TaskMenuTCB, 			//tcb
                 "App_Menu_Task",				//name
				 App_Menu_Task,				//func
                  0u,					//arg
                  TASK_MENU_PRIO,			//prio
                 &TaskMenuStk[0u],			//stack
                  TASK_MENU_STK_SIZE_LIMIT,	//stack limit
                  TASK_MENU_STK_SIZE,		//stack size
                  0u,
                  0u,
                  0u,
                 (OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
                 &os_err);

    while (1) {
        OSTimeDlyHMSM(0u, 0u, 0u, 999u, OS_OPT_TIME_HMSM_STRICT, &os_err);
        // LED_G_TOGGLE();
        // gpioToggle(STATUS0);
    }
}
