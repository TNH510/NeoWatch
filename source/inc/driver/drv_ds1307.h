/**
 * @file       driver_ds1307.h
 * @copyright  Copyright (C) 2019 Fiot Co., Ltd. All rights reserved.
 * @license    This project is released under the Fiot License.
 * @version    2.0.1
 * @date       2023-08-21
 * @author     Hieu Tran
 *
 * @brief      ds1307 Driver
 * @example
 * 
 *     char buffer[20] = "2023-09-11 11:11:11";
 *     bool status = ds1307_set_time(&ds1307, buffer);
 *
 *     while (1)
 *     {
 *        rtc = *ds1307_get_time(&ds1307);
 *        HAL_Delay(500);
 *      }
 *
 *
**/

/* Define to prevent recursive inclusion ------------------------------ */
#ifndef __DRIVER_DS1307_H
#define __DRIVER_DS1307_H

/* Includes ----------------------------------------------------------- */
#include "bsp_i2c.h"
#include "stdint.h"
#include <time.h>
#include <string.h>

/* Public defines ----------------------------------------------------- */
#define DS1307_ADDRESS_8BIT          0xD0
#define DS1307_ADDRESS_7BIT          0x1101000
#define DS1307_MINIMAL_BUFFER_LENGTH 19

/**
 * Registers addresses.
 */
#define DS1307_REGISTER_SECOND       0x00
#define DS1307_REGISTER_MINUTE       0x01
#define DS1307_REGISTER_HOUR         0x02
#define DS1307_REGISTER_DAY_OF_WEEK  0x03
#define DS1307_REGISTER_DAY_OF_MONTH 0x04
#define DS1307_REGISTER_MONTH        0x05
#define DS1307_REGISTER_YEAR         0x06
#define DS1307_REGISTER_CONTROL      0x07
#define DS1307_REGISTER_RAM_START    0x08
#define DS1307_REGISTER_RAM_END      0x3F

/**
 * Registers masks. All time data are stored in BCD.
 * Example 21 seconds are stored like:
 * DS1307_MASK_SECOND = 1 and DS1307_MASK_SECOND_10 = 2.
 */
#define DS1307_MASK_SECOND           0b01111111
#define DS1307_MASK_MINUTE           0b01111111
#define DS1307_MASK_HOUR             0b00111111
#define DS1307_MASK_DAY_OF_WEEK      0b00000111
#define DS1307_MASK_DAY_OF_MONTH     0b00111111
#define DS1307_MASK_MONTH            0b00011111
#define DS1307_MASK_YEAR             0b11111111

/**
 * Clock Halt (CH) - register DS1307_REGISTER_SECOND (0x00).
 * When this bit is set to 1 RTC was halt - default state.
 */
#define DS1307_FLAG_0X00_CH          0b10000000

/**
 * 12h mode - register DS1307_REGISTER_HOUR (0x02)
 */
#define DS1307_FLAG_0X02_12_PM       0b01100000
#define DS1307_FLAG_0X02_12_AM       0b01000000

/*
 * Bit sets enable / disable square wave output.
 */
#define DS1307_FLAG_SQWE             0b00010000

/**
 * Values of control register that config and enable
 * square wave generator. Freqs:
 *
 * DS1307_SET_SQ_1_HZ - 1Hz
 * DS1307_SET_SQ_4_KHZ - 4.096kHz
 * DS1307_SET_SQ_8_KHZ - 8.192kHz
 * DS1307_SET_SQ_32_KHZ - 32.768kHz
 */
#define DS1307_SQW_1_HZ              0b00010000
#define DS1307_SQW_4_KHZ             0b00010001
#define DS1307_SQW_8_KHZ             0b00010010
#define DS1307_SQW_32_KHZ            0b00010011

/* Public enumerate/structure ----------------------------------------- */
/* Struct for time */
typedef struct
{
    uint8_t year;
    uint8_t month;
    uint8_t day_of_month;
    uint8_t day_of_week;
    uint8_t hour;
    uint8_t minute;
    uint8_t second;
} rtc_t;

