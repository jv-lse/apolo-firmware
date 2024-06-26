#ifndef APOLO_ESSENTIALS_H
#define APOLO_ESSENTIALS_H

#include "esp_log.h"

// LOGICAL STATES
#define APL_STATE_HIGH (1)
#define APL_STATE_LOW (0)

#define MIAS_CONFIG_SERVER_ADDR_MAX_LENGTH (50)
#define MIAS_CONFIG_SSID_MAX_LENGTH (32)
#define MIAS_CONFIG_PASS_MAX_LENGTH (32)

#define APL_SET_BIT(REG, BIT) ((REG) |= (1UL << BIT))
#define APL_CLR_BIT(REG, BIT) ((REG) &= ~(1UL << BIT))
#define APL_READ_BIT(REG, BIT) (((REG) >> (BIT)) & 1UL)

typedef enum
{
    APL_SUCCESS = 0,
    APL_FAIL,
    APL_TIMEOUT,
    APL_BUSY,
} apl_status_t;

#pragma pack(push)
#pragma pack(1)
typedef struct
{
    uint32_t wakeup_interval; // em segundos
    char server_addr[MIAS_CONFIG_SERVER_ADDR_MAX_LENGTH];
    char ssid[MIAS_CONFIG_SSID_MAX_LENGTH];
    char password[MIAS_CONFIG_PASS_MAX_LENGTH];
}mias_configs_t;
#pragma pack(pop)

#endif
