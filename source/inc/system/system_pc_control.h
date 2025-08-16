/**
 * @file       system_pc_control.h
 * @copyright  Copyright (C) 2019 Fiot Co., Ltd. All rights reserved.
 * @license    This project is released under the Fiot License.
 * @version    1.0.0
 * @date       2023-09-05
 * @author     Hieu Tran
 *
 * @brief      System to manage pc_control events
 */

/* Define to prevent recursive inclusion ------------------------------ */
#ifndef __SYSTEM_PC_CONTROL_H
#define __SYSTEM_PC_CONTROL_H

/* Includes ----------------------------------------------------------- */
#include "drv_button.h"
#include "drv_buzzer.h"
#include "drv_cjmcu75.h"
#include "drv_ds1307.h"
#include "drv_led_rgb.h"
#include "drv_uart.h"
#include "stdbool.h"
#include "string.h"
#include "system_display.h"
/* Public defines ----------------------------------------------------- */

/* Public enumerate/structure ----------------------------------------- */

typedef enum
{
    SYSTEM_PC_CONTROL_OK,
    SYSTEM_PC_CONTROL_ERROR,
} system_pc_control_error_t;

typedef enum
{
    SYSTEM_PC_CONTROL_COMMAND_RUN,
    SYSTEM_PC_CONTROL_COMMAND_CANCEL,
} system_pc_control_command_t;

enum system_pc_control_enum
{
    SYSTEM_PC_CONTROL_COMMAND,
    SYSTEM_PC_CONTROL_STREAM_VIDEO
};
/* Public macros ------------------------------------------------------ */

/* Public variables --------------------------------------------------- */

/* Public function prototypes ----------------------------------------- */
/**
 * @brief Reset system pc control
 *
 * @return
 *  None
 */
void system_pc_control_reset(void);

/**
 * @brief Init system pc control
 *
 * @return
 *  - SYSTEM_PC_CONTROL_OK : system pc control ok
 *  - SYSTEM_PC_CONTROL_ERROR : system pc control error
 */
system_pc_control_error_t system_pc_control_init(void);

/**
 * @brief Handle pc control
 *
 * @param[in] button_state state button click or hold and name button
 * @param[out] object_select object select game
 *
 * @return
 *  - SYSTEM_PC_CONTROL_OK : system pc control ok
 *  - SYSTEM_PC_CONTROL_ERROR : system pc control error
 */
system_pc_control_error_t system_pc_control_handle(button_name_t *button_state, uint8_t *object_select);

/**
 * @brief Control Clock with terminal
 *
 * @param[in] button_state state button click or hold and name button
 *
 * @return
 *  - SYSTEM_PC_CONTROL_OK : system pc control ok
 *  - SYSTEM_PC_CONTROL_ERROR : system pc control error
 */
system_pc_control_command_t system_pc_control_command_run(button_name_t *button_state);

/**
 * @brief Stream video from data via UART
 *
 * @param[in] button_state state button click or hold and name button
 *
 * @return system_pc_control_error_t
 *  - SYSTEM_PC_CONTROL_OK : system pc control ok
 *  - SYSTEM_PC_CONTROL_ERROR : system pc control error
 */
system_pc_control_error_t system_pc_control_stream_video(button_name_t *button_state);
#endif  // __SYSTEM_PC_CONTROL_H

/* End of file -------------------------------------------------------- */
