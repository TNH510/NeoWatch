/**
 * @file       system_display.c
 * @copyright  Copyright (C) 2023 QuyLe Co., Ltd. All rights reserved.
 * @license    This project is released under the QuyLe License.
 * @version    major.minor.patch
 * @date       2023-08-25
 * @author     quyle-itr-intern
 *
 * @brief      system display
 *
 * @note
 */

/* Includes ----------------------------------------------------------- */
#include "system_display.h"

#include "bsp_gpio.h"
#include "drv_button.h"
#include "drv_buzzer.h"
#include "drv_cjmcu75.h"
#include "drv_ds1307.h"
#include "fonts.h"
#include "horse_anim.h"
#include "icon_clock.h"
#include "snake_game.h"
#include "system_time.h"
/* Private defines ---------------------------------------------------- */
#define TIME_BLINK_NUMBER 20

#define SYSTEM_DISPLAY_CHECK_ERROR(status_1, status_2) \
    if (!(status_1))                                   \
        return (status_2);
/* Private enumerate/structure ---------------------------------------- */
enum system_display_clock_enum
{
    SYSTEM_DISPLAY_SET_TIME,
    SYSTEM_DISPLAY_SET_REMINDER,
    SYSTEM_DISPLAY_TIME_COUNTDOWN,
    SYSTEM_DISPLAY_TIME_STOPWATCH,
    SYSTEM_DISPLAY_TYPE_CLOCK,
    SYSTEM_DISPLAY_THEME_COLOR,
};
/* Private macros ----------------------------------------------------- */

/* Public variables --------------------------------------------------- */

/* Private variables -------------------------------------------------- */
static cjmcu75_t            cjmcu75;
static uint8_t              theme_color     = 0;
static int8_t               add_space_pixel = 0;
static drv_ssd1306_config_t drv_ssd1306;
static char *string_month[] = { "Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec" };
/* Private function prototypes ---------------------------------------- */
static system_display_error_t system_display_alarm_reminder(void);
/* Function definitions ----------------------------------------------- */
system_display_error_t system_display_reset_pointer_choose(void)
{
    add_space_pixel = 0;
    return SYSTEM_DISPLAY_OK;
}

system_display_error_t system_display_init(void)
{
    cjmcu75_init(&cjmcu75);
    drv_ssd1306.device_address      = 0x78;
    drv_ssd1306.i2c_is_device_ready = bsp_i2c3_is_device_ready;
    drv_ssd1306.i2c_read_at         = bsp_i2c3_read_mem;
    drv_ssd1306.i2c_write_at        = bsp_i2c3_write_mem_dma;
    SYSTEM_DISPLAY_CHECK_ERROR(drv_ssd1306_init(&drv_ssd1306) == DRV_SSD1306_OK, SYSTEM_DISPLAY_ERROR);
    system_data_get_theme_color(&theme_color);
    drv_ssd1306_invert_display(&drv_ssd1306, theme_color);
    system_time_alarm_set_callback(system_display_alarm_reminder);
    return SYSTEM_DISPLAY_OK;
}

static system_display_error_t system_display_alarm_reminder(void)
{
    char  string_time[10];
    rtc_t rtc;
    system_time_get_date_time(&rtc);

    uint8_t  toggle         = 0;
    uint16_t color_red[3]   = { 100, 0, 0 };
    uint16_t color_black[3] = { 0, 0, 0 };

    /* Clean screen */
    drv_ssd1306_update_screen(&drv_ssd1306);
    /* Off buzzer */
    drv_buzzer_off();
    /* Off led rgb */
    drv_led_off();
    /* Turn alarm */
    drv_buzzer_music(NOKIA_TUNE);

    while (1)
    {
        /* I2C ready */
        if (bsp_i2c3_is_busy() == true)
        {
            /* Screen alarm */
            static uint8_t dem = 0;
            drv_ssd1306_fill_theme_color();
            drv_ssd1306_draw_bitmap(0, 0, gif_alarm_reminder[dem++], 128, 64, 1);
            drv_ssd1306_draw_rectangle(0, 0, 128, 64, 1);
            drv_ssd1306_goto_xy(12, 42);
            sprintf(string_time, "%02d:%02d:%02d", rtc.hour, rtc.minute, rtc.second);
            drv_ssd1306_puts((uint8_t *) string_time, &font_10x19, 1);
            drv_ssd1306_update_screen(&drv_ssd1306);
            if (dem == 43)
            {
                if (toggle)
                    drv_led_set_color(color_red);
                else
                    drv_led_set_color(color_black);
                toggle = !toggle;
                drv_buzzer_music(NOKIA_TUNE);
                dem = 0;
            }

            /* Exit alarm */
            if (bsp_gpio_read_pin(GPIOC, GPIO_PIN_12) == false)
            {
                drv_led_off();
                drv_buzzer_off();
                break;
            }
        }
    }
    return SYSTEM_DISPLAY_OK;
}

system_display_error_t system_display_horse_animation(void)
{
    for (uint16_t i = 0; i < 10240; i += 1024)
    {
        drv_ssd1306_fill_theme_color();
        drv_ssd1306_update_screen(&drv_ssd1306);
    }
    return SYSTEM_DISPLAY_OK;
}

system_display_error_t system_display_update_theme_color(void)
{
    system_data_get_theme_color(&theme_color);
    drv_ssd1306_invert_display(&drv_ssd1306, theme_color);
    return SYSTEM_DISPLAY_OK;
}

system_display_error_t system_display_screen_save(void)
{
    drv_ssd1306_fill_theme_color();
    drv_ssd1306_draw_bitmap(0, 0, icon_screen_save, 128, 64, 1);
    drv_ssd1306_draw_rectangle(0, 0, 128, 64, DRV_SSD1306_COLOR_WHITE);
    drv_ssd1306_goto_xy(48, 50);
    drv_ssd1306_puts((uint8_t *) "SAVE", &font_7x10, 1);
    drv_ssd1306_update_screen(&drv_ssd1306);
    return SYSTEM_DISPLAY_OK;
}

