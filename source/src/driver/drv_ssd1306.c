/**
 * @file       DRV_SSDD1306.c
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

/* Includes ----------------------------------------------------------- */
#include "drv_ssd1306.h"

#include <math.h>
/* Private defines ---------------------------------------------------- */
/* Clock */
#define CENTER_X                      32
#define CENTER_Y                      32

#define degToRad(angleInDegrees)      ((angleInDegrees) *M_PI / 180.0)
/* Absolute value */
#define ABS(x)                        ((x) > 0 ? (x) : -(x))

#define DRV_SSD1306_DEACTIVATE_SCROLL 0x2E

#define DRV_SSD1306_CHECK_PARA(status) \
    if (!(status))                     \
        return DRV_SSD1306_ERROR_PARAMETER;

#define DRV_SSD1306_CHECK_ERROR(status_1, status_2) \
    if (!(status_1))                                \
        return (status_2);
/* Private enumerate/structure ---------------------------------------- */
typedef struct
{
    uint16_t current_x;
    uint16_t current_y;
    uint8_t  inverted;
    uint8_t  initialized;
} drv_ssd1306_position_t;

static drv_ssd1306_position_t drv_ssd1306_pos;

/* Private macros ----------------------------------------------------- */
#define DRV_SSD1306_REG_COMMAND   0x00
#define DRV_SSD1306_REG_DATA      0x40
#define DRV_SSD1306_NORMALDISPLAY 0xA6
#define DRV_SSD1306_INVERTDISPLAY 0xA7
/* Public variables --------------------------------------------------- */

/* Private variables -------------------------------------------------- */
static char *months[]  = { "Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec" };
static char *weekday[] = { "Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat" };
/* drv_ssd1306 data buffer */
static uint8_t drv_ssd1306_buffer[DRV_SSD1306_WIDTH * DRV_SSD1306_HEIGHT / 8];

/* Private function prototypes ---------------------------------------- */

static drv_ssd1306_err_t drv_ssd1306_write_command(drv_ssd1306_config_t *drv_ssd1306, uint8_t data_write);
static drv_ssd1306_err_t drv_ssd1306_write_data(drv_ssd1306_config_t *drv_ssd1306, uint8_t data_write);
static drv_ssd1306_err_t drv_ssd1306_is_ready(drv_ssd1306_config_t *drv_ssd1306);
static drv_ssd1306_err_t drv_ssd1306_send_data(drv_ssd1306_config_t *drv_ssd1306);
static void              drv_ssd1306_raw_circle(int16_t x0, int16_t y0, int16_t r, drv_ssd1306_color_t color);
static void              drv_ssd1306_raw_fill_circle(int16_t x0, int16_t y0, int16_t r, drv_ssd1306_color_t color);
static void              drv_ssd1306_draw_line(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, drv_ssd1306_color_t color);
static void              drv_ssd1306_draw_triangle(uint16_t            x1,
                                                   uint16_t            y1,
                                                   uint16_t            x2,
                                                   uint16_t            y2,
                                                   uint16_t            x3,
                                                   uint16_t            y3,
                                                   drv_ssd1306_color_t color);

/* Function definitions ----------------------------------------------- */

static drv_ssd1306_err_t drv_ssd1306_write_command(drv_ssd1306_config_t *drv_ssd1306, uint8_t data_write)
{
    DRV_SSD1306_CHECK_ERROR(
      (drv_ssd1306->i2c_write_at(drv_ssd1306->device_address, DRV_SSD1306_REG_COMMAND, &data_write, 1) == true),
      DRV_SSD1306_ERROR);
    return DRV_SSD1306_OK;
}

static drv_ssd1306_err_t drv_ssd1306_write_data(drv_ssd1306_config_t *drv_ssd1306, uint8_t data_write)
{
    DRV_SSD1306_CHECK_ERROR(
      (drv_ssd1306->i2c_write_at(drv_ssd1306->device_address, DRV_SSD1306_REG_DATA, &data_write, 1) == true), DRV_SSD1306_ERROR);
    return DRV_SSD1306_OK;
}

static drv_ssd1306_err_t drv_ssd1306_is_ready(drv_ssd1306_config_t *drv_ssd1306)
{
    DRV_SSD1306_CHECK_PARA(drv_ssd1306 != NULL);
    DRV_SSD1306_CHECK_ERROR((drv_ssd1306->i2c_is_device_ready(drv_ssd1306->device_address) == true), DRV_SSD1306_ERROR);
    return DRV_SSD1306_OK;
}

