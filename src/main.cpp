/* Starting point. Will eventually include
 * * blinking LED (done)
 * * WiFi support - associate with AP (done)
 * * MQTT support  - publish (added, not working)
 * * NTP support - synchronize time
 *
 * Some code shamelessly copied from some examples
 * * espidf-blink example.
 * * station_example_main.c from getting_started/station
 * */

#include <driver/gpio.h>
#include <freertos/FreeRTOS.h>
#include <freertos/event_groups.h>
#include <freertos/task.h>
#include <esp_system.h>
#include <esp_event.h>
#include <esp_log.h>
#include <nvs_flash.h>

#include "wifi.h"
#include "mqtt.h"
#include "my_sntp.h"
#include "my_adc.h"

static const bool chatty=false;
static const adc1_channel_t channel_1 = ADC1_CHANNEL_0; // GPIO36
static const adc1_channel_t channel_2 = ADC1_CHANNEL_3; // GPIO39


// LED ========================================

static const gpio_num_t blink_led = (gpio_num_t)2;

void setup_LED(gpio_num_t gpio)
{
    gpio_pad_select_gpio(gpio);
    /* Set the GPIO as a push/pull output */
    gpio_set_direction(gpio, GPIO_MODE_OUTPUT);
}

void blink_led_task(void *param)
{
    while (1)
    {
        /* Blink off (output low) */
        if(chatty) printf("Turning off the LED\n");
        gpio_set_level(blink_led, 0);
        vTaskDelay(990 / portTICK_PERIOD_MS);
        /* Blink on (output high) */
        if(chatty) printf("Turning on the LED\n");
        gpio_set_level(blink_led, 1);
        vTaskDelay(10 / portTICK_PERIOD_MS);
    }
}


// main =======================================

static const char *TAG = "esp32 main";
/* Variable holding number of times ESP32 restarted since first boot.
 * It is placed into RTC memory using RTC_DATA_ATTR and
 * maintains its value when ESP32 wakes from deep sleep.
 * 
 * Assumes that WiFi connection is already established.
 */
RTC_DATA_ATTR static int boot_count = 0;


extern "C" void app_main()
{
    boot_count++;
    setup_LED(blink_led);
    bool calibration_enabled;
    //uint32_t ADC_Results;

    // Initialize NVS
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND)
    {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    // Note: 512 is insufficient stack for blink_led_task
    xTaskCreate(&blink_led_task, "blink_led_task", 1024, NULL, 5, NULL);

    // start WiFi
    ESP_LOGI(TAG, "ESP_WIFI_MODE_STA");
    init_wifi();
    mqtt_app_start();
    time_t t = init_sntp();
    ESP_LOGI(TAG,"init_sntp(): %ld", t);

    calibration_enabled = adc_calibration_init();

    // Following loop is superfluous. App continues to execute
    // even if app_main() exits
    int loop_counter = 0;
    while (1)
    {
        // Blink off (output low)
        if(chatty) printf("looping main\n");
        vTaskDelay(1000 / portTICK_PERIOD_MS);
        if((++loop_counter %10) == 0) // publish every ten seconds
        {
            static const int buf_len = 100;
            char    uptime_buff[buf_len];
            snprintf(uptime_buff, buf_len, "uptime %d, timestamp %ld, ch1 %d, ch2 %d",
                    loop_counter, time(0),
                    get_adc_reading(channel_1, calibration_enabled),
                    get_adc_reading(channel_2, calibration_enabled));
            mqtt_publish(NULL, uptime_buff);
        }
        /*
        ADC_Results = get_adc_reading(channel_1, calibration_enabled);
        ESP_LOGI(TAG, "first   reading: %d mV", ADC_Results);
        ADC_Results = get_adc_reading(channel_1, calibration_enabled);
        ESP_LOGI(TAG, "voltage reading: %d mV", ADC_Results);
        ADC_Results = get_adc_reading(channel_1, calibration_enabled);
        ESP_LOGI(TAG, "voltage reading: %d mV", ADC_Results);
        ADC_Results = get_adc_reading(channel_1, calibration_enabled);
        ESP_LOGI(TAG, "voltage reading: %d mV", ADC_Results);
        ADC_Results = get_adc_reading(channel_1, calibration_enabled);
        ESP_LOGI(TAG, "voltage reading: %d mV", ADC_Results);
        ESP_LOGI(TAG, " ");
        ADC_Results = get_adc_reading(channel_2, calibration_enabled);
        ESP_LOGI(TAG, "first   reading 2: %d mV", ADC_Results);
        ADC_Results = get_adc_reading(channel_2, calibration_enabled);
        ESP_LOGI(TAG, "voltage reading 2: %d mV", ADC_Results);
        ADC_Results = get_adc_reading(channel_2, calibration_enabled);
        ESP_LOGI(TAG, "voltage reading 2: %d mV", ADC_Results);
        ADC_Results = get_adc_reading(channel_2, calibration_enabled);
        ESP_LOGI(TAG, "voltage reading 2: %d mV", ADC_Results);
        ADC_Results = get_adc_reading(channel_2, calibration_enabled);
        ESP_LOGI(TAG, "voltage reading 2: %d mV", ADC_Results);
        ESP_LOGI(TAG, " ");
        */
   }
}