system_display_error_t system_display_time_reminder(button_name_t button)
{
    static button_name_t          last_button = 0;
    static uint8_t                read_once   = 0;
    static system_time_reminder_t reminder_data;
    if (!read_once)
    {
        read_once++;
        system_time_get_reminder(&reminder_data);
    }
    rtc_t          rtc;
    char           string_time[30];
    uint8_t        pos_x = 25, pos_y = 6, index_tick = 0;
    static uint8_t count         = 0;
    static uint8_t pos_pointer_x = 25, pos_pointer_y = 14;
    static uint8_t start_list = 0, end_list = 3;

    if (button == HOLD_LEFT_BUTTON || button == HOLD_RIGHT_BUTTON || button == HOLD_UP_BUTTON || button == HOLD_DOWN_BUTTON)
    {
        for (volatile uint32_t i = 0; i < 0xFFFFF; i++) __NOP();
        goto start;
    }

    if (button == last_button)
        return SYSTEM_DISPLAY_OK;
    last_button = button;

start:;

    drv_ssd1306_fill_theme_color();
    for (uint8_t i = start_list; i < end_list; i++)
    {
        if (reminder_data.number_reminder & (1 << i))
        {
            drv_ssd1306_draw_bitmap(6, 2 + index_tick * 22, icon_tick_on, 16, 16, 1);
        }
        else
            drv_ssd1306_draw_bitmap(6, 2 + index_tick * 22, icon_tick_off, 16, 16, 1);
        ds1307_convert_to_date_time(&rtc, reminder_data.rtc_reminder[i]);
        sprintf(string_time, "%02d:%02d:%02d %02d-%02d-%02d", rtc.hour, rtc.minute, rtc.second, rtc.day_of_month, rtc.month,
                rtc.year);
        drv_ssd1306_goto_xy(pos_x, pos_y);
        drv_ssd1306_puts((uint8_t *) string_time, &font_6x8, 1);
        pos_y += 22;
        index_tick++;
    }

    if (button == CLICK_RIGHT_BUTTON || button == HOLD_RIGHT_BUTTON)
    {
        pos_pointer_x += 18;
        if (pos_pointer_x > 115)
        {
            pos_pointer_x = 25;
            pos_pointer_y += 22;
            if (pos_pointer_y > 58)
            {
                pos_pointer_y = 14;
                if (end_list != 6)
                {
                    start_list++;
                    end_list++;
                    read_once = 0;
                }
                else
                {
                    pos_pointer_x = 115;
                    pos_pointer_y = 58;
                }
            }
        }
    }
    else if (button == CLICK_LEFT_BUTTON || button == HOLD_LEFT_BUTTON)
    {
        pos_pointer_x -= 18;
        if (pos_pointer_x < 25)
        {
            pos_pointer_x = 25;
            if (pos_pointer_y == 14)
            {
                pos_pointer_y = 58;
                if (start_list != 0)
                {
                    pos_pointer_x = 115;
                    start_list--;
                    end_list--;
                    read_once = 0;
                }
                else
                {
                    pos_pointer_x = 25;
                    pos_pointer_y = 14;
                }
            }
            else
            {
                pos_pointer_x = 115;
                pos_pointer_y -= 22;
            }
        }
    }
    else if (button == CLICK_UP_BUTTON || button == CLICK_DOWN_BUTTON || button == HOLD_UP_BUTTON || button == HOLD_DOWN_BUTTON)
    {
        uint8_t index = 0;
        if (pos_pointer_y == 14)
            index = 0;
        else if (pos_pointer_y == 36)
            index = 1;
        else if (pos_pointer_y == 58)
            index = 2;
        ds1307_convert_to_date_time(&rtc, reminder_data.rtc_reminder[start_list + index]);
        if (button == CLICK_UP_BUTTON || button == HOLD_UP_BUTTON)
            system_time_change((pos_pointer_x - 25) / 18, &rtc, 1);
        else
            system_time_change((pos_pointer_x - 25) / 18, &rtc, -1);
        ds1307_convert_to_epoch_time(rtc, &reminder_data.rtc_reminder[start_list + index]);
    }
    else if (button == CLICK_SELECT_BUTTON)
    {
        system_display_screen_save();
        system_time_save_reminder(reminder_data);
        read_once = 0;
    }
    else if (button == HOLD_CANCEL_BUTTON && !count)
    {
        uint8_t index = 0;
        if (pos_pointer_y == 14)
            index = 0;
        else if (pos_pointer_y == 36)
            index = 1;
        else if (pos_pointer_y == 58)
            index = 2;
        if (reminder_data.number_reminder & (1 << (start_list + index)))
            reminder_data.number_reminder &= ~(1 << (start_list + index));
        else
            reminder_data.number_reminder |= (1 << (start_list + index));
        count++;
    }
    else if (button == NO_EVENT)
    {
        count = 0;
    }
    drv_ssd1306_draw_bitmap(pos_pointer_x, pos_pointer_y, icon_select, 12, 2, 1);
    drv_ssd1306_update_screen(&drv_ssd1306);
    return SYSTEM_DISPLAY_OK;
}

system_display_error_t system_display_set_time(button_name_t button)
{
    char                 string_time[20];
    static button_name_t last_button = 0;
    static uint8_t       pos_x = 15, pos_y = 36, read_once = 0;
    static rtc_t         rtc;

    if (button == HOLD_DOWN_BUTTON || button == HOLD_UP_BUTTON)
    {
        for (volatile uint32_t i = 0; i < 0xFFFFF; i++) __NOP();
        goto start;
    }
    if (button == last_button)
        return SYSTEM_DISPLAY_OK;

start:;
    last_button = button;

    if (!read_once)
    {
        read_once++;
        system_time_get_date_time(&rtc);
    }

    if (button == CLICK_RIGHT_BUTTON)
    {
        pos_x += 18;
        if (pos_x > 105)
            pos_x = 105;
    }
    else if (button == CLICK_LEFT_BUTTON)
    {
        if (pos_x != 15)
            pos_x -= 18;
    }
    else if (button == CLICK_UP_BUTTON || button == HOLD_UP_BUTTON)
    {
        system_time_change((pos_x - 15) / 18, &rtc, 1);
        uint32_t epoch_time = 0;
        ds1307_convert_to_epoch_time(rtc, &epoch_time);
        ds1307_convert_to_date_time(&rtc, epoch_time);
    }
    else if (button == CLICK_DOWN_BUTTON || button == HOLD_DOWN_BUTTON)
    {
        system_time_change((pos_x - 15) / 18, &rtc, -1);
        uint32_t epoch_time = 0;
        ds1307_convert_to_epoch_time(rtc, &epoch_time);
        ds1307_convert_to_date_time(&rtc, epoch_time);
    }
    else if (button == CLICK_SELECT_BUTTON)
    {
        system_time_set_value(rtc.second, rtc.minute, rtc.hour, rtc.day_of_week, rtc.day_of_month, rtc.month, rtc.year);
        system_display_screen_save();
        for (uint32_t i = 0; i < 0xFFFFF; i++)
        {
            __NOP();
        }
        return SYSTEM_DISPLAY_OK;
    }

    drv_ssd1306_fill_theme_color();
    drv_ssd1306_goto_xy(15, 28);
    sprintf(string_time, "%02d:%02d:%02d %02d-%02d-%02d", rtc.hour, rtc.minute, rtc.second, rtc.day_of_month, rtc.month,
            rtc.year);
    drv_ssd1306_puts((uint8_t *) string_time, &font_6x8, 1);
    drv_ssd1306_draw_bitmap(pos_x, pos_y, icon_select, 12, 2, 1);
    drv_ssd1306_update_screen(&drv_ssd1306);
    return SYSTEM_DISPLAY_OK;
}

