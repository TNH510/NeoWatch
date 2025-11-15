/**
 * @file       bsp_button.c
 * @copyright  Copyright (C) 2025 NEO. All rights reserved.
 * @license    This project is released under the NEO License.
 * @version    1.0.0
 * @date       2025-11
 * @author     Thach Vu
 * @brief      BSP for button
 * @note       None
 */
/* Includes ----------------------------------------------------------- */
#include "bsp_button.h"

/* Private defines ---------------------------------------------------- */
/* Private enumerate/structure ---------------------------------------- */
/* Private macros ----------------------------------------------------- */
/* Public variables --------------------------------------------------- */
/* Private variables -------------------------------------------------- */
/* Private function prototypes ---------------------------------------- */
static bsp_button_callback_t button_callback = NULL;

static button_t action_button = {
    .cfg.gpio_num = 7,
    .state        = IDLE,
    .timeout      = 0,
};

/* Function definitions ----------------------------------------------- */
void bsp_button_init(bsp_button_callback_t callback)
{
    // Set log level
    esp_log_level_set("BSP_BUTTON", ESP_LOG_NONE);
    
    // Init hardware
    gpio_config_t io_conf = { .pin_bit_mask = (1ULL << action_button.cfg.gpio_num),
                              .mode         = GPIO_MODE_INPUT,
                              .pull_up_en   = GPIO_PULLUP_ENABLE,
                              .pull_down_en = GPIO_PULLDOWN_DISABLE,
                              .intr_type    = GPIO_INTR_DISABLE };
    gpio_config(&io_conf);

    // Store callback
    button_callback = callback;
}

void bsp_button_task(void)
{
    // Check button state
    switch (action_button.state)
    {
    case IDLE:
    {
        if (gpio_get_level(action_button.cfg.gpio_num) == 0)
        {
            action_button.state   = WAIT_PRESS_TIMEOUT;
            action_button.timeout = xTaskGetTickCount() + 10;
            ESP_LOGI("BSP_BUTTON", "IDLE -> WAIT_PRESS_TIMEOUT");
        }
        break;
    }
    case WAIT_PRESS_TIMEOUT:
    {
        if (gpio_get_level(action_button.cfg.gpio_num) == 1 && (xTaskGetTickCount() <= action_button.timeout))
        {
            action_button.state = IDLE;
            ESP_LOGI("BSP_BUTTON", "WAIT_PRESS_TIMEOUT -> IDLE");
        }
        else if (gpio_get_level(action_button.cfg.gpio_num) == 0 && (xTaskGetTickCount() > action_button.timeout))
        {
            action_button.state = WAIT_CLICK_TIMEOUT;
            action_button.timeout = xTaskGetTickCount() + 180;
            ESP_LOGI("BSP_BUTTON", "WAIT_PRESS_TIMEOUT -> WAIT_CLICK_TIMEOUT");
        }
        break;
    }
    case WAIT_CLICK_TIMEOUT:
    {
        if (gpio_get_level(action_button.cfg.gpio_num) == 1 && (xTaskGetTickCount() <= action_button.timeout))
        {
            action_button.state = IDLE;

            // Click event detected
            if (button_callback != NULL)
            {
                button_callback();
            }
            
            ESP_LOGI("BSP_BUTTON", "------------------> Click event detected");
            ESP_LOGI("BSP_BUTTON", "WAIT_CLICK_TIMEOUT -> IDLE");
        }
        else if (gpio_get_level(action_button.cfg.gpio_num) == 1 && (xTaskGetTickCount() > action_button.timeout))
        {
            action_button.state = IDLE;
            ESP_LOGI("BSP_BUTTON", "WAIT_CLICK_TIMEOUT -> IDLE");
        }
        break;
    }
    }
}

/* End of file -------------------------------------------------------- */
