/**
 * @file       fonts.h
 * @copyright  Copyright (C) 2019 QuyLe Co., Ltd. All rights reserved.
 * @license    This project is released under the QuyLe License.
 * @version    0.0.1
 * @date       2023
 * @author     Quy Le
 * @author     Quy Le
 *
 * @brief      Middleware for Font
 *
 * @note       None
 * @example    None
 */

/* Define to prevent recursive inclusion ------------------------------ */
#ifndef __FONTS_H
#define __FONTS_H

/* Includes ----------------------------------------------------------- */
/**
 *
 * Default fonts library. It is used in all LCD based libraries.
 *
 * \par Supported fonts
 *
 * Currently, these fonts are supported:
 *  - 7 x 10 pixels
 *  - 11 x 18 pixels
 *  - 16 x 26 pixels
 *  - 16 x 24 pixels
 *  - 6 x 8 pixels
 *  - 15 x 25 pixels
 *  - 10 x 19 pixels
 */
#include "main.h"
#include "string.h"

#include <stdint.h>

/* Public defines ----------------------------------------------------- */

/* Public enumerate/structure ----------------------------------------- */

/**
 * @defgroup LIB_Typedefs
 * @brief    Library Typedefs
 * @{
 */

/**
 * @brief  Font structure used on my LCD libraries
 */
typedef struct
{
    uint8_t         font_width;  /*!< Font width in pixels */
    uint8_t         font_height; /*!< Font height in pixels */
    const uint16_t *data;        /*!< Pointer to data font data array */
} font_def_t;

/**
 * @brief  Font structure used on my LCD libraries
 */
typedef struct
{
    uint8_t        font_width;  /*!< Font width in pixels */
    uint8_t        font_height; /*!< Font height in pixels */
    const uint8_t *data;        /*!< Pointer to data font data array */
} font_def_custom_t;

/**
 * @brief  String length and height
 */
typedef struct
{
    uint16_t length; /*!< String length in units of pixels */
    uint16_t height; /*!< String height in units of pixels */
} font_size_t;

/* Public macros ------------------------------------------------------ */

/* Public variables --------------------------------------------------- */

/**
 * @brief  7 x 10 pixels font size structure
 */
extern font_def_t font_7x10;

/**
 * @brief  11 x 18 pixels font size structure
 */
extern font_def_t font_11x18;

/**
 * @brief  16 x 26 pixels font size structure
 */
extern font_def_t font_16x26;

/**
 * @brief  16 x 24 pixels font size structure
 */
extern font_def_t font_16x24;

/**
 * @brief  6 x 8 pixels font size structure
 */
extern font_def_t font_6x8;

/**
 * @brief  15 x 25 pixels font size structure
 */
extern font_def_t font_15x25;

/**
 * @brief  10 x 19 pixels font size structure
 */
extern font_def_t font_10x19;

/* Public function prototypes ----------------------------------------- */

/**
 * @brief  Calculates string length and height in units of pixels depending on string and font used
 * @param  *str: String to be checked for length and height
 * @param  *size_struct: Pointer to empty @ref font_size_t structure where informations will be saved
 * @param  *font: Pointer to @ref font_def_t font used for calculations
 * @retval Pointer to string used for length and height
 */
char *font_get_string_size(char *str, font_size_t *size_struct, font_def_t *font);

#endif