system_display_error_t system_display_snake_game_start(uint8_t *button, uint8_t *object_select)
{
    if (*button == 3 || *button == 2)
        drv_ssd1306_fill_theme_color();

    drv_ssd1306_draw_bitmap(0, 5, icon_snake, 55, 55, DRV_SSD1306_COLOR_WHITE);
    drv_ssd1306_draw_bitmap(50, 3, icon_snake_game_text, 76, 15, DRV_SSD1306_COLOR_WHITE);
    drv_ssd1306_goto_xy(70, 30);
    drv_ssd1306_puts((uint8_t *) "New Game", &font_7x10, DRV_SSD1306_COLOR_WHITE);
    drv_ssd1306_goto_xy(70, 40);
    drv_ssd1306_puts((uint8_t *) "Mode", &font_7x10, DRV_SSD1306_COLOR_WHITE);
    drv_ssd1306_goto_xy(70, 50);
    drv_ssd1306_puts((uint8_t *) "About", &font_7x10, DRV_SSD1306_COLOR_WHITE);

    // Down
    if (*button == 3)
    {
        add_space_pixel += 10;
        if (add_space_pixel >= 20)
            add_space_pixel = 20;
    }
    // Up
    else if (*button == 2)
    {
        add_space_pixel -= 10;
        if (add_space_pixel < 0)
            add_space_pixel = 0;
    }

    if (*button == 5)
    {
        *object_select  = add_space_pixel / 10;
        add_space_pixel = 0;
    }

    drv_ssd1306_draw_bitmap(60, 30 + add_space_pixel, icon_snake_choose_pointer, 7, 7, DRV_SSD1306_COLOR_WHITE);
    drv_ssd1306_update_screen(&drv_ssd1306);
    return SYSTEM_DISPLAY_OK;
}

system_display_error_t system_display_snake_game_mode_choose(uint8_t *button, uint8_t *object_select)
{
    if (*button == 3 || *button == 2)
        drv_ssd1306_fill_theme_color();

    drv_ssd1306_draw_bitmap(0, 5, icon_snake, 55, 55, DRV_SSD1306_COLOR_WHITE);
    drv_ssd1306_goto_xy(50, 3);
    drv_ssd1306_puts((uint8_t *) "Mode Choose:", &font_7x10, DRV_SSD1306_COLOR_WHITE);
    drv_ssd1306_goto_xy(70, 30);
    drv_ssd1306_puts((uint8_t *) "Classic", &font_7x10, DRV_SSD1306_COLOR_WHITE);
    drv_ssd1306_goto_xy(70, 40);
    drv_ssd1306_puts((uint8_t *) "Modern", &font_7x10, DRV_SSD1306_COLOR_WHITE);
    drv_ssd1306_goto_xy(70, 50);
    drv_ssd1306_puts((uint8_t *) "Infinity", &font_7x10, DRV_SSD1306_COLOR_WHITE);

    // Down
    if (*button == 3)
    {
        add_space_pixel += 10;
        if (add_space_pixel >= 20)
            add_space_pixel = 20;
    }
    // Up
    else if (*button == 2)
    {
        add_space_pixel -= 10;
        if (add_space_pixel < 0)
            add_space_pixel = 0;
    }

    if (*button == 5)
    {
        *object_select  = add_space_pixel / 10;
        add_space_pixel = 0;
    }

    drv_ssd1306_draw_bitmap(60, 30 + add_space_pixel, icon_snake_choose_pointer, 7, 7, DRV_SSD1306_COLOR_WHITE);
    drv_ssd1306_update_screen(&drv_ssd1306);
    return SYSTEM_DISPLAY_OK;
}

system_display_error_t system_display_snake_game_level_choose(uint8_t *button, uint8_t *object_select)
{
    if (*button == 3 || *button == 2)
        drv_ssd1306_fill_theme_color();

    drv_ssd1306_draw_bitmap(0, 5, icon_snake, 55, 55, DRV_SSD1306_COLOR_WHITE);
    drv_ssd1306_goto_xy(50, 3);
    drv_ssd1306_puts((uint8_t *) "Level Choose:", &font_7x10, DRV_SSD1306_COLOR_WHITE);
    drv_ssd1306_goto_xy(70, 30);
    drv_ssd1306_puts((uint8_t *) "Easy", &font_7x10, DRV_SSD1306_COLOR_WHITE);
    drv_ssd1306_goto_xy(70, 40);
    drv_ssd1306_puts((uint8_t *) "Medium", &font_7x10, DRV_SSD1306_COLOR_WHITE);
    drv_ssd1306_goto_xy(70, 50);
    drv_ssd1306_puts((uint8_t *) "Hard", &font_7x10, DRV_SSD1306_COLOR_WHITE);

    // Down
    if (*button == 3)
    {
        add_space_pixel += 10;
        if (add_space_pixel >= 20)
            add_space_pixel = 20;
    }
    // Up
    else if (*button == 2)
    {
        add_space_pixel -= 10;
        if (add_space_pixel < 0)
            add_space_pixel = 0;
    }

    if (*button == 5)
    {
        *object_select  = add_space_pixel / 10;
        add_space_pixel = 0;
    }

    drv_ssd1306_draw_bitmap(60, 30 + add_space_pixel, icon_snake_choose_pointer, 7, 7, DRV_SSD1306_COLOR_WHITE);
    drv_ssd1306_update_screen(&drv_ssd1306);
    return SYSTEM_DISPLAY_OK;
}