drv_ssd1306_color_t drv_ssd1306_fill(drv_ssd1306_color_t color)
{
    /* Set memory */
    memset(drv_ssd1306_buffer, (color == DRV_SSD1306_COLOR_BLACK) ? 0x00 : 0xFF, sizeof(drv_ssd1306_buffer));
    return DRV_SSD1306_OK;
}

drv_ssd1306_color_t drv_ssd1306_fill_theme_color(void)
{
    uint8_t theme = 0;
    system_data_get_theme_color(&theme);
    drv_ssd1306_fill(theme);
    return DRV_SSD1306_OK;
}

#if DRV_SSD1306_I2C_DMA == 0
drv_ssd1306_err_t drv_ssd1306_update_screen(drv_ssd1306_config_t *drv_ssd1306)
{
    uint8_t m;
    for (m = 0; m < 8; m++)
    {
        drv_ssd1306_write_command(drv_ssd1306, 0xB0 + m);
        drv_ssd1306_write_command(drv_ssd1306, 0x00);
        drv_ssd1306_write_command(drv_ssd1306, 0x10);
        /* Write multi data */
        DRV_SSD1306_CHECK_ERROR((drv_ssd1306->i2c_write_at(drv_ssd1306->device_address, DRV_SSD1306_REG_DATA,
                                                           &drv_ssd1306_buffer[DRV_SSD1306_WIDTH * m], DRV_SSD1306_WIDTH))
                                  == true,
                                DRV_SSD1306_ERROR);
    }
    return DRV_SSD1306_OK;
}
#elif DRV_SSD1306_I2C_DMA == 1
uint8_t drv_ssd1306_update_status = 0;
uint8_t drv_ssd1306_update_end    = 0;
uint8_t drv_ssd1306_i2c_command   = 0;

drv_ssd1306_err_t drv_ssd1306_update_screen(drv_ssd1306_config_t *drv_ssd1306)
{
    if (drv_ssd1306_update_status == 0)
    {
        drv_ssd1306_update_status = DRV_SSD1306_HEIGHT;
        drv_ssd1306_update_end    = DRV_SSD1306_HEIGHT + (DRV_SSD1306_HEIGHT / 2);
        drv_ssd1306_i2c_command   = 0xB0;
        /* Write multi data */
        DRV_SSD1306_CHECK_ERROR(
          (drv_ssd1306->i2c_write_at(drv_ssd1306->device_address, DRV_SSD1306_REG_COMMAND, &drv_ssd1306_i2c_command, 1)) == true,
          DRV_SSD1306_ERROR);
        drv_ssd1306_send_data(drv_ssd1306);
    }
    else
    {
        drv_ssd1306_send_data(drv_ssd1306);
    }
    return DRV_SSD1306_OK;
}
#else
#error Select mode for drv ssd1306 mode DMA or not DMA I2C !
#endif

#if DRV_SSD1306_I2C_DMA == 1
static drv_ssd1306_err_t drv_ssd1306_send_data(drv_ssd1306_config_t *drv_ssd1306)
{
    uint32_t phase = 0;
start:;
    if (drv_ssd1306_update_status)
    {
        if (drv_ssd1306_update_status < drv_ssd1306_update_end)
        {
            drv_ssd1306_update_status++;
            phase = drv_ssd1306_update_status & 3;
            if (phase == 3)
            {
                /* Write multi data */
                DRV_SSD1306_CHECK_ERROR(
                  (drv_ssd1306->i2c_write_at(
                    drv_ssd1306->device_address, DRV_SSD1306_REG_DATA,
                    &drv_ssd1306_buffer[DRV_SSD1306_WIDTH * ((drv_ssd1306_update_status >> 2) & (DRV_SSD1306_HEIGHT / 8 - 1))],
                    DRV_SSD1306_WIDTH))
                    == true,
                  DRV_SSD1306_ERROR);
                goto start;
            }
            else
            {
                if (phase == 0)
                    drv_ssd1306_i2c_command = 0xB0 + ((drv_ssd1306_update_status >> 2) & (DRV_SSD1306_HEIGHT / 8 - 1));
                else if (phase == 1)
                    drv_ssd1306_i2c_command = 0x00;
                else if (phase == 2)
                    drv_ssd1306_i2c_command = 0x10;
                /* Write multi data */
                DRV_SSD1306_CHECK_ERROR(
                  drv_ssd1306->i2c_write_at(drv_ssd1306->device_address, DRV_SSD1306_REG_COMMAND, &drv_ssd1306_i2c_command, 1)
                    == true,
                  DRV_SSD1306_ERROR);
                goto start;
            }
        }
        else
        {
            drv_ssd1306_update_status = 0;
            return DRV_SSD1306_OK;
        }
    }
    return DRV_SSD1306_OK;
}
#endif

