#include "hardware.h"
#include  <os.h>
#include "drivers/gpio.h"
#include "config.h"
#include <stddef.h>
#include "App.h"

/* Task Start */
#define TASKSTART_STK_SIZE 		512u
#define TASKSTART_PRIO 			2u
static OS_TCB TaskStartTCB;
static CPU_STK TaskStartStk[TASKSTART_STK_SIZE];


int main(void) {
    OS_ERR err;

#if (CPU_CFG_NAME_EN == DEF_ENABLED)
    CPU_ERR  cpu_err;
#endif

    hw_Init();

    gpioMode(LED_R_PIN, OUTPUT);
    gpioMode(LED_G_PIN, OUTPUT);
    gpioMode(LED_B_PIN, OUTPUT);
    gpioWrite(LED_R_PIN, HIGH);
    gpioWrite(LED_G_PIN, HIGH);
    gpioWrite(LED_B_PIN, HIGH);

    gpioMode(STATUS0, OUTPUT);
    gpioMode(STATUS1, OUTPUT);
    gpioWrite(STATUS0, LOW);
    gpioWrite(STATUS1, LOW);

    OSInit(&err);

 #if OS_CFG_SCHED_ROUND_ROBIN_EN > 0u
	 /* Enable task round robin. */
	 OSSchedRoundRobinCfg((CPU_BOOLEAN)1, 0, &err);
 #endif
    OS_CPU_SysTickInit(SystemCoreClock / (uint32_t)OSCfg_TickRate_Hz);


    OSTaskCreate(&TaskStartTCB,
                 "App Task Start",
                  App_Start,
                  0u,
                  TASKSTART_PRIO,
                 &TaskStartStk[0u],
                 (TASKSTART_STK_SIZE / 10u),
                  TASKSTART_STK_SIZE,
                  0u,
                  0u,
                  0u,
                 (OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR | OS_OPT_TASK_SAVE_FP),
                 &err);


    OSStart(&err);

	/* Should Never Get Here */
    for(;;);
}