system_display_error_t system_display_snake_game_over(uint8_t *button, uint8_t *object_select, snake_game_t *snake_game)
{
    if (*button == 3 || *button == 2)
        drv_ssd1306_fill_theme_color();

    drv_ssd1306_goto_xy(30, 0);
    drv_ssd1306_puts((uint8_t *) "GAME OVER", &font_7x10, DRV_SSD1306_COLOR_WHITE);

    drv_ssd1306_goto_xy(20, 20);
    drv_ssd1306_puts((uint8_t *) "Replay", &font_7x10, DRV_SSD1306_COLOR_WHITE);

    drv_ssd1306_goto_xy(20, 30);
    drv_ssd1306_puts((uint8_t *) "Quit", &font_7x10, DRV_SSD1306_COLOR_WHITE);

    drv_ssd1306_goto_xy(20, 40);
    uint8_t str_score[20];
    sprintf(str_score, "Your Score:%u", snake_game->snake_score);
    drv_ssd1306_puts(str_score, &font_7x10, DRV_SSD1306_COLOR_WHITE);

    drv_ssd1306_goto_xy(20, 50);
    uint8_t str_highest_score[23];
    sprintf(str_highest_score, "Record:%u", snake_game->snake_highest_score);
    drv_ssd1306_puts(str_highest_score, &font_7x10, DRV_SSD1306_COLOR_WHITE);

    // Down
    if (*button == 3)
    {
        add_space_pixel += 10;
        if (add_space_pixel >= 10)
            add_space_pixel = 10;
    }
    // Up
    else if (*button == 2)
    {
        add_space_pixel -= 10;
        if (add_space_pixel < 0)
            add_space_pixel = 0;
    }

    if (*button == 5)
    {
        *object_select  = add_space_pixel / 10;
        add_space_pixel = 0;
    }

    drv_ssd1306_draw_bitmap(10, 20 + add_space_pixel, icon_snake_choose_pointer, 7, 7, DRV_SSD1306_COLOR_WHITE);
    drv_ssd1306_update_screen(&drv_ssd1306);
}

system_display_error_t system_display_snake_game_win(uint8_t *button, uint8_t *object_select, snake_game_t *snake_game)
{
    if (*button == 3 || *button == 2)
        drv_ssd1306_fill(0);

    drv_ssd1306_goto_xy(30, 0);
    drv_ssd1306_puts((uint8_t *) "GAME WIN", &font_7x10, DRV_SSD1306_COLOR_WHITE);

    drv_ssd1306_goto_xy(20, 20);
    drv_ssd1306_puts((uint8_t *) "Replay", &font_7x10, DRV_SSD1306_COLOR_WHITE);

    drv_ssd1306_goto_xy(20, 30);
    drv_ssd1306_puts((uint8_t *) "Quit", &font_7x10, DRV_SSD1306_COLOR_WHITE);

    drv_ssd1306_goto_xy(20, 40);
    uint8_t str_score[20];
    sprintf(str_score, "Your Score:%u", snake_game->snake_score);
    drv_ssd1306_puts(str_score, &font_7x10, DRV_SSD1306_COLOR_WHITE);

    drv_ssd1306_goto_xy(20, 50);
    uint8_t str_highest_score[23];
    sprintf(str_highest_score, "Record:%u", snake_game->snake_highest_score);
    drv_ssd1306_puts(str_highest_score, &font_7x10, DRV_SSD1306_COLOR_WHITE);

    // Down
    if (*button == 3)
    {
        add_space_pixel += 10;
        if (add_space_pixel >= 10)
            add_space_pixel = 10;
    }
    // Up
    else if (*button == 2)
    {
        add_space_pixel -= 10;
        if (add_space_pixel < 0)
            add_space_pixel = 0;
    }

    if (*button == 5)
    {
        *object_select  = add_space_pixel / 10;
        add_space_pixel = 0;
    }

    drv_ssd1306_draw_bitmap(10, 20 + add_space_pixel, icon_snake_choose_pointer, 7, 7, DRV_SSD1306_COLOR_WHITE);
    drv_ssd1306_update_screen(&drv_ssd1306);
    return SYSTEM_DISPLAY_OK;
}

system_display_error_t system_display_snake_game_info()
{
    drv_ssd1306_goto_xy(10, 0);
    drv_ssd1306_puts((uint8_t *) "SNAKE GAME INFO", &font_7x10, DRV_SSD1306_COLOR_WHITE);

    drv_ssd1306_goto_xy(0, 10);
    drv_ssd1306_puts((uint8_t *) "About:Snake eat food", &font_6x8, DRV_SSD1306_COLOR_WHITE);

    drv_ssd1306_goto_xy(0, 20);
    drv_ssd1306_puts((uint8_t *) "Producer: ITR VN", &font_6x8, DRV_SSD1306_COLOR_WHITE);

    drv_ssd1306_goto_xy(0, 30);
    drv_ssd1306_puts((uint8_t *) "Author:Quynh,Quy,Hieu", &font_6x8, DRV_SSD1306_COLOR_WHITE);

    drv_ssd1306_goto_xy(0, 40);
    drv_ssd1306_puts((uint8_t *) "Released: 07/09/2023", &font_6x8, DRV_SSD1306_COLOR_WHITE);

    drv_ssd1306_goto_xy(0, 50);
    drv_ssd1306_puts((uint8_t *) "Version: 1.0.0", &font_6x8, DRV_SSD1306_COLOR_WHITE);

    drv_ssd1306_update_screen(&drv_ssd1306);
}

