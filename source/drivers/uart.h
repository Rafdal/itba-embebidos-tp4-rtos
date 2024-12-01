#ifndef UART_DRIVER_H
#define UART_DRIVER_H

#include <stdint.h>
#include <stdbool.h>

#include "uart_k64_frdm.h"

/**
 * @brief: Initialize UART port with a given baud rate and default settings (8N1)
 * @param: port port to be initialized (0, 1, 2, 3, 4)
 * @param: baud_rate, baud rate to be used
 * @return: uart_instance_t, instance of the initialized UART channel
 */
void uart_init(uint8_t uart_port, uint32_t baud_rate);    

/**
 * @brief: Check if there is data available to be read
 * @param: instance, instance of the UART channel
 * @return: bool, true if there is data available, false otherwise
 */
bool uart_available(uint8_t port_id);

/**
 * @brief: Write an array of bytes to the UART channel
 * @param: instance, instance of the UART channel
 * @param: data, pointer to the array of bytes to be written
 * @param: size, size of the array of bytes
 */
void uart_write_array(uint8_t port_id, const uint8_t *data, unsigned long size);

/**
 * @brief: Load a byte into the UART buffer
 * @param: instance, instance of the UART channel
 * @param: data, byte to be loaded into the buffer
 */
void uart_load_buffer(uint8_t port_id, uint8_t data);

/**
 * @brief: Enable the UART TX interrupt. This will trigger the transmission of the data in the buffer
 * @param: instance, instance of the UART channel
 */
void uart_enable_tx_interrupt(uint8_t port_id);
void uart_disable_tx_interrupt(uint8_t port_id);

/**
 * @brief: Read an array of bytes from the UART channel
 * @param: instance, instance of the UART channel
 * @param: data, pointer to the array of bytes to be read
 * @param: size, size of the array of bytes
 */
void uart_write(uint8_t port_id, uint8_t data);

/**
 * @brief: Read a byte from the UART channel
 * @param: instance, instance of the UART channel
 * @return: uint8_t, byte read from the UART channel
 */
uint8_t uart_read(uint8_t port_id);

#endif // UART_DRIVER_H
