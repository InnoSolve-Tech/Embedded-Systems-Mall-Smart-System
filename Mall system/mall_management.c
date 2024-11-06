/*
 * mall_management.c
 *
 * Created: 10/20/2024 12:18:09 PM
 *  Author: ssali
 */ 

#include "mall_management.h"
#include "tenant.h"
#include "uart.h"
#include <string.h>
#include <stdio.h>
#include <avr/eeprom.h>

Tenant tenants[MAX_TENANTS];
uint8_t tenant_count = 0;

#define NUM_FLOORS 3

typedef struct {
    uint16_t base_rent;
    uint8_t escalator_state[NUM_FLOORS];
} MallConfig;

MallConfig mall_config;

// Global variable to track total earnings
static uint32_t total_earnings = 0;
#define EEPROM_TOTAL_EARNINGS_ADDR (sizeof(MallConfig))

void init_mall_management(void) {
    // Load mall configuration from EEPROM
    eeprom_read_block((void*)&mall_config, (const void*)0, sizeof(MallConfig));
    
    // Set default values if EEPROM is empty
    if (mall_config.base_rent == 0xFFFF) {
        mall_config.base_rent = 1000;  // Default base rent
        for (int i = 0; i < NUM_FLOORS; i++) {
            mall_config.escalator_state[i] = 1;  // Escalators on by default
        }
        save_mall_config();
    }

    // Load total earnings from EEPROM
    eeprom_read_block((void*)&total_earnings, (const void*)EEPROM_TOTAL_EARNINGS_ADDR, sizeof(total_earnings));
}

void save_mall_config(void) {
    // Save mall configuration to EEPROM
    eeprom_write_block((const void*)&mall_config, (void*)0, sizeof(MallConfig));
}

void process_management_command(char* command) {
	if (strncmp(command, "rent ", 5) == 0) {
		uint16_t new_rent;
		sscanf(command + 5, "%hu", &new_rent);
		set_base_rent(new_rent);
		uart_print("\r\n");  // Add new line
		} else if (strncmp(command, "escalator ", 10) == 0) {
		uint8_t floor, state;
		sscanf(command + 10, "%hhu %hhu", &floor, &state);
		toggle_escalator(floor, state);
		uart_print("\r\n");  // Add new line
		} else if (strncmp(command, "disable ", 8) == 0) {
		uint8_t tenant_id;
		sscanf(command + 8, "%hhu", &tenant_id);
		disable_tenant_account(tenant_id);
		uart_print("\r\n");  // Add new line
		} else if (strcmp(command, "earnings") == 0) {
		view_total_earnings();
		uart_print("\r\n");  // Add new line
		} else {
		uart_print("Unknown management command\r\n");
	}
}

void view_total_earnings(void) {
	char response[50];
	sprintf(response, "Total earnings: $%lu\r\n", total_earnings);
	uart_print(response);
}

void set_base_rent(uint16_t rent) {
    mall_config.base_rent = rent;
    save_mall_config();
    
    // Update rent for all tenants
    for (int i = 0; i < tenant_count; i++) {
        if (tenants[i].floor == 0) {
            tenants[i].rent = rent;
        } else if (tenants[i].floor == 1) {
            tenants[i].rent = rent * 3 / 4;
        } else if (tenants[i].floor == 2) {
            tenants[i].rent = rent * 3 / 4 * 3 / 4;
        }
    }
    save_tenants();
    
    char response[50];
    sprintf(response, "Base rent set to %d\n", rent);
    uart_print(response);
}

void toggle_escalator(uint8_t floor, uint8_t state) {
    if (floor < NUM_FLOORS) {
        mall_config.escalator_state[floor] = state;
        save_mall_config();
        
        char response[50];
        sprintf(response, "Escalator on floor %d set to %s\n", floor, state ? "ON" : "OFF");
        uart_print(response);
    } else {
        uart_print("Error: Invalid floor number\n");
    }
}

void disable_tenant_account(uint8_t tenant_id) {
    for (int i = 0; i < tenant_count; i++) {
        if (tenants[i].id == tenant_id) {
            tenants[i].rent_paid = 0;  // Reset rent paid to effectively disable the account
            save_tenants();
            
            char response[50];
            sprintf(response, "Tenant account %d disabled\n", tenant_id);
            uart_print(response);
            return;
        }
    }
    uart_print("Error: Tenant not found\n");
}

void update_total_earnings(uint16_t amount) {
    total_earnings += amount;
    
    // Save total earnings in EEPROM
    eeprom_write_block((const void*)&total_earnings, (void*)EEPROM_TOTAL_EARNINGS_ADDR, sizeof(total_earnings));
    
    // Optionally, print the total earnings if needed
    char response[50];
    sprintf(response, "Total earnings updated: $%lu\n", total_earnings);
    uart_print(response);
}