system_display_error_t system_display_snake_game_playground(uint8_t snake_grid_map[32][16])
{
    for (uint8_t x_pos = 0; x_pos < 32; x_pos++)
    {
        for (uint8_t y_pos = 0; y_pos < 16; y_pos++)
        {
            if (snake_grid_map[x_pos][y_pos] == 2)
                for (uint8_t extend_x_pos = x_pos * 4; extend_x_pos < (x_pos * 4 + 4); extend_x_pos++)
                    for (uint8_t extend_y_pos = y_pos * 4; extend_y_pos < (y_pos * 4 + 4); extend_y_pos++)
                        drv_ssd1306_draw_pixel(extend_x_pos, extend_y_pos, DRV_SSD1306_COLOR_WHITE);

            if (snake_grid_map[x_pos][y_pos] == 3)
            {
                for (uint8_t extend_x_pos = x_pos * 4; extend_x_pos < (x_pos * 4 + 4); extend_x_pos++)
                    for (uint8_t extend_y_pos = y_pos * 4; extend_y_pos < (y_pos * 4 + 4); extend_y_pos++)
                        drv_ssd1306_draw_pixel(extend_x_pos, extend_y_pos, DRV_SSD1306_COLOR_WHITE);
            }

            if (snake_grid_map[x_pos][y_pos] == 1)
            {
                // draw cicle
                for (uint8_t extend_x_pos = x_pos * 4; extend_x_pos < (x_pos * 4 + 4); extend_x_pos++)
                {
                    for (uint8_t extend_y_pos = y_pos * 4; extend_y_pos < (y_pos * 4 + 4); extend_y_pos++)
                    {
                        if (extend_x_pos == x_pos * 4 || extend_y_pos == y_pos * 4 || extend_x_pos == (x_pos * 4 + 4) - 1
                            || extend_y_pos == (y_pos * 4 + 4) - 1)
                            drv_ssd1306_draw_pixel(extend_x_pos, extend_y_pos, DRV_SSD1306_COLOR_WHITE);
                        if ((extend_x_pos == x_pos * 4 && extend_y_pos == y_pos * 4)
                            || (extend_x_pos == (x_pos * 4 + 4) - 1 && extend_y_pos == (y_pos * 4))
                            || (extend_x_pos == x_pos * 4 + 4 - 1 && extend_y_pos == y_pos * 4 + 4 - 1)
                            || (extend_x_pos == x_pos * 4 && extend_y_pos == y_pos * 4 + 4 - 1))
                        {
                            drv_ssd1306_draw_pixel(extend_x_pos, extend_y_pos, DRV_SSD1306_COLOR_BLACK);
                        }
                    }
                }
            }

            if (snake_grid_map[x_pos][y_pos] == 0)
                for (uint8_t extend_x_pos = x_pos * 4; extend_x_pos < (x_pos * 4 + 4); extend_x_pos++)
                    for (uint8_t extend_y_pos = y_pos * 4; extend_y_pos < (y_pos * 4 + 4); extend_y_pos++)
                        drv_ssd1306_draw_pixel(extend_x_pos, extend_y_pos, DRV_SSD1306_COLOR_BLACK);
        }
    }

    drv_ssd1306_update_screen(&drv_ssd1306);
    return SYSTEM_DISPLAY_OK;
}

system_display_error_t system_display_time_countdown(uint8_t button)
{
    char           string[20];
    static uint8_t time_seconds = 0, time_minutes = 0, time_hours = 0, index_select = 2, mode_setting = 1, start_count = 0,
                   init_time_count = 0, padding = 0, flag_timedown = 0, time_seconds_last = 0, time_minutes_last = 0,
                   time_hours_last  = 0;
    static uint8_t       pos_x      = 7;
    static uint8_t       pos_y      = 18;
    static uint32_t      time_count = 0, time_current = 0, time_last = 0;
    static rtc_t         rtc;
    static button_name_t last_button = 0;

    if (button == HOLD_DOWN_BUTTON || button == HOLD_UP_BUTTON)
    {
        for (volatile uint32_t i = 0; i < 0xFFFFF; i++) __NOP();
        goto start;
    }

    if (button == last_button)
    {
        if (start_count == 1 && mode_setting == 0)
            goto start;
        return SYSTEM_DISPLAY_OK;
    }
    last_button = button;

start:;

    drv_ssd1306_fill_theme_color();
    drv_ssd1306_goto_xy(10, 4);
    drv_ssd1306_puts((uint8_t *) "Time Count Down", &font_7x10, 1);
    drv_ssd1306_draw_rectangle(0, 0, 128, 64, 1);
    system_time_get_date_time(&rtc);
    sprintf(string, (char *) "%02d %s 20%02d", rtc.day_of_month, string_month[rtc.month - 1], rtc.year);
    drv_ssd1306_goto_xy(25, 53);
    drv_ssd1306_puts((uint8_t *) string, &font_7x10, 1);

    if (button == HOLD_SELECT_BUTTON)
    {
        if (mode_setting == 0)
            init_time_count = 0;
        mode_setting = !mode_setting;
    }
    else if (button == CLICK_SELECT_BUTTON)
    {
        if (flag_timedown)
        {
            drv_buzzer_off();
            flag_timedown = 0;
        }
        else
        {
            if (mode_setting == 0)
            {
                system_time_get_epoch_time(&time_last);
                start_count = !start_count;
            }
        }
    }
    else if (button == HOLD_CANCEL_BUTTON)
    {
        time_hours   = 0;
        time_minutes = 0;
        time_seconds = 0;
    }

    if (start_count && mode_setting == 0)
    {
        if (!init_time_count)
        {
            time_count = (((time_hours * 60) + time_minutes) * 60 + time_seconds);
            if (time_count == 0)
            {
                start_count     = 0;
                init_time_count = 0;
                return SYSTEM_DISPLAY_OK;
            }
            system_time_get_epoch_time(&time_last);
            time_seconds_last = time_seconds;
            time_minutes_last = time_minutes;
            time_hours_last   = time_hours;
            init_time_count   = 1;
        }
        system_time_get_epoch_time(&time_current);
        time_count -= time_current - time_last;
        if (time_current == time_last)
            return SYSTEM_DISPLAY_OK;
        time_last    = time_current;
        time_hours   = time_count / 3600;
        time_minutes = (time_count % 3600) / 60;
        time_seconds = time_count % 60;

        if (time_hours == 0 && time_minutes == 0 && time_seconds == 0)
        {
            drv_buzzer_music(NOKIA_TUNE);
            time_seconds    = time_seconds_last;
            time_minutes    = time_minutes_last;
            time_hours      = time_hours_last;
            flag_timedown   = 1;
            init_time_count = 0;
            start_count     = 0;
        }
    }

    /* Setting time count down */
    if (mode_setting == 1)
    {
        if (button == CLICK_LEFT_BUTTON)
        {
            if (index_select == 0)
                index_select = 0;
            else
                index_select--;
        }
        else if (button == CLICK_RIGHT_BUTTON)
        {
            if (index_select == 2)
                index_select = 2;
            else
                index_select++;
        }
        else if (button == CLICK_UP_BUTTON || button == HOLD_UP_BUTTON)
        {
            if (index_select == 0)
            {
                if (time_hours != 60)
                    time_hours++;
            }
            else if (index_select == 1)
            {
                if (time_minutes != 60)
                    time_minutes++;
            }
            else
            {
                if (time_seconds != 60)
                    time_seconds++;
            }
        }
        else if (button == CLICK_DOWN_BUTTON || button == HOLD_DOWN_BUTTON)
        {
            if (index_select == 0)
            {
                if (time_hours != 0)
                    time_hours--;
            }
            else if (index_select == 1)
            {
                if (time_minutes != 0)
                    time_minutes--;
            }
            else
            {
                if (time_seconds != 0)
                    time_seconds--;
            }
        }
        if (index_select == 2)
            padding = 19;
        else
            padding = 0;
        drv_ssd1306_draw_bitmap(13 + 53 * index_select - padding, 45, icon_select_big, 23, 2, 1);
    }
    drv_ssd1306_goto_xy(pos_x, pos_y);
    sprintf(string, "%02d:%02d", time_hours, time_minutes);
    drv_ssd1306_puts((uint8_t *) string, &font_15x25, 1);
    drv_ssd1306_goto_xy(pos_x + 93, pos_y + 6);
    sprintf(string, "%02d", time_seconds);
    drv_ssd1306_puts((uint8_t *) string, &font_10x19, 1);
    drv_ssd1306_update_screen(&drv_ssd1306);
    return SYSTEM_DISPLAY_OK;
}

