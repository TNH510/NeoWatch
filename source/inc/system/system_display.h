/**
 * @file       system_display.h
 * @copyright  Copyright (C) 2023 QuyLe Co., Ltd. All rights reserved.
 * @license    This project is released under the QuyLe License.
 * @version    v1.0.0
 * @date       2023-08-25
 * @author     quyle-itr-intern
 *
 * @brief      system display
 *
 * @note
 */

/* Define to prevent recursive inclusion ------------------------------ */
#ifndef __SYSTEM_DISPLAY_H
#define __SYSTEM_DISPLAY_H

/* Includes ----------------------------------------------------------- */
#include "bsp_i2c.h"
#include "drv_led_rgb.h"
#include "drv_ssd1306.h"
#include "drv_button.h"
#include "icon_clock.h"
#include "main.h"
#include "snake_game.h"

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
/* Public defines ----------------------------------------------------- */

/* Public enumerate/structure ----------------------------------------- */
typedef enum
{
    SYSTEM_DISPLAY_OK,
    SYSTEM_DISPLAY_ERROR,
} system_display_error_t;

typedef enum
{
    GAME_START,
    GAME_MAIN,
    GAME_OVER,
    GAME_WIN
} system_display_snake_game_notify_t;

/* Public macros ------------------------------------------------------ */

/* Public variables --------------------------------------------------- */

/* Public function prototypes ----------------------------------------- */
/**
 * @brief Reset pointer choose
 *
 * @return system_display_error_t
 *  - SYSTEM_DISPLAY_OK : system display ok
 *  - SYSTEM_DISPLAY_ERROR : system display error
 */
system_display_error_t system_display_reset_pointer_choose(void);

/**
 * @brief Init system display
 *
 * @return system_display_error_t
 *  - SYSTEM_DISPLAY_OK : system display ok
 *  - SYSTEM_DISPLAY_ERROR : system display error
 */
system_display_error_t system_display_init(void);

/**
 * @brief Display horse animation
 *
 * @return system_display_error_t
 *  - SYSTEM_DISPLAY_OK : system display ok
 *  - SYSTEM_DISPLAY_ERROR : system display error
 */
system_display_error_t system_display_horse_animation(void);

/**
 * @brief Display clock
 *
 * @param[in] mode mode select type clock
 *
 * @return system_display_error_t
 *  - SYSTEM_DISPLAY_OK : system display ok
 *  - SYSTEM_DISPLAY_ERROR : system display error
 */
system_display_error_t system_display_clock(uint8_t mode);

/**
 * @brief Display snake game start
 *
 * @param[in] button state button click or hold and name button
 * @param[out] object_select object select game
 *
 * @return system_display_error_t
 *  - SYSTEM_DISPLAY_OK : system display ok
 *  - SYSTEM_DISPLAY_ERROR : system display error
 */
system_display_error_t system_display_snake_game_start(uint8_t *button, uint8_t *object_select);

/**
 * @brief Display snake game playground
 *
 * @param[in] snake_grid_map grid map snake game
 *
 * @return system_display_error_t
 *  - SYSTEM_DISPLAY_OK : system display ok
 *  - SYSTEM_DISPLAY_ERROR : system display error
 */
system_display_error_t system_display_snake_game_playground(uint8_t snake_grid_map[32][16]);

/**
 * @brief Display time reminder
 *
 * @param[in] button state button click or hold and name button
 *
 * @return system_display_error_t
 *  - SYSTEM_DISPLAY_OK : system display ok
 *  - SYSTEM_DISPLAY_ERROR : system display error
 */
system_display_error_t system_display_time_reminder(uint8_t button);

/**
 * @brief Display time countdown
 *
 * @param[in] button state button click or hold and name button
 *
 * @return system_display_error_t
 *  - SYSTEM_DISPLAY_OK : system display ok
 *  - SYSTEM_DISPLAY_ERROR : system display error
 */
system_display_error_t system_display_time_countdown(uint8_t button);

/**
 * @brief Display time stopwatch
 *
 * @param[in] button state button click or hold and name button
 *
 * @return system_display_error_t
 *  - SYSTEM_DISPLAY_OK : system display ok
 *  - SYSTEM_DISPLAY_ERROR : system display error
 */
system_display_error_t system_display_time_stopwatch(uint8_t button);

/**
 * @brief Display set time
 *
 * @param[in] button state button click or hold and name button
 *
 * @return system_display_error_t
 *  - SYSTEM_DISPLAY_OK : system display ok
 *  - SYSTEM_DISPLAY_ERROR : system display error
 */
system_display_error_t system_display_set_time(uint8_t button);

/**
 * @brief Display snake game mode choose
 *
 * @param[in] button state button click or hold and name button
 * @param[out] object_select object select
 *
 * @return system_display_error_t
 *  - SYSTEM_DISPLAY_OK : system display ok
 *  - SYSTEM_DISPLAY_ERROR : system display error
 */
system_display_error_t system_display_snake_game_mode_choose(uint8_t *button, uint8_t *object_select);

