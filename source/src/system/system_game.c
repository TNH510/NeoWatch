/**
 * @file       system_game.c
 * @copyright  Copyright (C) 2019 Fiot Co., Ltd. All rights reserved.
 * @license    This project is released under the Fiot License.
 * @version    1.0.0
 * @date       2023-08-30
 * @author     Hieu Tran
 *
 * @brief      System to manage game events
 */

/* Includes ----------------------------------------------------------- */
#include "system_game.h"

/* Private defines ---------------------------------------------------- */

/* Private enumerate/structure ---------------------------------------- */

typedef enum
{
    START_MENU,
    MAP_MENU,
    SPEED_MENU,
    START_NEWGAME_STATE,
    GAME_OVER_STATE,
    GAME_WIN_STATE,
    INFO
} system_game_state_t;

/* Private enumerate/structure ---------------------------------------- */

/* Private macros ----------------------------------------------------- */

/* Public variables --------------------------------------------------- */

/* Private variables -------------------------------------------------- */
static uint8_t             start_screen_choose   = 3;
static uint8_t             mode_screen_choose    = 3;
static uint8_t             mode_level_choose     = 3;
static uint8_t             mode_game_over_choose = 3;
static uint8_t             mode_game_win_choose  = 3;
static uint16_t            speed_game;
static system_game_state_t game_state = START_MENU;
snake_game_t               snake_game;
/* Private function prototypes ---------------------------------------- */

/* Function definitions ----------------------------------------------- */
system_game_error_t system_game_init(void)
{
    mdv_snake_grid_init(&snake_game);
    system_data_get_high_score(&(snake_game.snake_highest_score));
    drv_buzzer_init();
    return SYSTEM_GAME_OK;
}

