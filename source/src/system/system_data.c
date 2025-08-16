/**
 * @file       system_data.c
 * @copyright  Copyright (C) 2019 QuyLe Co., Ltd. All rights reserved.
 * @license    This project is released under the QuyLe License.
 * @version    0.0.1
 * @date       2023
 * @author     QuyLe
 * @author     QuyLe
 *
 * @brief      System manager data save to flash memory
 *
 * @note       None
 * @example    None
 */

/* Includes ----------------------------------------------------------- */
#include "system_data.h"
/* Private defines ---------------------------------------------------- */

/* Private enumerate/structure ---------------------------------------- */
typedef struct
{
    /* data */
    uint8_t                theme_color;
    uint8_t                type_clock;
    uint16_t               high_score;
    system_time_reminder_t data_reminder;
} system_data_flash_t;

/* Private macros ----------------------------------------------------- */

/* Public variables --------------------------------------------------- */

/* Private variables -------------------------------------------------- */

static system_data_flash_t data_flash = { .theme_color                   = 0,
                                          .type_clock                    = 0,
                                          .high_score                    = 0,
                                          .data_reminder.number_reminder = 0,
                                          .data_reminder.rtc_reminder    = { 0 } };

/* Private function prototypes ---------------------------------------- */
static system_data_error_t system_data_save_flash(void);

/* Function definitions ----------------------------------------------- */

system_data_error_t system_data_init(void)
{
    return bsp_flash_read(SYSTEM_DATA_ADDRESS_SAVE, (uint32_t *) &data_flash, sizeof(data_flash) / 4) ? SYSTEM_DATA_OK
                                                                                                      : SYSTEM_DATA_ERROR;
}

static system_data_error_t system_data_save_flash(void)
{
    bsp_flash_erase(SYSTEM_DATA_ADDRESS_SAVE);
    return bsp_flash_write(SYSTEM_DATA_ADDRESS_SAVE, (uint32_t *) &data_flash, sizeof(data_flash) / 4) ? SYSTEM_DATA_OK
                                                                                                       : SYSTEM_DATA_ERROR;
}

system_data_error_t system_data_get_reminder(system_time_reminder_t *data_reminder)
{
    *data_reminder = data_flash.data_reminder;
    return SYSTEM_DATA_OK;
}

system_data_error_t system_data_get_theme_color(uint8_t *theme_color)
{
    *theme_color = data_flash.theme_color ? 1 : 0;
    return SYSTEM_DATA_OK;
}

system_data_error_t system_data_get_type_clock(uint8_t *type_clock)
{
    *type_clock = data_flash.type_clock ? 1 : 0;
    return SYSTEM_DATA_OK;
}

system_data_error_t system_data_get_high_score(uint16_t *high_score)
{
    *high_score = data_flash.high_score;
    return SYSTEM_DATA_OK;
}

system_data_error_t system_data_set_reminder(system_time_reminder_t data_reminder)
{
    data_flash.data_reminder = data_reminder;
    system_data_save_flash();
    return SYSTEM_DATA_OK;
}

system_data_error_t system_data_set_theme_color(uint8_t theme_color)
{
    data_flash.theme_color = theme_color;
    system_data_save_flash();
    return SYSTEM_DATA_OK;
}

system_data_error_t system_data_set_type_clock(uint8_t type_color)
{
    data_flash.type_clock = type_color;
    system_data_save_flash();
    return SYSTEM_DATA_OK;
}

system_data_error_t system_data_set_high_score(uint16_t high_score)
{
    data_flash.high_score = high_score;
    system_data_save_flash();
    return SYSTEM_DATA_OK;
}

/* End of file -------------------------------------------------------- */
