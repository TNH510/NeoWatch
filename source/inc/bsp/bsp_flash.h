/**
 * @file       bsp_flash.h
 * @copyright  Copyright (C) 2023 QuyLe Co., Ltd. All rights reserved.
 * @license    This project is released under the QuyLe License.
 * @version    1.0.0
 * @date       2023-08-14
 * @author     quyle-itr-intern
 *
 * @brief      bsp flash
 *
 * @note
 */

/* Define to prevent recursive inclusion ------------------------------ */
#ifndef BSP_FLASH_H
#define BSP_FLASH_H
/* Includes ----------------------------------------------------------- */
#include "main.h"
#include "stdint.h"
/* Public defines ----------------------------------------------------- */
#define BSP_FLASH_TIMEOUT 20000

/* Define Flash Sector 0 */
#ifndef BSP_FLASH_SECTOR_0
#define BSP_FLASH_SECTOR_0 0
#endif

/* Define Flash Sector 1 */
#ifndef BSP_FLASH_SECTOR_1
#define BSP_FLASH_SECTOR_1 1
#endif

/* Define Flash Sector 2 */
#ifndef BSP_FLASH_SECTOR_2
#define BSP_FLASH_SECTOR_2 2
#endif

/* Define Flash Sector 3 */
#ifndef BSP_FLASH_SECTOR_3
#define BSP_FLASH_SECTOR_3 3
#endif

/* Define Flash Sector 4 */
#ifndef BSP_FLASH_SECTOR_4
#define BSP_FLASH_SECTOR_4 4
#endif

/* Define Flash Sector 5 */
#ifndef BSP_FLASH_SECTOR_5
#define BSP_FLASH_SECTOR_5 5
#endif

/* Define Flash Sector 6 */
#ifndef BSP_FLASH_SECTOR_6
#define BSP_FLASH_SECTOR_6 6
#endif

/* Define Flash Sector 7 */
#ifndef BSP_FLASH_SECTOR_7
#define BSP_FLASH_SECTOR_7 7
#endif

/* Public enumerate/structure ----------------------------------------- */
/**
 * @brief enum flash status
 * 
 * @return
 */
typedef enum
{
  bsp_flash_status_errors = 0,
  bsp_flash_status_ok,
} bsp_flash_status_t;

#pragma pack(1)
typedef struct
{
  uint8_t no;
  uint8_t ssid[25];
  uint8_t pass[25];
  uint8_t id;
} wifi_info_t;
#pragma pack()

/* Public macros ------------------------------------------------------ */

/* Public function prototypes ----------------------------------------- */

/**
 * @brief  flash unlock to operate with flash memory
 *
 * @return
 */
void bsp_flash_unlock(void);

/**
 * @brief  get sector from address
 *
 * @param[in]     start_address 		address memory
 * @param[out]    uint8_t       		sector number
 *
 * @return
 *  - 0: bsp_flash_status_errors
 *  - 1: bsp_flash_status_ok
 */
uint8_t bsp_get_sector_from_address(uint32_t start_address);

/**
 * @brief  get address from sector
 *
 * @param[in]     sector_number 		sector  memory
 *
 * @return
 *  - 0: bsp_flash_status_errors
 *  - 1: bsp_flash_status_ok
 */
uint32_t bsp_get_address_from_sector(uint8_t sector_number);

/**
 * @brief  erase sector from address
 *
 * @param[in]     start_address 	    address earse memory
 * @param[out]    bsp_flash_status_t 	status result
 *
 * @attention erase to sector not byte, page
 *
 * @return
 *  - 0: bsp_flash_status_errors
 *  - 1: bsp_flash_status_ok
 */
bsp_flash_status_t bsp_flash_erase(uint32_t start_address);

/**
 * @brief  write data to address memory
 *
 * @param[in]     start_address 		  address write memory
 * @param[in]     buffer_write 			  data write memory
 * @param[in]     length 				      length data write to memory
 * @param[out]    bsp_flash_status_t 	status result
 *
 * @attention write data word to flash memory
 *
 * @return
 *  - 0: bsp_flash_status_errors
 *  - 1: bsp_flash_status_ok
 */
