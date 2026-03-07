/*
 * snack_game.c
 *
 *  Created on: Aug 28, 2023
 *      Author: dell
 */

/* Includes ----------------------------------------------------------- */
#include "snake_game.h"
/* Private defines ---------------------------------------------------- */

/* Private enumerate/structure ---------------------------------------- */

/* Private macros ----------------------------------------------------- */

/* Public variables --------------------------------------------------- */

/* Private variables -------------------------------------------------- */

/* Private function prototypes ---------------------------------------- */

/* Function definitions ----------------------------------------------- */

void mdv_snake_food_random(snake_game_t *snake_game)
{
    uint8_t x_food = (rand() % (snake_game->x_grid - 1));
    uint8_t y_food = (rand() % (snake_game->y_grid - 1));

    while (snake_game->snake_grid_map[x_food][y_food] != NO_OBJECT)
    {
        x_food = (rand() % (snake_game->x_grid - 1));
        y_food = (rand() % (snake_game->y_grid - 1));
    }

    snake_game->snake_grid_map[x_food][y_food] = FOOD;
}

void mdv_snake_move(snake_game_t *snake_game)
{
    // delete all current position of snake
    for (int i = snake_game->snake_length - 1; i >= 0; i--)
    {
        snake_game->snake_grid_map[snake_game->snake_position[0][i]][snake_game->snake_position[1][i]] = NO_OBJECT;
    }

    // save a current position of head snake
    snake_game->snake_old_pos[0] = snake_game->snake_position[0][snake_game->snake_length - 1];
    snake_game->snake_old_pos[1] = snake_game->snake_position[1][snake_game->snake_length - 1];

    // move head snake to new position
    snake_game->snake_position[0][snake_game->snake_length - 1] += snake_game->snake_step[0];
    snake_game->snake_position[1][snake_game->snake_length - 1] += snake_game->snake_step[1];

    snake_game->snake_position[0][snake_game->snake_length - 1] %= (snake_game->x_grid);
    snake_game->snake_position[1][snake_game->snake_length - 1] %= (snake_game->y_grid);

    // update new position of head snake
    snake_game->snake_pos_head[0] = snake_game->snake_position[0][snake_game->snake_length - 1];
    snake_game->snake_pos_head[1] = snake_game->snake_position[1][snake_game->snake_length - 1];

    // move the rest part of snake to new position
    for (int i = snake_game->snake_length - 2; i >= 0; i--)
    {
        uint8_t nx = snake_game->snake_position[0][i] % (snake_game->x_grid);
        uint8_t ny = snake_game->snake_position[1][i] % (snake_game->y_grid);

        snake_game->snake_position[0][i] = snake_game->snake_old_pos[0] % (snake_game->x_grid);
        snake_game->snake_position[1][i] = snake_game->snake_old_pos[1] % (snake_game->y_grid);

        snake_game->snake_old_pos[0] = nx;
        snake_game->snake_old_pos[1] = ny;
    }

    // add new postion of snake to grip map
    for (int i = snake_game->snake_length - 1; i >= 0; i--)
    {
        snake_game->snake_grid_map[snake_game->snake_position[0][i]][snake_game->snake_position[1][i]] = SNAKE;
    }
}

bool mdv_snake_check_gameover(snake_game_t *snake_game)
{
    if (snake_game->mode_game == CLASSIC)
    {
        // check limit of bound
        if (snake_game->snake_pos_head[0] >= snake_game->x_grid - 1 || snake_game->snake_pos_head[0] < 1)
        {
            return true;
        }
        if (snake_game->snake_pos_head[1] >= snake_game->y_grid - 1 || snake_game->snake_pos_head[1] < 1)
        {
            return true;
        }
    }

    if (snake_game->mode_game == MODERN)
    {
        // check limit of bound
        if (snake_game->snake_pos_head[0] >= snake_game->x_grid - 1 || snake_game->snake_pos_head[0] < 1)
        {
            return true;
        }
        if (snake_game->snake_pos_head[1] >= snake_game->y_grid - 1 || snake_game->snake_pos_head[1] < 1)
        {
            return true;
        }
        // check if it crashed into obstacle
        if (snake_game->snake_grid_map[snake_game->snake_pos_head[0] + snake_game->snake_step[0]]
                                      [snake_game->snake_pos_head[1] + snake_game->snake_step[1]]
            == WALL)
        {
            return true;
        }
    }

    // check whether snake touch itself
    if (snake_game->snake_grid_map[snake_game->snake_pos_head[0] + snake_game->snake_step[0]]
                                  [snake_game->snake_pos_head[1] + snake_game->snake_step[1]]
        == SNAKE)
    {
        return true;
    }

    return false;
}

