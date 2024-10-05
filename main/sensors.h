/*
 * analog.h
 *
 *  Created on: 5 paź 2024
 *      Author: majorbien
 */

#ifndef MAIN_ANALOG_H_
#define MAIN_ANALOG_H_

#include <stdio.h>
#include <inttypes.h>
#include <string.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "driver/adc.h"
#include "esp_adc_cal.h"
#include "driver/gpio.h"
#include "tasks_settings.h"
static const char *TAG = "ANALOG";


//adc varriables
static esp_adc_cal_characteristics_t *adc_chars;
static const adc_channel_t channel = ADC_CHANNEL_6;
static const adc_channel_t channel2 = ADC_CHANNEL_7;
static const adc_channel_t channel3 = ADC_CHANNEL_4;
static const adc_atten_t atten = ADC_ATTEN_DB_0;
static const adc_unit_t unit = ADC_UNIT_1;
//adc varriables

void sensorsInit(void);

double scaleXnormX(uint32_t x, uint32_t in_min, uint32_t in_max, double out_min, double out_max);

void sensorsTaskStart(void);



#endif /* MAIN_ANALOG_H_ */
