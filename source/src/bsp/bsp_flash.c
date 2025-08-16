/**
 * @file       bsp_flash.c
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

/* Includes ----------------------------------------------------------- */
#include "bsp_flash.h"

/* Private defines ---------------------------------------------------- */
#define KEY1 (uint32_t) 0x45670123
#define KEY2 (uint32_t) 0xCDEF89AB

/* Private enumerate/structure ---------------------------------------- */

/* Private macros ----------------------------------------------------- */

/* Public variables --------------------------------------------------- */

/* Private variables -------------------------------------------------- */

/* Private function prototypes ---------------------------------------- */

/* Function definitions ----------------------------------------------- */
void bsp_flash_unlock(void)
{
  FLASH->KEYR = KEY1;
  FLASH->KEYR = KEY2;
}

uint8_t bsp_get_sector_from_address(uint32_t start_address)
{
  uint8_t sector_number = 0;
  if ((start_address >= 0x08000000) && (start_address < 0x08003FFF))
    sector_number = BSP_FLASH_SECTOR_0;
  else if ((start_address >= 0x08004000) && (start_address < 0x08007FFF))
    sector_number = BSP_FLASH_SECTOR_1;
  else if ((start_address >= 0x08008000) && (start_address < 0x0800BFFF))
    sector_number = BSP_FLASH_SECTOR_2;
  else if ((start_address >= 0x0800C000) && (start_address < 0x0800FFFF))
    sector_number = BSP_FLASH_SECTOR_3;
  else if ((start_address >= 0x08010000) && (start_address < 0x0801FFFF))
    sector_number = BSP_FLASH_SECTOR_4;
  else if ((start_address >= 0x08020000) && (start_address < 0x0803FFFF))
    sector_number = BSP_FLASH_SECTOR_5;
  else if ((start_address >= 0x08040000) && (start_address < 0x0805FFFF))
    sector_number = BSP_FLASH_SECTOR_6;
  else if ((start_address >= 0x08060000) && (start_address < 0x0807FFFF))
    sector_number = BSP_FLASH_SECTOR_7;
  return sector_number;
}

uint32_t bsp_get_address_from_sector(uint8_t sector_number)
{
  uint32_t address_sector = 0;
  if (sector_number == BSP_FLASH_SECTOR_0)
    address_sector = 0x08000000;
  else if (sector_number == BSP_FLASH_SECTOR_1)
    address_sector = 0x08004000;
  else if (sector_number == BSP_FLASH_SECTOR_2)
    address_sector = 0x08008000;
  else if (sector_number == BSP_FLASH_SECTOR_3)
    address_sector = 0x0800C000;
  else if (sector_number == BSP_FLASH_SECTOR_4)
    address_sector = 0x08010000;
  else if (sector_number == BSP_FLASH_SECTOR_5)
    address_sector = 0x08020000;
  else if (sector_number == BSP_FLASH_SECTOR_6)
    address_sector = 0x08040000;
  else if (sector_number == BSP_FLASH_SECTOR_7)
    address_sector = 0x08060000;
  return address_sector;
}

bsp_flash_status_t bsp_flash_erase(uint32_t start_address)
{
  uint32_t time_out = BSP_FLASH_TIMEOUT;
  /* Check that no Flash memory operation is ongoing by checking the BSY bit in the FLASH CR register */
  while (((FLASH->SR & FLASH_SR_BSY) == FLASH_SR_BSY) && time_out)
  {
    if (--time_out == 0x00)
    {
      return bsp_flash_status_errors;
    }
  }
  /* Wating for Bsy bit */
  /* Check unlock sequences */
  if ((FLASH->CR & FLASH_CR_LOCK) == FLASH_CR_LOCK)
  {
    bsp_flash_unlock();
  }
  FLASH->CR |= FLASH_PSIZE_WORD;
  /* Set the SER bit in the FLASH_CR register */
  FLASH->CR |= FLASH_CR_SER;
  /* Program the FLASH_CR register to select a sector_number to erase */
  FLASH->CR |= bsp_get_sector_from_address(start_address) << 3;
  /* Set the STRT bit in the FLASH CR register */
  FLASH->CR |= FLASH_CR_STRT;
  /* Wait for the BSY bit to be reset */
  time_out = BSP_FLASH_TIMEOUT;
  while (((FLASH->SR & FLASH_SR_BSY) == FLASH_SR_BSY) && time_out)
  {
    /* Wating for Bsy bit */
    if (--time_out == 0x00)
    {
      return bsp_flash_status_errors;
    }
  }
  /* Check FLASH End of Operation flag  */
  if ((FLASH->SR & FLASH_FLAG_EOP) != RESET)
  {
    /* Clear FLASH End of Operation pending bit */
    FLASH->SR &= ~FLASH_FLAG_EOP;
  }
  /* Clear SER bit in the FLASH_CR register */
  FLASH->CR &= FLASH_CR_SER;
  /* Clear STRT bit in the FLASH CR register */
  FLASH->CR &= FLASH_CR_STRT;
  /* Set the LOCK Bit to lock the FLASH Registers access */
  FLASH->CR |= FLASH_CR_LOCK;
  return bsp_flash_status_ok;
}