/**
 * @brief Display snake game level choose
 *
 * @param[in] button state button click or hold and name button
 * @param[out] object_select object select
 *
 * @return system_display_error_t
 *  - SYSTEM_DISPLAY_OK : system display ok
 *  - SYSTEM_DISPLAY_ERROR : system display error
 */
system_display_error_t system_display_snake_game_level_choose(uint8_t *button, uint8_t *object_select);

/**
 * @brief Display boot state
 *
 * @return system_display_error_t
 *  - SYSTEM_DISPLAY_OK : system display ok
 *  - SYSTEM_DISPLAY_ERROR : system display error
 */
system_display_error_t system_display_boot_state(void);

/**
 * @brief Display wait state
 *
 * @return system_display_error_t
 *  - SYSTEM_DISPLAY_OK : system display ok
 *  - SYSTEM_DISPLAY_ERROR : system display error
 */
system_display_error_t system_display_wait_state(void);

/**
 * @brief Display menu state
 *
 * @param[in] system_id_app id system app select
 *
 * @return system_display_error_t
 *  - SYSTEM_DISPLAY_OK : system display ok
 *  - SYSTEM_DISPLAY_ERROR : system display error
 */
system_display_error_t system_display_menu_state(uint8_t system_id_app);

/**
 * @brief Display screen save
 *
 * @return system_display_error_t
 *  - SYSTEM_DISPLAY_OK : system display ok
 *  - SYSTEM_DISPLAY_ERROR : system display error
 */
system_display_error_t system_display_screen_save(void);

/**
 * @brief Display diagnostic
 *
 * @param[in] button state button click or hold and name button
 * @param[out] object_select object select
 *
 * @return system_display_error_t
 *  - SYSTEM_DISPLAY_OK : system display ok
 *  - SYSTEM_DISPLAY_ERROR : system display error
 */
system_display_error_t system_display_diagnostic(uint8_t *button, uint8_t *object_select);

/**
 * @brief Display category clock
 *
 * @param[in] button state button click or hold and name button
 * @param[out] object_select object select
 *
 * @return system_display_error_t
 *  - SYSTEM_DISPLAY_OK : system display ok
 *  - SYSTEM_DISPLAY_ERROR : system display error
 */
system_display_error_t system_display_category_clock(uint8_t *button, uint8_t *object_select);

/**
 * @brief Display theme list
 *
 * @param[in] button state button click or hold and name button
 * @param[out] object_select object select
 *
 * @return system_display_error_t
 *  - SYSTEM_DISPLAY_OK : system display ok
 *  - SYSTEM_DISPLAY_ERROR : system display error
 */
system_display_error_t system_display_theme_list(uint8_t *button, uint8_t *object_select);

/**
 * @brief Display update theme color
 *
 * @return system_display_error_t
 *  - SYSTEM_DISPLAY_OK : system display ok
 *  - SYSTEM_DISPLAY_ERROR : system display error
 */
system_display_error_t system_display_update_theme_color(void);

/**
 * @brief Display pc control mode
 *
 * @param[in] button state button click or hold and name button
 * @param[out] object_select object select
 *
 * @return system_display_error_t
 *  - SYSTEM_DISPLAY_OK : system display ok
 *  - SYSTEM_DISPLAY_ERROR : system display error
 */
system_display_error_t system_display_pc_control_mode(uint8_t *button, uint8_t *object_select);

/**
 * @brief Check screen ready
 *
 * @return
 *  - true : system display ok
 *  - false : system display error
 */
bool system_display_is_screen_ready(void);

/**
 * @brief Display update screen
 *
 * @return
 *  None
 */
void system_display_update(void);

/**
 * @brief Display snake game win
 *
 * @param[in] button state button click or hold and name button
 * @param[out] object_select object select
 * @param[out] snake_game structure data snake game
 *
 * @return system_display_error_t
 *  - SYSTEM_DISPLAY_OK : system display ok
 *  - SYSTEM_DISPLAY_ERROR : system display error
 */
system_display_error_t system_display_snake_game_win(uint8_t *button, uint8_t *object_select, snake_game_t *snake_game);

/**
 * @brief Display snake game over
 *
 * @param[in] button state button click or hold and name button
 * @param[out] object_select object select
 * @param[out] snake_game structure data snake game
 *
 * @return system_display_error_t
 *  - SYSTEM_DISPLAY_OK : system display ok
 *  - SYSTEM_DISPLAY_ERROR : system display error
 */
system_display_error_t system_display_snake_game_over(uint8_t *button, uint8_t *object_select, snake_game_t *snake_game);

system_display_error_t system_display_type_clock(button_name_t *button_state);

system_display_error_t system_display_theme_color(button_name_t *button_state);
/**
 * @brief Display snake game information
 *
 * @return system_display_error_t
 */
system_display_error_t system_display_snake_game_info(void);

#endif  // __SYSTEM_DISPLAY_H

/* End of file -------------------------------------------------------- */
