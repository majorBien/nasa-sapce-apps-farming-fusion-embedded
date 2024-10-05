/*
 * analog.c
 *
 *  Created on: 5 paź 2024
 *      Author: majorbien
 */

#include "sensors.h"
#include <bmp280.h>
#include <string.h>
#include "tasks_settings.h"


#define DEFAULT_VREF    1100        // Default reference voltage in mV
#define NO_OF_SAMPLES   64          // Number of samples for averaging


void sensorsInit(void){
		//bme280 error check
		ESP_ERROR_CHECK(i2cdev_init());	
	
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
	
}

double scaleXnormX(uint32_t x, uint32_t in_min, uint32_t in_max, double out_min, double out_max) {
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}


void Sensors(void *pvParameters)
{
	//bme280
	bmp280_params_t params;
    bmp280_init_default_params(&params);
    bmp280_t dev;
    memset(&dev, 0, sizeof(bmp280_t));

    ESP_ERROR_CHECK(bmp280_init_desc(&dev, BMP280_I2C_ADDRESS_0, 0, 17, 16));
    ESP_ERROR_CHECK(bmp280_init(&dev, &params));

    bool bme280p = dev.id == BME280_CHIP_ID;
    printf("BMP280: found %s\n", bme280p ? "BME280" : "BMP280");

    float pressure, temperature, humidity;
	
	
	
	//analog
    double analog1 = 0.0;
    double analog2 = 0.0;
    double analog3 = 0.0;

    while (1) {
		//analog
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

        analog1 = scaleXnormX(adc_reading, 0, 4096, 0, 100);
        ESP_LOGI(TAG, "analog1: %.1f", analog1);
        analog2 = scaleXnormX(adc_reading2, 0, 4096, 0, 100);
        ESP_LOGI(TAG, "analog2: %.1f", analog2);
        analog3 = scaleXnormX(adc_reading3, 0, 4096, 0, 100);
        ESP_LOGI(TAG, "analog3: %.1f", analog3);
        
        //bme280
        if (bmp280_read_float(&dev, &temperature, &pressure, &humidity) != ESP_OK)
        {
            printf("Temperature/pressure reading failed\n");
            continue;
        }
 
        printf("Pressure: %.2f Pa, Temperature: %.2f C", pressure, temperature);
        if (bme280p)
            printf(", Humidity: %.2f\n", humidity);
        else
            printf("\n");

        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

void sensorsTaskStart(void){
		xTaskCreatePinnedToCore(&Sensors, "Sensors", SENSORS_STACK_SIZE, NULL, SENSORS_PRIORITY, NULL, SENSORS_CORE_ID);
	
}