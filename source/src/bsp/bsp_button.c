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
#define BUTTON_PIN 7

/* Private enumerate/structure ---------------------------------------- */
/* Private macros ----------------------------------------------------- */
/* Public variables --------------------------------------------------- */
/* Private variables -------------------------------------------------- */
/* Private function prototypes ---------------------------------------- */
static bsp_button_callback_t button_callback = NULL;

static gpio_config_t io_conf = {
    .pin_bit_mask = (1ULL << BUTTON_PIN),  
    .mode         = GPIO_MODE_INPUT,  
    .pull_up_en   = GPIO_PULLUP_ENABLE,   
    .pull_down_en = GPIO_PULLDOWN_DISABLE,
    .intr_type    = GPIO_INTR_DISABLE   
};

/* Function definitions ----------------------------------------------- */
void bsp_button_init(bsp_button_callback_t callback)
{
    // Init hardware
    gpio_config(&io_conf);

    // Store callback
    button_callback = callback;
}

void bsp_button_task(void)
{
    // Check button state
    // Print button pin state for demonstration
    ESP_LOGI("BSP_BUTTON", "Pin state: %d", gpio_get_level(BUTTON_PIN));
}

/* End of file -------------------------------------------------------- */
