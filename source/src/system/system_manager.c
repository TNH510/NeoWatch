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

#include "system_button.h"

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
    // Create system manager main thread
    xTaskCreate(system_manager_task, "system_manager", 4096, NULL, 10, NULL);

    // Initialize system for buttons
    system_button_init();
}

static void system_manager_task(void *pvParameters)
{
    // Log hello world every second
    while (1)
    {
        vTaskDelay(portMAX_DELAY);
    }
}
/* End of file -------------------------------------------------------- */
