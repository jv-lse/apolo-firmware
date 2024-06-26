#ifndef APL_TASK_LED_H
#define APL_TASK_LED_H

#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_log.h"
// #include "led_strip.h"
#include "sdkconfig.h"


#define PIN_LED GPIO_NUM_42 // GPIO do LED (substitua pelo GPIO desejado)

void led_webserver(void *pvParameter);
void led_connect_fail(void *pvParameter);
void apl_blinkled_connect_fail(void);
void apl_blinkled_webserver(void);
void led_off(void);
void led_on(void);
void apl_delete_blinkled_wifi(void);
void server_fail_connect(void);


#endif