drv_ssd1306_err_t drv_ssd1306_init(drv_ssd1306_config_t *drv_ssd1306)
{
    /* Init I2C */
    DRV_SSD1306_CHECK_PARA(drv_ssd1306 != NULL && drv_ssd1306->i2c_is_device_ready != NULL && drv_ssd1306->i2c_read_at != NULL
                           && drv_ssd1306->i2c_write_at != NULL)

    DRV_SSD1306_CHECK_ERROR((drv_ssd1306_is_ready(drv_ssd1306) == DRV_SSD1306_OK), DRV_SSD1306_ERROR);

    /* Init LCD */
    drv_ssd1306_write_command(drv_ssd1306, 0xAE); /* display off */
    drv_ssd1306_write_command(drv_ssd1306, 0x20); /* Set Memory Addressing Mode */
    drv_ssd1306_write_command(drv_ssd1306, 0x10); /* 00,Horizontal Addressing Mode;
                                                  01,Vertical Addressing Mode;
                                                  10,Page Addressing Mode (RESET);
                                                  11,Invalid */
    drv_ssd1306_write_command(drv_ssd1306, 0xB0); /* Set Page Start Address for Page Addressing Mode,0-7 */
    drv_ssd1306_write_command(drv_ssd1306, 0xC8); /* Set COM Output Scan Direction */
    drv_ssd1306_write_command(drv_ssd1306, 0x00); /*---set low column address */
    drv_ssd1306_write_command(drv_ssd1306, 0x10); /*---set high column address */
    drv_ssd1306_write_command(drv_ssd1306, 0x40); /*--set start line address */
    drv_ssd1306_write_command(drv_ssd1306, 0x81); /*--set contrast control register */
    drv_ssd1306_write_command(drv_ssd1306, 0xFF);
    drv_ssd1306_write_command(drv_ssd1306, 0xA1); /*--set segment re-map 0 to 127*/
    drv_ssd1306_write_command(drv_ssd1306, 0xA6); /*--set normal display*/
    drv_ssd1306_write_command(drv_ssd1306, 0xA8); /*--set multiplex ratio(1 to 64)*/
    drv_ssd1306_write_command(drv_ssd1306, 0x3F); /**/
    drv_ssd1306_write_command(drv_ssd1306, 0xA4); /* 0xa4,Output follows RAM content;0xa5,Output ignores RAM content*/
    drv_ssd1306_write_command(drv_ssd1306, 0xD3); /*-set display offset*/
    drv_ssd1306_write_command(drv_ssd1306, 0x00); /*-not offset*/
    drv_ssd1306_write_command(drv_ssd1306, 0xD5); /*--set display clock divide ratio/oscillator frequency*/
    drv_ssd1306_write_command(drv_ssd1306, 0xF0); /*--set divide ratio*/
    drv_ssd1306_write_command(drv_ssd1306, 0xD9); /*--set pre-charge period*/
    drv_ssd1306_write_command(drv_ssd1306, 0x22); /**/
    drv_ssd1306_write_command(drv_ssd1306, 0xDA); /*--set com pins hardware configuration*/
    drv_ssd1306_write_command(drv_ssd1306, 0x12);
    drv_ssd1306_write_command(drv_ssd1306, 0xDB); /*--set vcomh */
    drv_ssd1306_write_command(drv_ssd1306, 0x20); /* 0x20,0.77xVcc */
    drv_ssd1306_write_command(drv_ssd1306, 0x8D); /*--set DC-DC enable */
    drv_ssd1306_write_command(drv_ssd1306, 0x14); /* */
    drv_ssd1306_write_command(drv_ssd1306, 0xAF); /*--turn on drv_ssd1306 panel */

    drv_ssd1306_write_command(drv_ssd1306, DRV_SSD1306_DEACTIVATE_SCROLL);

    /* Clear screen */
    drv_ssd1306_fill(DRV_SSD1306_COLOR_BLACK);

    /* Update screen */
    drv_ssd1306_update_screen(drv_ssd1306);

    /* Set default values */
    drv_ssd1306_pos.current_x = 0;
    drv_ssd1306_pos.current_y = 0;

    /* Initialized OK */
    drv_ssd1306_pos.initialized = 1;

    /* Return OK */
    return DRV_SSD1306_OK;
}

