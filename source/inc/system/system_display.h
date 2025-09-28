/**
 * @file       system_display.h
 * @copyright  Copyright (C) 2025 NEO. All rights reserved.
 * @license    This project is released under the NEO License.
 * @version    1.0.0
 * @date       2025-09-28
 * @author     Thach Nguyen Ba Vu
 * @brief      system display
 * @note       None
 */

/* Define to prevent recursive inclusion ------------------------------ */
#ifndef __SYSTEM_DISPLAY_H
#define __SYSTEM_DISPLAY_H

/* Includes ----------------------------------------------------------- */
#include "bsp_i2c.h"

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
/* Public defines ----------------------------------------------------- */

/* Public enumerate/structure ----------------------------------------- */

/* Public macros ------------------------------------------------------ */

/* Public variables --------------------------------------------------- */

/* Public function prototypes ----------------------------------------- */

/**
 * @brief Init system display
 *
 * @return system_display_error_t
 */
void system_display_init(void);

/**
 * @brief Display clock
 *
 * @param[in] mode mode select type clock
 *
 * @return system_display_error_t
 */
void system_display_clock(void);

/**
 * @brief Display update screen
 *
 * @return
 *  None
 */
void system_display_update(void);


#endif  // __SYSTEM_DISPLAY_H

/* End of file -------------------------------------------------------- */
