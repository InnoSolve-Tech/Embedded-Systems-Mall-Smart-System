#ifndef UART_H
#define UART_H

#include <avr/io.h>

void uart_init(void);
void uart_transmit(unsigned char data);
unsigned char uart_receive(void);
void uart_print(char* str);

#endif // UART_H