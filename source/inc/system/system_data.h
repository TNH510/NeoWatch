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

/* Define to prevent recursive inclusion ------------------------------ */
#ifndef __SYSTEM_DATA_H
#define __SYSTEM_DATA_H

/* Includes ----------------------------------------------------------- */
#include "main.h"
#include "system_time.h"

/* Public defines ----------------------------------------------------- */
#define SYSTEM_DATA_ADDRESS_SAVE 0x08060000

/* Public enumerate/structure ----------------------------------------- */

typedef enum
{
    SYSTEM_DATA_ERROR = 0U,
    SYSTEM_DATA_OK,
} system_data_error_t;

/* Public macros ------------------------------------------------------ */

/* Public variables --------------------------------------------------- */

/* Public function prototypes ----------------------------------------- */

/**
 * @brief Init system data
 *
 * @return system_data_error_t
 */
system_data_error_t system_data_init(void);

/**
 * @brief Get data reminder 
 *
 * @param[out] data_reminder structure data reminder get
 *
 * @return
 *  - SYSTEM_DATA_ERROR : system data error
 *  - SYSTEM_DATA_OK : system data ok
 */
system_data_error_t system_data_get_reminder(system_time_reminder_t *data_reminder);

/**
 * @brief Get theme color
 *
 * @param[out] theme_color theme color get
 *
 * @return
 *  - SYSTEM_DATA_ERROR : system data error
 *  - SYSTEM_DATA_OK : system data ok
 */
system_data_error_t system_data_get_theme_color(uint8_t *theme_color);

/**
 * @brief Get type clock
 *
 * @param[out] type_clock type clock get
 *
 * @return
 *  - SYSTEM_DATA_ERROR : system data error
 *  - SYSTEM_DATA_OK : system data ok
 */
system_data_error_t system_data_get_type_clock(uint8_t *type_clock);

/**
 * @brief Get high score
 *
 * @param[out] high_score high score get
 *
 * @return
 *  - SYSTEM_DATA_ERROR : system data error
 *  - SYSTEM_DATA_OK : system data ok
 */
system_data_error_t system_data_get_high_score(uint16_t *high_score);

/**
 * @brief Set data reminder
 *
 * @param[in] data_reminder structure data reminder set
 *
 * @return
 *  - SYSTEM_DATA_ERROR : system data error
 *  - SYSTEM_DATA_OK : system data ok
 */
system_data_error_t system_data_set_reminder(system_time_reminder_t data_reminder);

/**
 * @brief Set theme color
 *
 * @param[in] theme_color theme color set
 *
 * @return
 *  - SYSTEM_DATA_ERROR : system data error
 *  - SYSTEM_DATA_OK : system data ok
 */
system_data_error_t system_data_set_theme_color(uint8_t theme_color);

/**
 * @brief Set type clock
 *
 * @param[in] type_clock type clock set
 *
 * @return
 *  - SYSTEM_DATA_ERROR : system data error
 *  - SYSTEM_DATA_OK : system data ok
 */
system_data_error_t system_data_set_type_clock(uint8_t type_color);

/**
 * @brief Set high score
 *
 * @param[in] high_score high score set
 *
 * @return
 *  - SYSTEM_DATA_ERROR : system data error
 *  - SYSTEM_DATA_OK : system data ok
 */
system_data_error_t system_data_set_high_score(uint16_t high_score);

#endif /* __SYSTEM_DATA_H */
