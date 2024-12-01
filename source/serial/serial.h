/**
 * @file serial.h
 * @brief Serial communication driver for the K64F (UART)
 * @date 2024-10-06
 * 
 * @author Rafael Dalzotto
 */

#ifndef _SERIAL_H_
#define _SERIAL_H_

#include <stdint.h>
#include <stdbool.h>
#include <stdarg.h>

/**
 * @brief Initialize the serial communication
 * 
 * @param baud_rate Baud rate to be used
 */
void serial_init(unsigned long baud_rate);

/**
 * @brief Print a string through the serial communication
 * 
 * @param str String to be printed
 */
void serial_print(const char *str);

void serial_println(const char *str);

/**
 * @brief Print a formatted string through the serial communication
 * 
 * @param format Format of the string
 * @param ... Arguments to be formatted
 */
//void serial_printf(const char *format, ...);


#endif // _SERIAL_H_
