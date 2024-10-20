/*
 * access_control.c
 *
 * Created: 10/20/2024 12:16:45 PM
 *  Author: ssali
 */ 

#include "access_control.h"
#include "tenant.h"
#include "uart.h"
#include <string.h>
#include <stdio.h>
#include <avr/eeprom.h>

#define MAX_TEMP_ACCESS 10


typedef struct {
	uint8_t tenant_id;
	uint8_t temp_code[4];
	uint16_t timeout;
} TempAccess;

TempAccess temp_access_list[MAX_TEMP_ACCESS];
uint8_t temp_access_count = 0;

void init_access_control(void) {
	// Initialize temp access list
	memset(temp_access_list, 0, sizeof(temp_access_list));
	
	// Load temp access list from EEPROM
	eeprom_read_block((void*)&temp_access_count, (const void*)0, sizeof(uint8_t));
	eeprom_read_block((void*)temp_access_list, (const void*)sizeof(uint8_t), sizeof(TempAccess) * MAX_TEMP_ACCESS);
}

void save_temp_access(void) {
	// Save temp access list to EEPROM
	eeprom_write_block((const void*)&temp_access_count, (void*)0, sizeof(uint8_t));
	eeprom_write_block((const void*)temp_access_list, (void*)sizeof(uint8_t), sizeof(TempAccess) * MAX_TEMP_ACCESS);
}

void process_access_command(char* command) {
	if (strncmp(command, "check ", 6) == 0) {
		uint8_t tenant_id;
		uint8_t code[4];
		sscanf(command + 6, "%hhu %hhu%hhu%hhu%hhu", &tenant_id, &code[0], &code[1], &code[2], &code[3]);
		if (check_washroom_access(tenant_id, code)) {
			uart_print("Access granted\n");
			} else {
			uart_print("Access denied\n");
		}
		} else if (strncmp(command, "temp ", 5) == 0) {
		uint8_t tenant_id, temp_code[4];
		uint16_t timeout;
		sscanf(command + 5, "%hhu %hhu%hhu%hhu%hhu %hu", &tenant_id, &temp_code[0], &temp_code[1], &temp_code[2], &temp_code[3], &timeout);
		set_temporary_access(tenant_id, temp_code, timeout);
		} else {
		uart_print("Unknown access command\n");
	}
}

uint8_t check_washroom_access(uint8_t tenant_id, uint8_t* code) {
	// Check permanent access
	for (int i = 0; i < tenant_count; i++) {
		if (tenants[i].id == tenant_id) {
			if (memcmp(tenants[i].washroom_code, code, 4) == 0 && tenants[i].rent_paid >= tenants[i].rent) {
				return 1;
			}
			break;
		}
	}
	
	// Check temporary access
	for (int i = 0; i < temp_access_count; i++) {
		if (temp_access_list[i].tenant_id == tenant_id && memcmp(temp_access_list[i].temp_code, code, 4) == 0) {
			if (temp_access_list[i].timeout > 0) {
				temp_access_list[i].timeout--;
				save_temp_access();
				return 1;
				} else {
				// Remove expired temporary access
				for (int j = i; j < temp_access_count - 1; j++) {
					temp_access_list[j] = temp_access_list[j+1];
				}
				temp_access_count--;
				save_temp_access();
				break;
			}
		}
	}
	
	return 0;
}

void set_temporary_access(uint8_t tenant_id, uint8_t* temp_code, uint16_t timeout) {
	if (temp_access_count < MAX_TEMP_ACCESS) {
		TempAccess new_temp_access;
		new_temp_access.tenant_id = tenant_id;
		memcpy(new_temp_access.temp_code, temp_code, 4);
		new_temp_access.timeout = timeout;
		
		temp_access_list[temp_access_count++] = new_temp_access;
		save_temp_access();
		
		uart_print("Temporary access set\n");
		} else {
		uart_print("Error: Maximum number of temporary access codes reached\n");
	}
}