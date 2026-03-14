/**
 * @file       bsp_i2c.h
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

/* Define to prevent recursive inclusion ------------------------------ */
#ifndef __BSP_I2C_H
#define __BSP_I2C_H

/* Includes ----------------------------------------------------------- */
#include "base_type.h"
#include "driver/i2c_master.h"

/* Public defines ----------------------------------------------------- */
#define BSP_I2C_PORT        0    /*!< I2C bus port number              */
#define BSP_I2C_SCL_PIN     12   /*!< GPIO pin number for SCL          */
#define BSP_I2C_SDA_PIN     13   /*!< GPIO pin number for SDA          */
#define BSP_I2C_TIMEOUT_MS  100  /*!< Default transaction timeout (ms) */

/* Public enumerate/structure ----------------------------------------- */
/* Public macros ------------------------------------------------------ */
/* Public variables --------------------------------------------------- */
/* Public function prototypes ----------------------------------------- */

/**
 * @brief  Initialize the I2C master bus
 *
 * @return BS_OK   Initialization succeeded
 * @return BS_ERROR  Initialization failed
 */
base_status_t bsp_i2c_init(void);

/**
 * @brief  De-initialize the I2C master bus and free all resources
 */
void bsp_i2c_deinit(void);

/**
 * @brief  Get the I2C master bus handle
 *
 * @note   bsp_i2c_init() must be called before using this handle.
 *
 * @return i2c_master_bus_handle_t  Active bus handle, or NULL if not initialized
 */
i2c_master_bus_handle_t bsp_i2c_get_handle(void);

/**
 * @brief  Add an I2C device to the bus
 *
 * @param[in]  dev_addr     7-bit device address
 * @param[in]  scl_speed_hz SCL clock frequency in Hz (e.g. 400000 for Fast-mode)
 * @param[out] dev_handle   Handle for the newly added device
 *
 * @return ESP_OK   Device added successfully
 * @return ESP_ERR_INVALID_ARG  Failed to add device due to invalid arguments
 * @return ESP_FAIL Failed to add device due to other errors
 */
base_status_t bsp_i2c_add_device(uint16_t dev_addr, uint32_t scl_speed_hz,
                        i2c_master_dev_handle_t *dev_handle);

/**
 * @brief  Remove a previously added I2C device from the bus
 *
 * @param[in] dev_handle  Device handle returned by bsp_i2c_add_device()
 *
 * @return ESP_OK   Device removed successfully
 * @return ESP_ERR_INVALID_ARG  Failed to remove device due to invalid arguments
 * @return ESP_FAIL Failed to remove device due to other errors
 */
base_status_t bsp_i2c_remove_device(i2c_master_dev_handle_t dev_handle);

/**
 * @brief  Write data to an I2C device
 *
 * @param[in] dev_handle  Device handle returned by bsp_i2c_add_device()
 * @param[in] data        Pointer to the transmit buffer
 * @param[in] len         Number of bytes to write
 *
 * @return ESP_OK   Write succeeded
 * @return ESP_ERR_INVALID_ARG  Write failed due to invalid arguments
 * @return ESP_FAIL Write failed due to other errors
 */
base_status_t bsp_i2c_write(i2c_master_dev_handle_t dev_handle, const uint8_t *data, size_t len);

/**
 * @brief  Read data from an I2C device
 *
 * @param[in]  dev_handle  Device handle returned by bsp_i2c_add_device()
 * @param[out] data        Pointer to the receive buffer
 * @param[in]  len         Number of bytes to read
 *
 * @return ESP_OK   Read succeeded
 * @return ESP_ERR_INVALID_ARG  Read failed due to invalid arguments
 * @return ESP_FAIL Read failed due to other errors
 */
base_status_t bsp_i2c_read(i2c_master_dev_handle_t dev_handle, uint8_t *data, size_t len);

/**
 * @brief  Perform a combined write-then-read transaction (register access)
 *
 * Sends @p write_len bytes (e.g. a register address) and then reads
 * @p read_len bytes in a single I2C transaction without releasing the bus.
 *
 * @param[in]  dev_handle  Device handle returned by bsp_i2c_add_device()
 * @param[in]  write_data  Data to transmit (e.g. register address)
 * @param[in]  write_len   Number of bytes to write
 * @param[out] read_data   Buffer to store the received bytes
 * @param[in]  read_len    Number of bytes to read
 *
 * @return ESP_OK   Transaction succeeded
 * @return ESP_ERR_INVALID_ARG  Transaction failed due to invalid arguments
 * @return ESP_FAIL Transaction failed due to other errors
 */
base_status_t bsp_i2c_write_read(i2c_master_dev_handle_t dev_handle,
                        const uint8_t          *write_data, size_t write_len,
                        uint8_t                *read_data,  size_t read_len);

#endif  // __BSP_I2C_H

/* End of file -------------------------------------------------------- */
