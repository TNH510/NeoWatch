/**
 * @file       system_diagnostic.c
 * @copyright  Copyright (C) 2019 Fiot Co., Ltd. All rights reserved.
 * @license    This project is released under the Fiot License.
 * @version    1.0.0
 * @date       2023-09-04
 * @author     Hieu Tran
 *
 * @brief      System to manage diagnostic events
 */

/* Includes ----------------------------------------------------------- */
#include "system_diagnostic.h"

#include "drv_uart.h"
#include "video_frame.h"

/* Private defines ---------------------------------------------------- */
static uint8_t  count_time    = 0;
static uint16_t count_x_axis  = 0;
static uint8_t  volume        = 10;
static uint16_t frame_number  = 0;
static uint8_t  name_mussic[] = { 'S', 'u', 'p', 'e', 'r', ' ', 'M', 'a', 'r', 'i', 'o', ' ', 'S', 'o', 'n', 'g', '.', ' ' };
/* Private enumerate/structure ---------------------------------------- */
enum system_diagnostic_enum
{
    SYSTEM_DIAGNOSTIC_SCREEN,
    SYSTEM_DIAGNOSTIC_REALTIME,
    SYSTEM_DIAGNOSTIC_TEMP,
    SYSTEM_DIAGNOSTIC_BUTTON,
    SYSTEM_DIAGNOSTIC_LED,
    SYSTEM_DIAGNOSTIC_BUZZER,
};
/* Private enumerate/structure ---------------------------------------- */

/* Private macros ----------------------------------------------------- */

/* Public variables --------------------------------------------------- */

/* Private variables -------------------------------------------------- */

/* Private function prototypes ---------------------------------------- */

/* Function definitions ----------------------------------------------- */
system_diagnostic_error_t system_diagnostic_init(void)
{
    return SYSTEM_DIAGNOSTIC_OK;
}
system_diagnostic_error_t system_diagnostic_reset_variable(void)
{
    frame_number  = 0;
}

system_diagnostic_error_t system_diagnostic_screen(button_name_t *button_state)
{
    if (frame_number < 142)
    {
        drv_ssd1306_fill_theme_color();
        drv_ssd1306_draw_bitmap(0, 0, throw_computer_video[frame_number], 128, 64, 1);
        bsp_delay(30);
        frame_number++;
    }
    else
    {
        drv_ssd1306_fill_theme_color();
        drv_ssd1306_goto_xy(1, 10);
        drv_ssd1306_puts((uint8_t *) "End Video", &font_6x8, DRV_SSD1306_COLOR_WHITE);
        drv_ssd1306_goto_xy(1, 30);
        drv_ssd1306_puts((uint8_t *) "Can you see anything ?", &font_6x8, DRV_SSD1306_COLOR_WHITE);
    }
    system_display_update();
    return SYSTEM_DIAGNOSTIC_OK;
}

system_diagnostic_error_t system_diagnostic_realtime(button_name_t *button_state)
{
    drv_ssd1306_fill_theme_color();
    drv_ssd1306_goto_xy(35, 45);

    if (system_time_is_realtime_ready() == true)
    {
        drv_ssd1306_puts((uint8_t *) "CONNECTED", &font_7x10, DRV_SSD1306_COLOR_WHITE);
    }
    else
    {
        drv_ssd1306_puts((uint8_t *) "DISCONNECTED", &font_7x10, DRV_SSD1306_COLOR_WHITE);
    }

    system_display_update();
    return SYSTEM_DIAGNOSTIC_OK;
}

system_diagnostic_error_t system_diagnostic_temp_sensor(button_name_t *button_state)
{
    drv_ssd1306_fill_theme_color();
    drv_ssd1306_goto_xy(35, 45);

    if (system_time_is_temp_sensor_ready() == true)
    {
        drv_ssd1306_puts((uint8_t *) "CONNECTED", &font_7x10, DRV_SSD1306_COLOR_WHITE);
    }
    else
    {
        drv_ssd1306_puts((uint8_t *) "DISCONNECTED", &font_7x10, DRV_SSD1306_COLOR_WHITE);
    }

    system_display_update();
    return SYSTEM_DIAGNOSTIC_OK;
}

