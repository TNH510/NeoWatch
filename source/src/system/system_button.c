/**
 * @file       system_button.c
 * @copyright  Copyright (C) 2025 NEO. All rights reserved.
 * @license    This project is released under the NEO License.
 * @version    1.0.0
 * @date       2025-10
 * @author     Thach Vu 
 * @brief      System Button Module
 * @note       None
 */
/* Includes ----------------------------------------------------------- */
#include "system_button.h"
#include "base_type.h"
#include "bsp_button.h"

/* Private defines ---------------------------------------------------- */
/* Private enumerate/structure ---------------------------------------- */
/* Private macros ----------------------------------------------------- */
/* Public variables --------------------------------------------------- */
/* Private variables -------------------------------------------------- */
/* Private function prototypes ---------------------------------------- */
static void system_button_task(void *pvParameters);

/* Function definitions ----------------------------------------------- */
void system_button_init(void)
{
    // Init BSP for system button
    bsp_button_init(NULL);

    // Create a task for system button handling
    xTaskCreate(system_button_task, "system_button_task", 2048, NULL, 10, NULL);
}

static void system_button_task(void *pvParameters)
{
    // Log hello world every second
    while (1)
    {
        /* End of task for system button */
        bsp_button_task();
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}
/* End of file -------------------------------------------------------- */
