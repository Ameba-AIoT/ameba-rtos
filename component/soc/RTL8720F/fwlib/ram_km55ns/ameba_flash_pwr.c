/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "ameba_soc.h"

#define FLASH_RAM_TEXT_SECTION	SRAM_ONLY_TEXT_SECTION

/* Flash capacity < 16MB normally in this project */
#define FLASH_ADDR_PHASE_LEN ADDR_3_BYTE

/**
* @brief  This function is used to enter User mode.
* @param none
* @retval none
*/
FLASH_RAM_TEXT_SECTION
void FLASH_UserMode_Enter(void)
{
	SPIC_TypeDef *spi_flash = SPIC;

	__DSB();
	__ISB();

	spi_flash->CTRLR0 |= BIT_USER_MODE;
	/* user mode is entered after auto cmd is done, which means if SPIC BUSY=1, HW will not write Ctrl0 */
	while (!(spi_flash->CTRLR0 & BIT_USER_MODE));
}

/**
* @brief  This function is used to Exit User mode.
* @param none
* @retval none
*/
FLASH_RAM_TEXT_SECTION
void FLASH_UserMode_Exit(void)
{
	SPIC_TypeDef *spi_flash = SPIC;
	spi_flash->CTRLR0 &= ~BIT_USER_MODE;
	while (spi_flash->CTRLR0 & BIT_USER_MODE);
	__ISB();
}

FLASH_RAM_TEXT_SECTION
void FLASH_TxBasic_InUserMode(u8 cmd, u8 AddrValid, u32 StartAddr, u32 DataPhaseLen, u8 *pData)
{
	SPIC_TypeDef *spi_flash = SPIC;
	u32 tx_num = 0;
	u32 temp, idx;
	u32 *aligned_buf;
	u32 addr_length;

	/* set CMD */
	spi_flash->DR[0].BYTE = cmd;

	/* set Address, backup USER_LENGTH */
	addr_length = spi_flash->USER_LENGTH;
	temp = spi_flash->USER_LENGTH & ~MASK_USER_ADDR_LENGTH;
	if (AddrValid) {
		spi_flash->USER_LENGTH = temp | FLASH_ADDR_PHASE_LEN;

		if (FLASH_ADDR_PHASE_LEN == ADDR_4_BYTE) {
			spi_flash->DR[0].BYTE = (u8)((StartAddr & 0xFF000000) >> 24);
		}
		spi_flash->DR[0].BYTE = (u8)((StartAddr & 0xFF0000) >> 16);
		spi_flash->DR[0].BYTE = (u8)((StartAddr & 0xFF00) >> 8);
		spi_flash->DR[0].BYTE = (u8)(StartAddr & 0xFF);
	} else {
		spi_flash->USER_LENGTH = temp | USER_ADDR_LENGTH(0);
	}

	/* set DATA length */
	spi_flash->RX_NDF = 0;
	spi_flash->TX_NDF = TX_NDF(DataPhaseLen);

	/* Enable SSIENR to start the transfer */
	spi_flash->SSIENR = BIT_SPIC_EN;

	if (DataPhaseLen > 0) {
		/* write some address non-aligned data in byte */
		temp = UNALIGNED32(pData);
		if (temp) {
			while (tx_num < DataPhaseLen) {
				spi_flash->DR[0].BYTE = pData[tx_num];
				tx_num += 1;
				if (tx_num == U32BLOCKSIZE - temp) {
					break;
				}
			}
		}

		/* write the address aligned data in word */
		aligned_buf = (u32 *)&pData[tx_num];
		while (DataPhaseLen - tx_num >= U32BLOCKSIZE) {
			temp = SPIC_DR_DEPTH - GET_TXFLR(spi_flash->TXFLR);
			temp = MIN(DataPhaseLen - tx_num, temp) / U32BLOCKSIZE;
			for (idx = 0; idx < temp; idx++) {
				spi_flash->DR[0].WORD = aligned_buf[idx];
			}
			aligned_buf += idx;
			tx_num += idx * U32BLOCKSIZE;
		}

		while (tx_num < DataPhaseLen) {
			if (spi_flash->SR & BIT_TFNF) {
				spi_flash->DR[0].BYTE = pData[tx_num];
				tx_num += 1;
			}
		}
	}
	/* Wait transfer complete */
	while (spi_flash->SR & BIT_BUSY);

	/* Restore USER_LENGTH */
	spi_flash->USER_LENGTH = addr_length;

	/* Call FLASH_WaitBusy(WAIT_FLASH_BUSY) after this function:
		1) wait flash busy done (wip=0)
		2) restore SPIC to auto mode */
}

/**
* @brief  This function is used to send Flash Instruction with Data Phase.
* @param  cmd: The command need to be sent.
* @param  DataPhaseLen: The length of Data phase.
* @param  pData: pointer to byte array which are to be sent in Data phase.
* @note
*		- This function works in OneBitMode and User mode.
*		- This function can only be used to tx cmd(WREN,WRSR,DP,RDP,ERASE etc.),not used to tx data.
* @retval none
*/
FLASH_RAM_TEXT_SECTION
void FLASH_TxCmd_InUserMode(u8 cmd, u8 DataPhaseLen, u8 *pData)
{
	SPIC_TypeDef *spi_flash = SPIC;
	u32 ctrl0;

	/* Caller should Enter user mode first */

	/* backup bitmode & addr_len */
	ctrl0 = spi_flash->CTRLR0;

	/* set CTRLR0: TX mode and one bit mode */
	spi_flash->CTRLR0 = ctrl0 & ~(TMOD(3) | CMD_CH(3) | ADDR_CH(3) | DATA_CH(3));

	/* start transfer data */
	FLASH_TxBasic_InUserMode(cmd, FALSE, NULL, DataPhaseLen, pData);

	/* Restore bitmode */
	spi_flash->CTRLR0 = ctrl0;

	/* Call FLASH_WaitBusy(WAIT_FLASH_BUSY) after this function:
		1) wait flash busy done (wip=0)
		2) restore SPIC to auto mode */
}


/**
  * @brief  This function is used to make flash enter/release from deep power down mode.
  * @param NewState: ENABLE or DISABLE
  * @retval none
  * @note Release from power-down will take the time duration of tRES1 before the device will resume normal
  *    operation and other instructions are accepted. The value of tRES1 is as following:
  *    Winbond: 3us, MXIC: 8.8/20/30/100 us, Micron: 30us, GD: 20/30 us
  */
__weak FLASH_RAM_TEXT_SECTION
void FLASH_DeepPowerDown(u32 NewState)
{
	if (NewState == ENABLE) {
		if (!SYSCFG_OTP_FlashDSleepEn()) {
			/* control by efuse to avoid bug */
			return;
		}

		FLASH_UserMode_Enter();
		DelayUs(100);
		/* Set flash_cmd: WREN to FIFO */
		FLASH_TxCmd_InUserMode(FLASH_CMD_WREN, 0, 0);
		FLASH_TxCmd_InUserMode(FLASH_CMD_DP, 0, 0);
		FLASH_UserMode_Exit();
	} else {
		FLASH_UserMode_Enter();
		FLASH_TxCmd_InUserMode(FLASH_CMD_RDP, 0, 0);
		DelayUs(100);
		FLASH_UserMode_Exit();
	}
}