/* Struct for DS1307 object */
typedef struct
{
    rtc_t   rtc;
    uint8_t device_address; /* I2C device address */
    bool (*i2c_is_device_ready)(uint8_t device_address);
    bool (*i2c_read_at)(uint8_t device_address, uint8_t reg_read, uint8_t *data_read, uint16_t size_data);
    bool (*i2c_write_at)(uint8_t device_address, uint8_t reg_write, uint8_t *data_write, uint16_t size_data);
} ds1307_t;

/* Public macros ------------------------------------------------------ */

/* Public variables --------------------------------------------------- */

/* Public function prototypes ----------------------------------------- */
/**
 * @brief Use Zeller's Congruence Algorithm to calculate day of week
 *
 * @param day_of_month  Day of month 
 * @param month         Month
 * @param year          Year
 * @return uint8_t
 */
uint8_t calculate_day_of_week(uint8_t day_of_month, uint8_t month, uint8_t year);
/**
 * @brief Set day of month
 *
 * @param ds1307 Object ds1307
 * @param buffer Pointer to array contain data
 * @return true  Set success
 * @return false Set fail
 */
bool ds1307_set_day_of_month(ds1307_t *ds1307, char *buffer);

/**
 * @brief Set day of week
 *
 * @param ds1307 Object ds1307
 * @param buffer Pointer to array contain data
 * @return true  Set success
 * @return false Set fail
 */
bool ds1307_set_day_of_week(ds1307_t *ds1307, char *buffer);

/**
 * @brief Set month
 *
 * @param ds1307 Object ds1307
 * @param buffer Pointer to array contain data
 * @return true  Set success
 * @return false Set fail
 */
bool ds1307_set_month(ds1307_t *ds1307, char *buffer);

/**
 * @brief Set year
 *
 * @param ds1307 Object ds1307
 * @param buffer Pointer to array contain data
 * @return true  Set success
 * @return false Set fail
 */
bool ds1307_set_year(ds1307_t *ds1307, char *buffer);

/**
 * @brief Set second
 *
 * @param ds1307 Object ds1307
 * @param buffer Pointer to array contain data
 * @return true  Set success
 * @return false Set fail
 */
bool ds1307_set_sec(ds1307_t *ds1307, char *buffer);

/**
 * @brief Set minute
 *
 * @param ds1307 Object ds1307
 * @param buffer Pointer to array contain data
 * @return true  Set success
 * @return false Set fail
 */
bool ds1307_set_min(ds1307_t *ds1307, char *buffer);

/**
 * @brief Set hour 24h mode
 *
 * @param ds1307 Object ds1307
 * @param buffer Pointer to array contain data
 * @return true  Set success
 * @return false Set fail
 */
bool ds1307_set_hour_24h(ds1307_t *ds1307, char *buffer);

/**
 * @brief Set hour 12h mode
 *
 * @param ds1307 Object ds1307
 * @param buffer Pointer to array contain data
 * @return true  Set success
 * @return false Set fail
 */
bool ds1307_set_hour_12h(ds1307_t *ds1307, char *buffer);

/**
 * @brief Initialize da1307 driver
 *
 * @param ds1307 Object ds1307
 * @param bsp_i2c Object bsp_i2c
 */
void ds1307_init(ds1307_t *ds1307);

/**
 * @brief Get all time registers
 *
 * @param ds1307  Object ds1307
 * @return rtc_t* Pointer to the stuct contain result
 */
rtc_t *ds1307_get_time(ds1307_t *ds1307);

/**
 * @brief Set time (YEAR-MONTH-DAY HH:MM:SS)
 *
 * @param ds1307  Object ds1307
 * @param buffer  Pointer to array contain data
 * @return true   Set success
 * @return false  Set fail
 */
bool ds1307_set_time(ds1307_t *ds1307, char *buffer);

/**
 * @brief Convert DS1307 time to epoch_time
 * 
 * @param rtc           RTC struct 
 * @param epoch_time    Variable to save time
 */
void ds1307_convert_to_epoch_time(rtc_t rtc, uint32_t *epoch_time);

/**
 * @brief Convert epoch_time to DS1307 time
 * 
 * @param rtc           Pointer RTC struct to save time
 * @param epoch_time    Epoch time want to convert
 */
void ds1307_convert_to_date_time(rtc_t* rtc, uint32_t epoch_time);

#endif  // __DRIVER_DS1307_H

/* End of file -------------------------------------------------------- */
