/**
 * @file       driver_cjmcu75.h
 * @copyright  Copyright (C) 2019 Fiot Co., Ltd. All rights reserved.
 * @license    This project is released under the Fiot License.
 * @version    1.0.0
 * @date       2023-08-28
 * @author     Hieu Tran
 *
 * @brief      CJMCU75 Driver
 *
 **/

/* Define to prevent recursive inclusion ------------------------------ */
#ifndef __DRIVER_CJMCU75_H
#define __DRIVER_CJMCU75_H

/* Includes ----------------------------------------------------------- */
#include "bsp_i2c.h"
#include "stdbool.h"
#include "stdint.h"

/* Public defines ----------------------------------------------------- */
#define CJMCU75_ADDRESS_8BIT  0b10010000
#define CJMCU75_ADDRESS_7BIT  0b1001000
#define CJMCU75_TEMP_REG      0x00
#define CJMCU75_CONFIG_REG    0x01
/* Public enumerate/structure ----------------------------------------- */
/**
 * @brief Struct for cjmcu75 object
 */
typedef struct
{
    uint8_t device_address; /* I2C device address */
    int8_t  temp;
    
    bool (*i2c_is_device_ready)(uint8_t device_address);
    bool (*i2c_read_at)(uint8_t device_address, uint8_t reg_read, uint8_t *data_read, uint16_t size_data);
    bool (*i2c_write_at)(uint8_t device_address, uint8_t reg_write, uint8_t *data_write, uint16_t size_data);
} cjmcu75_t;

/* Public macros ------------------------------------------------------ */

/* Public variables --------------------------------------------------- */

/* Public function prototypes ----------------------------------------- */
/**
 * @brief Init cjmcu75 driver
 * 
 * @param cjmcu75 CJMCU object
 */
void cjmcu75_init(cjmcu75_t *cjmcu75);

/**
 * @brief Check cjmcu75 connection
 * 
 * @param cjmcu75   CJMCU object
 * @return true     Connect successfully
 * @return false    Connect failure
 */
bool cjmcu75_check_ready(cjmcu75_t *cjmcu75);

/**
 * @brief Get integer temperature value
 * 
 * @param cjmcu75   CJMCU object
 * @return int8_t   Temp value 
 */
int8_t cjmcu75_get_temp(cjmcu75_t *cjmcu75);
#endif  // __DRIVER_DS1307_H

/* End of file -------------------------------------------------------- */
