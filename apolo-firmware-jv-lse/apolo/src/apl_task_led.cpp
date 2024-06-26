#include "apl_task_led.hpp"

const char *LED = "Pisca led";

// Handle para task blinkled tentativa wifi
 TaskHandle_t led_connect_fail_handle = NULL;

void led_webserver(void *pvParameter) {
    esp_rom_gpio_pad_select_gpio(PIN_LED);
    gpio_set_direction(PIN_LED,GPIO_MODE_OUTPUT);
    led_off();
    while (1) {
        for(uint8_t i=0;i<2;i++)
        {
            // printf("Minha Tarefa em execucao!\n");
            gpio_set_level(PIN_LED, 1); // Acende o LED
            vTaskDelay(pdMS_TO_TICKS(100)); 
            gpio_set_level(PIN_LED, 0); // Apaga o LED
            vTaskDelay(pdMS_TO_TICKS(100)); 
        }
        gpio_set_level(PIN_LED, 0); // Apaga o LED
        vTaskDelay(pdMS_TO_TICKS(1000)); 
    }
}
void led_connect_fail(void *pvParameter)
{
    esp_rom_gpio_pad_select_gpio(PIN_LED);
    gpio_set_direction(PIN_LED,GPIO_MODE_OUTPUT);
    while (1) 
    {
        printf("Falha na conexão!\n");
        gpio_set_level(PIN_LED, 1); // Acende o LED
        vTaskDelay(pdMS_TO_TICKS(1000)); 
        gpio_set_level(PIN_LED, 0); // Apaga o LED
        vTaskDelay(pdMS_TO_TICKS(1000)); 
    }
        gpio_set_level(PIN_LED, 0); // Apaga o LED
        vTaskDelay(pdMS_TO_TICKS(1000));
}
// Task para sinalização de falha de conexão
void apl_blinkled_connect_fail(void)
{
    xTaskCreate(&led_connect_fail, LED, 2048, NULL, 1, &led_connect_fail_handle);
}

// Task para sinalização do funcionamento do Webserver
void apl_blinkled_webserver(void)
{
    xTaskCreate(&led_webserver, LED, 2048, NULL, 1, NULL);
}

void led_off(void)
{
    gpio_set_level(PIN_LED, 0);
}

void led_on(void)
{
    gpio_set_level(PIN_LED, 1);
}

void apl_delete_blinkled_wifi(void)
{
    vTaskDelete(led_connect_fail_handle); // Silas não existe mais
}


// Função para falha de conexão com o servidor

void server_fail_connect(void)
{
    for(uint8_t i=0;i<20;i++)
        {
            // printf("Minha Tarefa em execucao!\n");
            gpio_set_level(PIN_LED, 1); // Acende o LED
            vTaskDelay(pdMS_TO_TICKS(50)); 
            gpio_set_level(PIN_LED, 0); // Apaga o LED
            vTaskDelay(pdMS_TO_TICKS(50)); 
        }
    
}
// void app_main(void)
// {
//     // xTaskCreate(&blink_led, LED, 2048, NULL, 1, NULL);
//     apl_blinkled_webserver();
// }