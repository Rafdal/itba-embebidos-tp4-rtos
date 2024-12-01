#ifndef UART_K64_FRDM_H
#define UART_K64_FRDM_H

#include "gpio.h"
#include "../../SDK/CMSIS/MK64F12.h"

#define UART_MUX 3

// UART 0
#define UART0_RX PORTNUM2PIN(PB, 16)
#define UART0_TX PORTNUM2PIN(PB, 17)

// UART 1
#define UART1_RX PORTNUM2PIN(PC, 3)
#define UART1_TX PORTNUM2PIN(PC, 4)

// UART 2
#define UART2_RX PORTNUM2PIN(PD, 2)
#define UART2_TX PORTNUM2PIN(PD, 3)

// UART 3
#define UART3_RX PORTNUM2PIN(PC, 16)
#define UART3_TX PORTNUM2PIN(PC, 17)
// #define UART3_RX_2 PORTNUM2PIN(PB, 10)
// #define UART3_TX_2 PORTNUM2PIN(PB, 11)

// UART 4
#define UART4_RX PORTNUM2PIN(PC, 14)
#define UART4_TX PORTNUM2PIN(PC, 15)
// #define UART4_RX_2 PORTNUM2PIN(PE, 25)
// #define UART4_TX_2 PORTNUM2PIN(PE, 24)


/**
 * PTA0     UART0_CTS_b (alt2)
 * PTA1     UART0_RX (alt2)
 * PTA2     UART0_TX (alt2)
 * PTA3     UART0_RTS_b (alt2)     (visto en esquematico, SWD_DIO)
 * 
 * PTB2     UART0_RTS_b (alt3)
 * PTB3     UART0_CTS_b (alt3)
 * PTB9     UART3_CTS_b (alt3)
 * PTB10    UART3_RX (alt3)
 * PTB11    UART3_TX (alt3)
 * 
 * PTC0
 * PTC1     UART1_RTS_b (alt3)
 * PTC2     UART1_CTS_b (alt3)
 * PTC3     UART1_RX (alt3)
 * PTC4     UART1_TX (alt3)
 * PTC5
 * PTC12    UART4_RTS_b (alt3)
 * PTC14    UART4_RX (alt3)
 * PTC15    UART4_TX (alt3)
 * PTC16    UART3_RX (alt3)
 * PTC17    UART3_TX (alt3)
 * PTC18    UART3_RTS_b (alt3)
 * 
 * PTD0     UART2_RTS_b (alt3)
 * PTD1     UART2_CTS_b (alt3)
 * PTD2     UART2_RX (alt3)
 * PTD3     UART2_TX (alt3)
 * PTD4     UART0_RTS_b (alt3)
 * PTD5     UART0_CTS_b (alt3)
 * PTD6     UART0_RX (alt3)
 * PTD7     UART0_TX (alt3)
 * 
 * PTE24    UART4_TX (alt3)
 * PTE25    UART4_RX (alt3)
 * PTE26    UART4_CTS_b (alt3)
 * 
 */

/**
 *      UART 0
 * PTA1     UART0_RX (alt2)
 * PTA2     UART0_TX (alt2)
 * PTD6     UART0_RX (alt3)
 * PTD7     UART0_TX (alt3)
 * 
 *      UART 1
 * PTC3     UART1_RX (alt3)
 * PTC4     UART1_TX (alt3)
 * 
 *      UART 2
 * PTD2     UART2_RX (alt3)
 * PTD3     UART2_TX (alt3)
 * 
 *      UART 3
 * PTB10    UART3_RX (alt3)
 * PTB11    UART3_TX (alt3)
 * PTC16    UART3_RX (alt3)
 * PTC17    UART3_TX (alt3)
 * 
 *      UART 4
 * PTC14    UART4_RX (alt3)
 * PTC15    UART4_TX (alt3)
 * PTE24    UART4_TX (alt3)
 * PTE25    UART4_RX (alt3)
 */

#endif // UART_K64_FRDM_H