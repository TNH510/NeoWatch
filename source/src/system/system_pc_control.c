/**
 * @file       system_pc_control.c
 * @copyright  Copyright (C) 2019 Fiot Co., Ltd. All rights reserved.
 * @license    This project is released under the Fiot License.
 * @version    1.0.0
 * @date       2023-09-05
 * @author     Hieu Tran
 *
 * @brief      System to manage pc_control events
 */

/* Includes ----------------------------------------------------------- */
#include "system_pc_control.h"

/* Private defines ---------------------------------------------------- */

/* Private enumerate/structure ---------------------------------------- */

/* Private enumerate/structure ---------------------------------------- */

/* Private macros ----------------------------------------------------- */

/* Public variables --------------------------------------------------- */
extern ds1307_t ds1307;
/* Private variables -------------------------------------------------- */
static bool    enable_transmit = true;
static uint8_t command_array[30];
static uint8_t read_once = 0;
/* Private function prototypes ---------------------------------------- */
static uint8_t str_to_byte(char *buffer);

static uint16_t str_to_int16_3_digit(char *buffer);

static bool system_pc_check_command_array(void);

static void system_pc_reset_command_array(void);

static bool system_pc_check_year(uint8_t command_array[22], uint8_t start_pos_check);

static bool system_pc_check_month(uint8_t command_array[22], uint8_t start_pos_check);

static bool system_pc_check_day(uint8_t command_array[22], uint8_t start_pos_check);

static bool system_pc_check_hour(uint8_t command_array[22], uint8_t start_pos_check);

static bool system_pc_check_min(uint8_t command_array[22], uint8_t start_pos_check);

static bool system_pc_check_sec(uint8_t command_array[22], uint8_t start_pos_check);

static bool system_pc_check_space(uint8_t command_array[22], uint8_t start_pos_check);

static bool system_pc_check_time_format(uint8_t command_array[22], uint8_t start_pos_check);
/* Function definitions ----------------------------------------------- */
static uint8_t str_to_byte(char *buffer)
{
    uint8_t byte_data;

    byte_data = ((buffer[0] - '0') * 10) + (buffer[1] - '0');

    return byte_data;
}

static uint16_t str_to_int16_3_digit(char *buffer)
{
    uint16_t byte_data;

    byte_data = (((buffer[0] - '0') * 100) + ((buffer[1] - '0') * 10) + (buffer[2] - '0'));

    return byte_data;
}

static bool system_pc_check_year(uint8_t command_array[22], uint8_t start_pos_check)
{
    uint8_t year = str_to_byte(command_array + start_pos_check);

    if (year > 99 || year < 0)
        return false;
    else
        return true;
}

static bool system_pc_check_month(uint8_t command_array[22], uint8_t start_pos_check)
{
    uint8_t month = str_to_byte(command_array + start_pos_check + 3);

    if (month > 12 || month < 0)
        return false;
    else
        return true;
}

static bool system_pc_check_day(uint8_t command_array[22], uint8_t start_pos_check)
{
    uint8_t day = str_to_byte(command_array + start_pos_check + 6);

    if (day > 31 || day < 0)
        return false;
    else
        return true;
}

static bool system_pc_check_hour(uint8_t command_array[22], uint8_t start_pos_check)
{
    uint8_t hour = str_to_byte(command_array + start_pos_check + 9);

    if (hour > 23 || hour < 0)
        return false;
    else
        return true;
}

static bool system_pc_check_min(uint8_t command_array[22], uint8_t start_pos_check)
{
    uint8_t min = str_to_byte(command_array + start_pos_check + 12);

    if (min > 59 || min < 0)
        return false;
    else
        return true;
}

static bool system_pc_check_sec(uint8_t command_array[22], uint8_t start_pos_check)
{
    uint8_t sec = str_to_byte(command_array + start_pos_check + 15);

    if (sec > 59 || sec < 0)
        return false;
    else
        return true;
}

static bool system_pc_check_space(uint8_t command_array[22], uint8_t start_pos_check)
{
    if (command_array[start_pos_check + 2] != '-' || command_array[start_pos_check + 5] != '-'
        || command_array[start_pos_check + 11] != ':' || command_array[start_pos_check + 14] != ':')
        return false;
    else
        return true;
}

static bool system_pc_check_time_format(uint8_t command_array[30], uint8_t start_pos_check)
{
    if ((system_pc_check_year(command_array, start_pos_check) && system_pc_check_month(command_array, start_pos_check)
         && system_pc_check_day(command_array, start_pos_check) && system_pc_check_hour(command_array, start_pos_check)
         && system_pc_check_min(command_array, start_pos_check) && system_pc_check_sec(command_array, start_pos_check)
         && system_pc_check_space(command_array, start_pos_check))
        == true)
        return true;
    else
        return false;
}

