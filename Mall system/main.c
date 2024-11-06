/*
 * Kasamba Smart Mall System for ATmega32
 *
 * Created: 10/16/2024 8:58:59 AM
 * Modified: 10/20/2024 (ATmega32 version)
 * Author : ssali
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <string.h>
#include <stdio.h>
#include <avr/eeprom.h>
#include "uart.h"
#include "tenant.h"
#include "access_control.h"
#include "mall_management.h"

// Define pins for entrance and exit buttons
#define ENTRANCE_1_PIN PD2
#define ENTRANCE_2_PIN PD3
#define EXIT_PIN PD4

#define F_CPU 80000000UL
#define DEBOUNCE_TIME 1000 // debounce time in milliseconds

// Global variables
volatile uint16_t people_count = 0;
volatile uint8_t floor_count[3] = {0};

// Function prototypes
void init_system(void);
void process_entrance_exit(void);
void process_command(char* command);
uint8_t debounce(uint8_t pin);

int main(void) {
    init_system();
    
    char command[50];
    uint8_t index = 0;

    while (1) {
        process_entrance_exit();

        // Check for incoming commands
        if (UCSRA & (1<<RXC)) {
            char c = uart_receive();
			
			 // Echo the received character back to the terminal
			 uart_transmit(c);
			
            if (c == '\n' || c == '\r') {
                command[index] = '\0';
                process_command(command);
                index = 0;
            } else {
                command[index++] = c;
            }
        }
    }

    return 0;
}

void init_system(void) {
    // Initialize I/O
    DDRD &= ~((1 << ENTRANCE_1_PIN) | (1 << ENTRANCE_2_PIN) | (1 << EXIT_PIN)); // Set as inputs
    PORTD |= (1 << ENTRANCE_1_PIN) | (1 << ENTRANCE_2_PIN) | (1 << EXIT_PIN);   // Enable pull-up resistors

    // Initialize UART
    uart_init();
    uart_print("Hello, UART!\r\n");  

    // Initialize tenant system
    init_tenant_system();

    // Initialize access control system
    init_access_control();

    // Initialize mall management system
    init_mall_management();

    // Enable global interrupts
    sei();
}

uint8_t debounce(uint8_t pin) {
    if (!(PIND & (1 << pin))) {
        _delay_ms(DEBOUNCE_TIME);
        if (!(PIND & (1 << pin))) {
            return 1; // Button is pressed
        }
    }
    return 0; // Button is not pressed
}

void process_entrance_exit(void) {
    // Check entrance 1
    if (debounce(ENTRANCE_1_PIN)) {
        people_count++;
        floor_count[0]++;
    }

    // Check entrance 2
    if (debounce(ENTRANCE_2_PIN)) {
        people_count++;
        floor_count[0]++;
    }

    // Check exit
    if (debounce(EXIT_PIN)) {
        if (people_count > 0) {
            people_count--;
            if (floor_count[0] > 0) floor_count[0]--;
        }
    }
}

void process_command(char* command) {
    char response[100];

    if (strcmp(command, "count") == 0) {
        sprintf(response, "Total people: %d\r\nGround floor: %d\r\nFirst floor: %d\r\nSecond floor: %d\r\n",
                people_count, floor_count[0], floor_count[1], floor_count[2]);
        uart_print(response);
    } else if (strncmp(command, "move ", 5) == 0) {
        int from_floor = command[5] - '0';
        int to_floor = command[7] - '0';
        if (from_floor >= 0 && from_floor < 3 && to_floor >= 0 && to_floor < 3 && from_floor != to_floor) {
            if (floor_count[from_floor] > 0) {
                floor_count[from_floor]--;
                floor_count[to_floor]++;
                sprintf(response, "Moved 1 person from floor %d to floor %d\r\n", from_floor, to_floor);
            } else {
                sprintf(response, "Error: No people on floor %d\r\n", from_floor);
            }
        } else {
            sprintf(response, "Error: Invalid floor numbers\r\n");
        }
        uart_print(response);
    } else if (strncmp(command, "tenant ", 7) == 0) {
        process_tenant_command(command + 7);
    } else if (strncmp(command, "access ", 7) == 0) {
        process_access_command(command + 7);
    } else if (strncmp(command, "manage ", 7) == 0) {
        process_management_command(command + 7);
    } else {
        uart_print("Unknown command\r\n");
    }
}
