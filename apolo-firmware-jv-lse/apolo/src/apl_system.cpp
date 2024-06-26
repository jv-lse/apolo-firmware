#include "driver/rtc_io.h"
#include "driver/timer.h"
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "freertos/task.h"
#include "driver/uart.h"
#include "esp_pm.h"
#include "esp_rom_gpio.h"
#include "esp_sleep.h"
#include "esp_system.h"

#include "apl_storage.hpp"
#include "apl_system.hpp"
#include "apl_camera.hpp"
#include "apl_sdcard.hpp"

#include "apl_wifi.hpp"


#define START_SLEEP_TIMEOUT (100)

// GPIO DEFINITIONS
#define EXTERNAL_LED_PIN (GPIO_NUM_42)
#define MULTIPURPOSE_BUTTON_PIN (GPIO_NUM_1)

// TIMER RELATED
#define TIMER_BASE_CLK APB_CLK_FREQ
#define BTN_SECS_CONTER_TIMER_GROUP (TIMER_GROUP_0)
#define BTN_SECS_CONTER_TIMER_IDX (TIMER_0)
#define TIMER_DIVIDER (16)
#define TIMER_SCALE (TIMER_BASE_CLK / TIMER_DIVIDER)

#define WEBSERVER_SELECTION_TIMEOUT (3)
#define BTN_CLICK_TIMEOUT (1000)

typedef enum {
    APL_TIMER_TIME_COUNTER_EVENT = 0,
    APL_OPEN_WEBSERVER_EVENT = 1,
    APL_BTN_BUTTON_WEBSERVER_TAKE_PICTURE_EVENT = 2,

} system_events_e;

typedef enum {
    VALID_CLICK = 0,
    INVALID_CLICK = 1,
} click_authenticity_t;

typedef enum {
    BTN_INITIALISED = 0,
    BTN_NOT_INITIALISED = 1,
} btn_initialized_t;

static const char *SYSTEM_TAG = "SYSTEM";

// Inserir o ID do dispositivo aqui
static const char *CONFIG_ID_STRING = "";

static uint8_t seconds_counter = 0;
static volatile click_authenticity_t click_authenticity_flag = VALID_CLICK;

static SemaphoreHandle_t btn_timeout_sem = NULL;
static task_parameters_t *bin_evaluation_task_parameters = NULL;
static task_parameters_t *webserver_task_parameters = NULL;
TaskHandle_t tBtnListener_handler = NULL;

// SYSTEM FUNCTIONS
// =======================================================================================================================
static void expose_reset_reason(void)
{
    const char *SYSTEM_TAG = "RST_REASON";
    esp_err_t reset_reason = esp_reset_reason();

    switch (reset_reason) {
        case ESP_RST_POWERON:
            ESP_LOGW(SYSTEM_TAG, ">>>>> alimentação <<<<<");
            break;
        case ESP_RST_EXT:
            ESP_LOGW(SYSTEM_TAG, ">>>>> reinicio externo <<<<<");
            break;
        case ESP_RST_PANIC:
            ESP_LOGW(SYSTEM_TAG, ">>>>> erro fatal <<<<<");
            break;
        default:
            ESP_LOGW(SYSTEM_TAG, ">>>>> causa nao mapeada <<<<<");
            break;
    }
}