system_game_error_t system_game_run(button_name_t *button_state)
{
    system_game_error_t status = SYSTEM_GAME_OK;

    while (status == SYSTEM_GAME_OK)
    {
        uint8_t count_speed = 0;
        uint16_t green[3] = {0,255,0};
        uint16_t red[3] = {255,0,0};
        uint16_t blue[3] = {0,0,255};
        uint16_t violet[3] = {255,255,255};

        drv_button_check_event(button_state);
        drv_ssd1306_fill_theme_color();

        /*FSM*/
        switch (game_state)
        {
        case START_MENU:

            system_display_snake_game_start(button_state, &start_screen_choose);
            drv_buzzer_off();
            drv_led_off();

            // handle cancle button
            if (*button_state == CLICK_CANCEL_BUTTON)
            	return 1;
            if (*button_state == HOLD_CANCEL_BUTTON)
            	return 2;

            if (start_screen_choose == 0)
                game_state = START_NEWGAME_STATE;
            else if (start_screen_choose == 1)
                game_state = MAP_MENU;
            else if (start_screen_choose == 2)
                game_state = INFO;

            // reset flag unless it will jumpt to START_MENU again
            start_screen_choose = 3;

            break;

        case START_NEWGAME_STATE:

        	// set up map and config snake
            mdv_snake_map_mode(&snake_game);
            mdv_snake_food_random(&snake_game);

            drv_buzzer_off();


            speed_game = snake_game.speed_game;

            while (mdv_snake_check_gameover(&snake_game) == false && mdv_snake_win(&snake_game) == false && *button_state != 6)
            {
            	drv_buzzer_music(Super_Mario);
            	drv_led_fade_in_fade_out();
                if (count_speed > speed_game)
                {
                    mdv_snake_check_food(&snake_game);
                    mdv_snake_move(&snake_game);
                    count_speed = 0;
                }

                // change speed based on click or hold status
                if ((snake_game.snake_step[0] == 1 && snake_game.snake_step[1] == 0) && *button_state == HOLD_RIGHT_BUTTON)
                    speed_game = snake_game.speed_game - 2;
                else if ((snake_game.snake_step[0] == -1 && snake_game.snake_step[1] == 0) && *button_state == HOLD_LEFT_BUTTON)
                    speed_game = snake_game.speed_game - 2;
                else if ((snake_game.snake_step[0] == 0 && snake_game.snake_step[1] == 1) && *button_state == HOLD_DOWN_BUTTON)
                    speed_game = snake_game.speed_game - 2;
                else if ((snake_game.snake_step[0] == 0 && snake_game.snake_step[1] == -1) && *button_state == HOLD_UP_BUTTON)
                    speed_game = snake_game.speed_game - 2;
                else
                    speed_game = snake_game.speed_game;

                drv_button_check_event(button_state);
                mdv_snake_check_button(&snake_game, *button_state);
                system_display_snake_game_playground(snake_game.snake_grid_map);

                count_speed++;
                bsp_delay(1);
            }

            if (*button_state == 6)
            {
                game_state = START_MENU;
            }
            else if (mdv_snake_check_gameover(&snake_game))
            {
                game_state = GAME_OVER_STATE;
            }
            else if (mdv_snake_win(&snake_game))
            {
                game_state = GAME_WIN_STATE;
            }

            break;

        case GAME_WIN_STATE:


            drv_led_blink_color(green);

            system_display_snake_game_win(button_state, &mode_game_win_choose, &snake_game);

            if (mode_game_over_choose == 0)
            {
                game_state = START_NEWGAME_STATE;
            }
            else if (mode_game_over_choose == 1)
            {
                game_state = START_MENU;
            }
            // implement saved action when any option being selected
            if (mode_game_over_choose == 0 || mode_game_over_choose == 1)
            {
            	// save current score if it is the highest one
                if (snake_game.snake_score >= snake_game.snake_highest_score)
                {
                    system_data_set_high_score(snake_game.snake_score);
                    snake_game.snake_highest_score = snake_game.snake_score;
                }
            }

            mode_game_over_choose = 3;

            break;

        case GAME_OVER_STATE:

            system_display_snake_game_over(button_state, &mode_game_over_choose, &snake_game);

            drv_led_blink_color(red);

            if (mode_game_over_choose == 0)
            {
                game_state = START_NEWGAME_STATE;
            }
            else if (mode_game_over_choose == 1)
            {
                game_state = START_MENU;
            }

            // implement saved action when any option being selected
            if (mode_game_over_choose == 0 || mode_game_over_choose == 1)
            {
            	// save current score if it is the highest one
                if (snake_game.snake_score >= snake_game.snake_highest_score)
                {
                    system_data_set_high_score(snake_game.snake_score);
                    snake_game.snake_highest_score = snake_game.snake_score;
                }
            }

            mode_game_over_choose = 3;

            break;

        case MAP_MENU:

        	drv_led_set_color(blue);

            system_display_snake_game_mode_choose(button_state, &mode_screen_choose);

            if (*button_state == 6)
                game_state = START_MENU;
            if (mode_screen_choose == 0)
                snake_game.mode_game = CLASSIC;
            else if (mode_screen_choose == 1)
                snake_game.mode_game = MODERN;
            else if (mode_screen_choose == 2)
                snake_game.mode_game = INFINITY;
            if (mode_screen_choose == 0 || mode_screen_choose == 1 || mode_screen_choose == 2)
                game_state = SPEED_MENU;

            mode_screen_choose = 3;

            break;

        case SPEED_MENU:

        	drv_led_set_color(violet);

            system_display_snake_game_level_choose(button_state, &mode_level_choose);

            if (mode_level_choose == 0)
                snake_game.speed_game = EASSY;
            else if (mode_level_choose == 1)
                snake_game.speed_game = MEDIUM;
            else if (mode_level_choose == 2)
                snake_game.speed_game = HARD;
            if (mode_level_choose == 0 || mode_level_choose == 1 || mode_level_choose == 2)
                game_state = START_NEWGAME_STATE;
            if (*button_state == 6)
                game_state = MAP_MENU;

            mode_level_choose = 3;

            break;

        case INFO:

        	system_display_snake_game_info();

            if (*button_state == 6)
                game_state = START_MENU;

        	break;

        default :

        	break ;
        }
    }

    return status;
}
/* End of file -------------------------------------------------------- */
