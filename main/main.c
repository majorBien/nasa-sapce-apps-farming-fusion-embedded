

#include <stdio.h>
#include <inttypes.h>
#include <string.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"

#include "ra01s.h"
#include "lora.h"
#include "nvs_flash.h"
#include "driver/adc.h"
#include "esp_adc_cal.h"
#include "driver/gpio.h"

#define DEFAULT_VREF    1100        // Default reference voltage in mV
#define NO_OF_SAMPLES   64          // Number of samples for averaging



static const char *TAG = "MAIN";

//adc varriables
static esp_adc_cal_characteristics_t *adc_chars;
static const adc_channel_t channel = ADC_CHANNEL_6;
static const adc_channel_t channel2 = ADC_CHANNEL_7;
static const adc_channel_t channel3 = ADC_CHANNEL_4;
static const adc_atten_t atten = ADC_ATTEN_DB_0;
static const adc_unit_t unit = ADC_UNIT_1;
//adc varriables

double scaleXnormX(uint32_t x, uint32_t in_min, uint32_t in_max, double out_min, double out_max) {
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}
void AnalogSensors(void *pvParameters)
{
    double analog1 = 0.0;
    double analog2 = 0.0;
    double analog3 = 0.0;

    while (1) {
        uint32_t adc_reading = 0;
        uint32_t adc_reading2 = 0;
        uint32_t adc_reading3 = 0;

        // Multisampling for channel 1
        for (int i = 0; i < NO_OF_SAMPLES; i++) {
            adc_reading += adc1_get_raw((adc1_channel_t)channel);
        }
        adc_reading /= NO_OF_SAMPLES;

        // Multisampling for channel 2
        for (int i = 0; i < NO_OF_SAMPLES; i++) {
            adc_reading2 += adc1_get_raw((adc1_channel_t)channel2);
        }
        adc_reading2 /= NO_OF_SAMPLES;
        
        //Multisampling for channel 3
        for (int i = 0; i < NO_OF_SAMPLES; i++){
			adc_reading3 += adc1_get_raw((adc1_channel_t)channel3);
		}
		adc_reading3 /= NO_OF_SAMPLES;
		
        // Convert adc_reading to voltage in mV
        uint32_t voltage = esp_adc_cal_raw_to_voltage(adc_reading, adc_chars);
        uint32_t voltage2 = esp_adc_cal_raw_to_voltage(adc_reading2, adc_chars);
        uint32_t voltage3 = esp_adc_cal_raw_to_voltage(adc_reading3, adc_chars);

        analog1 = scaleXnormX(adc_reading, 0, 4096, 0, 40);
        ESP_LOGI(TAG, "analog1: %.1f", analog1);
        analog2 = scaleXnormX(adc_reading2, 0, 4096, 0, 40);
        ESP_LOGI(TAG, "analog2: %.1f", analog2);
        analog3 = scaleXnormX(adc_reading, 0, 4096, 0, 40);
        ESP_LOGI(TAG, "analog3: %.1f", analog3);

        vTaskDelay(pdMS_TO_TICKS(50));
    }
}




void app_main()
{

	ESP_LOGI(TAG, "APP START");
	esp_err_t ret = nvs_flash_init();
	if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND)
	{
		ESP_ERROR_CHECK(nvs_flash_erase());
		ret = nvs_flash_init();
	}
	ESP_ERROR_CHECK(ret);
	
	//loraInit();
	//loraTaskStart();	
	
	//adc
		// Configure ADC for channel 1
	    if (unit == ADC_UNIT_1) {
	        adc1_config_width(ADC_WIDTH_BIT_12);
	        adc1_config_channel_atten(channel, atten);
	    } else {
	        adc2_config_channel_atten((adc2_channel_t)channel, atten);
	    }

	    // Configure ADC for channel 2
	     if (unit == ADC_UNIT_1) {
	         adc1_config_channel_atten(channel2, atten);
	     } else {
	         adc2_config_channel_atten((adc2_channel_t)channel2, atten);
	     }
	     // Configure ADC dor channel 3
	     if(unit == ADC_UNIT_1){
			adc1_config_channel_atten((adc2_channel_t)channel3, atten);
		 } else{
			 adc2_config_channel_atten((adc2_channel_t)channel3, atten);
		 }

	    // Characterize ADC
	    adc_chars = calloc(1, sizeof(esp_adc_cal_characteristics_t));
	    esp_adc_cal_characterize(unit, atten, ADC_WIDTH_BIT_12, DEFAULT_VREF, adc_chars);
	
	xTaskCreatePinnedToCore(AnalogSensors, "AnalogSensors",1024*2, NULL, 0, 	NULL, 0);
	//adc
}

