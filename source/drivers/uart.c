#include "uart.h"

#include <stdint.h>
#include "../../SDK/startup/hardware.h"
#include "../utils/circular_buffer.h"

#define UART_BUF_SIZE 256

typedef struct UART_Data
{
    UART_Type *ptr;
    uint32_t baud_rate;
    CircularBuffer_t* buf_tx;
    CircularBuffer_t* buf_rx;
} UART_Data_t;

UART_Data_t uart[5] = {
    {UART0, 0, NULL, NULL},
    {UART1, 0, NULL, NULL},
    {UART2, 0, NULL, NULL},
    {UART3, 0, NULL, NULL},
    {UART4, 0, NULL, NULL},
};   // Son necesarios para las interrupciones

static void uart_set_baudrate(uint8_t port_id, uint32_t baudrate);
static void uart_handle_interrupt(uint8_t port_id);

/**
 * @brief: Initialize UART port with a given baud rate and default settings (8N1)
 * @param: port, UART_Type port to be initialized (UART0, UART1, UART2, UART3, UART4, UART5)
 * @param: baud_rate, baud rate to be used
 * @return: uart_instance_t, instance of the initialized UART channel
 */
void uart_init(uint8_t port_id, uint32_t baud_rate)
{
    if (port_id > 4)
        return;

    uart[port_id].baud_rate = baud_rate;

    uart[port_id].buf_tx = circular_buffer_init(UART_BUF_SIZE);
    uart[port_id].buf_rx = circular_buffer_init(UART_BUF_SIZE);
    
    switch (port_id)
    {
    case 0:
        {
            SIM->SCGC4 |= SIM_SCGC4_UART0(1);
            // PORTD->PCR[UART0_RX] = 0x0; // Clear all bits
            PORTB->PCR[PIN2NUM(UART0_RX)] = PORT_PCR_MUX(UART_MUX);            
            // PORTD->PCR[UART0_RX] = PORT_PCR_IRQC(0);
            // PORTD->PCR[UART0_TX] = 0x0; // Clear all bits
            PORTB->PCR[PIN2NUM(UART0_TX)] = PORT_PCR_MUX(UART_MUX);
            // PORTD->PCR[UART0_TX] = PORT_PCR_IRQC(0);
            NVIC_EnableIRQ(UART0_RX_TX_IRQn);
        }
        break;
    case 1:
        {
            SIM->SCGC4 |= SIM_SCGC4_UART1(1);
            PORTC->PCR[PIN2NUM(UART1_RX)] = PORT_PCR_MUX(UART_MUX);
            PORTC->PCR[PIN2NUM(UART1_TX)] = PORT_PCR_MUX(UART_MUX);
            NVIC_EnableIRQ(UART1_RX_TX_IRQn);
        }
        break;
    case 2:
        {
            SIM->SCGC4 |= SIM_SCGC4_UART2(1);
            PORTD->PCR[PIN2NUM(UART2_RX)] = PORT_PCR_MUX(UART_MUX);
            PORTD->PCR[PIN2NUM(UART2_TX)] = PORT_PCR_MUX(UART_MUX);
            NVIC_EnableIRQ(UART2_RX_TX_IRQn);
        }
        break;
    case 3:
        {
            SIM->SCGC4 |= SIM_SCGC4_UART3(1);
            PORTC->PCR[PIN2NUM(UART3_RX)] = PORT_PCR_MUX(UART_MUX);
            PORTC->PCR[PIN2NUM(UART3_TX)] = PORT_PCR_MUX(UART_MUX);
            NVIC_EnableIRQ(UART3_RX_TX_IRQn);
        }
        break;
    case 4:
        {
            SIM->SCGC1 |= SIM_SCGC1_UART4(1);
            PORTC->PCR[PIN2NUM(UART4_RX)] = PORT_PCR_MUX(UART_MUX);
            PORTC->PCR[PIN2NUM(UART4_TX)] = PORT_PCR_MUX(UART_MUX);
            NVIC_EnableIRQ(UART4_RX_TX_IRQn);
        }
        break;
    default:
        return;
        break;
    }
    uart_set_baudrate(port_id, baud_rate);
    uart[port_id].ptr->C2 |= UART_C2_TE_MASK | UART_C2_RE_MASK | UART_C2_RIE_MASK; // Enable TX, RX and RX interrupt
}