drv_ssd1306_err_t drv_ssd1306_invert_display(drv_ssd1306_config_t *drv_ssd1306, uint8_t invert)
{
    drv_ssd1306_pos.inverted = invert;
    return DRV_SSD1306_OK;
}

drv_ssd1306_color_t
drv_ssd1306_draw_bitmap(int16_t x, int16_t y, const unsigned char *bitmap, int16_t w, int16_t h, uint16_t color)
{
    int16_t byteWidth = (w + 7) / 8;  // Bitmap scanline pad = whole byte
    uint8_t byte      = 0;

    for (int16_t j = 0; j < h; j++, y++)
    {
        for (int16_t i = 0; i < w; i++)
        {
            if (i & 7)
            {
                byte <<= 1;
            }
            else
            {
                byte = (*(const unsigned char *) (&bitmap[j * byteWidth + i / 8]));
            }
            if (byte & 0x80)
                drv_ssd1306_draw_pixel(x + i, y, color);
        }
    }
    return DRV_SSD1306_OK;
}

drv_ssd1306_err_t drv_ssd1306_draw_pixel(uint16_t x, uint16_t y, drv_ssd1306_color_t color)
{
    if (x >= DRV_SSD1306_WIDTH || y >= DRV_SSD1306_HEIGHT)
    {
        /* Error */
        return DRV_SSD1306_ERROR_PARAMETER;
    }
    /* Check if pixels are inverted */
    if (drv_ssd1306_pos.inverted)
    {
        color = (drv_ssd1306_color_t) !color;
    }
    /* Set color */
    if (color == DRV_SSD1306_COLOR_WHITE)
    {
        drv_ssd1306_buffer[x + (y / 8) * DRV_SSD1306_WIDTH] |= 1 << (y % 8);
    }
    else
    {
        drv_ssd1306_buffer[x + (y / 8) * DRV_SSD1306_WIDTH] &= ~(1 << (y % 8));
    }
    return DRV_SSD1306_OK;
}

drv_ssd1306_err_t drv_ssd1306_goto_xy(uint16_t x, uint16_t y)
{
    /* Set write pointers */
    if (x > DRV_SSD1306_WIDTH || y > DRV_SSD1306_HEIGHT)
        return DRV_SSD1306_ERROR_PARAMETER;
    drv_ssd1306_pos.current_x = x;
    drv_ssd1306_pos.current_y = y;
    return DRV_SSD1306_OK;
}

drv_ssd1306_err_t drv_ssd1306_send_font_custom(int x, int y, unsigned char *char_custom, int w, int h)
{
    int bytes     = 0;
    int indexbyte = 0;
    int step      = 0;

    for (int j = 0; j < w; j++)
    {
        step = 0;
        for (int i = 0; i < h; i++)
        {
            if (step & 7)
            {
                bytes >>= 1;
            }
            else
            {
                bytes = char_custom[indexbyte];
                indexbyte++;
            }
            step++;
            if (bytes & 0x01)
            {
                drv_ssd1306_draw_pixel(x, y + i, 1);
            }
        }
        x++;
    }
    return DRV_SSD1306_OK;
}

drv_ssd1306_err_t drv_ssd1306_putc(uint8_t __char, font_def_t *font, drv_ssd1306_color_t color)
{
    uint32_t i, b, j;
    /* Check available space in LCD */
    if (DRV_SSD1306_WIDTH <= (drv_ssd1306_pos.current_x + font->font_width)
        || DRV_SSD1306_HEIGHT <= (drv_ssd1306_pos.current_y + font->font_height))
    {
        /* Error */
        return DRV_SSD1306_ERROR_PARAMETER;
    }
    if ((font->font_height == 25 && font->font_width == 15) || (font->font_height == 19 && font->font_width == 10))
    {
        uint16_t padding = 0;
        if ((font->font_height == 19 && font->font_width == 10))
        {
            padding = 15;
        }
        else if (font->font_height == 25 && font->font_width == 15)
        {
            padding = 30;
        }
        drv_ssd1306_send_font_custom(drv_ssd1306_pos.current_x, drv_ssd1306_pos.current_y,
                                     (uint8_t *) &font->data[(__char - 32) * padding], font->font_width, font->font_height);
        /* Increase pointer */
        drv_ssd1306_pos.current_x += font->font_width + 3;
        goto end;
    }
    /* Go through font */
    for (i = 0; i < font->font_height; i++)
    {
        b = font->data[(__char - 32) * font->font_height + i];
        for (j = 0; j < font->font_width; j++)
        {
            if ((b << j) & 0x8000)
            {
                drv_ssd1306_draw_pixel(drv_ssd1306_pos.current_x + j, (drv_ssd1306_pos.current_y + i),
                                       (drv_ssd1306_color_t) color);
            }
            else
            {
                drv_ssd1306_draw_pixel(drv_ssd1306_pos.current_x + j, (drv_ssd1306_pos.current_y + i),
                                       (drv_ssd1306_color_t) !color);
            }
        }
    }
    /* Increase pointer */
    drv_ssd1306_pos.current_x += font->font_width;
end:;
    return DRV_SSD1306_OK;
}

