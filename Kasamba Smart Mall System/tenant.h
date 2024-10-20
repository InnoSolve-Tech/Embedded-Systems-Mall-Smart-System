#ifndef TENANT_H
#define TENANT_H

#include <stdint.h>

#define MAX_TENANTS 20

typedef struct {
	uint8_t id;
	char name[20];
	uint8_t floor;
	uint16_t rent;
	uint8_t washroom_code[4];
	uint8_t rent_paid;
} Tenant;

extern Tenant tenants[MAX_TENANTS]; // Extern declaration
extern uint8_t tenant_count; // Extern declaration

void init_tenant_system(void);
void process_tenant_command(char* command);
void add_tenant(char* name, uint8_t floor);
void remove_tenant(uint8_t id);
void pay_rent(uint8_t id, uint16_t amount);
void list_tenants(void);

#endif // TENANT_H
