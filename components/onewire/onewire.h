#ifndef ONEWIRE_H
#define ONEWIRE_H

#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "driver/gpio.h"
#include "esp_types.h"
#include "esp_err.h"

#define _ONEWIRE_WRITE1_LOW      6
#define _ONEWIRE_WRITE1_WAIT     64
#define _ONEWIRE_WRITE0_LOW      60
#define _ONEWIRE_WRITE0_WAIT     10
#define _ONEWIRE_READ_WAIT       9
#define _ONEWIRE_READ_RECOVERY   55
#define _ONEWIRE_RESET_WAIT      480
#define _ONEWIRE_PRESENCE_WAIT   70
#define _ONEWIRE_RESET_RECOVERY  410

#define _BLOCK_TIME      pdMS_TO_TICKS(1000)
#define _SEMFAIL_MSG     "Failed to obtain semaphore. (%s)"

static const char *TAG_ONEWIRE = "ONEWIRE";

typedef enum {
    _ROM_READ   = 0x33,
    _ROM_SEARCH = 0xF0,
    _ROM_MATCH  = 0x55,
    _ROM_SKIP   = 0xCC
} onewire_rom_commands_t;

typedef struct {
    gpio_num_t pin;
    SemaphoreHandle_t mutex;
} onewire_bus_handle_t;

/**
 * @brief Configure gpio pins for onewire communication
 * 
 * Set `custom_config` to NULL for default config.
 * 
 * @param pin Bus pin
 * @param custom_config Custom gpio config
 * 
 * @retval 1: Success
 * @retval 0: Incorrect pin or gpio configuration failed (Logs tells which happened)
 */
uint8_t onewire_configure_gpio(gpio_num_t pin, gpio_config_t *custom_config);

/**
 * @brief Initalize onewire bus
 * 
 * Set `custom_config` to NULL for default config.
 * @warning MUST be called before any other library function!
 * 
 * @param bus Bus handle 
 * @param pin Bus pin
 * @param custom_config Custom gpio config
 * 
 * @retval 1: Success
 * @retval 0: `bus` is NULL or gpio configuration failed (Logs tells which happened)
 */
uint8_t onewire_init(onewire_bus_handle_t *bus, gpio_num_t bus_pin, gpio_config_t *custom_config);

/**
 * @brief Send reset pulse
 * 
 * @param bus Bus handle
 * 
 * @retval 1: Success (device sent presence pulse)
 * @retval -1: Failed to obtain semaphore for gpio handling
 * @retval 0: Device failed to return presence pulse
 */
uint8_t onewire_reset(onewire_bus_handle_t *bus);

/**
 * @brief Write bit
 * 
 * @param bus Bus handle
 * @param bit Bit to send
 */
void onewire_write_bit(onewire_bus_handle_t *bus, uint8_t bit);

/**
 * @brief Write byte
 * 
 * @param bus Bus handle
 * @param bit Byte to send
 */
void onewire_write_byte(onewire_bus_handle_t *bus, uint8_t byte);

/**
 * @brief Read bit
 * 
 * @param bus Bus handle
 * 
 * @retval 1: Device returned 1
 * @retval 0: Device returned 0
 * @retval -1: Failed to obtain semaphore for gpio handling
 */
uint8_t onewire_read_bit(onewire_bus_handle_t *bus);

/**
 * @brief Read bit
 * 
 * @param bus Bus handle
 * 
 * @return Byte returned by device
 */
uint8_t onewire_read_byte(onewire_bus_handle_t *bus);

/**
 * @brief Send command to device
 * 
 * @param bus Bus handle
 * @param command Onewire rom command
 * 
 */
void onewire_send_command(onewire_bus_handle_t *bus, onewire_rom_commands_t command);

#endif