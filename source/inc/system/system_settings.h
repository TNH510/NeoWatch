/**
 * @file       system_settings.h
 * @copyright  Copyright (C) 2025 NEO. All rights reserved.
 * @license    This project is released under the NEO License.
 * @version    1.0.0
 * @date       2025-10
 * @author     Hieu Tran Ngoc
 * @brief      System Settings
 * @note       None
 */

/* Define to prevent recursive inclusion ------------------------------ */
#ifndef __SYSTEM_SETTINGS_H
#define __SYSTEM_SETTINGS_H

/* Includes ----------------------------------------------------------- */
#include "base_type.h"

/* Public defines ----------------------------------------------------- */
#define SYSTEM_SETTINGS_BRIGHTNESS_MIN (0)
#define SYSTEM_SETTINGS_BRIGHTNESS_MAX (100)

#define SYSTEM_SETTINGS_TIME_ZONE_MIN  (-12)
#define SYSTEM_SETTINGS_TIME_ZONE_MAX  (14)

/* Public enumerate/structure ----------------------------------------- */
/**
 * @brief Clock display type
 */
typedef enum
{
    SYSTEM_SETTINGS_CLOCK_TYPE_LEFT,
    SYSTEM_SETTINGS_CLOCK_TYPE_RIGHT,
    SYSTEM_SETTINGS_CLOCK_TYPE_MAX,
} system_settings_clock_type_t;

/**
 * @brief System settings structure
 */
typedef struct
{
    uint8_t                      brightness;
    system_settings_clock_type_t clock_type;
    int8_t                       time_zone;
} system_settings_t;

/* Public macros ------------------------------------------------------ */
/* Public variables --------------------------------------------------- */
/* Public function prototypes ----------------------------------------- */

/**
 * @brief  Initialize system settings, load from NVS or apply defaults
 *
 * @return BS_OK     Initialization succeeded
 * @return BS_ERROR  Initialization failed
 */
base_status_t system_settings_init(void);

/**
 * @brief  Get the current system settings
 *
 * @param[out] settings  Pointer to settings structure to fill
 *
 * @return BS_OK     Settings retrieved successfully
 * @return BS_ERROR  Failed to retrieve settings
 */
base_status_t system_settings_get(system_settings_t *settings);

/**
 * @brief  Set the clock display type
 *
 * @param[in] clock_type  Clock display type to set
 *
 * @return BS_OK     Setting saved successfully
 * @return BS_ERROR  Failed to save setting
 */
base_status_t system_settings_set_clock_type(system_settings_clock_type_t clock_type);

/**
 * @brief  Get the clock display type
 *
 * @return system_settings_clock_type_t  Current clock display type
 */
system_settings_clock_type_t system_settings_get_clock_type(void);

/**
 * @brief  Set the display brightness
 *
 * @param[in] brightness  Brightness level (0-100)
 *
 * @return BS_OK     Setting saved successfully
 * @return BS_ERROR  Failed to save setting
 */
base_status_t system_settings_set_brightness(uint8_t brightness);

/**
 * @brief  Get the display brightness
 *
 * @return uint8_t  Current brightness level (0-100)
 */
uint8_t system_settings_get_brightness(void);

/**
 * @brief  Set the time zone offset
 *
 * @param[in] time_zone  Time zone offset in hours (-12 to +14)
 *
 * @return BS_OK     Setting saved successfully
 * @return BS_ERROR  Failed to save setting
 */
base_status_t system_settings_set_time_zone(int8_t time_zone);

/**
 * @brief  Get the time zone offset
 *
 * @return int8_t  Current time zone offset in hours (-12 to +14)
 */
int8_t system_settings_get_time_zone(void);

/**
 * @brief  Reset all settings to factory defaults and save to NVS
 *
 * @return BS_OK     Settings reset successfully
 * @return BS_ERROR  Failed to reset settings
 */
base_status_t system_settings_factory_reset(void);

/* -------------------------------------------------------------------- */
#endif /* __SYSTEM_SETTINGS_H */

/* End of file -------------------------------------------------------- */
