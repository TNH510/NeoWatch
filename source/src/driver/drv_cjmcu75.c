/**
 * @file       driver_cjmcu75.h
 * @copyright  Copyright (C) 2019 Fiot Co., Ltd. All rights reserved.
 * @license    This project is released under the Fiot License.
 * @version    1.0.0
 * @date       2023-08-28
 * @author     Hieu Tran
 *
 * @brief      cjmcu75 driver
 */

/* Includes ----------------------------------------------------------- */
#include "drv_cjmcu75.h"

/* Private defines ---------------------------------------------------- */

/* Private enumerate/structure ---------------------------------------- */

/* Private macros ----------------------------------------------------- */

/* Public variables --------------------------------------------------- */

/* Private variables -------------------------------------------------- */

/* Private function prototypes ---------------------------------------- */

/* Function definitions ----------------------------------------------- */
void cjmcu75_init(cjmcu75_t *cjmcu75)
{
    cjmcu75->device_address      = CJMCU75_ADDRESS_8BIT;
    cjmcu75->i2c_is_device_ready = bsp_i2c1_is_device_ready;
    cjmcu75->i2c_read_at         = bsp_i2c1_read_mem;
    cjmcu75->i2c_write_at        = bsp_i2c1_write_mem;
}

bool cjmcu75_check_ready(cjmcu75_t *cjmcu75)
{
    return cjmcu75->i2c_is_device_ready(CJMCU75_ADDRESS_8BIT);
}

int8_t cjmcu75_get_temp(cjmcu75_t *cjmcu75)
{
    uint8_t temp_reg[2];
    float   temp;

    cjmcu75->i2c_read_at(cjmcu75->device_address, CJMCU75_TEMP_REG, temp_reg, 2);

    uint16_t temp_reg_full = ((temp_reg[0] << 8) & 0xFF00) | (temp_reg[1] & 0x00FF);

    // Check if the MSB bit is 1 (negative value)
    if (temp_reg_full & 0x8000)
    {
        temp_reg_full |= 0xF000;  // Sign extension
        temp = (float) (temp_reg_full >> 8) - ((temp_reg_full >> 7) & 0x0001) * 0.5;
    }
    else
    {
        temp = (float) (temp_reg_full >> 8) + ((temp_reg_full >> 7) & 0x0001) * 0.5;
    }

    return (int8_t) temp;  // Return integer value
}
/* End of file -------------------------------------------------------- */
