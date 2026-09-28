/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _AMEBA_NOR_FLASH_H
#define _AMEBA_NOR_FLASH_H

#ifdef __cplusplus
extern "C" {
#endif

/* Exported constants --------------------------------------------------------*/

/* Above this flash clock some vendors need extra dummy cycles and a DC bit in
 * the configuration register, i.e. High Performance Mode. */
#define FLASH_HPM_CLK_LIMIT		(104 * MHZ_TICK_CNT)

/* Exported functions --------------------------------------------------------*/

/**
  * @brief  Identify the NOR flash chip and re-initialize flash_init_para accordingly.
  * @note   Reads the flash ID, looks it up in Flash_AVL and dispatches to the
  *         FLASH_StructInit_xxx matching its flash_class. Must be called before
  *         flash_nor_set_status_register() and flash_nor_get_chip_info().
  * @retval None
  */
void flash_nor_get_vendor(void);

/**
  * @brief  Set the NOR flash status register: set QE bit, clear protection bits.
  * @note   flash_nor_get_vendor() must be called first to get the status mask.
  * @retval None
  */
void flash_nor_set_status_register(void);

/**
  * @brief  Get the AVL entry of the identified NOR flash chip.
  * @retval Pointer to the Flash_AVL entry, or NULL if flash_nor_get_vendor() has not run.
  */
const FlashInfo_TypeDef *flash_nor_get_chip_info(void);

/**
  * @brief  Set up the flash to be read at the given bitmode and clock.
  * @param  spic_mode: the bitmode about to be used, one of Spic1IOBitMode ... Spic4IOBitMode.
  * @param  flash_clk: the flash clock in Hz that spic_mode will run at, 0 if the flash
  *                    still runs on the boot clock (no High Performance Mode needed).
  * @note   Above FLASH_HPM_CLK_LIMIT the 4IO read needs extra dummy cycles, which is
  *         applied to flash_init_para for every flash. On top of that the vendors that
  *         keep the dummy cycle on the flash side are programmed here:
  *         - Micron: write the read dummy cycle of spic_mode into its volatile
  *           configuration register (always, independent of flash_clk).
  *         - PUYA: set the DC bit once flash_clk exceeds FLASH_HPM_CLK_LIMIT.
  *         Must be called before FLASH_Init(spic_mode), and after flash_nor_get_vendor().
  * @retval None
  */
void flash_nor_set_hpm_mode(u32 spic_mode, u32 flash_clk);

#ifdef __cplusplus
}
#endif

#endif  //_AMEBA_NOR_FLASH_H