bsp_flash_status_t bsp_flash_write(volatile uint32_t start_address, uint32_t *buffer_write, uint32_t length)
{
  uint32_t count    = 0u;
  uint32_t time_out = BSP_FLASH_TIMEOUT;
  /* Check input parameters */
  if ((buffer_write == 0u) || (length == 0u))
  {
    return bsp_flash_status_errors;
  }
  /* Check thet no Flash memory ope ration is ongoing by checking the BSY bit in the FLASH_CR register*/
  while (((FLASH->SR & FLASH_SR_BSY) == FLASH_SR_BSY) && time_out)
  {
    /* Wating for Bsy bit */
    if (--time_out == 0x00)
    {
      return bsp_flash_status_errors;
    }
  }
  /* Check unlock sequences */
  if ((FLASH->CR & FLASH_CR_LOCK) == FLASH_CR_LOCK)
  {
    bsp_flash_unlock();
  }
  FLASH->CR |= FLASH_PSIZE_WORD;
  /* Write FLASH CR_PG to 1 */
  FLASH->CR |= FLASH_CR_PG;
  /* Perform word write at the desired address */
  for (count = 0; count < length; count++)
  {
    *(__IO uint32_t *) (start_address + count * 4u) = *(uint32_t *) (buffer_write + count);
  }
  /* Wait for the BSY bit to be reset */
  time_out = BSP_FLASH_TIMEOUT;
  while (((FLASH->SR & FLASH_SR_BSY) == FLASH_SR_BSY) && time_out)
  {
    /* Wating for Bsy bit */
    if (--time_out == 0x00)
    {
      return bsp_flash_status_errors;
    }
  }
  /* Clear PG bit in the FLASH CR register */
  FLASH->CR &= FLASH_CR_PG;
  /* Set the LOCK Bit to lock the FLASH Registers access */
  FLASH->CR |= FLASH_CR_LOCK;
  return bsp_flash_status_ok;
}

bsp_flash_status_t bsp_flash_read(uint32_t start_address, uint32_t *buffer_read, uint32_t length)
{
  uint32_t count = 0u;
  /* Check input paras */
  if ((buffer_read == 0u) || (length == 0u))
  {
    return bsp_flash_status_errors;
  }
  /* Read data buffer */
  for (count = 0u; count < length; count++)
  {
    *(uint32_t *) (buffer_read + count) = *(uint32_t *) (start_address + count * 4u);
  }
  return bsp_flash_status_ok;
}

