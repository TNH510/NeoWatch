/**
 * @file       DRV_SSDD1306.h
 * @copyright  Copyright (C) 2023 QuyLe Co., Ltd. All rights reserved.
 * @license    This project is released under the QuyLe License.
 * @version    v1.0.1
 * @date       2023-09-06
 * @author     Quy Le
 *
 * @brief      driver ssd1306
 *
 * @note
 */

/* Define to prevent recursive inclusion ------------------------------ */
#ifndef __DRV_SSD1306_H
#define __DRV_SSD1306_H

/* Includes ----------------------------------------------------------- */
#include "drv_cjmcu75.h"
#include "drv_ds1307.h"
#include "fonts.h"
#include "icon_clock.h"
#include "main.h"
#include "system_data.h"

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
/* Public defines ----------------------------------------------------- */

#define DRV_SSD1306_I2C_DMA 1

/* DRV_SSDD1306 width in pixels */
#define DRV_SSD1306_WIDTH   128

/* DRV_SSDD1306 LCD height in pixels */
#define DRV_SSD1306_HEIGHT  64

/* Public enumerate/structure ----------------------------------------- */

/**
 * @brief LM77 error codes
 */
typedef enum
{
    DRV_SSD1306_OK,
    DRV_SSD1306_ERROR,
    DRV_SSD1306_ERROR_PARAMETER,
} drv_ssd1306_err_t;

/**
 * @brief  drv ssd1306 color enumeration
 */
typedef enum
{
    DRV_SSD1306_COLOR_BLACK = 0x00, /*!< Black color, no pixel */
    DRV_SSD1306_COLOR_WHITE = 0x01  /*!< Pixel is set. Color depends on LCD */
} drv_ssd1306_color_t;

/**
 * @brief SSD1305 struct
 */
typedef struct
{
    uint8_t device_address; /* I2C device address */
    /* Function pointers --------------------------------------------------------- */
    bool (*i2c_is_device_ready)(uint8_t device_address);
    bool (*i2c_read_at)(uint8_t device_address, uint8_t reg_read, uint8_t *data_read, uint16_t size_data);
    bool (*i2c_write_at)(uint8_t device_address, uint8_t reg_write, uint8_t *data_write, uint16_t size_data);
} drv_ssd1306_config_t;

/* Public macros ------------------------------------------------------ */

/* Public variables --------------------------------------------------- */

/* Public function prototypes ----------------------------------------- */
/**
 * @brief Init driver for screen
 *
 * @param drv_ssd1306         Object ssd1306
 * @return drv_ssd1306_err_t  Function status
 */
drv_ssd1306_err_t drv_ssd1306_init(drv_ssd1306_config_t *drv_ssd1306);

/**
 * @brief Display char in screen
 *
 * @param ch                  Char want to display
 * @param font                Font want to use
 * @param color               Color want to display
 * @return drv_ssd1306_err_t  Function status
 */
drv_ssd1306_err_t drv_ssd1306_putc(uint8_t ch, font_def_t *font, drv_ssd1306_color_t color);

/**
 * @brief Display string in screen
 *
 * @param str                 String want to display
 * @param font                Font want to use
 * @param color               Color want to display
 * @return drv_ssd1306_err_t  Function status
 */
drv_ssd1306_err_t drv_ssd1306_puts(uint8_t *str, font_def_t *font, drv_ssd1306_color_t color);

/**
 * @brief Update screen change
 *
 * @param drv_ssd1306         Object ssd1306
 * @return drv_ssd1306_err_t  Function status
 */
drv_ssd1306_err_t drv_ssd1306_update_screen(drv_ssd1306_config_t *drv_ssd1306);

/**
 * @brief Fill screen in black or white
 *
 * @param color                Color want to fill
 * @return drv_ssd1306_color_t Function status
 */
drv_ssd1306_color_t drv_ssd1306_fill(drv_ssd1306_color_t color);

/**
 * @brief Draw bitmap picture on screen
 *
 * @param x                    Start point x axis
 * @param y                    Start point y axis
 * @param bitmap               Start address of bitmap picture
 * @param w                    Width
 * @param h                    High
 * @param color                Black or White
 * @return drv_ssd1306_color_t Function status
 */
drv_ssd1306_color_t
drv_ssd1306_draw_bitmap(int16_t x, int16_t y, const unsigned char *bitmap, int16_t w, int16_t h, uint16_t color);

/**
 * @brief Draw pixel on screen
 * 
 * @param x                    Start point x axis
 * @param y                    Start point y axis
 * @param color                Black or White
 * @return drv_ssd1306_color_t Function status
 */
drv_ssd1306_err_t drv_ssd1306_draw_pixel(uint16_t x, uint16_t y, drv_ssd1306_color_t color);

/**
 * @brief Display clock analog clock
 * 
 * @param drv_ssd1306          Object ssd1306
 * @param rtc                  Time in trc struct
 * @return drv_ssd1306_color_t Function status 
 */
drv_ssd1306_err_t drv_ds1306_clock_style_simple(drv_ssd1306_config_t *drv_ssd1306, rtc_t rtc);

/**
 * @brief Display clock digital clock
 * 
 * @param drv_ssd1306          Object ssd1306
 * @param temp                 Temperature value
 * @param rtc                  Time in trc struct
 * @return drv_ssd1306_color_t Function status 
 */
drv_ssd1306_err_t drv_ds1306_clock_style_digital(drv_ssd1306_config_t *drv_ssd1306, rtc_t rtc, int8_t temp);

/**
 * @brief Display char in screen in custom font
 *
 * @param ch                  Char want to display
 * @param font                Font want to use
 * @param color               Color want to display
 * @return drv_ssd1306_err_t  Function status
 */
drv_ssd1306_err_t drv_ssd1306_send(uint8_t ch, font_def_custom_t *font, drv_ssd1306_color_t color);

/**
 * @brief Goto any point in screen (prepare to put char or put string)
 * 
 * @param x                    Start point x axis
 * @param y                    Start point y axis
 * @return drv_ssd1306_color_t Function status
 */
drv_ssd1306_err_t drv_ssd1306_goto_xy(uint16_t x, uint16_t y);

/**
 * @brief Draw rectangle in screen
 * 
 * @param x                    Start point x axis
 * @param y                    Start point y axis
 * @param w                    Width of rectangle
 * @param h                    High of rectangle
 * @param color                Black or White
 */
void drv_ssd1306_draw_rectangle(uint16_t x, uint16_t y, uint16_t w, uint16_t h, drv_ssd1306_color_t color);

/**
 * @brief Invert screen color
 * 
 * @param drv_ssd1306          Object ssd1306
 * @param invert               Choose invert or not
 * @return drv_ssd1306_color_t Function status
 */
drv_ssd1306_err_t drv_ssd1306_invert_display(drv_ssd1306_config_t *drv_ssd1306, uint8_t invert);

/**
 * @brief Fill screen with Black or White (choose in drv_ssd1306_invert_display())
 * 
 * @return drv_ssd1306_color_t Function status 
 */
drv_ssd1306_color_t drv_ssd1306_fill_theme_color(void);

#endif  // __DRV_SSD1306_H

/* End of file -------------------------------------------------------- */