bool uart_available(uint8_t port_id)
{
    if (port_id > 4)
        return false;
    return !circular_buffer_empty(uart[port_id].buf_rx);
}

uint8_t uart_read(uint8_t port_id)
{
    if (port_id > 4)
        return 0;
    if (circular_buffer_empty((uart[port_id].buf_rx)))
        return 0;
    return circular_buffer_pop(uart[port_id].buf_rx);
}

static void uart_set_baudrate(uint8_t port_id, uint32_t baudrate)
{
    if (port_id > 4)
        return;
	uint16_t sbr, brfa;
	uint32_t clock;
    uint8_t old_C2;

	clock = ((uart[port_id].ptr == UART0) || (uart[port_id].ptr == UART1))?(__CORE_CLOCK__):(__CORE_CLOCK__ >> 1);

	sbr = clock / (baudrate << 4);
	brfa = (clock << 1) / baudrate - (sbr << 5);

    // Store C2 before disable Tx and Rx
    old_C2 = uart[port_id].ptr->C2;
    uart[port_id].ptr->C2 &= ~((uint8_t)UART_C2_TE_MASK | (uint8_t)UART_C2_RE_MASK);
    
    uart[port_id].ptr->C1 = 0; // 8-bit mode, no parity
	
    // Set Baud Rate
    uart[port_id].ptr->BDH = UART_BDH_SBR(sbr>>8);
	uart[port_id].ptr->BDL = UART_BDL_SBR(sbr);
	uart[port_id].ptr->C4 = (uart[port_id].ptr->C4 & ~UART_C4_BRFA_MASK) | UART_C4_BRFA(brfa);
    // Restore C2
    uart[port_id].ptr->C2 = old_C2;
}


// [ ] TODO: Test this function with UART0, UART1, UART2, UART3, UART4
// void uart_set_baudrate(.....)
// {
//     ubd = (uint16)((sysclk*1000)/(baud * 16));
//     /* Save off the current value of the UARTx_BDH except for the SBR */
//     temp = uart->BDH & ~(UART_BDH_SBR(0x1F));
//     uart->BDH = temp | UART_BDH_SBR(((ubd & 0x1F00) >> 8));
//     uart->BDL = (uint8_t)(ubd & UART_BDL_SBR_MASK);
//     /* Determine if a fractional divider is needed to get closer to the baud rate */
//     brfa = (((sysclk*32000)/(baud * 16)) - (ubd * 32));
//     /* Save off the current value of the UARTx_C4 register except for the BRFA */
//     temp = uart->C4 & ~(UART_C4_BRFA(0x1F));
//     uart->C4 = temp | UART_C4_BRFA(brfa);
//     /* Enable receiver and transmitter */
//     uart->C2 |= (UART_C2_TE_MASK | UART_C2_RE_MASK);
// }

// [ ] TODO: Test this function with UART0, UART1, UART2, UART3, UART4
// void uart_set_baudrate(UART_Type *uart, uint32_t baud_rate)
// {
//     uint32_t sbr = 0;
//     uint32_t baudDiff = 0;
//     uint8_t oldCtrl;

//     /* Calculate the baud rate modulo divisor, sbr*/
//     sbr = __CORE_CLOCK__ / (baud_rate * 16U);
//     /* set sbrTemp to 1 if the sourceClockInHz can not satisfy the desired baud rate */
//     if (sbr == 0U)
//     {
//         sbr = 1U;
//     }

//     baudDiff = (__CORE_CLOCK__ / (sbr * 16U)) - baud_rate;

//     /* Select the better value between sbr and (sbr + 1) */
//     if (baudDiff > (baud_rate - (__CORE_CLOCK__ / (16U * (sbr + 1U)))))
//     {
//         baudDiff = baud_rate - (__CORE_CLOCK__ / (16U * (sbr + 1U)));
//         sbr++;
//     }