bsp_flash_status_t bsp_flash_write_erase(uint32_t start_address, uint32_t *buffer_write, uint32_t length)
{
  uint16_t count    = 0u;
  uint16_t time_out = BSP_FLASH_TIMEOUT;
  /* Check input parameters */
  if ((buffer_write == 0u) || (length == 0u))
  {
    return bsp_flash_status_errors;
  }
  /* --- BEGIN ERASE sector_number WIRTE DATA TO FLASH MEMORY  --- */
  /* Get the number of sector_number to erase */
  uint8_t start_sector  = bsp_get_sector_from_address(start_address);
  uint8_t end_sector    = bsp_get_sector_from_address(start_address + length * 4);
  uint8_t number_sector = end_sector - start_sector + 1;
  /* Erase sector_number for write data */
  for (uint8_t i = 0; i < number_sector; i++)
  {
    bsp_flash_erase(bsp_get_address_from_sector(start_sector));
    start_sector++;
  }
  /* --- END ERASE sector_number WIRTE DATA TO FLASH MEMORY  --- */
  /* --- BEGIN WRITE DATA TO FLASH MEMORY --- */
  /* Check thet no Flash memory ope ration is ongoing by checking the BSY bit in the FLASH_CR register*/
  while (((FLASH->SR & FLASH_SR_BSY) == FLASH_SR_BSY) && time_out)
  {
    /* Wating for Bsy bit */
    if (--time_out == 0x00)
    {
      return bsp_flash_status_errors;
    }
  }
  /* Check unlock sequences */
  if ((FLASH->CR & FLASH_CR_LOCK) == FLASH_CR_LOCK)
  {
    bsp_flash_unlock();
  }
  FLASH->CR |= FLASH_PSIZE_WORD;
  /* Write FLASH CR_PG to 1 */
  FLASH->CR |= FLASH_CR_PG;
  /* Perform word write at the desired address */
  for (count = 0; count < length; count++)
  {
    *(__IO uint32_t *) (start_address + count * 4u) = *(uint32_t *) (buffer_write + count);
  }
  /* Wait for the BSY bit to be reset */
  time_out = BSP_FLASH_TIMEOUT;
  while (((FLASH->SR & FLASH_SR_BSY) == FLASH_SR_BSY) && time_out)
  {
    /* Wating for Bsy bit */
    if (--time_out == 0x00)
    {
      return bsp_flash_status_errors;
    }
  }
  /* Clear PG bit in the FLASH CR register */
  FLASH->CR &= FLASH_CR_PG;
  /* Set the LOCK Bit to lock the FLASH Registers access */
  FLASH->CR |= FLASH_CR_LOCK;
  /* --- END WRITE DATA TO FLASH MEMORY --- */
  return bsp_flash_status_ok;
}

bsp_flash_status_t bsp_flash_write_float(uint32_t start_address, float float_buffer_write)
{
  uint8_t data[4];
  *(float *) data = float_buffer_write;
  if (bsp_flash_write(start_address, (uint32_t *) data, 1) != bsp_flash_status_ok)
    return bsp_flash_status_errors;
  return bsp_flash_status_ok;
}

bsp_flash_status_t bsp_flash_read_float(uint32_t start_address, float *float_buffer_read)
{
  uint32_t data;
  if (bsp_flash_read(start_address, &data, 1) != bsp_flash_status_ok)
    return bsp_flash_status_errors;
  *float_buffer_read = *(float *) &data;
  return bsp_flash_status_ok;
}

bsp_flash_status_t bsp_flash_write_multi_float(uint32_t start_address, float *float_buffer_write, uint32_t length)
{
  uint8_t  data[4];
  uint32_t data32[length];
  for (uint16_t i = 0; i < length; i++)
  {
    *(float *) data = *(float_buffer_write + i);
    data32[i]       = *(uint32_t *) data;
  }
  if (bsp_flash_write(start_address, data32, length) != bsp_flash_status_ok)
    return bsp_flash_status_errors;
  return bsp_flash_status_ok;
}

bsp_flash_status_t bsp_flash_read_multi_float(uint32_t start_address, float *float_buffer_read, uint32_t length)
{
  uint32_t data[length];
  if (bsp_flash_read(start_address, data, length) != bsp_flash_status_ok)
    return bsp_flash_status_errors;
  for (uint16_t i = 0; i < length; i++)
  {
    *(float_buffer_read + i) = *(float *) (data + i);
  }
  return bsp_flash_status_ok;
}

bsp_flash_status_t bsp_flash_write_struct(uint32_t start_address, wifi_info_t wifi_data)
{
  if (bsp_flash_write(start_address, (uint32_t *) &wifi_data, sizeof(wifi_info_t) / 4 + (sizeof(wifi_info_t) % 4 != 0)) != bsp_flash_status_ok)
    return bsp_flash_status_errors;
  return bsp_flash_status_ok;
}

bsp_flash_status_t bsp_flash_read_struct(uint32_t start_address, wifi_info_t *wifi_data)
{
  if (bsp_flash_read(start_address, (uint32_t *) wifi_data, sizeof(wifi_info_t) / 4 + (sizeof(wifi_info_t) % 4 != 0)) != bsp_flash_status_ok)
    return bsp_flash_status_errors;
  return bsp_flash_status_ok;
}

/* End of file -------------------------------------------------------- */