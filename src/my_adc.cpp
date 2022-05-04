/* Provide code to read two ADC channels on ADC1. (ADC2 cannot be used when WiFi is
 * in use)
 * Cribbed from example single_read.c in .../examples/peripherals/adc/single_read/main
 */
extern "C"
{
#include <stdlib.h>
#include <esp_log.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <driver/adc.h>
}

#include "my_adc.h"

#define ADC1_EXAMPLE_CHAN0 ADC1_CHANNEL_6
#define desired_attenuation ADC_ATTEN_DB_11
static const char *TAG = "ADC";
static const adc_bits_width_t bit_width = ADC_WIDTH_BIT_12;

static esp_adc_cal_characteristics_t adc1_chars;

bool adc_calibration_init(void)
{
    esp_err_t ret;
    bool cali_enable = false;

    ret = esp_adc_cal_check_efuse(ESP_ADC_CAL_VAL_EFUSE_VREF);
    if (ret == ESP_ERR_NOT_SUPPORTED)
    {
        ESP_LOGW(TAG, "Calibration scheme not supported, skip software calibration");
    }
    else if (ret == ESP_ERR_INVALID_VERSION)
    {
        ESP_LOGW(TAG, "eFuse not burnt, skip software calibration");
    }
    else if (ret == ESP_OK)
    {
        ESP_LOGW(TAG, "software calibration enabled");
        cali_enable = true;
        esp_adc_cal_characterize(ADC_UNIT_1, desired_attenuation, bit_width, 0, &adc1_chars);
    }
    else
    {
        ESP_LOGE(TAG, "Invalid arg");
    }

    return cali_enable;
}

// see https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/peripherals/adc.html#_CPPv414adc1_channel_t
// for channel IDs

uint32_t get_adc_reading(adc1_channel_t channel, bool cali_enable)
{
    esp_err_t ret = ESP_OK;
    uint32_t voltage = 0;
    int i;
    static const int average_count = 6;

    ESP_ERROR_CHECK(adc1_config_width(ADC_WIDTH_BIT_12));
    ESP_ERROR_CHECK(adc1_config_channel_atten(channel, desired_attenuation));

    for (i = 0; i < average_count; i++)
    {
        do
        {
            ret = adc1_get_raw(channel);
        } while (ret == ESP_ERR_INVALID_STATE);

        if (cali_enable)
        {
            voltage += esp_adc_cal_raw_to_voltage(ret, &adc1_chars);
        }
        else
        {
            return ret;
        }
    }
    return (voltage + average_count / 2) / average_count;
}

/* Calculation of temperature baased on sample readings and curve fitting.
* https://github.com/HankB/BBQ-temp-publisher/blob/main/calibration/README.md
*/

double calc_temp(int reading) {
    double accumulator;

    accumulator = -1.385464e-14 * reading;
    accumulator = (1.203064e-10 + accumulator) * reading;
    accumulator = (-4.069134e-07 + accumulator) * reading;
    accumulator = (6.709066e-04 + accumulator) * reading;
    accumulator = (-6.062113e-01 + accumulator) * reading;
    accumulator += 4.916665e+02;

    return accumulator;
}