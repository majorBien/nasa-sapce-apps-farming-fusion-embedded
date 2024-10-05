/*
 * lora.c
 *
 *  Created on: 14 wrz 2024
 *      Author: majorbien
 */

#include <stdio.h>
#include <inttypes.h>
#include <string.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"

#include "ra01s.h"
#include "tasks_settings.h"

static const char *TAG = "MAIN";

void task_tx(void *pvParameters)
{
	ESP_LOGI(pcTaskGetName(NULL), "Start");
	uint8_t buf[256]; 
	while(1) {
		TickType_t nowTick = xTaskGetTickCount();
		int txLen = sprintf((char *)buf, "Hello World %"PRIu32, nowTick);
		ESP_LOGI(pcTaskGetName(NULL), "%d byte packet sent...", txLen);

		if (LoRaSend(buf, txLen, SX126x_TXMODE_SYNC) == false) {
			ESP_LOGE(pcTaskGetName(NULL),"LoRaSend fail");
		}
		int lost = GetPacketLost();
		if (lost != 0) {
			ESP_LOGW(pcTaskGetName(NULL), "%d packets lost", lost);
		}

		vTaskDelay(pdMS_TO_TICKS(1000));
	} 
}

void loraInit(void)
{
	uint32_t frequencyInHz = 0;

#if CONFIG_169MHZ
	frequencyInHz = 169000000;
	ESP_LOGI(TAG, "Frequency is 169MHz");
#elif CONFIG_433MHZ
	frequencyInHz = 433000000;
	ESP_LOGI(TAG, "Frequency is 433MHz");
#elif CONFIG_470MHZ
	frequencyInHz = 470000000;
	ESP_LOGI(TAG, "Frequency is 470MHz");
#elif CONFIG_866MHZ
	frequencyInHz = 866000000;
	ESP_LOGI(TAG, "Frequency is 866MHz");
#elif CONFIG_915MHZ
	frequencyInHz = 915000000;
	ESP_LOGI(TAG, "Frequency is 915MHz");
#elif CONFIG_OTHER
	ESP_LOGI(TAG, "Frequency is %dMHz", CONFIG_OTHER_FREQUENCY);
	frequencyInHz = CONFIG_OTHER_FREQUENCY * 1000000;
#endif

	LoRaInit();
	int8_t txPowerInDbm = 22;

	ESP_LOGW(TAG, "Enable TCXO");
	float tcxoVoltage = 3.3; 
	bool useRegulatorLDO = true; 

	if (LoRaBegin(frequencyInHz, txPowerInDbm, tcxoVoltage, useRegulatorLDO) != 0) {
		ESP_LOGE(TAG, "Does not recognize the module");
		while (1) {
			vTaskDelay(1);
		}
	}

	uint8_t spreadingFactor = 7;
	uint8_t bandwidth = 4;
	uint8_t codingRate = 1;
	uint16_t preambleLength = 8;
	uint8_t payloadLen = 0;
	bool crcOn = true;
	bool invertIrq = false;

	LoRaConfig(spreadingFactor, bandwidth, codingRate, preambleLength, payloadLen, crcOn, invertIrq);

}

void loraTaskStart(void)
{
	xTaskCreatePinnedToCore(&task_tx, "loraSend", LORA_SEND_STACK_SIZE, NULL, LORA_SEND_PRIORITY, NULL, LORA_SEND_CORE_ID);
}