static void system_pc_reset_command_array(void)
{
    for (uint8_t i = 0; i < sizeof(command_array) / sizeof(command_array[0]); i++)
    {
        command_array[i] = 0x00u;
    }
}

static bool system_pc_check_command_array(void)
{
    bool is_empty = false;

    for (uint8_t i = 0; i < sizeof(command_array) / sizeof(command_array[0]); i++)
    {
        if (command_array[i] == 0x00u)
            is_empty = true;
        else
        {
            is_empty = false;
            return is_empty;
        }
    }

    return is_empty;
}

void system_pc_control_reset(void)
{
    enable_transmit = true;
    read_once       = 0;
    drv_uart_clear_cb();
    system_pc_reset_command_array();
}

system_pc_control_error_t system_pc_control_init(void)
{
    drv_uart_init();
    return SYSTEM_PC_CONTROL_OK;
}

system_pc_control_error_t system_pc_control_handle(button_name_t *button_state, uint8_t *object_select)
{
    if (*object_select == SYSTEM_PC_CONTROL_COMMAND)
    {
        if (system_pc_control_command_run(button_state) == SYSTEM_PC_CONTROL_COMMAND_CANCEL)
        {
            *object_select = 0xff;
        }
    }
    else if (*object_select == SYSTEM_PC_CONTROL_STREAM_VIDEO)
    {
        system_pc_control_stream_video(button_state);
    }

    return SYSTEM_PC_CONTROL_OK;
}

