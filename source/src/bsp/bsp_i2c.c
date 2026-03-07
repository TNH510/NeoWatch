/**
 * @file       bsp_i2c.c
 * @copyright  Copyright (C) 2019 Fiot Co., Ltd. All rights reserved.
 * @license    This project is released under the QuyLe License.
 * @version    1.1.0
 * @date       2025-08-30
 * @author     Thach Nguyen Ba Vu
 *
 * @brief      Board Support Package - I2C master driver
 *
 * @note       Uses ESP-IDF i2c_master driver (ESP-IDF >= 5.x)
 */

/* Includes ----------------------------------------------------------- */
#include "bsp_i2c.h"

#include "driver/i2c_master.h"

/* Private defines ---------------------------------------------------- */
/* Private enumerate/structure ---------------------------------------- */
/* Private macros ----------------------------------------------------- */

/* Public variables --------------------------------------------------- */
/* Private variables -------------------------------------------------- */
static i2c_master_bus_config_t s_i2c_bus_config = {
    .clk_source                   = I2C_CLK_SRC_DEFAULT,
    .i2c_port                     = BSP_I2C_PORT,
    .scl_io_num                   = BSP_I2C_SCL_PIN,
    .sda_io_num                   = BSP_I2C_SDA_PIN,
    .glitch_ignore_cnt            = 7,
    .flags.enable_internal_pullup = true,
};
static i2c_master_bus_handle_t s_i2c_bus = NULL;

/* Private function prototypes ---------------------------------------- */

/* Function definitions ----------------------------------------------- */

bool bsp_i2c_init(void)
{
    if (s_i2c_bus != NULL)
    {
        return true;
    }
    return (i2c_new_master_bus(&s_i2c_bus_config, &s_i2c_bus) == ESP_OK);
}

void bsp_i2c_deinit(void)
{
    if (s_i2c_bus == NULL)
    {
        return;
    }
    i2c_del_master_bus(s_i2c_bus);
    s_i2c_bus = NULL;
}

i2c_master_bus_handle_t bsp_i2c_get_handle(void)
{
    return s_i2c_bus;
}

bool bsp_i2c_add_device(uint16_t dev_addr, uint32_t scl_speed_hz,
                        i2c_master_dev_handle_t *dev_handle)
{
    if ((s_i2c_bus == NULL) || (dev_handle == NULL))
    {
        return false;
    }

    i2c_device_config_t dev_config = {
        .dev_addr_length = I2C_ADDR_BIT_LEN_7,
        .device_address  = dev_addr,
        .scl_speed_hz    = scl_speed_hz,
    };

    return (i2c_master_bus_add_device(s_i2c_bus, &dev_config, dev_handle) == ESP_OK);
}

bool bsp_i2c_remove_device(i2c_master_dev_handle_t dev_handle)
{
    if (dev_handle == NULL)
    {
        return false;
    }
    return (i2c_master_bus_rm_device(dev_handle) == ESP_OK);
}

bool bsp_i2c_write(i2c_master_dev_handle_t dev_handle, const uint8_t *data, size_t len)
{
    if ((dev_handle == NULL) || (data == NULL))
    {
        return false;
    }
    return (i2c_master_transmit(dev_handle, data, len, BSP_I2C_TIMEOUT_MS) == ESP_OK);
}

bool bsp_i2c_read(i2c_master_dev_handle_t dev_handle, uint8_t *data, size_t len)
{
    if ((dev_handle == NULL) || (data == NULL))
    {
        return false;
    }
    return (i2c_master_receive(dev_handle, data, len, BSP_I2C_TIMEOUT_MS) == ESP_OK);
}

bool bsp_i2c_write_read(i2c_master_dev_handle_t dev_handle,
                        const uint8_t          *write_data, size_t write_len,
                        uint8_t                *read_data,  size_t read_len)
{
    if ((dev_handle == NULL) || (write_data == NULL) || (read_data == NULL))
    {
        return false;
    }
    return (i2c_master_transmit_receive(dev_handle,
                                        write_data, write_len,
                                        read_data, read_len,
                                        BSP_I2C_TIMEOUT_MS) == ESP_OK);
}

/* End of file -------------------------------------------------------- */
