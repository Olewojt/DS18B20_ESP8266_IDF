#ifndef DS18B20_H
#define DS18B20_H

#include "onewire.h"

static const char* TAG_DS18B20 = "DS18B20";
static const uint16_t ds18b20_temp_conv_time[] = {94, 188, 375, 750}; // ms
static const uint16_t ds18b20_resolution_val[] = {0x1F, 0x3F, 0x5F, 0x7F};

typedef enum {
    TEMP_RES_9_BIT  = 0,
    TEMP_RES_10_BIT = 1,
    TEMP_RES_11_BIT = 2,
    TEMP_RES_12_BIT = 3
} ds18b20_temp_res_t;

typedef enum {
    _SCRATCH_WRITE = 0x4E,
    _SCRATCH_READ  = 0xBE,
    _SCRATCH_COPY  = 0x48,
    _CONVERT_T     = 0x44
} ds18b20_commands_t;

typedef uint8_t ds18b20_scratchpad_t[9];

typedef struct {
    onewire_bus_handle_t bus;
    ds18b20_temp_res_t res;
    ds18b20_scratchpad_t scratchpad;
} ds18b20_handler_t;

uint8_t ds18b20_init(ds18b20_handler_t *device, gpio_num_t pin, ds18b20_temp_res_t resolution);
void ds18b20_send_command(ds18b20_handler_t *device, ds18b20_commands_t command);

void ds18b20_write_scratchpad(ds18b20_handler_t *device);
void ds18b20_read_scratchpad(ds18b20_handler_t *device);
void ds18b20_copy_scratchpad(ds18b20_handler_t *device);
void ds18b20_print_scratchpad(ds18b20_handler_t *device);

void ds18b20_convert_temp(ds18b20_handler_t *device);
float ds18b20_read_temp(ds18b20_handler_t *device);

#endif