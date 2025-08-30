/**
 * @file       system_game.h
 * @copyright  Copyright (C) 2019 Fiot Co., Ltd. All rights reserved.
 * @license    This project is released under the Fiot License.
 * @version    1.0.0
 * @date       2023-08-30
 * @author     Hieu Tran
 *
 * @brief      System to manage game events
 */

/* Define to prevent recursive inclusion ------------------------------ */
#ifndef __SYSTEM_GAME_H
#define __SYSTEM_GAME_H

/* Includes ----------------------------------------------------------- */
#include "drv_button.h"
#include "drv_buzzer.h"
#include "drv_ssd1306.h"
#include "snake_game.h"
#include "system_data.h"
#include "system_display.h"

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
/* Public defines ----------------------------------------------------- */

/* Public enumerate/structure ----------------------------------------- */
typedef enum
{
    SYSTEM_GAME_OK,
    SYSTEM_GAME_ERROR,
} system_game_error_t;
/* Public macros ------------------------------------------------------ */

/* Public variables --------------------------------------------------- */

/* Public function prototypes ----------------------------------------- */
/**
 * @brief Init system game manager
 *
 * @return
 *  - SYSTEM_GAME_OK : system game ok
 *  - SYSTEM_GAME_ERROR : system game error
 */
system_game_error_t system_game_init(void);

/**
 * @brief Run system game
 * 
 * @param button_state state button click or hold and name button
 * 
 * @return
 *  - SYSTEM_GAME_OK : system game ok
 *  - SYSTEM_GAME_ERROR : system game error
 */
system_game_error_t system_game_run(button_name_t *button_state);

#endif  // __SYSTEM_GAME_H

/* End of file -------------------------------------------------------- */
