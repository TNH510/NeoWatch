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

/* Define to prevent recursive inclusion ------------------------------ */
#ifndef __BSP_I2C_H
#define __BSP_I2C_H

/* Includes ----------------------------------------------------------- */
#include "base_type.h"

/* Public defines ----------------------------------------------------- */
/* Public enumerate/structure ----------------------------------------- */
/* Public macros ------------------------------------------------------ */
/* Public variables --------------------------------------------------- */
/* Public function prototypes ----------------------------------------- */

/**
 * @brief Allocate an I2C master bus
 *
 * @param[in] bus_config I2C master bus configuration.
 * @param[out] ret_bus_handle I2C bus handle
 * @return
 *      - ESP_OK: I2C master bus initialized successfully.
 *      - ESP_ERR_*: I2C master bus initialize failed.
 */
bool bsp_i2c_init(void);

#endif  // __BSP_I2C_H

/* End of file -------------------------------------------------------- */
