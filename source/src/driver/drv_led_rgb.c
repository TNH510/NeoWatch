/**
 * @file       drv_led_rgb.c
 * @copyright  Copyright (C) 2019 Fiot Co., Ltd. All rights reserved.
 * @license    This project is released under the Fiot License.
 * @version    0.0.1
 * @date       2023
 * @author     Quynh Le
 * @author     Quynh Le
 *
 * @brief      Driver for LED RGB
 *
 * @note       None
 * @example    None
 */

/* Includes ----------------------------------------------------------- */
#include "drv_led_rgb.h"

/* Private defines ---------------------------------------------------- */
#define DELAY_BLINK       (100)
#define NUMBER_COLOR_FIFO (7)
#define LED_AUTORELOAD        (1000-1)
#define LED_PRESCALE          (100-1)
/* Private enumerate/structure ---------------------------------------- */
/* Private macros ----------------------------------------------------- */
/* Public variables --------------------------------------------------- */
/* Private variables -------------------------------------------------- */
static uint8_t color_table[NUMBER_COLOR_FIFO][3] = {

    { 255, 0, 0 } , {0,255,0} , {0,0,255} , {255,255,0} , {255,0,255} , {0,255,255} , {255,255,255}
};
static uint16_t color_blink[3];
static uint16_t count_blink      = 0;
static uint16_t count_fifo       = 0;
static uint16_t count_fifo_color = 0;
static uint16_t count_fifo_pwm   = 0;
static bool     fade_in_fade_out = false;
static bool     flag_to_blink    = false;
static bool     flag_to_fifo     = false;
/* Private function prototypes ---------------------------------------- */
/* Function definitions ----------------------------------------------- */
void drv_led_init()
{
	bsp_tim_set_prescale(&htim3, LED_PRESCALE);
	bsp_tim_set_autoreload(&htim3, LED_AUTORELOAD);
    bsp_tim3_start_tim_it();
    bsp_tim3_pwm_start();
}

void drv_led_off()
{
    uint16_t buffer_color[3] = { 0, 0, 0 };
    drv_led_set_color(buffer_color);
    flag_to_blink = false;
    flag_to_fifo = false ;
}

void drv_led_set_color(uint16_t *buffer_color)
{
    // counter period = 1000-1
    // convert value of buffer_color to the correct ratio of counter period
    uint16_t pwm_ch1 = (buffer_color[0] * 1000) / 255;
    uint16_t pwm_ch2 = (buffer_color[1] * 1000) / 255;
    uint16_t pwm_ch3 = (buffer_color[2] * 1000) / 255;

    bsp_tim3_pwm_set_compare(RED_CHANNEL, pwm_ch1);
    bsp_tim3_pwm_set_compare(GREEN_CHANNEL, pwm_ch2);
    bsp_tim3_pwm_set_compare(BLUE_CHANNEL, pwm_ch3);
}

void drv_led_blink_color(uint16_t *buffer_color)
{
    flag_to_blink = true;
    memcpy(color_blink, buffer_color, sizeof(color_blink));
}

void drv_led_fade_in_fade_out()
{
    flag_to_fifo = true;
}

void bsp_tim3_handle()
{
    // blink led rgb
    if ((color_blink[0] != 0 || color_blink[1] != 0 || color_blink[2] != 0) && flag_to_blink == true)
    {
        count_blink++;
        if (count_blink == 1000 - 1)
        {
            drv_led_set_color(color_blink);
            count_blink = 0;
        }
        else if (count_blink == 500 - 1)
        {
            uint16_t buffer_color[3] = { 0, 0, 0 };
            drv_led_set_color(buffer_color);
        }
    }
    // enable fade in fade out mode for led rgb
    else if (flag_to_fifo == true)
    {
        count_fifo++;

        // delay 9 ms for each level of pwm
        if (count_fifo == 9)
        {
            uint16_t color_pwm_ch[3];
            color_pwm_ch[0] = count_fifo_pwm * color_table[count_fifo_color][0] / 1000;
            color_pwm_ch[1] = count_fifo_pwm * color_table[count_fifo_color][1] / 1000;
            color_pwm_ch[2] = count_fifo_pwm * color_table[count_fifo_color][2] / 1000;

            drv_led_set_color(color_pwm_ch);

            // fade in mode
            if (fade_in_fade_out == false)
            {
                count_fifo_pwm += 2;

                if (count_fifo_pwm >= 999)
                {
                    fade_in_fade_out = true;
                }
            }
            // fade out mode
            else if (fade_in_fade_out == true)
            {
                count_fifo_pwm -= 2;

                if (count_fifo_pwm == 0)
                {
                	// back to fade in but next color
                    fade_in_fade_out = false;
                    // check whether it reach last color in table color
                    count_fifo_color = (count_fifo_color == NUMBER_COLOR_FIFO -1 ) ? 0 : count_fifo_color + 1;
                }
            }

            // reset delay time
            count_fifo = 0;
        }
    }
}

/* End of file -------------------------------------------------------- */
