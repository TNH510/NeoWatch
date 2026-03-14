/**
 * @file       sys_manager.c
 * @copyright  Copyright (C) 2025 NEO. All rights reserved.
 * @license    This project is released under the NEO License.
 * @version    1.0.0
 * @date       2025-10
 * @author     Hieu Tran Ngoc
 * @brief      System Manager
 * @note       None
 */

/* Includes ----------------------------------------------------------- */
#include "system_manager.h"

#include "system_display.h"

/* Private defines ---------------------------------------------------- */
/* Private enumerate/structure ---------------------------------------- */
typedef enum
{
    SM_MODE_BOOT,
    SM_MODE_STANDBY,
    SM_MODE_MENU,
    SM_MODE_MAX
} sm_mode_t;

typedef enum
{
    SM_APP_SETTINGS,
    SM_APP_MAX,
} sm_app_t;

/* Private enumerate/structure ---------------------------------------- */

/* Private macros ----------------------------------------------------- */

/* Public variables --------------------------------------------------- */

/* Private variables -------------------------------------------------- */
sm_mode_t system_state  = SM_MODE_BOOT;
sm_app_t  system_id_app = SM_APP_SETTINGS;

/* Private function prototypes ---------------------------------------- */
static void system_manager_task(void *pvParameters);

/* Function definitions ----------------------------------------------- */
void system_manager_init(void)
{
    // Init system display
    system_display_init();

    system_display_clock();

    // Create system manager main thread
    xTaskCreate(system_manager_task, "system_manager", 4096, NULL, 10, NULL);
}

static void system_manager_task(void *pvParameters)
{
    // Log hello world every second
    while (1)
    {
        // ESP_LOGI("SYSTEM_MANAGER", "Hello World from System Manager!");
        // vTaskDelay(pdMS_TO_TICKS(1000));
        system_display_task();
    }
}
/* End of file -------------------------------------------------------- */