drv_ssd1306_err_t drv_ssd1306_send(uint8_t __char, font_def_custom_t *font, drv_ssd1306_color_t color)
{
    uint32_t i, b, j;
    /* Check available space in LCD */
    if (DRV_SSD1306_WIDTH <= (drv_ssd1306_pos.current_x + font->font_width)
        || DRV_SSD1306_HEIGHT <= (drv_ssd1306_pos.current_y + font->font_height))
    {
        /* Error */
        return DRV_SSD1306_ERROR_PARAMETER;
    }
    /* Go through font */
    for (i = 0; i < font->font_height; i++)
    {
        b = font->data[(__char - 32) * font->font_height + i];
        b = b << (16 - font->font_width);
        for (j = 0; j < font->font_width; j++)
        {
            if ((b << j) & 0x8000)
            {
                drv_ssd1306_draw_pixel(drv_ssd1306_pos.current_x + j, (drv_ssd1306_pos.current_y + i),
                                       (drv_ssd1306_color_t) color);
            }
            else
            {
                drv_ssd1306_draw_pixel(drv_ssd1306_pos.current_x + j, (drv_ssd1306_pos.current_y + i),
                                       (drv_ssd1306_color_t) !color);
            }
        }
    }
    /* Increase pointer */
    drv_ssd1306_pos.current_x += font->font_width;
    /* Return character written */
    return DRV_SSD1306_OK;
}

drv_ssd1306_err_t drv_ssd1306_puts(uint8_t *str, font_def_t *font, drv_ssd1306_color_t color)
{
    /* Write characters */
    while (*str)
    {
        /* Write character by character */
        if (drv_ssd1306_putc(*str, font, color) != DRV_SSD1306_OK)
        {
            /* Return error */
            return DRV_SSD1306_ERROR;
        }
        /* Increase string pointer */
        str++;
    }
    /* Everything OK, zero should be returned */
    return DRV_SSD1306_OK;
}

static void drv_ssd1306_raw_circle(int16_t x0, int16_t y0, int16_t r, drv_ssd1306_color_t color)
{
    int16_t f     = 1 - r;
    int16_t ddf_x = 1;
    int16_t ddf_y = -2 * r;
    int16_t x     = 0;
    int16_t y     = r;

    drv_ssd1306_draw_pixel(x0, y0 + r, color);
    drv_ssd1306_draw_pixel(x0, y0 - r, color);
    drv_ssd1306_draw_pixel(x0 + r, y0, color);
    drv_ssd1306_draw_pixel(x0 - r, y0, color);

    while (x < y)
    {
        if (f >= 0)
        {
            y--;
            ddf_y += 2;
            f += ddf_y;
        }
        x++;
        ddf_x += 2;
        f += ddf_x;

        drv_ssd1306_draw_pixel(x0 + x, y0 + y, color);
        drv_ssd1306_draw_pixel(x0 - x, y0 + y, color);
        drv_ssd1306_draw_pixel(x0 + x, y0 - y, color);
        drv_ssd1306_draw_pixel(x0 - x, y0 - y, color);

        drv_ssd1306_draw_pixel(x0 + y, y0 + x, color);
        drv_ssd1306_draw_pixel(x0 - y, y0 + x, color);
        drv_ssd1306_draw_pixel(x0 + y, y0 - x, color);
        drv_ssd1306_draw_pixel(x0 - y, y0 - x, color);
    }
}

