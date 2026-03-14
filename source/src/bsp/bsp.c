/**
 * @file       bsp.c
 * @copyright  Copyright (C) 2025 NEO. All rights reserved.
 * @license    This project is released under the NEO License.
 * @version    1.0.0
 * @date       2025-10
 * @author     Hieu Tran Ngoc
 * @brief      BSP General
 * @note       None
 */
/* Includes ----------------------------------------------------------- */
#include "bsp.h"
#include "bsp_i2c.h"
#include "bsp_rtc.h"
#include "bsp_lcd.h"
#include "bsp_gpio.h"
#include "bsp_button.h"
#include "base_type.h"

/* Private defines ---------------------------------------------------- */
/* Private enumerate/structure ---------------------------------------- */
/* Private macros ----------------------------------------------------- */
/* Public variables --------------------------------------------------- */
/* Private variables -------------------------------------------------- */
/* Private function prototypes ---------------------------------------- */
/* Function definitions ----------------------------------------------- */
void bsp_init(void)
{
    // Initialize all BSP components
    // CHECK_STATUS(bsp_i2c_init());
    // bsp_i2c_init();
    // bsp_lcd_init();
    bsp_gpio_init();
    bsp_button_init();
    // CHECK_STATUS(bsp_rtc_init());
    // CHECK_STATUS(bsp_lcd_init());
    // else{
    //     ESP_LOGI("BSP", "I2C bus initialized successfully");
    //     /* connect device */
    //     i2c_master_dev_handle_t i2c_dev_handle;
    //     bool i2c_init_ok = bsp_i2c_add_device(0x3C, 400000, &i2c_dev_handle);
    //     uint8_t read_data[2] = {0};
    //     uint8_t write_data[2] = {0x00, 0xA5}; // Example data to write
    //     uint8_t write_data2[2] = {0x00, 0xAF}; // Example data to write
    //         if (!i2c_init_ok) {
    //         ESP_LOGE("BSP", "Failed to add I2C device");
    //     } else {
    //         ESP_LOGI("BSP", "I2C device added successfully");
    //             /* Write address  */
    //         bool i2c_init_ok = bsp_i2c_write(i2c_dev_handle, write_data, 2);
    //         if (!i2c_init_ok) {
    //             ESP_LOGE("BSP", "Failed to write to I2C device");
    //         } else {
    //             ESP_LOGI("BSP", "I2C write successful");
    //             bool i2c_init_ok2 = bsp_i2c_write(i2c_dev_handle, write_data2, 2);
    //             if (!i2c_init_ok2) {
    //                 ESP_LOGE("BSP", "Failed to write to I2C device");
    //             } else {
    //                 ESP_LOGI("BSP", "I2C write successful");
    //             }
    //         }
    //         /* Write Read Device*/
    //         // bsp_i2c_write_read(i2c_dev_handle, write_data, 2, read_data, 2);
    //         // ESP_LOGI("BSP", "I2C write-read successful, data: 0x%02X 0x%02X", read_data[0], read_data[1]);
    //     }
    // }
    // m_i2c_init();
    // bsp_lcd_init();
    // bsp_rtc_init();
}

/* End of file -------------------------------------------------------- */
