/*
 * uart.h
 *
 * Created: 10/20/2024 12:14:09 PM
 *  Author: ssali
 */ 

#ifndef UART_H
#define UART_H

#include <avr/io.h>

#define F_CPU 16000000UL
#define BAUD 9600
#define MYUBRR F_CPU/16/BAUD-1

void uart_init(void);
void uart_transmit(unsigned char data);
unsigned char uart_receive(void);
void uart_print(char* str);

#endif // UART_H