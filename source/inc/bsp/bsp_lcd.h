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
/**
 * @brief
 *
 * @return base_status_t  
 */
base_status_t bsp_lcd_init(void);
/**
 * @brief Function to display clock
 *
 * @param epoch_time
 */
void bsp_lcd_clock_display(void);
/* Public enumerate/structure ----------------------------------------- */
/* Public macros ------------------------------------------------------ */
/* Public variables --------------------------------------------------- */
/* Public function prototypes ----------------------------------------- */

/* -------------------------------------------------------------------- */
#endif /* __BSP_LCD_H */

/* End of file -------------------------------------------------------- */
