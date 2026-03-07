/**
 * @file       bsp_i2c.h
 * @copyright  Copyright (C) 2019 Fiot Co., Ltd. All rights reserved.
 * @license    This project is released under the QuyLe License.
 * @version    1.0.0
 * @date       2025-08-30
 * @author     Thach Nguyen Ba Vu
 *
 * @brief      handle i2c
 *
 * @note
 */

/* Includes ----------------------------------------------------------- */
#include "bsp_i2c.h"

#include "driver/i2c_master.h"

/* Private defines ---------------------------------------------------- */
#define BSP_I2C_TIMEOUT     100
#define BSP_I2C_SCL_PORT    22
#define BSP_I2C_SDA_PORT    21
#define BSP_I2C_PIN_NUM_RST -1
#define BSP_I2C_HW_ADDR     0x3C  // 0x3C for SSD1315
#define BSP_I2C_BUS_PORT    0
/* Private enumerate/structure ---------------------------------------- */
/* Private macros ----------------------------------------------------- */

/* Public variables --------------------------------------------------- */
/* Private variables -------------------------------------------------- */
i2c_master_bus_config_t i2c_bus_config = {
    .clk_source                   = I2C_CLK_SRC_DEFAULT,
    .i2c_port                     = BSP_I2C_BUS_PORT,
    .scl_io_num                   = BSP_I2C_SCL_PORT,
    .sda_io_num                   = BSP_I2C_SDA_PORT,
    .glitch_ignore_cnt            = 7,
    .trans_queue_depth            = 37,
    .flags.enable_internal_pullup = true,
};
i2c_master_bus_handle_t i2c_bus = NULL;
/* Private function prototypes ---------------------------------------- */

/* Function definitions ----------------------------------------------- */

bool bsp_i2c_init(void)
{
    return (i2c_new_master_bus(&i2c_bus_config, &i2c_bus) == ESP_OK) ? true : false;
}

/* End of file -------------------------------------------------------- */