//     /* next, check to see if actual baud rate is within 3% of desired baud rate
//      * based on the calculate SBR value */
//     if (baudDiff < ((baud_rate / 100U) * 3U))
//     {
//         /* Store C2 before disable Tx and Rx */
//         oldCtrl = uart->C2;

//         /* Disable UART TX RX before setting. */
//         uart->C2 &= ~((uint8_t)UART_C2_TE_MASK | (uint8_t)UART_C2_RE_MASK);

//         /* Write the sbr value to the BDH and BDL registers*/
//         uart->BDH = (uart->BDH & ~(uint8_t)UART_BDH_SBR_MASK) | (uint8_t)(sbr >> 8);
//         uart->BDL = (uint8_t)sbr;

//         uart->C2 = oldCtrl;
//     }
// }

void uart_load_buffer(uint8_t port_id, uint8_t data)
{
    if (port_id > 4)
        return;
    if (!circular_buffer_full(uart[port_id].buf_tx))
        circular_buffer_push(uart[port_id].buf_tx, data);
}

void uart_disable_tx_interrupt(uint8_t port_id)
{
    if (port_id > 4)
        return;
    uart[port_id].ptr->C2 &= ~UART_C2_TIE_MASK; // Disable TX interrupt
}

void uart_enable_tx_interrupt(uint8_t port_id)
{
    if (port_id > 4)
        return;
    uart[port_id].ptr->C2 |= UART_C2_TIE_MASK; // Enable TX interrupt
}


void uart_write_array(uint8_t port_id, const uint8_t *data, unsigned long size)
{
    if (port_id > 4 || data == NULL || size == 0 || size > UART_BUF_SIZE - 1)
        return;

    uart[port_id].ptr->C2 &= ~UART_C2_TIE_MASK;
    for (unsigned long i = 0; i < size; i++)
    {
        if (!circular_buffer_full(uart[port_id].buf_tx))
        {
            circular_buffer_push(uart[port_id].buf_tx, data[i]);
        }
    }
    uart[port_id].ptr->C2 |= UART_C2_TIE_MASK; // Enable TX interrupt
}

void uart_write(uint8_t port_id, uint8_t data)
{
    if (port_id > 4)
        return;
    uart[port_id].ptr->C2 &= ~UART_C2_TIE_MASK; // Disable TX interrupt
    if (!circular_buffer_full(uart[port_id].buf_tx))
    {
        circular_buffer_push(uart[port_id].buf_tx, data);
    }
    uart[port_id].ptr->C2 |= UART_C2_TIE_MASK; // Enable TX interrupt
}







static void uart_handle_interrupt(uint8_t port_id)
{
    uint8_t status = uart[port_id].ptr->S1;
    
    // Receive Data Register Full Flag
    if (status & UART_S1_RDRF_MASK)
    {
        circular_buffer_push(uart[port_id].buf_rx, uart[port_id].ptr->D);
    }

    // Transmit Data Register Empty Flag
    if (status & UART_S1_TDRE_MASK)
    {
        if (!circular_buffer_empty(uart[port_id].buf_tx))
        {
            uart[port_id].ptr->D = circular_buffer_pop(uart[port_id].buf_tx);
        }
        else
        {
            // Disable TX interrupt
            uart[port_id].ptr->C2 &= ~UART_C2_TIE_MASK;
        }
    }
}

__ISR__ UART0_RX_TX_IRQHandler(void) {uart_handle_interrupt(0);}
__ISR__ UART1_RX_TX_IRQHandler(void) {uart_handle_interrupt(1);}
__ISR__ UART2_RX_TX_IRQHandler(void) {uart_handle_interrupt(2);}
__ISR__ UART3_RX_TX_IRQHandler(void) {uart_handle_interrupt(3);}
__ISR__ UART4_RX_TX_IRQHandler(void) {uart_handle_interrupt(4);}