system_diagnostic_error_t system_diagnostic_button(button_name_t *button_state)
{
    drv_ssd1306_fill_theme_color();

    /* Draw button map */
    drv_ssd1306_draw_bitmap(0, 0, icon_button_theme, 128, 64, 1);

    /* Display oke with button have click */
    switch (*button_state)
    {
    case CLICK_UP_BUTTON:
        drv_ssd1306_goto_xy(90, 15);
        drv_ssd1306_puts((uint8_t *) "C", &font_6x8, DRV_SSD1306_COLOR_WHITE);
        break;

    case CLICK_DOWN_BUTTON:
        drv_ssd1306_goto_xy(90, 42);
        drv_ssd1306_puts((uint8_t *) "C", &font_6x8, DRV_SSD1306_COLOR_WHITE);
        break;

    case CLICK_LEFT_BUTTON:
        drv_ssd1306_goto_xy(70, 29);
        drv_ssd1306_puts((uint8_t *) "C", &font_6x8, DRV_SSD1306_COLOR_WHITE);
        break;

    case CLICK_RIGHT_BUTTON:
        drv_ssd1306_goto_xy(110, 29);
        drv_ssd1306_puts((uint8_t *) "C", &font_6x8, DRV_SSD1306_COLOR_WHITE);
        break;
    case CLICK_CANCEL_BUTTON:
        drv_ssd1306_goto_xy(23, 40);
        drv_ssd1306_puts((uint8_t *) "C", &font_6x8, DRV_SSD1306_COLOR_WHITE);
        break;

    case CLICK_SELECT_BUTTON:
        drv_ssd1306_goto_xy(23, 15);
        drv_ssd1306_puts((uint8_t *) "C", &font_6x8, DRV_SSD1306_COLOR_WHITE);
        break;

    case HOLD_UP_BUTTON:
        drv_ssd1306_goto_xy(90, 15);
        drv_ssd1306_puts((uint8_t *) "H", &font_6x8, DRV_SSD1306_COLOR_WHITE);
        break;

    case HOLD_DOWN_BUTTON:
        drv_ssd1306_goto_xy(90, 42);
        drv_ssd1306_puts((uint8_t *) "H", &font_6x8, DRV_SSD1306_COLOR_WHITE);
        break;

    case HOLD_LEFT_BUTTON:
        drv_ssd1306_goto_xy(70, 29);
        drv_ssd1306_puts((uint8_t *) "H", &font_6x8, DRV_SSD1306_COLOR_WHITE);
        break;

    case HOLD_RIGHT_BUTTON:
        drv_ssd1306_goto_xy(110, 29);
        drv_ssd1306_puts((uint8_t *) "H", &font_6x8, DRV_SSD1306_COLOR_WHITE);
        break;
    case HOLD_CANCEL_BUTTON:
        drv_ssd1306_goto_xy(23, 40);
        drv_ssd1306_puts((uint8_t *) "H", &font_6x8, DRV_SSD1306_COLOR_WHITE);
        break;

    case HOLD_SELECT_BUTTON:
        drv_ssd1306_goto_xy(23, 15);
        drv_ssd1306_puts((uint8_t *) "H", &font_6x8, DRV_SSD1306_COLOR_WHITE);
        break;

    default: break;
    }

    system_display_update();
    return SYSTEM_DIAGNOSTIC_OK;
}

system_diagnostic_error_t system_diagnostic_led_rgb(button_name_t *button_state)
{
    drv_ssd1306_fill_theme_color();
    drv_led_fade_in_fade_out();

    drv_ssd1306_goto_xy(0, 10);
    drv_ssd1306_puts((uint8_t *) "Do you see anything?", &font_6x8, DRV_SSD1306_COLOR_WHITE);

    drv_ssd1306_goto_xy(0, 20);
    drv_ssd1306_puts((uint8_t *) "If it is,check power ", &font_6x8, DRV_SSD1306_COLOR_WHITE);

    drv_ssd1306_goto_xy(0, 30);
    drv_ssd1306_puts((uint8_t *) "and check wire connect", &font_6x8, DRV_SSD1306_COLOR_WHITE);

    drv_ssd1306_goto_xy(0, 40);
    drv_ssd1306_puts((uint8_t *) "Remember:Not our bug", &font_6x8, DRV_SSD1306_COLOR_WHITE);

    if (*button_state == CLICK_CANCEL_BUTTON)
    {
        drv_led_off();
        return SYSTEM_DIAGNOSTIC_OK;
    }

    system_display_update();
    return SYSTEM_DIAGNOSTIC_OK;
}

system_diagnostic_error_t system_diagnostic_buzzer(button_name_t *button_state)
{
    drv_buzzer_music(Super_Mario);

    /* Display name song */
    if (count_time == 2)
    {
        uint16_t x_axis = 0;
        for (uint16_t i = 0; i < strlen(name_mussic); i++)
        {
            x_axis = (i * 7 + count_x_axis) % 128;
            drv_ssd1306_goto_xy(x_axis, 1);
            drv_ssd1306_puts(&name_mussic[i], &font_7x10, DRV_SSD1306_COLOR_WHITE);
        }
        count_time   = 0;
        count_x_axis = (count_x_axis > 128) ? 0 : count_x_axis + 1;
    }

    /* Display volume value */
    drv_ssd1306_goto_xy(10, 20);
    uint8_t volume_string[20];
    sprintf(volume_string, "Volume : %02u", volume);
    drv_ssd1306_puts(volume_string, &font_7x10, DRV_SSD1306_COLOR_WHITE);
    drv_buzzer_volume(volume);

    system_display_update();
    count_time++;
    bsp_delay(1);

    if (*button_state == CLICK_CANCEL_BUTTON)
    {
        drv_buzzer_off();
        return SYSTEM_DIAGNOSTIC_OK;
    }

    if (*button_state == CLICK_UP_BUTTON)
    {
        volume = (volume >= 10) ? 10 : volume + 1;
        // drv_ssd1306_fill_theme_color();
    }

    if (*button_state == CLICK_DOWN_BUTTON)
    {
        volume = (volume <= 0) ? 0 : volume - 1;
        // drv_ssd1306_fill_theme_color();
    }

    system_display_update();
    return SYSTEM_DIAGNOSTIC_ERROR;
}

system_diagnostic_error_t system_diagnostic_handle(button_name_t *button_state, uint8_t *object_select)
{
    if (*object_select == SYSTEM_DIAGNOSTIC_SCREEN)
        system_diagnostic_screen(button_state);

    else if (*object_select == SYSTEM_DIAGNOSTIC_REALTIME)
        system_diagnostic_realtime(button_state);

    else if (*object_select == SYSTEM_DIAGNOSTIC_TEMP)
        system_diagnostic_temp_sensor(button_state);

    else if (*object_select == SYSTEM_DIAGNOSTIC_BUTTON)
        system_diagnostic_button(button_state);

    else if (*object_select == SYSTEM_DIAGNOSTIC_LED)
        system_diagnostic_led_rgb(button_state);

    else if (*object_select == SYSTEM_DIAGNOSTIC_BUZZER)
        system_diagnostic_buzzer(button_state);

    return SYSTEM_DIAGNOSTIC_OK;
}

/* End of file -------------------------------------------------------- */