system_display_error_t system_display_time_stopwatch(uint8_t button)
{
    char                 string[20];
    static uint8_t       pos_x = 7, pos_y = 18, start_count = 0, time_hours = 0, time_minutes = 0, time_seconds = 0;
    static uint32_t      time_count = 0, time_current = 0, time_last = 0;
    static rtc_t         rtc;
    static button_name_t last_button = 0;

    if (button == last_button)
    {
        if (start_count == 1)
            goto start;
        return SYSTEM_DISPLAY_OK;
    }
    last_button = button;

start:;

    drv_ssd1306_fill_theme_color();
    drv_ssd1306_goto_xy(10, 4);
    drv_ssd1306_puts((uint8_t *) "Time Stop Watch", &font_7x10, 1);
    drv_ssd1306_draw_rectangle(0, 0, 128, 64, 1);
    system_time_get_date_time(&rtc);
    sprintf(string, (char *) "%02d %s 20%02d", rtc.day_of_month, string_month[rtc.month - 1], rtc.year);
    drv_ssd1306_goto_xy(25, 53);
    drv_ssd1306_puts((uint8_t *) string, &font_7x10, 1);

    if (button == CLICK_SELECT_BUTTON)
    {
        system_time_get_epoch_time(&time_last);
        start_count = !start_count;
    }
    else if (button == HOLD_CANCEL_BUTTON)
    {
        time_count   = 0;
        time_seconds = 0;
        time_minutes = 0;
        time_hours   = 0;
    }

    if (start_count)
    {
        system_time_get_epoch_time(&time_current);
        time_count += time_current - time_last;
        if (time_current == time_last)
            return SYSTEM_DISPLAY_OK;
        time_last    = time_current;
        time_hours   = time_count / 3600;
        time_minutes = (time_count % 3600) / 60;
        time_seconds = time_count % 60;
    }

    drv_ssd1306_goto_xy(pos_x, pos_y);
    sprintf(string, "%02d:%02d", time_hours, time_minutes);
    drv_ssd1306_puts((uint8_t *) string, &font_15x25, 1);
    drv_ssd1306_goto_xy(pos_x + 93, pos_y + 6);
    sprintf(string, "%02d", time_seconds);
    drv_ssd1306_puts((uint8_t *) string, &font_10x19, 1);
    drv_ssd1306_update_screen(&drv_ssd1306);
    return SYSTEM_DISPLAY_OK;
}

system_display_error_t system_display_snake_game_notify(system_display_snake_game_notify_t type_of_notify)
{
    switch (type_of_notify)
    {
    case GAME_START:
        /**/
        break;
    case GAME_MAIN:
        /**/
        break;
    case GAME_OVER:
        /**/
        break;
    case GAME_WIN:
        /**/
        break;
    default: break;
    }
    return SYSTEM_DISPLAY_OK;
}

system_display_error_t system_display_clock(uint8_t mode)
{
    static uint32_t time_current  = 0;
    static uint32_t time_previous = 0;
    system_time_get_epoch_time(&time_current);
    if (time_current == time_previous)
        return SYSTEM_DISPLAY_OK;
    switch (mode)
    {
    case 0:
    {
        rtc_t rtc;
        system_time_get_date_time(&rtc);
        drv_ds1306_clock_style_simple(&drv_ssd1306, rtc);
        break;
    }
    case 1:
    {
        rtc_t rtc;
        system_time_get_date_time(&rtc);
        int8_t temp = cjmcu75_get_temp(&cjmcu75);
        drv_ds1306_clock_style_digital(&drv_ssd1306, rtc, temp);
        break;
    }
    }
    time_previous = time_current;
    return SYSTEM_DISPLAY_OK;
}

system_display_error_t system_display_boot_state(void)
{
    static uint32_t count = 0;
    count++;
    if (count == 70000)
    {
        drv_ssd1306_fill_theme_color();
        drv_ssd1306_draw_bitmap(0, 0, aVy_kocuoi, 128, 64, 1);
        drv_ssd1306_update_screen(&drv_ssd1306);
    }
    else if (count == 140000)
    {
        drv_ssd1306_fill_theme_color();
        drv_ssd1306_draw_bitmap(0, 0, aVy_cuoi, 128, 64, 1);
        drv_ssd1306_update_screen(&drv_ssd1306);
    }
    if (count >= 140000)
        count = 0;
    return SYSTEM_DISPLAY_OK;
}

system_display_error_t system_display_wait_state(void)
{
    uint8_t mode = 0;
    system_data_get_type_clock(&mode);
    system_display_clock(mode);
    return SYSTEM_DISPLAY_OK;
}

system_display_error_t system_display_menu_state(uint8_t system_id_app)
{
    /* Turn off all */
    drv_buzzer_off();
    drv_led_off();

    /* Display menu icons */
    drv_ssd1306_fill_theme_color();
    drv_ssd1306_draw_bitmap(3, 7, icon_menu_time, 26, 26, 1);
    drv_ssd1306_draw_bitmap(32, 7, icon_menu_snake, 26, 26, 1);
    drv_ssd1306_draw_bitmap(61, 7, icon_menu_theme, 26, 26, 1);
    drv_ssd1306_draw_bitmap(90, 7, icon_menu_setting, 26, 26, 1);
    drv_ssd1306_draw_bitmap(3, 35, icon_menu_pc_control, 26, 26, 1);

    /* Draw bound */
    if (system_id_app != 5)
    {
        drv_ssd1306_draw_bitmap(2 + (29 * system_id_app - 29), 6, icon_menu_bound, 28, 28, 1);

        /* Display app name */
        drv_ssd1306_goto_xy(45, 45);

        if (system_id_app == 1)
            drv_ssd1306_puts((uint8_t *) "CLOCK", &font_7x10, DRV_SSD1306_COLOR_WHITE);

        else if (system_id_app == 2)
            drv_ssd1306_puts((uint8_t *) "SNAKE GAME", &font_7x10, DRV_SSD1306_COLOR_WHITE);

        else if (system_id_app == 3)
            drv_ssd1306_puts((uint8_t *) "THEME", &font_7x10, DRV_SSD1306_COLOR_WHITE);

        else if (system_id_app == 4)
            drv_ssd1306_puts((uint8_t *) "DIANOSTIC", &font_7x10, DRV_SSD1306_COLOR_WHITE);
    }
    else
    {
        drv_ssd1306_draw_bitmap(2, 34, icon_menu_bound, 28, 28, 1);

        drv_ssd1306_goto_xy(45, 45);
        drv_ssd1306_puts((uint8_t *) "PC CONTROL", &font_7x10, DRV_SSD1306_COLOR_WHITE);
    }

    drv_ssd1306_update_screen(&drv_ssd1306);

    return SYSTEM_DISPLAY_OK;
}

