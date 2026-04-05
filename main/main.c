/**
 * @file       main.c
 * @copyright  Copyright (C) 2025 NEO. All rights reserved.
 * @license    This project is released under the NEO License.
 * @version    1.0.0
 * @date       2025-10
 * @author     Hieu Tran Ngoc
 * @brief      Main file
 * @note       None
 */
/* Includes ----------------------------------------------------------- */
#include "bsp.h"
#include "system_manager.h"
#include "system_ui.h"

/* Private defines ---------------------------------------------------- */
/* Private enumerate/structure ---------------------------------------- */
/* Private macros ----------------------------------------------------- */
/* Public variables --------------------------------------------------- */
/* Private variables -------------------------------------------------- */
/* Private function prototypes ---------------------------------------- */
/* Function definitions ----------------------------------------------- */
void app_main(void)
{
    // Initialize BSP and libraries
    bsp_init();

    // TODO: restore after LED test
    // system_manager_init();

    // Standalone LED blink test (blocks forever)
    system_ui_test();
}
/* End of file -------------------------------------------------------- */
