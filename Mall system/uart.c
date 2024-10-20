/*
 * uart.c for ATmega32
 *
 * Created: 10/20/2024 12:13:50 PM
 * Modified: 10/20/2024 (ATmega32 version)
 * Author : ssali
 */ 
#include "uart.h"

#include <util/delay.h>

void uart_init(void) {
	UBRRH = 0x00;
	UBRRL = 0x33;

	UCSRB = (1<<RXEN)|(1<<TXEN);
	UCSRC = (1<<UCSZ1)|(1<<UCSZ0);
	
}

void uart_transmit(unsigned char data) {
	// Wait for empty transmit buffer
	while (!(UCSRA & (1<<UDRE)));
	// Put data into buffer, sends the data
	UDR = data;
	// Add a small delay after each transmission
	_delay_ms(1);
}

unsigned char uart_receive(void) {
	// Wait for data to be received
	while (!(UCSRA & (1<<RXC)));
	// Get and return received data from buffer
	return UDR;
}

void uart_print(char* str) {
	while (*str) {
		uart_transmit(*str);
		_delay_ms(1);
		str++;
	}
	// Add a delay after sending the entire string
}