system_display_error_t system_display_diagnostic(uint8_t *button, uint8_t *object_select)
{
    static button_name_t button_last = NO_EVENT;
    if (*button == button_last)
        return SYSTEM_DISPLAY_OK;
    button_last = *button;

    drv_ssd1306_fill_theme_color();
    /* Display list of all module in smart desk clock */
    drv_ssd1306_goto_xy(20, 0);
    drv_ssd1306_puts((uint8_t *) "Screen", &font_7x10, DRV_SSD1306_COLOR_WHITE);

    drv_ssd1306_goto_xy(20, 10);
    drv_ssd1306_puts((uint8_t *) "Realtime module", &font_7x10, DRV_SSD1306_COLOR_WHITE);

    drv_ssd1306_goto_xy(20, 20);
    drv_ssd1306_puts((uint8_t *) "Temp sensor", &font_7x10, DRV_SSD1306_COLOR_WHITE);

    drv_ssd1306_goto_xy(20, 30);
    drv_ssd1306_puts((uint8_t *) "Button", &font_7x10, DRV_SSD1306_COLOR_WHITE);

    drv_ssd1306_goto_xy(20, 40);
    drv_ssd1306_puts((uint8_t *) "Led RGB", &font_7x10, DRV_SSD1306_COLOR_WHITE);

    drv_ssd1306_goto_xy(20, 50);
    drv_ssd1306_puts((uint8_t *) "Buzzer", &font_7x10, DRV_SSD1306_COLOR_WHITE);

    /* Display pointer to the module user want to check */
    // Down
    if (*button == CLICK_DOWN_BUTTON)
    {
        add_space_pixel += 10;
        if (add_space_pixel >= 50)
            add_space_pixel = 50;
    }
    // Up
    else if (*button == CLICK_UP_BUTTON)
    {
        add_space_pixel -= 10;
        if (add_space_pixel < 0)
            add_space_pixel = 0;
    }

    if (*button == HOLD_SELECT_BUTTON)
    {
        drv_ssd1306_fill_theme_color();
        *object_select  = add_space_pixel / 10;
        add_space_pixel = 0;
    }

    drv_ssd1306_draw_bitmap(0, add_space_pixel, icon_snake_choose_pointer, 7, 7, DRV_SSD1306_COLOR_WHITE);
    drv_ssd1306_update_screen(&drv_ssd1306);
    return SYSTEM_DISPLAY_OK;
}

system_display_error_t system_display_pc_control_mode(uint8_t *button, uint8_t *object_select)
{
    static button_name_t button_last = NO_EVENT;
    if (*button == button_last)
        return SYSTEM_DISPLAY_OK;
    button_last = *button;

    drv_ssd1306_fill_theme_color();
    /* Display list of all module in smart desk clock */
    drv_ssd1306_goto_xy(20, 0);
    drv_ssd1306_puts((uint8_t *) "Command Control", &font_7x10, DRV_SSD1306_COLOR_WHITE);

    drv_ssd1306_goto_xy(20, 10);
    drv_ssd1306_puts((uint8_t *) "Stream video", &font_7x10, DRV_SSD1306_COLOR_WHITE);

    /* Display pointer to the module user want to check */
    // Down
    if (*button == CLICK_DOWN_BUTTON)
    {
        add_space_pixel += 10;
        if (add_space_pixel >= 10)
            add_space_pixel = 10;
    }
    // Up
    else if (*button == CLICK_UP_BUTTON)
    {
        add_space_pixel -= 10;
        if (add_space_pixel < 0)
            add_space_pixel = 0;
    }

    if (*button == HOLD_SELECT_BUTTON)
    {
        *object_select  = add_space_pixel / 10;
        add_space_pixel = 0;
    }

    drv_ssd1306_draw_bitmap(0, add_space_pixel, icon_snake_choose_pointer, 7, 7, DRV_SSD1306_COLOR_WHITE);
    drv_ssd1306_update_screen(&drv_ssd1306);
    return SYSTEM_DISPLAY_OK;
}

system_display_error_t system_display_category_clock(uint8_t *button, uint8_t *object_select)
{
    static button_name_t button_last = NO_EVENT;
    if (*button == button_last)
        return SYSTEM_DISPLAY_OK;
    button_last = *button;

    drv_ssd1306_fill_theme_color();
    /* Display list of all module in smart desk clock */
    drv_ssd1306_goto_xy(20, 0);
    drv_ssd1306_puts((uint8_t *) "Set time clock", &font_7x10, DRV_SSD1306_COLOR_WHITE);

    drv_ssd1306_goto_xy(20, 10);
    drv_ssd1306_puts((uint8_t *) "Set reminder", &font_7x10, DRV_SSD1306_COLOR_WHITE);

    drv_ssd1306_goto_xy(20, 20);
    drv_ssd1306_puts((uint8_t *) "Time countdown", &font_7x10, DRV_SSD1306_COLOR_WHITE);

    drv_ssd1306_goto_xy(20, 30);
    drv_ssd1306_puts((uint8_t *) "Time stopwatch", &font_7x10, DRV_SSD1306_COLOR_WHITE);

    /* Display pointer to the module user want to check */
    // Down
    if (*button == CLICK_DOWN_BUTTON)
    {
        add_space_pixel += 10;
        if (add_space_pixel >= 30)
            add_space_pixel = 30;
    }
    // Up
    else if (*button == CLICK_UP_BUTTON)
    {
        add_space_pixel -= 10;
        if (add_space_pixel < 0)
            add_space_pixel = 0;
    }

    if (*button == HOLD_SELECT_BUTTON)
    {
        *object_select  = add_space_pixel / 10;
        add_space_pixel = 0;
    }

    drv_ssd1306_draw_bitmap(0, add_space_pixel, icon_snake_choose_pointer, 7, 7, DRV_SSD1306_COLOR_WHITE);
    drv_ssd1306_update_screen(&drv_ssd1306);
    return SYSTEM_DISPLAY_OK;
}