static void record_device_id(void)
{
#ifdef CONFIG_ENABLE_ID_RECORDING
    ESP_LOGD(SYSTEM_TAG, "ID Recording habilitado");
    apl_write_device_id(CONFIG_ID_STRING);
    ESP_LOGD(SYSTEM_TAG,
             "ID do dispositivo MIAS gravado! - por favor desabilitar a gravacao de ID de "
             "dispositvo no menuconfig");
#else
    ESP_LOGD(SYSTEM_TAG, "ID Recording desabilidtado");
    return;
#endif

    while (1) {
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}

static void seconds_counter_timer_start(void)
{
    ESP_ERROR_CHECK(timer_start(BTN_SECS_CONTER_TIMER_GROUP, BTN_SECS_CONTER_TIMER_IDX));
}

static void seconds_counter_timer_stop(void)
{
    ESP_ERROR_CHECK(timer_pause(BTN_SECS_CONTER_TIMER_GROUP, BTN_SECS_CONTER_TIMER_IDX));
    ESP_ERROR_CHECK(
        timer_set_counter_value(BTN_SECS_CONTER_TIMER_GROUP, BTN_SECS_CONTER_TIMER_IDX, 0));
}

static inline void restart_seconds_counter(void)
{
    seconds_counter = 0;
}

void apl_start_sleep(void)
{
    vTaskDelay(START_SLEEP_TIMEOUT);
    // esp_light_sleep_start(); 
    esp_deep_sleep_start(); 
}

void apl_wakeup_time_config(uint8_t wakeup_period)
{
    ESP_ERROR_CHECK(esp_sleep_enable_timer_wakeup(wakeup_period * 1000000));
    ESP_LOGD(SYSTEM_TAG, "Timer de wake up habilitado e configurado");
}

apl_wakeup_cause_t apl_wakeup_cause(void)
{
    esp_sleep_wakeup_cause_t wakeup_cause = esp_sleep_get_wakeup_cause();
    wakeup_cause = ESP_SLEEP_WAKEUP_TIMER;
    ESP_LOGI(SYSTEM_TAG, "[wake up] - Timer %d", wakeup_cause);
    switch(wakeup_cause)
    {
        case ESP_SLEEP_WAKEUP_TIMER:
            ESP_LOGI(SYSTEM_TAG, "[wake up] - Timer");
            return APL_WAKEUP_SLEEP_TIMER;
        break;
        case ESP_SLEEP_WAKEUP_GPIO:
            if((xSemaphoreTake(btn_timeout_sem, pdMS_TO_TICKS(BTN_CLICK_TIMEOUT)) == true))
            {
                xSemaphoreGive(btn_timeout_sem);
                ESP_LOGI(SYSTEM_TAG, "[wake up] - GPIO");
                return APL_WAKEUP_MULTIPURPOSE_BUTTON;
            }
            else
            {
                ESP_LOGW(SYSTEM_TAG, "[wake up] - Estouro de timeout de click do botao!");
            }
        break;
        default:
            ESP_LOGI(SYSTEM_TAG, "[wake up] - wakeup cause nao mapeado");
        break;
    }
    return APL_WAKEUP_INVALID_WAKEUP_CAUSE;
}

void apl_install_tasks_configs(task_parameters_t *incoming_bin_evaluation_task_parameters, task_parameters_t *incoming_webserver_task_parameters)
{
    if ((incoming_bin_evaluation_task_parameters->task_handle == NULL) ||
        (incoming_webserver_task_parameters->task_handle == NULL)) {
        ESP_LOGE(SYSTEM_TAG, "cadastro de configuracoes de task - sem task handle");
        return;
    }

    bin_evaluation_task_parameters = incoming_bin_evaluation_task_parameters;
    webserver_task_parameters = incoming_webserver_task_parameters;
}

void apl_task_start(task_parameters_t *task_parameters)
{
    if ((task_parameters->task_status == TASK_UNINITIALIZED) ||
        (task_parameters->task_status == TASK_DELETED)) {
        xTaskCreate(task_parameters->task_addr,
                    task_parameters->task_name,
                    task_parameters->a_task_stack,
                    task_parameters->task_parameters,
                    task_parameters->task_priority,
                    task_parameters->task_handle);

        task_parameters->task_status = TASK_ON_RUN;
    }
    else {
        ESP_LOGW(SYSTEM_TAG, "Incapaz de inicializar a task %s", task_parameters->task_name);
    }
}

static void apl_task_destroy(task_parameters_t *task_parameters)
{
    if (task_parameters->task_status == TASK_ON_RUN) {
        vTaskDelete(*(task_parameters->task_handle));
        *(task_parameters->task_handle) = NULL;
        task_parameters->task_status = TASK_DELETED;
    }
    else {
        ESP_LOGW(SYSTEM_TAG, "Incapaz de deletar a task %s", task_parameters->task_name);
    }
}

void apl_btn_function_selection(void)
{
    if (rtc_gpio_get_level(MULTIPURPOSE_BUTTON_PIN) == 0) {
        ESP_LOGD(SYSTEM_TAG, "botao pressionado");
        seconds_counter_timer_start();
        do {
            vTaskDelay(pdMS_TO_TICKS(10));
        } while (rtc_gpio_get_level(MULTIPURPOSE_BUTTON_PIN) == 0);

        ESP_LOGD(SYSTEM_TAG, "botao solto");
        seconds_counter_timer_stop();
        restart_seconds_counter();
        if ((seconds_counter < WEBSERVER_SELECTION_TIMEOUT) &&
            (bin_evaluation_task_parameters->task_status == TASK_ON_RUN))
            {
                xSemaphoreGive(btn_timeout_sem);
            }
        else if (seconds_counter > WEBSERVER_SELECTION_TIMEOUT) {
            apl_task_destroy(bin_evaluation_task_parameters);
            apl_task_start(webserver_task_parameters);
            ESP_LOGI(SYSTEM_TAG, "webserver em execucao");
        }
    }
}

void apl_system_block(void)
{
    while (1) {
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

void apl_check_n_init_webserver_on_startup(void)
{
    if (rtc_gpio_get_level(MULTIPURPOSE_BUTTON_PIN) == 0)
    {
        ESP_LOGW(SYSTEM_TAG, "Iniciando webserver task!");
        apl_task_start(webserver_task_parameters);
        apl_task_destroy(bin_evaluation_task_parameters);
    }
}

// TASKS RELATED
// =======================================================================================================================
void vBtnListener(void *pvParameters)
{
    while(1)
    {
        if (rtc_gpio_get_level(MULTIPURPOSE_BUTTON_PIN) == 0)
        {
            ESP_LOGI(SYSTEM_TAG, "[btn listener] - botao pressionado");
            seconds_counter_timer_start();
            do {
                vTaskDelay(pdMS_TO_TICKS(10));
            } while (rtc_gpio_get_level(MULTIPURPOSE_BUTTON_PIN) == 0);

            ESP_LOGI(SYSTEM_TAG, "[btn listener] - botao solto");
            seconds_counter_timer_stop();
            restart_seconds_counter();
            if ((seconds_counter < WEBSERVER_SELECTION_TIMEOUT) &&
                (bin_evaluation_task_parameters->task_status == TASK_ON_RUN))
                {
                    xSemaphoreGive(btn_timeout_sem);
                }
            else if (seconds_counter > WEBSERVER_SELECTION_TIMEOUT) {
                apl_task_destroy(bin_evaluation_task_parameters);
                apl_task_start(webserver_task_parameters);
                ESP_LOGI(SYSTEM_TAG, "[btn listener] - webserver em execucao");
                vTaskDelete(NULL);
            }
        }
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}

void apl_start_btn_listener(void)
{
    xTaskCreate(&vBtnListener, "btn listener", 3 * 1024, NULL, 1, &tBtnListener_handler);
}

void apl_stop_btn_listener(void)
{
    vTaskDelete(tBtnListener_handler);
}

// TODO: rotina de acionamento de watchdog

// ISR
// =======================================================================================================================
static void IRAM_ATTR gpio_isr_handler(void *arg)
{
    static btn_initialized_t web_server_btn_initialised = BTN_NOT_INITIALISED;

    if ((click_authenticity_flag == VALID_CLICK) && (web_server_btn_initialised == BTN_INITIALISED)) {
        click_authenticity_flag = INVALID_CLICK;
        esp_event_isr_post(
            APL_BTN_EVENT_SET, APL_BTN_BUTTON_WEBSERVER_TAKE_PICTURE_EVENT, NULL, 0, NULL);
    }
    else {
        web_server_btn_initialised = BTN_INITIALISED;
    }
}

static bool IRAM_ATTR secs_counter_timer_callback(void *args)
{
    BaseType_t high_task_awoken = pdFALSE;
    seconds_counter++;
    esp_event_isr_post(APL_TIMER_EVENT_SET, APL_TIMER_TIME_COUNTER_EVENT, NULL, 0, NULL);

    if (seconds_counter == WEBSERVER_SELECTION_TIMEOUT) {
        esp_event_isr_post(APL_TASK_SELECTION_EVENT_SET, APL_OPEN_WEBSERVER_EVENT, NULL, 0, NULL);
    }
    return (high_task_awoken == pdTRUE);
}

// EVENTS
// =======================================================================================================================
ESP_EVENT_DEFINE_BASE(APL_BTN_EVENT_SET);
static void btn_event_handler(void *handler_arg, esp_event_base_t base, int32_t id, void *event_data)
{
    switch (id) {
        case APL_BTN_BUTTON_WEBSERVER_TAKE_PICTURE_EVENT:
            vTaskDelay(pdMS_TO_TICKS(150));
            click_authenticity_flag = VALID_CLICK;
            break;
        default:
            ESP_LOGW(SYSTEM_TAG, "Evento de btn nao mapeado");
            break;
    }
}

ESP_EVENT_DEFINE_BASE(APL_TIMER_EVENT_SET);
static void timer_event_handler(void *handler_arg, esp_event_base_t base, int32_t id, void *event_data)
{
    if (id == APL_TIMER_TIME_COUNTER_EVENT) {
        ESP_LOGI(SYSTEM_TAG, "tempo atual: %d", seconds_counter);
    }
    else {
        ESP_LOGW(SYSTEM_TAG, "Evento de timer nao mapeado");
    }
}

ESP_EVENT_DEFINE_BASE(APL_TASK_SELECTION_EVENT_SET);
static void btn_selection_event_handler(void *handler_arg, esp_event_base_t base, int32_t id, void *event_data)
{
    if (id == APL_OPEN_WEBSERVER_EVENT) {
        seconds_counter_timer_stop();
        apl_task_destroy(bin_evaluation_task_parameters);
        apl_task_start(webserver_task_parameters);
        ESP_LOGI(SYSTEM_TAG, "webserver em execucao");
    }
    else {
        ESP_LOGW(SYSTEM_TAG, "Evento de selecao de funcao nao mapeado");
    }
}

// INITS
// ======================================================================================================================
static apl_status_t seconds_counter_timer_init(void)
{
    esp_err_t err;

    static timer_config_t counter_timer_cfg = {};
    counter_timer_cfg.alarm_en = TIMER_ALARM_EN;
    counter_timer_cfg.counter_en = TIMER_PAUSE;
    counter_timer_cfg.counter_dir = TIMER_COUNT_UP;
    counter_timer_cfg.auto_reload = TIMER_AUTORELOAD_EN;
    counter_timer_cfg.divider = TIMER_DIVIDER;

    err = timer_init(BTN_SECS_CONTER_TIMER_GROUP, BTN_SECS_CONTER_TIMER_IDX, &counter_timer_cfg);
    if (err == ESP_OK) {
        timer_set_counter_value(BTN_SECS_CONTER_TIMER_GROUP, BTN_SECS_CONTER_TIMER_IDX, 0);
        timer_set_alarm_value(BTN_SECS_CONTER_TIMER_GROUP, BTN_SECS_CONTER_TIMER_IDX, TIMER_SCALE);
        timer_enable_intr(TIMER_GROUP_0, TIMER_0);
        err = timer_isr_callback_add(TIMER_GROUP_0, TIMER_0, secs_counter_timer_callback, NULL, 0);
        if (err != ESP_OK) {
            ESP_LOGE(SYSTEM_TAG, "falha em registrar callback de timer");
            return APL_FAIL;
        }

        return APL_SUCCESS;
    }
    else {
        ESP_LOGE(SYSTEM_TAG, "Falha na inicializacao de timer");
        return APL_FAIL;
    }
}

static void event_init(void)
{
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    ESP_ERROR_CHECK(esp_event_handler_instance_register(APL_TASK_SELECTION_EVENT_SET, APL_OPEN_WEBSERVER_EVENT, &btn_selection_event_handler, NULL, NULL));
    ESP_ERROR_CHECK(esp_event_handler_instance_register(APL_TIMER_EVENT_SET, APL_TIMER_TIME_COUNTER_EVENT, &timer_event_handler, NULL, NULL));
    ESP_ERROR_CHECK(esp_event_handler_instance_register(APL_BTN_EVENT_SET,APL_BTN_BUTTON_WEBSERVER_TAKE_PICTURE_EVENT,&btn_event_handler, NULL, NULL));

    ESP_LOGI(SYSTEM_TAG, "Eventos inicializados");
}

static void external_led_init(void)
{
    esp_rom_gpio_pad_select_gpio(EXTERNAL_LED_PIN);
    gpio_set_direction(EXTERNAL_LED_PIN, GPIO_MODE_OUTPUT);
    gpio_set_level(EXTERNAL_LED_PIN, APL_STATE_LOW);
}

void multipurpose_button_bin_evaluation_init(void)
{
    gpio_config_t io_conf = {};

    io_conf.intr_type = GPIO_INTR_DISABLE;
    io_conf.pin_bit_mask = (1 << MULTIPURPOSE_BUTTON_PIN);
    io_conf.mode = GPIO_MODE_INPUT;
    io_conf.pull_up_en = GPIO_PULLUP_ENABLE;
    gpio_config(&io_conf);
    ESP_ERROR_CHECK(gpio_pullup_en(MULTIPURPOSE_BUTTON_PIN));

    ESP_ERROR_CHECK(gpio_wakeup_enable(MULTIPURPOSE_BUTTON_PIN, GPIO_INTR_LOW_LEVEL));
    // ESP_ERROR_CHECK(esp_sleep_enable_gpio_wakeup());

    ESP_LOGI(SYSTEM_TAG, "[EVAL_BIN] botao multiproposito inicializado");
}

void apl_multipurpose_button_webserver_init(void)
{
    gpio_config_t io_conf = {};

    io_conf.intr_type = GPIO_INTR_POSEDGE;
    io_conf.pin_bit_mask = (1 << MULTIPURPOSE_BUTTON_PIN);
    io_conf.mode = GPIO_MODE_INPUT;
    io_conf.pull_up_en = GPIO_PULLUP_ENABLE;
    gpio_config(&io_conf);

    ESP_ERROR_CHECK(gpio_install_isr_service(ESP_INTR_FLAG_LEVEL2));
    ESP_ERROR_CHECK(gpio_isr_handler_add(MULTIPURPOSE_BUTTON_PIN, &gpio_isr_handler, NULL));
    ESP_LOGI(SYSTEM_TAG, "[WEBSERVER] botao multiproposito inicializado");
}

static void apl_btn_timeout_semaphore_init(void)
{
    btn_timeout_sem = xSemaphoreCreateBinary();
}

apl_status_t apolo_system_init(void)
{
    esp_log_level_set(SYSTEM_TAG, ESP_LOG_VERBOSE);
    expose_reset_reason();
    external_led_init();
    apl_init_spiffs();
    record_device_id();
    apl_btn_timeout_semaphore_init();

    if(seconds_counter_timer_init() != APL_SUCCESS)
    {
        ESP_LOGE(SYSTEM_TAG, "Falha ao inicializar o sistema");
        return APL_FAIL;
    }
    event_init();
    return APL_SUCCESS;
}