bsp_flash_status_t bsp_flash_write(uint32_t start_address, uint32_t *buffer_write, uint32_t length);

/**
 * @brief  read data from address memory
 *
 * @param[in]     start_address 		  address read memory
 * @param[in]     buffer_read 			  buffer data read memory
 * @param[in]     length 				      length data read from memory
 * @param[out]    bsp_flash_status_t 	status result
 *
 * @attention read data word to flash memory
 *
 * @return
 *  - 0: bsp_flash_status_errors
 *  - 1: bsp_flash_status_ok
 */
bsp_flash_status_t bsp_flash_read(uint32_t start_address, uint32_t *buffer_read, uint32_t length);

/**
 * @brief  erase and write data to flash memory
 *
 * @param[in]     start_address 		  address write memory
 * @param[in]     buffer_write 			  buffer write read memory
 * @param[in]     length 				      length data write to memory
 * @param[out]    bsp_flash_status_t 	status result
 *
 * @attention earse memory before write data word to flash memory
 *
 * @return
 *  - 0: bsp_flash_status_errors
 *  - 1: bsp_flash_status_ok
 */
bsp_flash_status_t bsp_flash_write_erase(uint32_t start_address, uint32_t *buffer_write, uint32_t length);

/**
 * @brief  write data float to flash memory
 *
 * @param[in]     start_address 		  address write memory
 * @param[in]     buffer_write 			  buffer write read memory
 * @param[in]     length 				      length data write to memory
 * @param[out]    bsp_flash_status_t 	status result
 *
 * @attention earse memory before write data word to flash memory
 *
 * @return
 *  - 0: bsp_flash_status_errors
 *  - 1: bsp_flash_status_ok
 */
bsp_flash_status_t bsp_flash_write_float(uint32_t start_address, float float_buffer_write);

/**
 * @brief  read data float from flash memory
 *
 * @param[in]     start_address 		  address write memory
 * @param[in]     float_buffer_read 	buffer save data float read from memory
 * @param[out]    bsp_flash_status_t 	status result
 *
 * @return
 *  - 0: bsp_flash_status_errors
 *  - 1: bsp_flash_status_ok
 */
bsp_flash_status_t bsp_flash_read_float(uint32_t start_address, float *float_buffer_read);

/**
 * @brief  write multi data float to flash memory
 *
 * @param[in]     start_address 		  address write memory
 * @param[in]     float_buffer_write 	buffer write multi data float to memory
 * @param[out]    bsp_flash_status_t 	status result
 *
 * @return
 *  - 0: bsp_flash_status_errors
 *  - 1: bsp_flash_status_ok
 */
bsp_flash_status_t bsp_flash_write_multi_float(uint32_t start_address, float *float_buffer_write, uint32_t length);

/**
 * @brief  read multi data float to flash memory
 *
 * @param[in]     start_address 		  address write memory
 * @param[in]     float_buffer_write 	buffer save multi data float read from memory
 * @param[out]    bsp_flash_status_t 	status result
 *
 * @return
 *  - 0: bsp_flash_status_errors
 *  - 1: bsp_flash_status_ok
 */
bsp_flash_status_t bsp_flash_read_multi_float(uint32_t start_address, float *float_buffer_write, uint32_t length);

/**
 * @brief  write struct data to flash memory
 *
 * @param[in]     start_address 		  address read memory
 * @param[in]     wifi_data 	        buffer write struct wifi data to memory
 * @param[out]    bsp_flash_status_t 	status result
 *
 * @return
 *  - 0: bsp_flash_status_errors
 *  - 1: bsp_flash_status_ok
 */
bsp_flash_status_t bsp_flash_write_struct(uint32_t start_address, wifi_info_t wifi_data);

/**
 * @brief  read struct data from flash memory
 *
 * @param[in]     start_address 		  address read memory
 * @param[in]     wifi_data 	        buffer read struct wifi data from memory
 * @param[out]    bsp_flash_status_t 	status result
 *
 * @return
 *  - 0: bsp_flash_status_errors
 *  - 1: bsp_flash_status_ok
 */
bsp_flash_status_t bsp_flash_read_struct(uint32_t start_address, wifi_info_t *wifi_data);

#endif

/* End of file -------------------------------------------------------- */