void drv_ssd1306_raw_fill_circle(int16_t x0, int16_t y0, int16_t r, drv_ssd1306_color_t color)
{
    int16_t f     = 1 - r;
    int16_t ddF_x = 1;
    int16_t ddF_y = -2 * r;
    int16_t x     = 0;
    int16_t y     = r;

    drv_ssd1306_draw_pixel(x0, y0 + r, color);
    drv_ssd1306_draw_pixel(x0, y0 - r, color);
    drv_ssd1306_draw_pixel(x0 + r, y0, color);
    drv_ssd1306_draw_pixel(x0 - r, y0, color);
    drv_ssd1306_draw_line(x0 - r, y0, x0 + r, y0, color);

    while (x < y)
    {
        if (f >= 0)
        {
            y--;
            ddF_y += 2;
            f += ddF_y;
        }
        x++;
        ddF_x += 2;
        f += ddF_x;

        drv_ssd1306_draw_line(x0 - x, y0 + y, x0 + x, y0 + y, color);
        drv_ssd1306_draw_line(x0 + x, y0 - y, x0 - x, y0 - y, color);

        drv_ssd1306_draw_line(x0 + y, y0 + x, x0 - y, y0 + x, color);
        drv_ssd1306_draw_line(x0 + y, y0 - x, x0 - y, y0 - x, color);
    }
}

void drv_ssd1306_draw_line(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, drv_ssd1306_color_t color)
{
    int16_t dx, dy, sx, sy, err, e2, i, tmp;

    /* Check for overflow */
    if (x0 >= DRV_SSD1306_WIDTH)
    {
        x0 = DRV_SSD1306_WIDTH - 1;
    }
    if (x1 >= DRV_SSD1306_WIDTH)
    {
        x1 = DRV_SSD1306_WIDTH - 1;
    }
    if (y0 >= DRV_SSD1306_HEIGHT)
    {
        y0 = DRV_SSD1306_HEIGHT - 1;
    }
    if (y1 >= DRV_SSD1306_HEIGHT)
    {
        y1 = DRV_SSD1306_HEIGHT - 1;
    }

    dx  = (x0 < x1) ? (x1 - x0) : (x0 - x1);
    dy  = (y0 < y1) ? (y1 - y0) : (y0 - y1);
    sx  = (x0 < x1) ? 1 : -1;
    sy  = (y0 < y1) ? 1 : -1;
    err = ((dx > dy) ? dx : -dy) / 2;

    if (dx == 0)
    {
        if (y1 < y0)
        {
            tmp = y1;
            y1  = y0;
            y0  = tmp;
        }

        if (x1 < x0)
        {
            tmp = x1;
            x1  = x0;
            x0  = tmp;
        }

        /* Vertical line */
        for (i = y0; i <= y1; i++)
        {
            drv_ssd1306_draw_pixel(x0, i, color);
        }

        /* Return from function */
        return;
    }

    if (dy == 0)
    {
        if (y1 < y0)
        {
            tmp = y1;
            y1  = y0;
            y0  = tmp;
        }

        if (x1 < x0)
        {
            tmp = x1;
            x1  = x0;
            x0  = tmp;
        }

        /* Horizontal line */
        for (i = x0; i <= x1; i++)
        {
            drv_ssd1306_draw_pixel(i, y0, color);
        }

        /* Return from function */
        return;
    }

    while (1)
    {
        drv_ssd1306_draw_pixel(x0, y0, color);
        if (x0 == x1 && y0 == y1)
        {
            break;
        }
        e2 = err;
        if (e2 > -dx)
        {
            err -= dy;
            x0 += sx;
        }
        if (e2 < dy)
        {
            err += dx;
            y0 += sy;
        }
    }
}

void drv_ssd1306_draw_rectangle(uint16_t x, uint16_t y, uint16_t w, uint16_t h, drv_ssd1306_color_t color)
{
    /* Check input parameters */
    if (x >= DRV_SSD1306_WIDTH || y >= DRV_SSD1306_HEIGHT)
    {
        /* Return error */
        return;
    }
    /* Check width and height */
    if ((x + w) >= DRV_SSD1306_WIDTH)
    {
        w = DRV_SSD1306_WIDTH - x;
    }
    if ((y + h) >= DRV_SSD1306_HEIGHT)
    {
        h = DRV_SSD1306_HEIGHT - y;
    }
    /* Draw 4 lines */
    drv_ssd1306_draw_line(x, y, x + w, y, color);         /* Top line */
    drv_ssd1306_draw_line(x, y + h, x + w, y + h, color); /* Bottom line */
    drv_ssd1306_draw_line(x, y, x, y + h, color);         /* Left line */
    drv_ssd1306_draw_line(x + w, y, x + w, y + h, color); /* Right line */
}