void mdv_snake_check_food(snake_game_t *snake_game)
{
    if (snake_game->snake_grid_map[snake_game->snake_pos_head[0] + snake_game->snake_step[0]]
                                  [snake_game->snake_pos_head[1] + snake_game->snake_step[1]]
        == FOOD)
    {
        snake_game->snake_score++;
        snake_game->snake_length++;

        snake_game->snake_position[0][snake_game->snake_length - 1] = snake_game->snake_pos_head[0] + snake_game->snake_step[0];
        snake_game->snake_position[1][snake_game->snake_length - 1] = snake_game->snake_pos_head[1] + snake_game->snake_step[1];

        mdv_snake_food_random(snake_game);
    }
}

void mdv_snake_check_button(snake_game_t *snake_game, uint8_t button)
{
    // up
    if (button == 2 && snake_game->snake_step[1] == 0)
    {
        snake_game->snake_step[0] = 0;
        snake_game->snake_step[1] = -1;
    }
    // down
    if (button == 3 && snake_game->snake_step[1] == 0)
    {
        snake_game->snake_step[0] = 0;
        snake_game->snake_step[1] = 1;
    }
    // left
    if (button == 4 && snake_game->snake_step[0] == 0)
    {
        snake_game->snake_step[0] = -1;
        snake_game->snake_step[1] = 0;
    }
    // right
    if (button == 1 && snake_game->snake_step[0] == 0)
    {
        snake_game->snake_step[0] = 1;
        snake_game->snake_step[1] = 0;
    }
}

void mdv_snake_clear_map(snake_game_t *snake_game)
{
    for (int i = 0; i < snake_game->x_grid; i++)
    {
        for (int j = 0; j < snake_game->y_grid; j++)
        {
            snake_game->snake_grid_map[i][j] = 0;
        }
    }
}