system_pc_control_command_t system_pc_control_command_run(button_name_t *button_state)
{
    /* Display screen and some instructions */
    uint8_t mode = 0;
    system_data_get_type_clock(&mode);
    system_display_clock(mode);
    /* Transmit notify this is set time mode */
    if (enable_transmit == true)
    {
        drv_uart_printf((uint8_t *) "\r\n WELCOME TO SMART DESK CLOCK: \r\n");
        enable_transmit = false;
    }

    /* Read data from cbuffer */
    drv_uart_read_cb_data(command_array, drv_uart_num_unread_cb_data());

    /* TIME set command */
    if (!strncmp(command_array, "TIME", 4))
    {
        if (system_pc_check_time_format(command_array, 7) == true)
        {
            /* Handle set time command */
            drv_ssd1306_fill(0);
            ds1307_set_time(&ds1307, command_array + 5);
            drv_uart_printf((uint8_t *) "\r\n TIME SET OK \r\n");
        }
        else
        {
            drv_uart_printf((uint8_t *) "\r\n NOT CORRECT TIME FORMAT \r\n");
        }
    }
    /* REMINDER set command */
    else if (!strncmp(command_array, "REMINDER", 8))
    {
        if (!strncmp(command_array, "REMINDER DELETE ALL", 19))
        {
            system_time_clear_all_reminder();
        }
        else if (system_pc_check_time_format(command_array, 11) == true)
        {
            /* Handle set reminder command */
            drv_ssd1306_fill(0);
            system_time_set_reminder_str(command_array + 9);
            drv_uart_printf((uint8_t *) "\r\n REMINDER SET OK \r\n");
        }
        else
        {
            drv_uart_printf((uint8_t *) "\r\n NOT CORRECT FORMAT \r\n");
        }
    }
    /* THEME set command */
    else if (!strncmp(command_array, "THEME", 5))
    {
        if (!strncmp(command_array, "THEME DARK", 10))
        {
            system_data_set_theme_color(0);
            system_display_update_theme_color();
            drv_uart_printf((uint8_t *) "\r\n THEME DARK SET OK \r\n");
        }
        else if (!strncmp(command_array, "THEME LIGHT", 11))
        {
            system_data_set_theme_color(1);
            system_display_update_theme_color();
            drv_uart_printf((uint8_t *) "\r\n THEME LIGHT SET OK \r\n");
        }
        else
        {
            drv_uart_printf((uint8_t *) "\r\n NOT CORRECT THEME FORMAT \r\n");
        }
    }
    /* CLOCK type set command */
    else if (!strncmp(command_array, "CLOCK", 5))
    {
        if (!strncmp(command_array, "CLOCK DIGITAL", 13))
        {
            system_data_set_type_clock(1);
            drv_uart_printf((uint8_t *) "\r\n CLOCK DIGITAL SET OK \r\n");
        }
        else if (!strncmp(command_array, "CLOCK ANALOG", 12))
        {
            system_data_set_type_clock(0);
            drv_uart_printf((uint8_t *) "\r\n CLOCK ANALOG SET OK \r\n");
        }
        else
        {
            drv_uart_printf((uint8_t *) "\r\n NOT CORRECT CLOCK SET FORMAT \r\n");
        }
    }
    /* SONG set command */
    else if (!strncmp(command_array, "SONG", 4))
    {
        if (!strncmp(command_array, "SONG SUPERMARIO", 10))
        {
            drv_buzzer_off();
            drv_buzzer_music(Super_Mario);
        }
        else if (!strncmp(command_array, "SONG HAPPYBIRTHDAY", 18))
        {
            drv_buzzer_off();
            drv_buzzer_music(HAPPY_BIRTHDAY);
        }
        else if (!strncmp(command_array, "SONG NOKIA", 10))
        {
            drv_buzzer_off();
            drv_buzzer_music(NOKIA_TUNE);
        }
        else if (!strncmp(command_array, "SONG MERRYCHRISTMAS", 19))
        {
            drv_buzzer_off();
            drv_buzzer_music(Merry_CHRISTMAS);
        }
        else if (!strncmp(command_array, "SONG OFF", 8))
        {
            drv_buzzer_off();
        }
        else
        {
            drv_uart_printf((uint8_t *) "\r\n NOT CORRECT NAME SONG \r\n");
        }
    }
    /* VOLUME command */
    else if (!strncmp(command_array, "VOLUME", 6))
    {
        uint8_t volume = str_to_byte(command_array + 7);

        /* Check volume value */
        if (volume >= 0 && volume <= 10)
        {
            /* Change volume */
            drv_buzzer_volume(volume);

            char notify_volume[20];
            sprintf(notify_volume, "\r\n VOLUME SET LEVEL %d \r\n", volume);

            drv_uart_printf(notify_volume);
        }
        else
        {
            drv_uart_printf((uint8_t *) "\r\n NOT ACCEPTED VALUE (0->10) \r\n");
        }
    }
    /* LED command */
    else if (!strncmp(command_array, "LED", 3))
    {
        if (!strncmp(command_array, "LED OFF", 7))
        {
            drv_led_off();
        }
        /* Check r,b,g value */
        else if (!strncmp(command_array, "LED SET", 7))
        {
            uint16_t r = str_to_int16_3_digit(command_array + 8);
            uint16_t g = str_to_int16_3_digit(command_array + 12);
            uint16_t b = str_to_int16_3_digit(command_array + 16);
            uint16_t color[3];
            color[0] = r;
            color[1] = g;
            color[2] = b;

            if ((r >= 0 && r <= 255) && (g >= 0 && g <= 255) && (b >= 0 && b <= 255))
            {
                /* Change led color */
                drv_led_off();
                drv_led_set_color(color);

                char notify_led[20];
                sprintf(notify_led, "\r\n LED SET %d %d %d \r\n", r, g, b);
                drv_uart_printf(notify_led);
            }
            else
            {
                drv_uart_printf((uint8_t *) "\r\n NOT ACCEPTED VALUE (0->255) \r\n");
            }
        }
        else
        {
            drv_uart_printf((uint8_t *) "\r\n NOT CORRECT LED FORMAT \r\n");
        }
    }
    /* CANCEL pc-control mode command */
    else if (!strncmp(command_array, "CANCEL", 6))
    {
        return SYSTEM_PC_CONTROL_COMMAND_CANCEL;
    }
    /* Reset command */
    else if (!strncmp(command_array, "REBOOT", 6))
    {
        HAL_NVIC_SystemReset();
    }
    else if (!system_pc_check_command_array())
    {
        drv_uart_printf((uint8_t *) "\r\n WRONG COMMAND, PLEASE TRY AGAIN \r\n");
    }

    system_display_update();
    system_pc_reset_command_array();
    return SYSTEM_PC_CONTROL_COMMAND_RUN;
}

system_pc_control_error_t system_pc_control_stream_video(button_name_t *button_state)
{
    static uint8_t data[1024] = { 0 };
    if (!read_once)
    {
        drv_ssd1306_fill_theme_color();
        drv_ssd1306_goto_xy(10, 35);
        drv_ssd1306_puts((uint8_t *) "Waiting for data...", &font_7x10, DRV_SSD1306_COLOR_WHITE);
        system_display_update();
        read_once = 1;
    }

    if (drv_uart_num_unread_cb_data() >= 1023)
    {
        drv_ssd1306_fill_theme_color();
        drv_uart_read_cb_data(data, 1024);
        drv_ssd1306_draw_bitmap(0, 0, data, 128, 64, 1);
        system_display_update();
        drv_uart_printf((uint8_t *) "R");
    }
    return SYSTEM_PC_CONTROL_OK;
}
/* End of file -------------------------------------------------------- */
