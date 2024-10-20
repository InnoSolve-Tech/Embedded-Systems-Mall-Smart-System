/*
 * mall_management.h
 *
 * Created: 10/20/2024 12:17:40 PM
 *  Author: ssali
 */ 

#ifndef MALL_MANAGEMENT_H
#define MALL_MANAGEMENT_H

#include <stdint.h>

void init_mall_management(void);
void process_management_command(char* command);
void set_base_rent(uint16_t rent);
void toggle_escalator(uint8_t floor, uint8_t state);
void disable_tenant_account(uint8_t tenant_id);
void update_total_earnings(uint16_t amount);

#endif // MALL_MANAGEMENT_H