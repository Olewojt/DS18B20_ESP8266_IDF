# DS18B20 Component
Simple DS18B20 temperature sensor library for [ESP8266 RTOS SDK](https://github.com/espressif/ESP8266_RTOS_SDK) for reading Celsius temperature with different resolutions from singular device.

## Usage
```
// Create variable for handler
ds18b20_handler_t sensor;

// Check for any initialization failures
if (!ds18b20_init(&sensor, GPIO_NUM_12, TEMP_RES_12_BIT))        
{
    ESP_LOGE("TAG", "Failed to initalize DS18B20!");

    return 0; // Exit
} 

float temp = 0;

// Initalize conversion
ds18b20_convert_temp(&sensor);

// If you doesn't convert temperature you may read 85.0 Celsius, 
// as it is default temperature set by DS18B20 if convert command wasn't issued.
temp = ds18b20_read_temp(&sensor); // Read temperature

// Print temperature with 4 decimal places 
// (12 bit resolution measurement accuracy is 0.0625 Celsius)
ESP_LOGI("TAG", "Temperature = %.4f", temp);
```

> **_NOTE:_**  If last statement doesn't print temperature you may have to disable Newlib nano in `menuconfig` of RTOS SDK.