system_display_error_t system_display_theme_list(uint8_t *button, uint8_t *object_select)
{
    static button_name_t button_last = NO_EVENT;
    if (*button == button_last)
        return SYSTEM_DISPLAY_OK;
    button_last = *button;

    drv_ssd1306_fill_theme_color();
    /* Display list of all module in smart desk clock */
    drv_ssd1306_goto_xy(20, 0);
    drv_ssd1306_puts((uint8_t *) "Set type clock", &font_7x10, DRV_SSD1306_COLOR_WHITE);

    drv_ssd1306_goto_xy(20, 10);
    drv_ssd1306_puts((uint8_t *) "Set mode color", &font_7x10, DRV_SSD1306_COLOR_WHITE);

    /* Display pointer to the module user want to check */
    // Down
    if (*button == CLICK_DOWN_BUTTON)
    {
        add_space_pixel += 10;
        if (add_space_pixel >= 10)
            add_space_pixel = 10;
    }
    // Up
    else if (*button == CLICK_UP_BUTTON)
    {
        add_space_pixel -= 10;
        if (add_space_pixel < 0)
            add_space_pixel = 0;
    }

    if (*button == HOLD_SELECT_BUTTON)
    {
        *object_select  = add_space_pixel / 10 + 4;
        add_space_pixel = 0;
    }

    drv_ssd1306_draw_bitmap(0, add_space_pixel, icon_snake_choose_pointer, 7, 7, DRV_SSD1306_COLOR_WHITE);
    drv_ssd1306_update_screen(&drv_ssd1306);
    return SYSTEM_DISPLAY_OK;
}

bool system_display_is_screen_ready(void)
{
    return drv_ssd1306.i2c_is_device_ready(drv_ssd1306.device_address);
}

system_display_error_t system_display_type_clock(button_name_t *button_state)
{
    static uint8_t       index_select = 0;
    static button_name_t last_button  = 0;
    if (*button_state == last_button)
        return SYSTEM_DISPLAY_OK;
    last_button = *button_state;

    drv_ssd1306_fill_theme_color();
    /* Display list of all module in smart desk clock */
    drv_ssd1306_goto_xy(20, 0);
    drv_ssd1306_puts((uint8_t *) "Type simple", &font_7x10, DRV_SSD1306_COLOR_WHITE);

    drv_ssd1306_goto_xy(20, 10);
    drv_ssd1306_puts((uint8_t *) "Type digital", &font_7x10, DRV_SSD1306_COLOR_WHITE);

    /* Display pointer to the module user want to check */
    // Down
    if (*button_state == CLICK_DOWN_BUTTON)
    {
        index_select += 10;
        if (index_select >= 10)
            index_select = 10;
    }
    // Up
    else if (*button_state == CLICK_UP_BUTTON)
    {
        index_select -= 10;
        if (index_select < 0)
            index_select = 0;
    }

    if (*button_state == CLICK_SELECT_BUTTON)
    {
        system_display_screen_save();
        if (index_select == 0)
            system_data_set_type_clock(0);
        else
            system_data_set_type_clock(1);
        for (volatile uint32_t i = 0; i < 0xFFFFF; i++) __NOP();
        index_select = 0;
        return SYSTEM_DISPLAY_OK;
    }

    drv_ssd1306_draw_bitmap(0, index_select, icon_snake_choose_pointer, 7, 7, DRV_SSD1306_COLOR_WHITE);
    system_display_update();
    return SYSTEM_DISPLAY_OK;
}

system_display_error_t system_display_theme_color(button_name_t *button_state)
{
    static uint8_t       index_select = 0;
    static button_name_t last_button  = 0;
    if (*button_state == last_button)
        return SYSTEM_DISPLAY_OK;
    last_button = *button_state;

    drv_ssd1306_fill_theme_color();
    /* Display list of all module in smart desk clock */
    drv_ssd1306_goto_xy(20, 0);
    drv_ssd1306_puts((uint8_t *) "Dark mode", &font_7x10, DRV_SSD1306_COLOR_WHITE);

    drv_ssd1306_goto_xy(20, 10);
    drv_ssd1306_puts((uint8_t *) "Light mode", &font_7x10, DRV_SSD1306_COLOR_WHITE);

    /* Display pointer to the module user want to check */
    // Down
    if (*button_state == CLICK_DOWN_BUTTON)
    {
        index_select += 10;
        if (index_select >= 10)
            index_select = 10;
    }
    // Up
    else if (*button_state == CLICK_UP_BUTTON)
    {
        index_select -= 10;
        if (index_select < 0)
            index_select = 0;
    }

    if (*button_state == CLICK_SELECT_BUTTON)
    {
        system_display_screen_save();
        if (index_select == 0)
            system_data_set_theme_color(0);
        else
            system_data_set_theme_color(1);
        system_display_update_theme_color();
        for (volatile uint32_t i = 0; i < 0xFFFFF; i++) __NOP();
        index_select = 0;
        return SYSTEM_DISPLAY_OK;
    }

    drv_ssd1306_draw_bitmap(0, index_select, icon_snake_choose_pointer, 7, 7, DRV_SSD1306_COLOR_WHITE);
    system_display_update();
    return SYSTEM_DISPLAY_OK;
}

system_display_error_t system_display_clock_handle(button_name_t *button_state, uint8_t *object_select)
{
    if (*object_select == SYSTEM_DISPLAY_SET_TIME)
        system_display_set_time(*button_state);

    else if (*object_select == SYSTEM_DISPLAY_SET_REMINDER)
        system_display_time_reminder(*button_state);

    else if (*object_select == SYSTEM_DISPLAY_TIME_COUNTDOWN)
        system_display_time_countdown(*button_state);

    else if (*object_select == SYSTEM_DISPLAY_TIME_STOPWATCH)
        system_display_time_stopwatch(*button_state);

    else if (*object_select == SYSTEM_DISPLAY_TYPE_CLOCK)
        system_display_type_clock(button_state);

    else if (*object_select == SYSTEM_DISPLAY_THEME_COLOR)
        system_display_theme_color(button_state);

    return SYSTEM_DISPLAY_OK;
}

void system_display_update(void)
{
    drv_ssd1306_update_screen(&drv_ssd1306);
}
/* End of file -------------------------------------------------------- */