void drv_ssd1306_draw_triangle(uint16_t            x1,
                               uint16_t            y1,
                               uint16_t            x2,
                               uint16_t            y2,
                               uint16_t            x3,
                               uint16_t            y3,
                               drv_ssd1306_color_t color)
{
    /* Draw lines */
    drv_ssd1306_draw_line(x1, y1, x2, y2, color);
    drv_ssd1306_draw_line(x2, y2, x3, y3, color);
    drv_ssd1306_draw_line(x3, y3, x1, y1, color);
}

void drv_ssd1306_clear(drv_ssd1306_config_t *drv_ssd1306)
{
    drv_ssd1306_fill(0);
    drv_ssd1306_update_screen(drv_ssd1306);
}
void drv_ssd1306_on(drv_ssd1306_config_t *drv_ssd1306)
{
    drv_ssd1306_write_command(drv_ssd1306, 0x8D);
    drv_ssd1306_write_command(drv_ssd1306, 0x14);
    drv_ssd1306_write_command(drv_ssd1306, 0xAF);
}
void drv_ssd1306_off(drv_ssd1306_config_t *drv_ssd1306)
{
    drv_ssd1306_write_command(drv_ssd1306, 0x8D);
    drv_ssd1306_write_command(drv_ssd1306, 0x10);
    drv_ssd1306_write_command(drv_ssd1306, 0xAE);
}

// draw thin hand = second hand
void drv_ssd1306_draw_hand_thin(float hand_angle, float hand_lenght_long, float hand_legth_short)
{
    float xpos;
    float ypos;
    float xpos2;
    float ypos2;

    // calculate starting and ending position of the second hand
    xpos  = round(CENTER_X + sin(degToRad(hand_angle)) * hand_lenght_long);        // calculate x pos based on angle and radius
    ypos  = round(CENTER_Y - cos(degToRad(hand_angle)) * hand_lenght_long);        // calculate y pos based on angle and radius
    xpos2 = round(CENTER_X + sin(degToRad(hand_angle + 180)) * hand_legth_short);  // calculate x pos based on angle and radius
    ypos2 = round(CENTER_Y - cos(degToRad(hand_angle + 180)) * hand_legth_short);  // calculate y pos based on angle and radius

    drv_ssd1306_draw_line(xpos, ypos, xpos2, ypos2, 1);  // draw the main line
    drv_ssd1306_raw_fill_circle(xpos2, ypos2, 3, 0);
    drv_ssd1306_raw_circle(xpos2, ypos2, 3, 1);  // draw small outline white circle
}

void update_time_auto(volatile uint8_t *second, volatile uint8_t *minute, volatile uint8_t *hour)
{
    // increment the time
    *second++;
    if (*second >= 60)
    {
        *second = 0;
        *minute++;
        if (*minute >= 60)
        {
            *minute = 0;
            *hour++;
            if (*hour >= 12)
            {
                *hour = 0;
            }
        }
    }
}

