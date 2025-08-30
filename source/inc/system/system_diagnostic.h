/**
 * @file       system_diagnostic.h
 * @copyright  Copyright (C) 2019 Fiot Co., Ltd. All rights reserved.
 * @license    This project is released under the Fiot License.
 * @version    1.0.0
 * @date       2023-09-04
 * @author     Hieu Tran
 *
 * @brief      System to manage diagnostic events
 */

/* Define to prevent recursive inclusion ------------------------------ */
#ifndef __SYSTEM_DIAGNOSTIC_H
#define __SYSTEM_DIAGNOSTIC_H

/* Includes ----------------------------------------------------------- */
#include "drv_button.h"
#include "drv_buzzer.h"
#include "drv_cjmcu75.h"
#include "drv_ds1307.h"
#include "drv_led_rgb.h"
#include "system_display.h"
/* Public defines ----------------------------------------------------- */

/* Public enumerate/structure ----------------------------------------- */
typedef enum
{
    SYSTEM_DIAGNOSTIC_OK,
    SYSTEM_DIAGNOSTIC_ERROR,
} system_diagnostic_error_t;

/* Public macros ------------------------------------------------------ */

/* Public variables --------------------------------------------------- */

/* Public function prototypes ----------------------------------------- */
/**
 * @brief Init system diagnostic
 *
 * @return
 *  - SYSTEM_DIAGNOSTIC_OK : system diagnostic ok
 *  - SYSTEM_DIAGNOSTIC_ERROR : system diagnostic error
 */
system_diagnostic_error_t system_diagnostic_init(void);

/**
 * @brief Reset system diagnostic variable
 *
 * @return
 *  - SYSTEM_DIAGNOSTIC_OK : system diagnostic ok
 *  - SYSTEM_DIAGNOSTIC_ERROR : system diagnostic error
 */
system_diagnostic_error_t system_diagnostic_reset_variable(void);

/**
 * @brief Screen system diagnostic
 *
 * @param button_state state button click or hold and name button
 *
 * @return
 *  - SYSTEM_DIAGNOSTIC_OK : system diagnostic ok
 *  - SYSTEM_DIAGNOSTIC_ERROR : system diagnostic error
 */
system_diagnostic_error_t system_diagnostic_screen(button_name_t *button_state);

/**
 * @brief Check connection realtime
 *
 * @param button_state state button click or hold and name button
 *
 * @return
 *  - SYSTEM_DIAGNOSTIC_OK : system diagnostic ok
 *  - SYSTEM_DIAGNOSTIC_ERROR : system diagnostic error
 */
system_diagnostic_error_t system_diagnostic_realtime(button_name_t *button_state);

/**
 * @brief Check connection temp sensor
 *
 * @param button_state state button click or hold and name button
 *
 * @return
 *  - SYSTEM_DIAGNOSTIC_OK : system diagnostic ok
 *  - SYSTEM_DIAGNOSTIC_ERROR : system diagnostic error
 */
system_diagnostic_error_t system_diagnostic_temp_sensor(button_name_t *button_state);

/**
 * @brief system_diagnostic_button
 *
 * @param button_state state button click or hold and name button
 *
 * @return
 *  - SYSTEM_DIAGNOSTIC_OK : system diagnostic ok
 *  - SYSTEM_DIAGNOSTIC_ERROR : system diagnostic error
 */
system_diagnostic_error_t system_diagnostic_button(button_name_t *button_state);

/**
 * @brief system_diagnostic_led_rgb
 *
 * @param button_state state button click or hold and name button
 *
 * @return
 *  - SYSTEM_DIAGNOSTIC_OK : system diagnostic ok
 *  - SYSTEM_DIAGNOSTIC_ERROR : system diagnostic error
 */
system_diagnostic_error_t system_diagnostic_led_rgb(button_name_t *button_state);

/**
 * @brief system_diagnostic_buzzer
 *
 * @param button_state state button click or hold and name button
 *
 * @return
 *  - SYSTEM_DIAGNOSTIC_OK : system diagnostic ok
 *  - SYSTEM_DIAGNOSTIC_ERROR : system diagnostic error
 */
system_diagnostic_error_t system_diagnostic_buzzer(button_name_t *button_state);

#endif /* __SYSTEM_DIAGNOSTIC_H */

/* End of file -------------------------------------------------------- */
