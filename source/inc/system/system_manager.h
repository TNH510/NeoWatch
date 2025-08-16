/**
 * @file       system_manager.c
 * @copyright  Copyright (C) 2019 Fiot Co., Ltd. All rights reserved.
 * @license    This project is released under the Fiot License.
 * @version    1.0.0
 * @date       2023-08-31
 * @author     Hieu Tran
 *
 * @brief      System to manage manager all devices
 */

/* Define to prevent recursive inclusion ------------------------------ */
#ifndef __SYSTEM_MANAGER_H
#define __SYSTEM_MANAGER_H

/* Includes ----------------------------------------------------------- */
#include "drv_button.h"
#include "drv_buzzer.h"
#include "drv_led_rgb.h"
#include "drv_ssd1306.h"
#include "snake_game.h"
#include "system_data.h"
#include "system_diagnostic.h"
#include "system_display.h"
#include "system_game.h"
#include "system_pc_control.h"
#include "system_time.h"

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
/* Public defines ----------------------------------------------------- */

/* Public enumerate/structure ----------------------------------------- */

typedef enum
{
    SYSTEM_MANAGER_OK,
    SYSTEM_MANAGER_ERROR,
} system_manager_error_t;
/* Public macros ------------------------------------------------------ */

/* Public variables --------------------------------------------------- */

/* Public function prototypes ----------------------------------------- */
/**
 * @brief Init system manager
 *
 * @return system_manager_error_t
 */
system_manager_error_t system_manager_init(void);

/**
 * @brief system_diagnostic_handle
 *
 * @param button_state state button click or hold and name button
 * @param object_select object select
 *
 *
 * @return
 *  - SYSTEM_DIAGNOSTIC_OK : system diagnostic ok
 *  - SYSTEM_DIAGNOSTIC_ERROR : system diagnostic error
 */
system_manager_error_t system_diagnostic_handle(button_name_t *button_state, uint8_t *object_select);

/**
 * @brief Run system manager
 *
 * @param button_state
 *
 * @return
 *  - SYSTEM_MANAGER_OK : system manager ok
 *  - SYSTEM_MANAGER_ERROR : system manager error
 */
system_manager_error_t system_manager_run(button_name_t *button_state);

#endif /* __SYSTEM_MANAGER_H */

/* End of file -------------------------------------------------------- */
