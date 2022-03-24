#ifndef MY_ADC_H
#define MY_ADC_H

#include <esp_adc_cal.h>

bool adc_calibration_init(void);
uint32_t get_adc_reading(adc1_channel_t channel, bool cali_enable);
#endif // MY_ADC_H