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
#include "system_manager.h"

/* Private defines ---------------------------------------------------- */
/* Private enumerate/structure ---------------------------------------- */
/* Private macros ----------------------------------------------------- */
/* Public variables --------------------------------------------------- */
/* Private variables -------------------------------------------------- */
/* Private function prototypes ---------------------------------------- */
/* Function definitions ----------------------------------------------- */
void app_main(void)
{
    // Initialize system manager
    system_manager_init();

    while (1)
    {
        // Delay forever
        vTaskDelay(portMAX_DELAY);
    }
}
/* End of file -------------------------------------------------------- */
