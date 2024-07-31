#include "ds18b20.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"

uint8_t ds18b20_init(ds18b20_handler_t *device, gpio_num_t pin, ds18b20_temp_res_t resolution)
{
    if (!device)
    {
        ESP_LOGW(TAG_DS18B20, "device is null!");

        return 0;
    }

    if (!onewire_init(&device->bus, pin))
    {
        ESP_LOGW(TAG_DS18B20, "Failed to initialize onewire bus");

        return 0;
    }

    device->res = resolution;

    // Configure resolution
    ds18b20_write_scratchpad(device);
    ds18b20_read_scratchpad(device);

    return 1;
}

void ds18b20_send_command(ds18b20_handler_t *device, ds18b20_commands_t command)
{
    uint8_t payload = 0x0 ^ command;

    onewire_write_byte(&device->bus, payload);
}

void ds18b20_convert_temp(ds18b20_handler_t *device)
{
    onewire_reset(&device->bus);
    onewire_send_command(&device->bus, _ROM_SKIP);

    ds18b20_send_command(device, _CONVERT_T);

    vTaskDelay(pdMS_TO_TICKS(ds18b20_temp_conv_time[device->res]));
}

void ds18b20_write_scratchpad(ds18b20_handler_t *device)
{
    onewire_reset(&device->bus);
    onewire_send_command(&device->bus, _ROM_SKIP);

    ds18b20_send_command(device, _SCRATCH_WRITE);

    // Th and Tl registers
    onewire_write_byte(&device->bus, 0);
    onewire_write_byte(&device->bus, 0);
    // Resolution value
    onewire_write_byte(&device->bus, ds18b20_resolution_val[device->res]);
}

void ds18b20_copy_scratchpad(ds18b20_handler_t *device)
{
    onewire_reset(&device->bus);
    onewire_send_command(&device->bus, _ROM_SKIP);

    ds18b20_send_command(device, _SCRATCH_COPY);
}

void ds18b20_read_scratchpad(ds18b20_handler_t *device)
{
    onewire_reset(&device->bus);
    onewire_send_command(&device->bus, _ROM_SKIP);

    ds18b20_send_command(device, _SCRATCH_READ);

    uint8_t i;
    for (i = 0; i < 9; i++)
    {
        device->scratchpad[i] = onewire_read_byte(&device->bus);
    }
}

void ds18b20_print_scratchpad(ds18b20_handler_t *device)
{
    uint8_t i;
    for (i = 0; i < 9; i++)
    {
        printf("%x ", device->scratchpad[i]);
    }

    printf("\n");
}

float ds18b20_read_temp(ds18b20_handler_t *device)
{
    ds18b20_convert_temp(device);
    ds18b20_read_scratchpad(device);

    uint8_t sign = 0x0;
    uint8_t lsb = device->scratchpad[0];
    uint8_t mask = 0xFF << (TEMP_RES_12_BIT - device->res);
    lsb &= mask; // Mask out last 3 bits accordingly
    uint8_t msb = device->scratchpad[1];

    sign = msb & 0x80;
    int16_t temp = 0x0;

    temp = lsb + (msb << 8);

    if (sign)
    {
        temp = ~(-temp) + 1; // Convert signed two complement's
    }

    return temp / 16.0;
}