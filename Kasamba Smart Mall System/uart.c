/*
 * uart.c
 *
 * Created: 10/20/2024 12:13:50 PM
 *  Author: ssali
 */ 
#include "uart.h"

void uart_init(void) {
	// Set baud rate
	UBRR0H = (unsigned char)(MYUBRR>>8);
	UBRR0L = (unsigned char)MYUBRR;
	// Enable receiver and transmitter
	UCSR0B = (1<<RXEN0)|(1<<TXEN0);
	// Set frame format: 8data, 1stop bit
	UCSR0C = (3<<UCSZ00);
}

void uart_transmit(unsigned char data) {
	// Wait for empty transmit buffer
	while (!(UCSR0A & (1<<UDRE0)));
	// Put data into buffer, sends the data
	UDR0 = data;
}

unsigned char uart_receive(void) {
	// Wait for data to be received
	while (!(UCSR0A & (1<<RXC0)));
	// Get and return received data from buffer
	return UDR0;
}

void uart_print(char* str) {
	while (*str) {
		uart_transmit(*str);
		str++;
	}
}