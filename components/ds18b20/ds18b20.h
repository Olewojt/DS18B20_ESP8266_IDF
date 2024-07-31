#ifndef DS18B20_H
#define DS18B20_H

#include "onewire.h"

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

/**
 * @brief Initialize DS18B20
 * 
 * @param device DS18B20 handler
 * @param pin Data pin
 * @param resolution Temperature resolution
 * 
 * @retval 1: Success
 * @retval 0: Incorrect pin or gpio configuration failed (Logs tells which happened)
 */
uint8_t ds18b20_init(ds18b20_handler_t *device, gpio_num_t pin, ds18b20_temp_res_t resolution);

/**
 * @brief Send command to DS18B20
 * 
 * @param device DS18B20 handler
 * @param command Function command 
 */
void ds18b20_send_command(ds18b20_handler_t *device, ds18b20_commands_t command);

/**
 * @brief Write to scratchpad
 * 
 * @param device DS18B20 handler
 */
void ds18b20_write_scratchpad(ds18b20_handler_t *device);

/**
 * @brief Read from scratchpad
 * 
 * @param device DS18B20 handler
 */
void ds18b20_read_scratchpad(ds18b20_handler_t *device);

/**
 * @brief Copy to scratchpad
 * 
 * @param device DS18B20 handler
 */
void ds18b20_copy_scratchpad(ds18b20_handler_t *device);

/**
 * @brief Print scratchpad bytes
 * 
 * @param device DS18B20 handler
 */
void ds18b20_print_scratchpad(ds18b20_handler_t *device);

/**
 * @brief Initialize temperature conversion and wait for conversion
 * 
 * Function sends CONV_T command and waits for X ms according to `ds18b20_temp_conv_time` static array
 * 
 * @warning Should be called before `ds18b20_convert_temp()` function
 * 
 * @param device DS18B20 handler
 */
void ds18b20_convert_temp(ds18b20_handler_t *device);

/**
 * @brief Read temperature from scratchpad
 * 
 * Function reads temperature from scratchpad and converts it to Celsius.
 * @warning `ds18b20_convert_temp()` have to be called before for updated temperature.
 * 
 * @param device DS18B20 handler
 */
float ds18b20_read_temp(ds18b20_handler_t *device);

#endif