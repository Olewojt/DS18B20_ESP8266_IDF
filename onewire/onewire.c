#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "rom/ets_sys.h"
#include "onewire.h"

uint8_t onewire_configure_gpio(gpio_num_t pin, gpio_config_t *custom_config)
{
    if (!GPIO_IS_VALID_GPIO(pin))
    {
        ESP_LOGE(TAG_ONEWIRE, "Provided pin is incorrect!");

        return 0;
    }

    gpio_config_t config = {};

    if (!custom_config)
    {
        config.intr_type = GPIO_INTR_DISABLE;
        config.mode = GPIO_MODE_OUTPUT_OD;
        config.pin_bit_mask = ((uint32_t) 1 << pin);
        config.pull_down_en = 0;
        config.pull_up_en = 0;
    } 
    else
    {
        config = *custom_config;
    }

    if (gpio_config(&config) != ESP_OK)
    {
        return 0;
    }
    
    return 1;
}

uint8_t onewire_init(onewire_bus_handle_t *bus, gpio_num_t bus_pin, gpio_config_t *custom_config) 
{
    if (!bus)
    {
        ESP_LOGW(TAG_ONEWIRE, "bus is null! (onewire_init)");
        
        return 0;
    }

    bus->pin = bus_pin;
    bus->mutex = xSemaphoreCreateMutex();

    // configure GPIO
    if(!onewire_configure_gpio(bus_pin, custom_config))
    {
        return 0;
    }

    return 1;
}

uint8_t onewire_reset(onewire_bus_handle_t *bus)
{
    uint8_t presence;
    
    if (xSemaphoreTake(bus->mutex, _BLOCK_TIME))
    {
        gpio_set_level(bus->pin, 0); // Send reset pulse
        ets_delay_us(_ONEWIRE_RESET_WAIT);

        gpio_set_level(bus->pin, 1); // Leave floating
        ets_delay_us(_ONEWIRE_PRESENCE_WAIT);
        
        presence = !gpio_get_level(bus->pin);
        
        xSemaphoreGive(bus->mutex);
    } 
    else
    {
        ESP_LOGE(TAG_ONEWIRE, _SEMFAIL_MSG, "onewire_reset");

        return -1;
    }

    ets_delay_us(_ONEWIRE_RESET_RECOVERY);

    return presence;
}

void onewire_write_bit(onewire_bus_handle_t *bus, uint8_t bit)
{
    if (xSemaphoreTake(bus->mutex, _BLOCK_TIME))
    {
        if (bit)
        {
            // Write 1
            gpio_set_level(bus->pin, 0);
            ets_delay_us(_ONEWIRE_WRITE1_LOW);

            gpio_set_level(bus->pin, 1);
            ets_delay_us(_ONEWIRE_WRITE1_WAIT);
        }
        else
        {
            // Write 0
            gpio_set_level(bus->pin, 0);
            ets_delay_us(_ONEWIRE_WRITE0_LOW);

            gpio_set_level(bus->pin, 1);
            ets_delay_us(_ONEWIRE_WRITE0_WAIT);
        }

        xSemaphoreGive(bus->mutex);
    }
    else
    {
        ESP_LOGE(TAG_ONEWIRE, _SEMFAIL_MSG, "onewire_write_bit");
    }
}

uint8_t onewire_read_bit(onewire_bus_handle_t *bus)
{
    uint8_t bit;

    if (xSemaphoreTake(bus->mutex, _BLOCK_TIME))
    {
        gpio_set_level(bus->pin, 0);
        ets_delay_us(_ONEWIRE_WRITE1_LOW);

        gpio_set_level(bus->pin, 1);
        ets_delay_us(_ONEWIRE_READ_WAIT);

        bit = !gpio_get_level(bus->pin);
        
        xSemaphoreGive(bus->mutex);

        ets_delay_us(_ONEWIRE_READ_RECOVERY);
    }
    else
    {
        ESP_LOGE(TAG_ONEWIRE, _SEMFAIL_MSG, "onewire_read_bit");

        return -1;
    }

    return bit;
}

void onewire_write_byte(onewire_bus_handle_t *bus, uint8_t byte)
{
    uint8_t i;

    for (i = 0; i < 8; i++)
    {
        onewire_write_bit(bus, (byte >> i) & 0x01);
    }
}

uint8_t onewire_read_byte(onewire_bus_handle_t *bus)
{
    uint8_t i;
    uint8_t byte = 0x0;

    for (i = 0; i < 8; i++)
    {
        byte |= (!onewire_read_bit(bus) << i);
    }

    return byte;
}

void onewire_send_command(onewire_bus_handle_t *bus, onewire_rom_commands_t command)
{
    uint8_t payload = 0x0 ^ command;

    onewire_write_byte(bus, payload);
}