#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "apolo_main.hpp"
#include "apl_storage.hpp"
#include "apl_camera.hpp"
#include "esp_timer.h"

#include "esp_sleep.h"
#include "apl_sdcard.hpp"

#define EVATUATION_BIN_TASK_STACK_SIZE (40 * 1024)
#define WEBSERVER_TASK_STACK_SIZE (15 * 1024)

static const char *APL_TAG_MAIN = "APOLO_MAIN";
static const char *APL_TAG_AWAKE_TIME = "TEMPO ACORDADO";

TaskHandle_t tEval_bin_handler = NULL;
TaskHandle_t tWebserver_handler = NULL;

task_parameters_t eval_bin_strc = {};
task_parameters_t webserver_strc = {};


uint64_t apl_start_awaken_time; // Placa acordada
uint64_t apl_stop_awaken_time;  // Placa A mimir
uint64_t apl_elapsed_time; // Tempo total que a placa fica acordada
uint64_t apl_elapsed_time_ms; // Tempo total convertido em ms

void vEvatuationBinTask(void *pvParameters)
{
    ESP_LOGI(APL_TAG_MAIN, ">>> INIT - avaliacao de estado de BIN");
    apl_wakeup_cause_t wakeup_cause;
    mias_configs_t mias_config;
    mias_services_e chosen_service;
    time_t now = 0;
    char now_str[TIMESTAMP_STRING_SIZE];
    


    multipurpose_button_bin_evaluation_init();
    // esp_deep_sleep(5000000);
    apl_check_n_init_webserver_on_startup();

    apl_read_configs(&mias_config);
    apl_wakeup_time_config((uint8_t)mias_config.wakeup_interval);
    apl_response_timeout_semaphore_init();
    // vTaskDelay(200);
    // apl_start_sleep();
    // vTaskDelay(200);
    
    while (1)
    {
        apl_btn_function_selection();
        apl_start_btn_listener();
        wakeup_cause = apl_wakeup_cause();

        if(wakeup_cause != APL_WAKEUP_INVALID_WAKEUP_CAUSE)
        {
            ESP_LOGI(APL_TAG_MAIN, "Avaliacao de BIN");
            if(apl_wifi_start() == APL_SUCCESS)
            {
                apl_get_time(&now, now_str);

                switch(wakeup_cause)
                {
                    case APL_WAKEUP_SLEEP_TIMER:
                        apl_request_for_service(now);
                        do
                        {
                            chosen_service = apl_get_service_response();
                            ESP_LOGI(APL_TAG_MAIN, "servico selecionado: (%d)", chosen_service);

                            switch (chosen_service)
                            {
                                case GO_TO_SLEEP:
                                    ESP_LOGI(APL_TAG_MAIN, "Selecao de retorno ao modo low power");
                                    break;
                                case CONFIGURE_MIAS_DEVICE:
                                    chosen_service = GO_TO_SLEEP;
                                    ESP_LOGI(APL_TAG_MAIN, "Selecao de configuracao de dispositivo");
                                    break;
                                case EVALUATE_BIN_STATUS:
                                    ESP_LOGI(APL_TAG_MAIN, "Selecao de avaliacao de estado de BIN");
                                    if(ApoloCamera::Get_camera_device().init() == EL_OK)
                                        apl_send_image(now, SYNCHRONOUS_BIN_ANALYSIS_REQUEST);
                                    else
                                        chosen_service = GO_TO_SLEEP;
                                    ApoloCamera::Get_camera_device().deinit();
                                    break;
                                case SEND_MIAS_DEVICE_CONFIGURATION_DATA:
                                    ESP_LOGI(APL_TAG_MAIN, "Selecao de envio de configuracoes ao servidor");
                                    apl_send_mias_device_configs();
                                    break;
                                default:
                                    ESP_LOGE(APL_TAG_MAIN, "Servico nao mapeado!");
                                    chosen_service = GO_TO_SLEEP;
                                    break;
                            }
                        } while(chosen_service != GO_TO_SLEEP);
                    break;
                    case APL_WAKEUP_MULTIPURPOSE_BUTTON:
                        if(ApoloCamera::Get_camera_device().init() == EL_OK)
                            apl_send_image(now, ASYNCHRONOUS_BIN_ANALYSIS_REQUEST);
                        ApoloCamera::Get_camera_device().deinit();
                    break;
                    case APL_WAKEUP_INVALID_WAKEUP_CAUSE:
                        ESP_LOGE(APL_TAG_MAIN, "wakeup cause nao mapeado!");
                    break;
                }
            }
            else
            {
                ESP_LOGW(APL_TAG_MAIN, "Falha na inicializacao do wifi - Retonando para modo de baixo consumo");
            }
            apl_wifi_deinit();
        }
        apl_stop_btn_listener();
        
        apl_stop_awaken_time = esp_timer_get_time();
        apl_elapsed_time = apl_stop_awaken_time - apl_start_awaken_time;
        apl_elapsed_time_ms = apl_elapsed_time/1000;
        ESP_LOGI(APL_TAG_AWAKE_TIME,"%llu ms",apl_elapsed_time_ms);
        // vTaskDelay(1000/portTICK_PERIOD_MS);
        // printf("Tempo acordado %llu\n",apl_elapsed_time_ms);

        //TODO Verificar a rotina da placa no modo lightsleep e deepsleep
        // vTaskDelay(100);
        apl_start_sleep();
        
        apl_start_awaken_time = esp_timer_get_time(); // Início do tempo acordado
        // vTaskDelay(50000/portTICK_PERIOD_MS);

    }
}

