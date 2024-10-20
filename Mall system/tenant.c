/*
 * tenant.c
 *
 * Created: 10/20/2024 12:15:11 PM
 *  Author: ssali
 */ 

#include "tenant.h"
#include "uart.h"
#include "mall_management.h"
#include <string.h>
#include <stdio.h>
#include <avr/eeprom.h>

// EEPROM address definitions
#define EEPROM_TENANT_COUNT_ADDR 0
#define EEPROM_TENANTS_ADDR (EEPROM_TENANT_COUNT_ADDR + sizeof(uint8_t))

void init_tenant_system(void) {
	// Load tenant count from EEPROM
	eeprom_read_block((void*)&tenant_count, (const void*)EEPROM_TENANT_COUNT_ADDR, sizeof(uint8_t));
	
	// Load tenants from EEPROM
	if (tenant_count > 0 && tenant_count <= MAX_TENANTS) {
		eeprom_read_block((void*)tenants, (const void*)EEPROM_TENANTS_ADDR, sizeof(Tenant) * tenant_count);
		} else {
		// Initialize tenant count if EEPROM is empty or invalid
		tenant_count = 0;
		eeprom_write_byte((uint8_t*)EEPROM_TENANT_COUNT_ADDR, tenant_count);
	}
}

void save_tenants(void) {
	// Save tenant count to EEPROM
	eeprom_write_byte((uint8_t*)EEPROM_TENANT_COUNT_ADDR, tenant_count);
	
	// Save tenants to EEPROM
	eeprom_write_block((const void*)tenants, (void*)EEPROM_TENANTS_ADDR, sizeof(Tenant) * tenant_count);
}

void process_tenant_command(char* command) {
	char response[100];
	if (strncmp(command, "add ", 4) == 0) {
		char name[20];
		uint8_t floor;
		sscanf(command + 4, "%s %hhu", name, &floor);
		add_tenant(name, floor);
		} else if (strncmp(command, "remove ", 7) == 0) {
		uint8_t id;
		sscanf(command + 7, "%hhu", &id);
		remove_tenant(id);
		} else if (strncmp(command, "pay ", 4) == 0) {
		uint8_t id;
		uint16_t amount;
		sscanf(command + 4, "%hhu %hu", &id, &amount);
		pay_rent(id, amount);
		} else if (strcmp(command, "list") == 0) {
		list_tenants();
		} else {
		uart_print("Unknown tenant command\n");
	}
}

void add_tenant(char* name, uint8_t floor) {
	if (tenant_count < MAX_TENANTS) {
		Tenant new_tenant;
		new_tenant.id = tenant_count + 1;
		strncpy(new_tenant.name, name, 19);
		new_tenant.name[19] = '\0';
		new_tenant.floor = floor;
		new_tenant.rent = 0;  // Set initial rent
		new_tenant.rent_paid = 0;
		
		// Generate washroom code
		for (int i = 0; i < 4; i++) {
			new_tenant.washroom_code[i] = rand() % 10;
		}
		
		tenants[tenant_count++] = new_tenant;
		save_tenants();
		
		char response[50];
		sprintf(response, "Tenant added. ID: %d\n", new_tenant.id);
		uart_print(response);
		} else {
		uart_print("Error: Maximum number of tenants reached\n");
	}
}

void remove_tenant(uint8_t id) {
	for (int i = 0; i < tenant_count; i++) {
		if (tenants[i].id == id) {
			for (int j = i; j < tenant_count - 1; j++) {
				tenants[j] = tenants[j+1];
			}
			tenant_count--;
			save_tenants();
			uart_print("Tenant removed\n");
			return;
		}
	}
	uart_print("Error: Tenant not found\n");
}

void pay_rent(uint8_t id, uint16_t amount) {
	for (int i = 0; i < tenant_count; i++) {
		if (tenants[i].id == id) {
			tenants[i].rent_paid += amount;
			save_tenants();
			
			// Update total earnings
			update_total_earnings(amount);
			
			char response[50];
			sprintf(response, "Rent paid. New balance: $%d\n", tenants[i].rent_paid);
			uart_print(response);
			return;
		}
	}
	uart_print("Error: Tenant not found\n");
}

void list_tenants(void) {
	for (int i = 0; i < tenant_count; i++) {
		char tenant_info[100];
		sprintf(tenant_info, "ID: %d, Name: %s, Floor: %d, Rent Paid: $%d\n",
		tenants[i].id, tenants[i].name, tenants[i].floor, tenants[i].rent_paid);
		uart_print(tenant_info);
	}
}