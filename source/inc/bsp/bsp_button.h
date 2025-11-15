/**
 * @file       bsp_button.h
 * @copyright  Copyright (C) 2025 NEO. All rights reserved.
 * @license    This project is released under the NEO License.
 * @version    1.0.0
 * @date       2025-11
 * @author     Thach Vu
 * @brief      BSP for button
 * @note       None
 */

/* Define to prevent recursive inclusion ------------------------------ */
#ifndef __BSP_BUTTON_H
#define __BSP_BUTTON_H

/* Includes ----------------------------------------------------------- */
#include "basic_types.h"

/* Public defines ----------------------------------------------------- */
/* Public enumerate/structure ----------------------------------------- */
typedef enum
{
    CLICK_EVENT,
    INVALID_EVENT
} button_event_t;

typedef enum
{
    IDLE,
    WAIT_PRESS_TIMEOUT,
    WAIT_CLICK_TIMEOUT,
} button_state_t;

typedef struct
{
    gpio_num_t gpio_num;
} button_cfg_t;

typedef struct
{
    // Configuration
    const button_cfg_t cfg;

    // Variables
    button_state_t state;
    uint32_t       timeout;
} button_t;

/* Public macros ------------------------------------------------------ */
/* Public variables --------------------------------------------------- */
/* Public function prototypes ----------------------------------------- */
void bsp_button_init(void);

/* -------------------------------------------------------------------- */
#endif /* __BSP_BUTTON_H */

/* End of file -------------------------------------------------------- */
