#include "serial.h"
#include <stdarg.h>
/**
 * @file serial.c
 * @brief Serial communication driver for the K64F (UART)
 * @date 2024-10-06
 * 
 * @author Rafael Dalzotto
 */

#include "../drivers/uart.h"

void serial_init(unsigned long baud_rate)
{
    uart_init(0, baud_rate);
}

void serial_print(const char *str)
{
    uart_disable_tx_interrupt(0);
    uint8_t i = 0;
    while (*(str + i) != '\0')
    {
        uart_load_buffer(0, *(str + i));
        i++;
    }
    uart_enable_tx_interrupt(0);
}

void serial_println(const char *str)
{
	uart_disable_tx_interrupt(0);
	uint8_t i = 0;
	while (*(str + i) != '\0')
	{
		uart_load_buffer(0, *(str + i));
		i++;
	}
	uart_load_buffer(0, '\n');
	uart_enable_tx_interrupt(0);
}

//void serial_printf(const char *format, ...)
//{
//    va_list args;
//    va_start(args, format);
//    char buffer[256];
//    vsprintf(buffer, format, args);
//    serial_print(buffer);
//    va_end(args);
//}
