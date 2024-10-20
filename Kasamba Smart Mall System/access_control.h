/*
 * access_control.h
 *
 * Created: 10/20/2024 12:16:28 PM
 *  Author: ssali
 */ 

#ifndef ACCESS_CONTROL_H
#define ACCESS_CONTROL_H

#include <stdint.h>

void init_access_control(void);
void process_access_command(char* command);
uint8_t check_washroom_access(uint8_t tenant_id, uint8_t* code);
void set_temporary_access(uint8_t tenant_id, uint8_t* temp_code, uint16_t timeout);

#endif // ACCESS_CONTROL_H