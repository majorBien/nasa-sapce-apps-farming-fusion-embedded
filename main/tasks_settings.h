/*
 * tasks_settings.h
 *
 *  Created on: 5 paź 2024
 *      Author: majorbien
 */

#ifndef MAIN_TASKS_SETTINGS_H_
#define MAIN_TASKS_SETTINGS_H_


//sensors task
#define SENSORS_STACK_SIZE           		4096
#define SENSORS_PRIORITY					1
#define SENSORS_CORE_ID						0


// LoraWAN Send task
#define LORA_SEND_STACK_SIZE				4096
#define LORA_SEND_PRIORITY					5
#define LORA_SEND_CORE_ID					0

// Wifi task
#define WIFI_STACK_SIZE					    2048	
#define WIFI_PRIORITY						4
#define WIFI_CORE_ID						1

//Http task
#define HTTP_STACK_SIZE						4096
#define HTTP_PRIORITY						3
#define HTTP_CORE_ID						1




#endif /* MAIN_TASKS_SETTINGS_H_ */