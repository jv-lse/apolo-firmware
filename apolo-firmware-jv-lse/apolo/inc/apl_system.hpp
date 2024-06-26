#ifndef APOLO_SYSTEM_H
#define APOLO_SYSTEM_H

#include "esp_event.h"
#include "apolo_essentials.h"

#define TASK_NAME_LENGTH (20)

// #define CONFIG_ENABLE_ID_RECORDING

ESP_EVENT_DECLARE_BASE(APL_TASK_SELECTION_EVENT_SET);
ESP_EVENT_DECLARE_BASE(APL_TIMER_EVENT_SET);
ESP_EVENT_DECLARE_BASE(APL_BTN_EVENT_SET);
ESP_EVENT_DECLARE_BASE(APL_BTN_EVALBIN_ISR);

typedef enum
{
  TASK_ON_RUN,
  TASK_DELETED,
  TASK_UNINITIALIZED,
} apl_task_status_t;

typedef enum
{
  APL_WAKEUP_SLEEP_TIMER = 0,
  APL_WAKEUP_MULTIPURPOSE_BUTTON = 1,
  APL_WAKEUP_INVALID_WAKEUP_CAUSE = 2,
} apl_wakeup_cause_t;

typedef struct
{
  TaskFunction_t task_addr;
  char task_name[TASK_NAME_LENGTH];
  uint32_t a_task_stack;
  void *task_parameters;
  UBaseType_t task_priority;
  TaskHandle_t *task_handle;
  apl_task_status_t task_status;
} task_parameters_t;


void apl_wakeup_time_config(uint8_t wakeup_period);
void apl_btn_function_selection(void);
void apl_start_sleep(void);
apl_wakeup_cause_t apl_wakeup_cause(void);
void apl_system_block(void);

void apl_install_tasks_configs(task_parameters_t *incoming_bin_evaluation_task_parameters, task_parameters_t *incoming_webserver_task_parameters);
void apl_task_start(task_parameters_t *task_parameters);

void multipurpose_button_bin_evaluation_init(void);
void apl_multipurpose_button_webserver_init(void);
void apl_check_n_init_webserver_on_startup(void);

void apl_start_btn_listener(void);
void apl_stop_btn_listener(void);

apl_status_t apolo_system_init(void);

#endif
