/**
 * @file       snake_game.h
 * @copyright  Copyright (C) 2019 Fiot Co., Ltd. All rights reserved.
 * @license    This project is released under the Fiot License.
 * @version    0.0.1
 * @date       2023
 * @author     Quynh Le
 * @author     Quynh Le
 *
 * @brief      Middleware for Snake game
 *
 * @note       None
 * @example    None
 */

/* Define to prevent recursive inclusion ------------------------------ */
#ifndef __SNAKE_GAME_H
#define __SNAKE_GAME_H

/* Includes ----------------------------------------------------------- */
#include "bsp_timer_pwm.h"
#include "drv_ssd1306.h"
#include "stdlib.h"
#include "time.h"
/* Public defines ----------------------------------------------------- */
#define SNAKE_SIZE_WIDTH (4)
#define MAX_SCORE        (100)
/* Public enumerate/structure ----------------------------------------- */
/**
 * @brief which object on grid map
 */
typedef enum
{
    NO_OBJECT,
    FOOD,
    SNAKE,
    WALL
} snake_object_t;

/**
 * @brief Mode game enum
 */
typedef enum
{
    CLASSIC,
    MODERN,
    INFINITY
} snake_mode_t;

/**
 * @brief Speed game mode enum
 */
typedef enum
{
    EASSY  = 6,
    MEDIUM = 4,
    HARD   = 2
} snake_speed_t;
/**
 * @brief  Store snake information
 */
typedef struct
{
    snake_mode_t mode_game;
    uint8_t      speed_game;
    uint8_t      x_grid;
    uint8_t      y_grid;
    uint8_t      snake_grid_map[DRV_SSD1306_WIDTH / SNAKE_SIZE_WIDTH][DRV_SSD1306_HEIGHT / SNAKE_SIZE_WIDTH];
    uint8_t      snake_position[2][2000];
    uint16_t     snake_length;
    int          snake_step[2];
    uint16_t     snake_score;
    uint16_t     snake_highest_score;
    uint16_t     snake_old_pos[2];
    uint16_t     snake_pos_head[2];

} snake_game_t;

/* Public macros ------------------------------------------------------ */
/* Public variables --------------------------------------------------- */
/* Public function prototypes ----------------------------------------- */
/**
 * @brief  Initialize for grid which store information about position of object in screen
 *
 * @param[in]    snake_game    Struct which store information about snake game
 *
 * @attention Should call this function first before using other function
 * 			  First position of snake from (0,0) top left
 *
 * @return
 *  - None
 */
void mdv_snake_grid_init(snake_game_t *snake_game);

/**
 * @brief  Implement move of snake
 *
 * @param[in]    snake_game    Struct which store information about snake game
 *
 * @attention
 *
 * @return
 *  - None
 */
void mdv_snake_move(snake_game_t *snake_game);

/**
 * @brief  Snake eat food and increase score by 1
 *
 * @param[in]    snake_game    Struct which store information about snake game
 *
 * @attention
 *
 * @return
 *  - None
 */
void mdv_snake_check_food(snake_game_t *snake_game);

/**
 * @brief  Check whether game is over
 *
 * @param[in]    snake_game    Struct which store information about snake game
 *
 * @attention
 *
 * @return
 *  - gameover : true
 *  - game not over : false
 */
bool mdv_snake_check_gameover(snake_game_t *snake_game);

/**
 * @brief  Check button event from user
 *
 * @param[in]    button    button which user press
 * @param[in]    snake_game    Struct which store information about snake game
 *
 * @attention
 *
 * @return
 *  - None
 */
void mdv_snake_check_button(snake_game_t *snake_game, uint8_t button);

/**
 * @brief  Random position of food on grid map
 *
 * @param[in]    snake_game    Struct which store information about snake game
 *
 * @attention
 *
 * @return
 *  - None
 */
void mdv_snake_food_random(snake_game_t *snake_game);

/**
 * @brief  Clear Map
 *
 * @param[in]    snake_game    Struct which store information about snake game
 *
 * @attention
 *
 * @return
 *  - None
 */
void mdv_snake_clear_map(snake_game_t *snake_game);

/**
 * @brief  Clear map and reset game
 *
 * @param[in]    snake_game    Struct which store information about snake game
 *
 * @attention
 *
 * @return
 *  - None
 */
void mdv_snake_map_mode(snake_game_t *snake_game);

/**
 * @brief  Notificate win game
 *
 * @param[in]    snake_game    Struct which store information about snake game
 *
 * @attention
 *
 * @return
 *  - win : true
 *  - win yet : false
 */
bool mdv_snake_win(snake_game_t *snake_game);

#endif /* __SNAKE_GAME_H */
