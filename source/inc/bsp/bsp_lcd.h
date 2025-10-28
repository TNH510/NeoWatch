/**
 * @file       bsp_lcd.h
 * @copyright  Copyright (C) 2025 NEO. All rights reserved.
 * @license    This project is released under the NEO License.
 * @version    1.0.0
 * @date       2025-09
 * @author     Thach Nguyen Ba Vu
 * @brief      Board support package for LCD
 * @note       None
 */

/* Define to prevent recursive inclusion ------------------------------ */
#ifndef __BSP_LCD_H
#define __BSP_LCD_H

/* Includes ----------------------------------------------------------- */
#include "base_type.h"
/* Public defines ----------------------------------------------------- */
/* Public enumerate/structure ----------------------------------------- */
typedef enum
{
    BSP_LCD_CLOCK_TYPE_LEFT,
    BSP_LCD_CLOCK_TYPE_RIGHT,
    BSP_LCD_CLOCK_TYPE_MAX,
} bsp_lcd_clock_t;
/* Public macros ------------------------------------------------------ */
/* Public variables --------------------------------------------------- */
/* Public function prototypes ----------------------------------------- */
/**
 * @brief
 *
 * @return base_status_t  
 */
base_status_t bsp_lcd_init(void);
/**
 * @brief Function to display clock
 *
 * @param 
 */
void bsp_lcd_clock_display(uint16_t year, uint8_t month, uint16_t day, uint8_t hour, uint8_t min, uint8_t sec);
/**
 * @brief 
 * 
 * @param video_num 
 */
void bsp_lcd_demo_video(uint8_t video_num);
/**
 * @brief 
 * 
 * @param mode 
 * @return base_status_t 
 */
base_status_t bsp_lcd_clock_set_mode(bsp_lcd_clock_t mode);
/* -------------------------------------------------------------------- */
#endif /* __BSP_LCD_H */

/* End of file -------------------------------------------------------- */