void mdv_snake_map_mode(snake_game_t *snake_game)
{
    // reset map
    mdv_snake_clear_map(snake_game);

    // reset point
    snake_game->snake_score = 0;

    // reset direction
    snake_game->snake_step[0] = 1;
    snake_game->snake_step[1] = 0;

    // reset position of snake
    for (int i = 0; i < snake_game->snake_length; i++)
    {
        snake_game->snake_position[0][i] = 0;
        snake_game->snake_position[1][i] = 0;
    }
    snake_game->snake_length = 2;

    if (snake_game->mode_game == CLASSIC)
    {
        // place new posittion for snake
        snake_game->snake_grid_map[1][1] = SNAKE;
        snake_game->snake_grid_map[2][1] = SNAKE;
        snake_game->snake_position[0][0] = 1;
        snake_game->snake_position[1][0] = 1;
        snake_game->snake_position[0][1] = 2;
        snake_game->snake_position[1][1] = 1;

        snake_game->snake_pos_head[0] = snake_game->snake_position[0][1];
        snake_game->snake_pos_head[1] = snake_game->snake_position[1][1];

        // Make retangle bound wall
        for (int i = 0; i < snake_game->x_grid; i++)
        {
            for (int j = 0; j < snake_game->y_grid; j++)
            {
                if ((i == 0) || (j == snake_game->y_grid - 1) || (j == 0) || (i == snake_game->x_grid - 1))
                {
                    snake_game->snake_grid_map[i][j] = WALL;
                }
            }
        }
    }

    else if (snake_game->mode_game == MODERN)
    {
        // place new posittion for snake
        snake_game->snake_grid_map[1][1] = SNAKE;
        snake_game->snake_grid_map[2][1] = SNAKE;
        snake_game->snake_position[0][0] = 1;
        snake_game->snake_position[1][0] = 1;
        snake_game->snake_position[0][1] = 2;
        snake_game->snake_position[1][1] = 1;

        snake_game->snake_pos_head[0] = snake_game->snake_position[0][1];
        snake_game->snake_pos_head[1] = snake_game->snake_position[1][1];

        // Make retangle bound wall
        for (int i = 0; i < snake_game->x_grid; i++)
        {
            for (int j = 0; j < snake_game->y_grid; j++)
            {
                if ((i == 0) || (j == snake_game->y_grid - 1) || (j == 0) || (i == snake_game->x_grid - 1))
                {
                    snake_game->snake_grid_map[i][j] = WALL;
                }
            }
        }

        // make random obstacle with random size in map
        for (int number = 0; number < (rand() % 3 + 2); number++)
        {
            uint8_t x_obstacle = rand() % (snake_game->x_grid - 3) + 2;
            uint8_t y_obstacle = rand() % (snake_game->y_grid - 3) + 2;

            while (snake_game->snake_grid_map[x_obstacle][y_obstacle] != NO_OBJECT)
            {
                x_obstacle = rand() % (snake_game->x_grid - 3) + 2;
                y_obstacle = rand() % (snake_game->y_grid - 3) + 2;
            }

            uint8_t width_ob  = (rand() % 3 + 2);
            uint8_t heigth_ob = (rand() % 3 + 2);

            for (int width = 0; width < width_ob; width++)
            {
                for (int heigth = 0; heigth < heigth_ob; heigth++)
                {
                    snake_game->snake_grid_map[x_obstacle + width][y_obstacle + heigth] = WALL;
                }
            }
        }
    }

    else if (snake_game->mode_game == INFINITY)
    {
        snake_game->snake_grid_map[0][0] = SNAKE;
        snake_game->snake_grid_map[1][0] = SNAKE;
        snake_game->snake_position[0][0] = 0;
        snake_game->snake_position[1][0] = 0;
        snake_game->snake_position[0][1] = 1;
        snake_game->snake_position[1][1] = 0;

        snake_game->snake_pos_head[0] = snake_game->snake_position[0][1];
        snake_game->snake_pos_head[1] = snake_game->snake_position[1][1];
    }
}

bool mdv_snake_win(snake_game_t *snake_game)
{
    if (snake_game->snake_score == MAX_SCORE)
    {
        return true;
    }
    return false;
}

void mdv_snake_grid_init(snake_game_t *snake_game)
{
    // set up for random feature
    srand(HAL_GetTick() + 70);

    // size of grid map
    snake_game->x_grid = DRV_SSD1306_WIDTH / SNAKE_SIZE_WIDTH;
    snake_game->y_grid = DRV_SSD1306_HEIGHT / SNAKE_SIZE_WIDTH;

    // clear map bit before setting
    mdv_snake_clear_map(snake_game);

    // default mode game : classic
    snake_game->mode_game = INFINITY;

    // default speed game : eassy
    snake_game->speed_game = EASSY;

    snake_game->snake_grid_map[0][0] = SNAKE;
    snake_game->snake_grid_map[1][0] = SNAKE;

    snake_game->snake_position[0][0] = 0;
    snake_game->snake_position[1][0] = 0;
    snake_game->snake_position[0][1] = 1;
    snake_game->snake_position[1][1] = 0;

    snake_game->snake_length = 2;

    snake_game->snake_step[0] = 1;
    snake_game->snake_step[1] = 0;

    snake_game->snake_score = 0;
}

/* End of file -------------------------------------------------------- */