void vWebServerTask(void *pvParameters)
{
    ESP_LOGI(APL_TAG_MAIN, ">>> INIT - webserver");
    #if defined(APOLO_MIAS_PINOUT)
        esp_rom_gpio_pad_select_gpio(CAMERA_SUPPLY_PIN);
        gpio_set_direction(CAMERA_SUPPLY_PIN, GPIO_MODE_OUTPUT);
        gpio_set_level(CAMERA_SUPPLY_PIN, APL_STATE_HIGH);
        vTaskDelay(10);
    #endif
    apl_multipurpose_button_webserver_init();
    ApoloCamera::Get_camera_device().init();
    apl_wifi_nvs_init();
    apl_wifi_init_access_point();
    apl_start_webserver();

    while (1)
    {
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

static void init_apl_tasks(void)
{
    strcpy(eval_bin_strc.task_name, "eval_bin");
    eval_bin_strc.task_addr = &vEvatuationBinTask;
    eval_bin_strc.a_task_stack = EVATUATION_BIN_TASK_STACK_SIZE;
    eval_bin_strc.task_parameters = NULL;
    eval_bin_strc.task_priority = 1;
    eval_bin_strc.task_handle = &tEval_bin_handler;
    eval_bin_strc.task_status = TASK_UNINITIALIZED;

    strcpy(webserver_strc.task_name, "webserver");
    webserver_strc.task_addr = &vWebServerTask;
    webserver_strc.a_task_stack = WEBSERVER_TASK_STACK_SIZE;
    webserver_strc.task_parameters = NULL;
    webserver_strc.task_priority = 1;
    webserver_strc.task_handle = &tWebserver_handler;
    webserver_strc.task_status = TASK_UNINITIALIZED;

    apl_install_tasks_configs(&eval_bin_strc, &webserver_strc);
    apl_task_start(&eval_bin_strc);
}

void apolo_main_init(void)
{
    esp_log_level_set(APL_TAG_MAIN, ESP_LOG_VERBOSE);
    ESP_LOGI(APL_TAG_MAIN, "\n============================= APOLO =============================");
    apl_status_t apl_err;
    
    apl_err = apolo_system_init();
    if (apl_err == APL_SUCCESS)
    {
        init_apl_tasks();
        
    }
    else
    {
        ESP_LOGW(APL_TAG_MAIN, "Bloqueando operacao do sistema!");
        apl_system_block();
    }
}