drv_ssd1306_err_t drv_ds1306_clock_style_simple(drv_ssd1306_config_t *drv_ssd1306, rtc_t rtc)
{
    float xpos;
    float ypos;
    float xpos2;
    float ypos2;
    char  string[10];

    drv_ssd1306_fill_theme_color();                     // set the drawing color to white
    drv_ssd1306_raw_circle(CENTER_X, CENTER_Y, 31, 1);  // draw fullscreen circle

    // draw 60 dots (pixels) around the circle, one for every minute/second
    for (int i = 0; i < 60; i++)
    {                                                        // draw 60 pixels around the circle
        xpos = round(CENTER_X + sin(degToRad(i * 6)) * 29);  // calculate x pos based on angle and radius
        ypos = round(CENTER_Y - cos(degToRad(i * 6)) * 29);  // calculate y pos based on angle and radius
        drv_ssd1306_draw_pixel(xpos, ypos, 1);               // draw white pixel on position xpos and ypos
    }

    // drawing big tickmarks
    for (int i = 0; i < 12; i++)
    {
        if ((i % 3) != 0)
        {  // only draw tickmarks for some numbers, leave empty space for 12, 3, 6, and 9
            xpos  = round(CENTER_X + sin(degToRad(i * 30)) * 25);  // calculate x pos based on angle and radius
            ypos  = round(CENTER_Y - cos(degToRad(i * 30)) * 25);  // calculate y pos based on angle and radius
            xpos2 = round(CENTER_X + sin(degToRad(i * 30)) * 20);  // calculate x pos based on angle and radius
            ypos2 = round(CENTER_Y - cos(degToRad(i * 30)) * 20);  // calculate y pos based on angle and radius
            drv_ssd1306_draw_line(xpos, ypos, xpos2, ypos2, 1);    // draw a line for a tickmark
        }
    }

    // draw labels 12, 3, 6 and 9, positions are hardcoded
    drv_ssd1306_goto_xy(58 - CENTER_X, 5);
    drv_ssd1306_puts((uint8_t *) "12", &font_7x10, 1);
    drv_ssd1306_goto_xy(86 - CENTER_X, 28);
    drv_ssd1306_puts((uint8_t *) "3", &font_7x10, 1);
    drv_ssd1306_goto_xy(61 - CENTER_X, 51);
    drv_ssd1306_puts((uint8_t *) "6", &font_7x10, 1);
    drv_ssd1306_goto_xy(36 - CENTER_X, 28);
    drv_ssd1306_puts((uint8_t *) "9", &font_7x10, 1);

    drv_ssd1306_draw_hand_thin(rtc.second * 6, 27, 10);
    drv_ssd1306_draw_hand_thin(rtc.minute * 6, 17, 0);
    drv_ssd1306_draw_hand_thin(rtc.hour * 30 + (rtc.minute * 1.0 / 2), 14, 0);

    drv_ssd1306_goto_xy(65, 0);
    sprintf(string, "%s", weekday[rtc.day_of_week - 1]);
    drv_ssd1306_puts((uint8_t *) string, &font_11x18, 1);

    drv_ssd1306_goto_xy(70, 20);
    sprintf(string, "%02d", rtc.day_of_month);
    drv_ssd1306_puts((uint8_t *) string, &font_11x18, 1);

    drv_ssd1306_goto_xy(92, 20);
    sprintf(string, "%s", months[rtc.month - 1]);
    drv_ssd1306_puts((uint8_t *) string, &font_11x18, 1);

    drv_ssd1306_goto_xy(70, 40);
    sprintf(string, "20%02d", rtc.year);
    drv_ssd1306_puts((uint8_t *) string, &font_11x18, 1);

    drv_ssd1306_draw_bitmap(97, 0, icon_alarm, 16, 16, 1);
    drv_ssd1306_goto_xy(113, 5);
    uint8_t value = 0;
    system_time_number_reminder(&value);
    char number_reminder[2];
    sprintf(number_reminder, "%d", value);
    drv_ssd1306_puts((uint8_t *) number_reminder, &font_7x10, 1);

    drv_ssd1306_update_screen(drv_ssd1306);
    return DRV_SSD1306_OK;
}

drv_ssd1306_err_t drv_ds1306_clock_style_digital(drv_ssd1306_config_t *drv_ssd1306, rtc_t rtc, int8_t temp)
{
    char    string[20];
    uint8_t value = 0;

    drv_ssd1306_fill_theme_color();
    drv_ssd1306_goto_xy(20, 20);
    sprintf(string, "%02d:%02d:%02d", rtc.hour, rtc.minute, rtc.second);
    drv_ssd1306_puts((uint8_t *) string, &font_11x18, 1);
    sprintf(string, (char *) "%s %d %s 20%02d", weekday[rtc.day_of_week - 1], rtc.day_of_month, months[rtc.month - 1], rtc.year);
    drv_ssd1306_goto_xy(10, 53);
    drv_ssd1306_puts((uint8_t *) string, &font_7x10, 1);

    drv_ssd1306_goto_xy(2, 2);
    sprintf(string, "%d`C", temp);
    drv_ssd1306_puts((uint8_t *) string, &font_7x10, 1);

    drv_ssd1306_draw_bitmap(97, 0, icon_alarm, 16, 16, 1);
    drv_ssd1306_goto_xy(113, 5);
    system_time_number_reminder(&value);
    sprintf(string, "%d", value);
    drv_ssd1306_puts((uint8_t *) string, &font_7x10, 1);

    drv_ssd1306_update_screen(drv_ssd1306);
    return DRV_SSD1306_OK;
}

/* End of file -------------------------------------------------------- */
