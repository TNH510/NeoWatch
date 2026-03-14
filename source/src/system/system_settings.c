/**
 * @file       system_settings.c
 * @copyright  Copyright (C) 2025 NEO. All rights reserved.
 * @license    This project is released under the NEO License.
 * @version    1.0.0
 * @date       2025-10
 * @author     Hieu Tran Ngoc
 * @brief      System Settings
 * @note       None
 */

/* Includes ----------------------------------------------------------- */
#include "system_settings.h"

#include "bsp_flash.h"
#include "esp_log.h"

/* Private defines ---------------------------------------------------- */
static const char *TAG = "SYS_SETTINGS";

#define SETTINGS_NVS_NAMESPACE      "sys_settings"
#define SETTINGS_KEY_BRIGHTNESS     "brightness"
#define SETTINGS_KEY_CLOCK_TYPE     "clock_type"
#define SETTINGS_KEY_TIME_ZONE      "time_zone"

/* Default settings values */
#define SETTINGS_DEFAULT_BRIGHTNESS (50)
#define SETTINGS_DEFAULT_CLOCK_TYPE (SYSTEM_SETTINGS_CLOCK_TYPE_LEFT)
#define SETTINGS_DEFAULT_TIME_ZONE  (0)

/* Private enumerate/structure ---------------------------------------- */
/* Private macros ----------------------------------------------------- */
/* Public variables --------------------------------------------------- */
/* Private variables -------------------------------------------------- */
static system_settings_t s_settings;

/* Private function prototypes ---------------------------------------- */
static void          system_settings_set_defaults(void);
static base_status_t system_settings_save_all(void);

/* Function definitions ----------------------------------------------- */
base_status_t system_settings_init(void)
{
    /* Set defaults first */
    system_settings_set_defaults();

    /* Try to load settings from NVS */
    uint8_t u8_val;
    int8_t  i8_val;

    if (bsp_flash_get_u8(SETTINGS_NVS_NAMESPACE, SETTINGS_KEY_BRIGHTNESS, &u8_val) == BS_OK)
    {
        if (u8_val <= SYSTEM_SETTINGS_BRIGHTNESS_MAX)
        {
            s_settings.brightness = u8_val;
        }
    }

    if (bsp_flash_get_u8(SETTINGS_NVS_NAMESPACE, SETTINGS_KEY_CLOCK_TYPE, &u8_val) == BS_OK)
    {
        if (u8_val < SYSTEM_SETTINGS_CLOCK_TYPE_MAX)
        {
            s_settings.clock_type = (system_settings_clock_type_t) u8_val;
        }
    }

    if (bsp_flash_get_i8(SETTINGS_NVS_NAMESPACE, SETTINGS_KEY_TIME_ZONE, &i8_val) == BS_OK)
    {
        if (i8_val >= SYSTEM_SETTINGS_TIME_ZONE_MIN && i8_val <= SYSTEM_SETTINGS_TIME_ZONE_MAX)
        {
            s_settings.time_zone = i8_val;
        }
    }

    ESP_LOGI(TAG, "Settings loaded: brightness=%d, clock_type=%d, time_zone=%d", s_settings.brightness,
             s_settings.clock_type, s_settings.time_zone);

    return BS_OK;
}

base_status_t system_settings_get(system_settings_t *settings)
{
    CHECK(settings != NULL, BS_ERROR);

    *settings = s_settings;
    return BS_OK;
}

base_status_t system_settings_set_clock_type(system_settings_clock_type_t clock_type)
{
    CHECK(clock_type < SYSTEM_SETTINGS_CLOCK_TYPE_MAX, BS_ERROR);

    s_settings.clock_type = clock_type;
    return bsp_flash_set_u8(SETTINGS_NVS_NAMESPACE, SETTINGS_KEY_CLOCK_TYPE, (uint8_t) clock_type);
}

system_settings_clock_type_t system_settings_get_clock_type(void)
{
    return s_settings.clock_type;
}

base_status_t system_settings_set_brightness(uint8_t brightness)
{
    CHECK(brightness <= SYSTEM_SETTINGS_BRIGHTNESS_MAX, BS_ERROR);

    s_settings.brightness = brightness;
    return bsp_flash_set_u8(SETTINGS_NVS_NAMESPACE, SETTINGS_KEY_BRIGHTNESS, brightness);
}

uint8_t system_settings_get_brightness(void)
{
    return s_settings.brightness;
}

base_status_t system_settings_set_time_zone(int8_t time_zone)
{
    CHECK(time_zone >= SYSTEM_SETTINGS_TIME_ZONE_MIN && time_zone <= SYSTEM_SETTINGS_TIME_ZONE_MAX, BS_ERROR);

    s_settings.time_zone = time_zone;
    return bsp_flash_set_i8(SETTINGS_NVS_NAMESPACE, SETTINGS_KEY_TIME_ZONE, time_zone);
}

int8_t system_settings_get_time_zone(void)
{
    return s_settings.time_zone;
}

base_status_t system_settings_factory_reset(void)
{
    system_settings_set_defaults();

    CHECK_STATUS(bsp_flash_erase_all(SETTINGS_NVS_NAMESPACE));
    CHECK_STATUS(system_settings_save_all());

    ESP_LOGI(TAG, "Factory reset complete");
    return BS_OK;
}

static void system_settings_set_defaults(void)
{
    s_settings.brightness = SETTINGS_DEFAULT_BRIGHTNESS;
    s_settings.clock_type = SETTINGS_DEFAULT_CLOCK_TYPE;
    s_settings.time_zone  = SETTINGS_DEFAULT_TIME_ZONE;
}

static base_status_t system_settings_save_all(void)
{
    CHECK_STATUS(bsp_flash_set_u8(SETTINGS_NVS_NAMESPACE, SETTINGS_KEY_BRIGHTNESS, s_settings.brightness));
    CHECK_STATUS(
      bsp_flash_set_u8(SETTINGS_NVS_NAMESPACE, SETTINGS_KEY_CLOCK_TYPE, (uint8_t) s_settings.clock_type));
    CHECK_STATUS(bsp_flash_set_i8(SETTINGS_NVS_NAMESPACE, SETTINGS_KEY_TIME_ZONE, s_settings.time_zone));

    return BS_OK;
}

/* End of file -------------------------------------------------------- */
