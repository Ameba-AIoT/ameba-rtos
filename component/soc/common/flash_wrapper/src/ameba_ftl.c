/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/* Includes ------------------------------------------------------------------*/

#include "ameba_ftl.h"
#include "floader_rom_api.h"

/* Exported functions --------------------------------------------------------*/

/**
  * @brief  Enable flash 4-byte address mode
  * @param  void
  * @retval void
  */
void ftl_enable_4byte_address_mode(bool status)
{
	FLASH_UserMode_Enter();
	FLASH_WriteEn_InUserMode();
	if (status) {
		FLASH_TxCmd_InUserMode(FLASH_CMD_ENT_ADDR4B, 0, NULL);
	} else {
		FLASH_TxCmd_InUserMode(FLASH_CMD_EXT_ADDR4B, 0, NULL);
	}
	FLASH_WaitBusy_InUserMode(WAIT_FLASH_BUSY);
	FLASH_UserMode_Exit();
}
