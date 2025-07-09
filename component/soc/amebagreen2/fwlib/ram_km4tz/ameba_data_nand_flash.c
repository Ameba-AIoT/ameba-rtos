/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "ameba_soc.h"
#include "ameba_spinand.h"
/*
  * @If the system designs for signal channel not support multi-channel connection,
  *    @SI connects to spi_sin[0] and SO connects to spi_sout[0].
  * @Else if supporting multi-channel communication,
  *    @bidirectional pins of {HOLDn, WPn, SO, SI} connect to input ports of spi_sin[3:0]
  *    @and output ports of spi_sout[3:0]
  */
static const char *TAG = "PSRAMC_NAND";

/**
* @brief  This function is used to switch between User mode and Auto mode.
* @param  enable: This parameter can be: ENABLE or DISABLE.
*			@arg ENABLE: Set SPIC to user mode
*			@arg DISABLE: Set SPIC to auto mode
* @retval none
*/
void DATA_FLASH_UserModeEn(u8 enable)
{
	SPIC_TypeDef *spi_flash = SPIC_COMBO;

	/* Wait spic busy done before switch mode */
	DATA_NAND_WaitBusy(WAIT_SPIC_BUSY);

	if (enable) {
		spi_flash->CTRLR0 |= BIT_USER_MODE;
	} else {
		spi_flash->CTRLR0 &= ~BIT_USER_MODE;
	}
}

void DATA_NAND_UserModeEn(u8 enable)
{
	return DATA_FLASH_UserModeEn(enable);
}

/**
* @brief  This function is used to send Rx command to flash to get status register or flash id.
* @param  cmd: command that need to be sent.
* @param  addr_len: The length of Addr phase.
* @param  paddr: pointer to a byte array that is to be sent.
* @param  read_len: the number of bytes that will be read by SPIC after sending addr.
* @param  read_data: pointer to a byte array which is used to save datas received.
* @note This function is only used for rx status/flash id ,not used for read flash data.
*		Only work in OneBitMode.
* @retval none
*/
void DATA_NAND_RxCmd(u8 cmd, u8 addr_len, u8 *paddr, u32 read_len, u8 *read_data)
{
	SPIC_TypeDef *spi_flash = SPIC_COMBO;
	u32 idx;
	u32 value;
	u32 offset = 0;
	u32 read_word;
	u16 read_half;
	u8 read_byte;

	/* Enter User Mode */
	DATA_FLASH_UserModeEn(ENABLE);

	//  RTK_LOGI(TAG, "[%s] RD_DUMMY_LENGTH1:%d \n", __func__, GET_USER_RD_DUMMY_LENGTH(spi_flash->USER_LENGTH));

	/* Set OneBit Mode */
	DATA_NAND_SetSpiMode(&data_flash_init_para, Spic1IOBitMode);

	//  RTK_LOGI(TAG, "[%s] RD_DUMMY_LENGTH2:%d \n", __func__, GET_USER_RD_DUMMY_LENGTH(spi_flash->USER_LENGTH));

	/*Get Feature CMD do not have dummy cycle after address phase*/
	if (cmd == data_flash_init_para.FLASH_cmd_rd_status) {
		spi_flash->USER_LENGTH &= ~MASK_USER_RD_DUMMY_LENGTH;
	}

	/* Set CMD phase: 1 byte and ADDR phase */
	value = spi_flash->USER_LENGTH & ~MASK_USER_ADDR_LENGTH;
	spi_flash->USER_LENGTH = value | USER_ADDR_LENGTH(addr_len);

	//  RTK_LOGI(TAG, "[%s] CMD:0x%02x \n", __func__, cmd);
	//  RTK_LOGI(TAG, "[%s] ADDR_LENGTH:%d \n", __func__, GET_USER_ADDR_LENGTH(spi_flash->USER_LENGTH));
	//  RTK_LOGI(TAG, "[%s] CMD_LENGHT:%d \n", __func__, GET_USER_CMD_LENGHT(spi_flash->USER_LENGTH));
	//  RTK_LOGI(TAG, "[%s] RD_DUMMY_LENGTH3:%d \n", __func__, GET_USER_RD_DUMMY_LENGTH(spi_flash->USER_LENGTH));
	/* Set RX_NDF: frame number of receiving data. TX_NDF should be set in both transmit mode and receive mode.
		TX_NDF should be set to zero in receive mode to skip the TX_DATA phase. */
	spi_flash->RX_NDF = RX_NDF(read_len);
	spi_flash->TX_NDF = 0;

	/* set CTRLR0: RX_mode and one bit mode for data & addr*/
	spi_flash->CTRLR0 = ((spi_flash->CTRLR0 & 0xFFF0FFFF) | TMOD(3));

	/* set flash_cmd: write cmd to fifo */
	spi_flash->DR[0].BYTE = cmd;

	/* fill addr */
	for (idx = 0; idx < addr_len; idx++) {
		spi_flash->DR[0].BYTE = paddr[idx];
	}

	/* Enable SSIENR to start the transfer */
	spi_flash->SSIENR = BIT_SPIC_EN;

	/* RL7005 PSRAMC enabled DQ16, DR FIFO read / write must at least 2-Byte once */
	//  RTK_LOGI(TAG, "[%s] offset:%d, read_len:%d \n", __func__, offset, read_len);
	while (offset < read_len) {
		if (spi_flash->SR & BIT_RFNE) {
			if ((read_len >= 4) && (offset <= read_len - 4)) {
				read_word = spi_flash->DR[0].WORD;
				//  RTK_LOGI(TAG, "read_word:%08x\n", read_word);
				__UNALIGNED_UINT32_WRITE((u32)read_data + offset, read_word);
				offset += 4;
			} else if ((read_len >= 2) && (offset <= read_len - 2)) {
				read_half = spi_flash->DR[0].HALF;
				//  RTK_LOGI(TAG, "read_half:%04x\n", read_half);
				__UNALIGNED_UINT16_WRITE((u32)read_data + offset, read_half);
				offset += 2;
			} else {
				read_byte = spi_flash->DR[0].BYTE;
				//  RTK_LOGI(TAG, "read_byte:%02x\n", read_byte);
				HAL_WRITE8((u32)read_data, offset, read_byte);
				offset += 1;
			}
		}
	}

	/* Wait transfer complete. When complete, SSIENR.SPIC_EN are cleared by HW automatically. */
	DATA_NAND_WaitBusy(WAIT_TRANS_COMPLETE);

	/* Recover */
	DATA_NAND_SetSpiMode(&data_flash_init_para, data_flash_init_para.FLASH_cur_bitmode);

	/* Exit user mode */
	DATA_FLASH_UserModeEn(DISABLE);
}

/**
* @brief  This function is used to read data from flash in User Mode.
* @param  cmd: Read data command.
* @param  StartAddr: Start address in flash from which SPIC wants to read.
* @param  read_len: the number of bytes that SPIC wants to read.
* @param  read_data: pointer to a byte array which is used to save received data.
* @retval none
*/
void DATA_NAND_RxData(u8 cmd, u32 StartAddr, u32 read_len, u8 *read_data)
{
	SPIC_TypeDef *spi_flash = SPIC_COMBO;
	u8 rd_cmd = cmd;
	u32 temp;
	u32 offset = 0;
	u32 read_word;
	u16 read_half;
	u8 read_byte;

	if (rd_cmd == 0) {
		rd_cmd = data_flash_init_para.FLASH_cur_cmd;
	}

	/* Enter User Mode */
	DATA_NAND_UserModeEn(ENABLE);

	/* set CTRLR0: RX_mode */
	spi_flash->CTRLR0 |= TMOD(3);

	/* Set RX_NDF: frame number of receiving data. TX_NDF should be set in both transmit mode and receive mode.
		TX_NDF should be set to zero in receive mode to skip the TX_DATA phase. */
	spi_flash->RX_NDF = RX_NDF(read_len);
	spi_flash->TX_NDF = 0;

	/* Set ADDR length, and dummy byte is set by USER_LENGTH.USER_RD_DUMMY_LENGTH*/
	temp = spi_flash->USER_LENGTH & ~MASK_USER_ADDR_LENGTH;
	spi_flash->USER_LENGTH = temp | USER_ADDR_LENGTH(data_flash_init_para.FLASH_addr_phase_len);

	/* Set target slave */
	spi_flash->SER = 1;

	/* set flash_cmd: write cmd & address to fifo & addr is MSB */
	spi_flash->DR[0].BYTE = rd_cmd;

	/*read from cache of GigaDevice Nand flash need dummy data after cmd phase.*/
	if (data_flash_init_para.FLASH_addr_phase_len == NAND_COL_ADDR_3_BYTE) {
		spi_flash->DR[0].BYTE = 0x0; 	/*put dummy data after cmd for GigaDevice*/
	}
	spi_flash->DR[0].BYTE = (u8)((StartAddr & 0xFF00) >> 8);
	spi_flash->DR[0].BYTE = (u8)(StartAddr & 0xFF);

	/* Enable SSIENR to start the transfer */
	spi_flash->SSIENR = BIT_SPIC_EN;

	/* RL7005 PSRAMC enabled DQ16, DR FIFO read / write must at least 2-Byte once */
	//  RTK_LOGI(TAG, "[%s] offset:%d, read_len:%d \n", __func__, offset, read_len);
	while (offset < read_len) {
		if (spi_flash->SR & BIT_RFNE) {
			if ((read_len >= 4) && (offset <= read_len - 4)) {
				read_word = spi_flash->DR[0].WORD;
				//  RTK_LOGI(TAG, "read_word:%08x\n", read_word);
				__UNALIGNED_UINT32_WRITE((u32)read_data + offset, read_word);
				offset += 4;
			} else if ((read_len >= 2) && (offset <= read_len - 2)) {
				read_half = spi_flash->DR[0].HALF;
				//  RTK_LOGI(TAG, "read_half:%04x\n", read_half);
				__UNALIGNED_UINT16_WRITE((u32)read_data + offset, read_half);
				offset += 2;
			} else {
				read_byte = spi_flash->DR[0].BYTE;
				//  RTK_LOGI(TAG, "read_byte:%02x\n", read_byte);
				HAL_WRITE8((u32)read_data, offset, read_byte);
				offset += 1;
			}
		}
	}

	/* Wait transfer complete. When complete, SSIENR.SPIC_EN are cleared by HW automatically. */
	DATA_NAND_WaitBusy(WAIT_TRANS_COMPLETE);

	/* Exit user mode */
	DATA_NAND_UserModeEn(DISABLE);
}


/**
* @brief  This function is used to wait until SPIC/Flash Busy done or WLE bit set.
* @param  WaitType:  wait type, which can be one of the following parameters:
*			@arg WAIT_SPIC_BUSY: wait until SPIC transmission done.
*			@arg WAIT_TRANS_COMPLETE: wait until user transfer done.
*			@arg WAIT_FLASH_BUSY: wait until Flash busy done.
*			@arg WAIT_WRITE_EN: wait until flash status WLE(Write Enbale Latch) bit is set.
* @retval  zero when waiting status of SPIC, or status of Flash if not timeout, otherwise the retval is 0xFF
* @note
*		-if nand is still busy when polling count is 0x200000, timeout error occured.
*			the max frequency of SPI is 100MHz, Get Status use 3Byte * 10ns = 240ns, and 0x200000 *240ns is 500ms
*			500ms is enough to wait Nand return status(Linux driver TO time is 400ms, and CPU running time is not count)
*			Contact with FAE of Dosilicon, beyond the max read/program/erase time, they will return error info to SPIC
*/
u8 DATA_NAND_WaitBusy(u32 WaitType)
{
	SPIC_TypeDef *spi_flash = SPIC_COMBO;
	u32 BusyCheck = 0;
	u8 status = 0;
	u32 i = 0;

	do {
		if (WaitType == WAIT_SPIC_BUSY) {
			BusyCheck = (spi_flash->SR & BIT_BUSY);

		} else if (WaitType == WAIT_TRANS_COMPLETE) {
			/* When transfer completes, SSIENR.SPIC_EN are cleared by HW automatically. */
			BusyCheck = (spi_flash->SSIENR & BIT_SPIC_EN);

		} else if (WaitType == WAIT_FLASH_BUSY) {
			i++;
			status = DATA_NAND_GetStatus(NAND_REG_STATUS);

			BusyCheck = (status & data_flash_init_para.FLASH_Busy_bit);
		} else if (WaitType == WAIT_WRITE_EN) {
			i++;
			status = DATA_NAND_GetStatus(NAND_REG_STATUS);

			/* write enable ok & flash not busy */
			BusyCheck = (!(status & data_flash_init_para.FLASH_WLE_bit)) || (status & data_flash_init_para.FLASH_Busy_bit);
		}

		if (!BusyCheck) {
			break;
		}

		if (i > 0x200000) {
			status = 0xFF;
			RTK_LOGS(TAG, RTK_LOG_ERROR, "TIME OUT\n");
			break;
		}
	} while (1);
	return status;
}

/**
* @brief  This function is used to send WREN(Write Enable) cmd to set WLE bit in flash status register.
* @param  none
* @note   This function must be used prior to every Program execute,Block Erase, OTP program/OTP protection.
* @retval none
*/
void DATA_NAND_WriteEn(void)
{
	/* Wait for flash busy done */
	DATA_NAND_WaitBusy(WAIT_FLASH_BUSY);

	/* Set flash_cmd: WREN to FIFO */
	DATA_NAND_TxCmd(data_flash_init_para.FLASH_cmd_wr_en, 0, NULL, 0, NULL);

	/* Wait for write enable done*/
	DATA_NAND_WaitBusy(WAIT_WRITE_EN);
}

/**
  * @brief  This function is used to transfer data to flash in OneBitMode and User Mode.
  * @param	cmd: The command need to be sent.
  * @param  AddrLen: The length of Addr phase.
  * @param  pAddr: pointer to a byte array that is to be sent.
  * @param  DataLen: the number of bytes that SPIC sends in Data Phase.
  * @param  pData: pointer to a byte array that is to be sent.
  * @note
  *		- RXI312_SPIC Tx data frames number can be 0~2^24-1 bytes.
  *			TX_NDF can prevent TX FIFO underflow, when TX FIFO empty and Tx num<TX_NDF, clock
  *			would be gated and CS keep low.
  *			TX_NDF can't prevent TX FIFO overflow, so driver should check SR.TFNF before push data to FIFO.
  * @retval none
  */
void DATA_NAND_TxCmd(u8 cmd, u8 AddrLen, u8 *pAddr, u32 DataLen, u8 *pData)
{
	SPIC_TypeDef *spi_flash = SPIC_COMBO;
	u32 tx_num = 0;
	u32 addr_length = spi_flash->USER_LENGTH;
	u32 ctrl0 = spi_flash->CTRLR0;
	u32 value;

	/* Enter user mode */
	DATA_FLASH_UserModeEn(ENABLE);

	/* set CTRLR0: TX mode and one bit mode */
	spi_flash->CTRLR0 &= ~(TMOD(3) | CMD_CH(3) | ADDR_CH(3) | DATA_CH(3));

	/* Set ADDR length */
	value = spi_flash->USER_LENGTH & ~MASK_USER_ADDR_LENGTH;
	spi_flash->USER_LENGTH = value | USER_ADDR_LENGTH(AddrLen);

	/* Set TX_NDF: frame number of Tx data. */
	spi_flash->RX_NDF = 0;
	spi_flash->TX_NDF = TX_NDF(DataLen);

	/* set flash_cmd: write cmd & address to fifo & addr is MSB */
	spi_flash->DR[0].BYTE = cmd;

	for (tx_num = 0; tx_num < AddrLen; tx_num++) {
		spi_flash->DR[0].BYTE = pAddr[tx_num];
	}

	/* we can not set ssienr=1 when FIFO empty */
	/* Pre-load data before enabling, but there are just 16 - 4 = 12 bytes fifo afer cmd+addr */
	tx_num = 0;
	while (tx_num < DataLen) {
		if (spi_flash->SR & BIT_TFNF) {
			spi_flash->DR[0].BYTE = pData[tx_num];
			tx_num += 1;
		} else {
			/* Here mean Transmit FIFO is Full. */
			break;
		}
	}

	/* Enable SSIENR to start the transfer */
	spi_flash->SSIENR = BIT_SPIC_EN;

	/* Put the remain data into transmit FIFO */
	while (tx_num < DataLen) {
		if (spi_flash->SR & BIT_TFNF) {
			spi_flash->DR[0].BYTE = pData[tx_num];
			tx_num += 1;
		}
	}

	/* Wait transfer complete. When complete, SSIENR.SPIC_EN are cleared by HW automatically. */
	DATA_NAND_WaitBusy(WAIT_TRANS_COMPLETE);

	/* Restore bitmode & addr_len */
	spi_flash->USER_LENGTH = addr_length;
	spi_flash->CTRLR0 = ctrl0;
}

/**
  * @brief  This function is used to transfer data to flash in OneBitMode and User Mode.
  * @param	cmd: The command need to be sent.
  * @param  StartAddr: Start address in a single page.
  * @param  ByteLen: the number of bytes that SPIC sends in Data Phase.
  * @param  pData: pointer to a byte array that is to be sent.
  * @note
  *		- RXI312_SPIC Tx data frames number can be 0~2^24-1 bytes.
  *			TX_NDF can prevent TX FIFO underflow, when TX FIFO empty and Tx num<TX_NDF, clock
  *			would be gated and CS keep low.
  *			TX_NDF can't prevent TX FIFO overflow, so driver should check SR.TFNF before push data to FIFO.
  * @retval none
  */
void DATA_NAND_TxData(u8 cmd, u32 StartAddr, u32 ByteLen, u8 *pData)
{
	SPIC_TypeDef *spi_flash = SPIC_COMBO;
	u32 tx_num = 0;
	u32 addr_length = spi_flash->USER_LENGTH;
	u32 ctrl0 = spi_flash->CTRLR0;
	u32 temp, idx;
	u32 *aligned_buf;

	/* Enter user mode */
	DATA_NAND_UserModeEn(ENABLE);

	/* set CTRLR0: TX mode and one bit mode */
	spi_flash->CTRLR0 &= ~(TMOD(3) | CMD_CH(3) | ADDR_CH(3) | DATA_CH(3));

	/* if cur bitmode is 4O/4IO, and cmd is X4 Data Xfer, use 1-1-4 mode, otherwise use 1-1-1 mode to xfer data */
	if ((cmd == NAND_CMD_QPP) || (cmd == NAND_CMD_QPP_RANDOM)) {
		spi_flash->CTRLR0 |= DATA_CH(2);
	}

	/* Set ADDR length */
	temp = spi_flash->USER_LENGTH & ~MASK_USER_ADDR_LENGTH;
	spi_flash->USER_LENGTH = temp | USER_ADDR_LENGTH(2);

	/* Set TX_NDF: frame number of Tx data. */
	spi_flash->RX_NDF = 0;
	spi_flash->TX_NDF = TX_NDF(ByteLen);

	/* set flash_cmd: write cmd & address to fifo & addr is MSB */
	spi_flash->DR[0].BYTE = cmd;
	spi_flash->DR[0].BYTE = (u8)((StartAddr & 0xFF00) >> 8);
	spi_flash->DR[0].BYTE = (u8)(StartAddr & 0xFF);


	/* we can not set ssienr=1 when FIFO empty */
	/* Pre-load data before enabling, but there are just 16 - 4 = 12 bytes fifo afer cmd+addr */
	tx_num = 0;

	temp = UNALIGNED32(pData);
	if (temp) { /* if buf is not aligned */
		while (tx_num < ByteLen) {
			spi_flash->DR[0].BYTE = pData[tx_num];
			tx_num += 1;
			if (tx_num == U32BLOCKSIZE - temp) {
				break;
			}
		}
	}

	/* Enable SSIENR to start the transfer */
	spi_flash->SSIENR = BIT_SPIC_EN;

	/* Put the remain data into transmit FIFO */
	aligned_buf = (u32 *)&pData[tx_num];
	while (tx_num < ByteLen) {
		temp = SPIC_DR_DEPTH - GET_TXFLR(spi_flash->TXFLR);
		temp = MIN(ByteLen - tx_num, temp) / U32BLOCKSIZE;
		for (idx = 0; idx < temp; idx++) {
			spi_flash->DR[0].WORD = aligned_buf[idx];
		}
		aligned_buf += idx;
		tx_num += idx * U32BLOCKSIZE;

		if (ByteLen - tx_num < U32BLOCKSIZE) {
			break;
		}
	}

	while (tx_num < ByteLen) {
		if (spi_flash->SR & BIT_TFNF) {
			spi_flash->DR[0].BYTE = pData[tx_num];
			tx_num += 1;
		}
	}

	/* Wait transfer complete. When complete, SSIENR.SPIC_EN are cleared by HW automatically. */
	DATA_NAND_WaitBusy(WAIT_TRANS_COMPLETE);

	/* Restore bitmode & addr_len */
	spi_flash->USER_LENGTH = addr_length;
	spi_flash->CTRLR0 = ctrl0;
}

/**
  * @brief  FLASH_GetStatus used to get register status.
  * @param    FeatureID: the address of status register
  * @retval     the value of the status register
  */
u8 DATA_NAND_GetStatus(u8 FeatureID)
{
	u8 status;
	DATA_NAND_RxCmd(data_flash_init_para.FLASH_cmd_rd_status, 1, &FeatureID, 1, &status);
	return status;
}

/**
  * @brief  FLASH_SetStatus used to set register status. DATA_NAND_WaitBusy
  *  		are included in this function to avoid hardfault when TxCmd in XIP
  * @param    FeatureID: the address of status register
  * @param    Status: pointer to byte array to be sent
  * @retval     none
  */
void DATA_NAND_SetStatus(u8 FeatureID, u8 Status)
{
	DATA_NAND_TxCmd(data_flash_init_para.FLASH_cmd_wr_status, 1, &FeatureID, 1, &Status);
	DATA_NAND_WaitBusy(WAIT_FLASH_BUSY);
}

/**
  * @brief  FLASH_SetStatusBits set or clear status bits., used to set protect bit or quad enable bit
  * @param    FeatureID: the address of status register
  * @param    SetBits: bit mask in the status register
  * @param    NewState: ENABLE/DISABLE
  * @retval none
  */
void DATA_NAND_SetStatusBits(u8 FeatureID, u8 SetBits, u8 NewState)
{
	u8 status = DATA_NAND_GetStatus(FeatureID);
	u8 status_mask = (u8)(SetBits & 0xFF);

	if (NewState == ENABLE) {
		status |=  status_mask;
	} else {
		status &=  ~status_mask;
	}

	DATA_NAND_SetStatus(FeatureID, status);
}

/**
  * @brief  This function is used to erase flash. EraseBlock: Erase specified block(128KB)
  * @param    PageAddr: should 4 byte align. The block which the address in will be erased.
  * @retval status: Success:0 or erase Fail: Others.(timeout is 0xFF)
  */
u8 DATA_NAND_Erase(u32 PageAddr)
{
	u8 Addr[3];
	u8 status;
	Addr[0] = (u8)((PageAddr >> 16) & 0xFF);
	Addr[1] = (u8)((PageAddr >> 8) & 0xFF);
	Addr[2] = (u8)(PageAddr & 0xFF);

	DATA_NAND_WriteEn();

	/* Block Erase Sequence is CMD + PA23-16 + PA15-8 + PA7-0. */
	DATA_NAND_TxCmd(data_flash_init_para.FLASH_cmd_block_e, 3, Addr, 0, NULL);

	/* polling WEL and busy done */
	status = DATA_NAND_WaitBusy(WAIT_FLASH_BUSY);

	if (status & data_flash_init_para.FLASH_EFail_bit) {
		return status;
	} else {
		return 0;
	}
}

/**
  * @brief  This function is used to read data from nand array to Nand Cache.
  * @param    PageAddr: which page will be read
  * @retval status: NAND_STATUS_ECC_NO_BITFLIPS:0 or Need to check: Others.(timeout is 0xFF)
  * @note
  * 	 if status is 0 means NAND_STATUS_ECC_NO_BITFLIPS, ortherwise need confirm spi nand datasheet.
  */
u8 DATA_NAND_Page_Read_ArrayToCache(u32 PageAddr)
{
	u8 Addr[3];
	u8 status;
	Addr[0] = (u8)((PageAddr >> 16) & 0xFF);
	Addr[1] = (u8)((PageAddr >> 8) & 0xFF);
	Addr[2] = (u8)(PageAddr & 0xFF);

	/* Page read Sequence is CMD + PA23-16 + PA15-8 + PA7-0. */
	DATA_NAND_TxCmd(data_flash_init_para.FLASH_cmd_page_read, 3, Addr, 0, NULL);

	status = DATA_NAND_WaitBusy(WAIT_FLASH_BUSY);
	/*if status is 0 means NAND_STATUS_ECC_NO_BITFLIPS, ortherwise need confirm spi nand datasheet.*/
	return status;
}

/**
  * @brief  This function is used to read data from cache
  * @param    ByteAddr: the offset of the page
  * @param    ByteLen: the number of bytes to be read
  * @param	 pData: pointer to a byte array which is used to save received data.
  * @retval none
  */
void DATA_NAND_Page_Read_FromCache(u8 Cmd, u32 ByteAddr, u32 ByteLen, u8 *pData)
{
	DATA_NAND_RxData(Cmd, ByteAddr, ByteLen, pData);
}

/**
  * @brief  This function is used to read data from page
  * @param    PageAddr: which page will be read
  * @param    ByteAddr: the offset of the page
  * @param    ByteLen: the number of bytes to be read
  * @param	 pData: pointer to a byte array which is used to save received data.
  * @retval status: NAND_STATUS_ECC_NO_BITFLIPS:0 or Need to check: Others.(timeout is 0xFF)
  * @note
  * 	 if status is 0 means NAND_STATUS_ECC_NO_BITFLIPS, ortherwise need confirm spi nand datasheet.
  */
u8 DATA_NAND_Page_Read(u32 PageAddr, u32 ByteAddr, u32 ByteLen, u8 *pData)
{
	u8 status = DATA_NAND_Page_Read_ArrayToCache(PageAddr);

	DATA_NAND_Page_Read_FromCache(data_flash_init_para.FLASH_cur_cmd, ByteAddr, ByteLen, pData);

	/*if status is 0 means NAND_STATUS_ECC_NO_BITFLIPS, ortherwise need confirm spi nand datasheet.*/
	return status;
}

/**
  * @brief  This function is used to Load data to Nand Cache
  * @param    cmd: The command need to be sent, Can be NAND_CMD_QPP_RANDOM/NAND_CMD_QPP/NAND_CMD_PP_RANDOM/NAND_CMD_PP
  * @param    ByteAddr: the offset of the page
  * @param    ByteLen: the number of bytes to be transfered
  * @param	 pData: pointer to a byte array that is to be sent.
  * @retval status: Success:1 or Program Fail: Others.
  * @note
  * 	  When cur bitmode is 4IO/4O, cmd can be NAND_CMD_QPP_RANDOM/NAND_CMD_QPP/NAND_CMD_PP_RANDOM/NAND_CMD_PP,
  *				or cmd shall be NAND_CMD_PP_RANDOM/NAND_CMD_PP
  */
void DATA_NAND_Page_Write_Data_Xfer(u8 Cmd, u32 ByteAddr, u32 ByteLen, u8 *pData)
{
	/*WEL bit shall be set, ortherwise Nand will ignore this cmd*/
	DATA_NAND_WriteEn();
	DATA_NAND_TxData(Cmd, ByteAddr, ByteLen, pData);
}

/**
  * @brief  This function is used to program Page
  * @param    PageAddr: which page will be program
  * @retval status: Success:0 or Program Fail: Others.(timeout is 0xFF)
  */
u8 DATA_NAND_Page_Write_Program_Execute(u32 PageAddr)
{
	u8 Addr[3];
	u8 status;
	Addr[0] = (u8)((PageAddr >> 16) & 0xFF);
	Addr[1] = (u8)((PageAddr >> 8) & 0xFF);
	Addr[2] = (u8)(PageAddr & 0xFF);

	/* Program Execution Sequence is CMD + PA23-16 + PA15-8 + PA7-0. */
	DATA_NAND_TxCmd(data_flash_init_para.FLASH_cmd_page_write, 3, Addr, 0, NULL);

	/* polling WEL and busy done */
	status = DATA_NAND_WaitBusy(WAIT_FLASH_BUSY);

	if (status & data_flash_init_para.FLASH_PFail_bit) {
		return status;
	} else {
		return 0;
	}
}

/**
  * @brief  This function is used to program Page
  * @param    PageAddr: which page will be program
  * @param    ByteAddr: the offset of the page
  * @param    ByteLen: the number of bytes to be programed
  * @param	 pData: pointer to a byte array that is to be sent.
  * @retval status: Success:0 or Program Fail: Others.(timeout is 0xFF)
  * @note
  * 	  this function ues program load X4(reset buffer) to transfer data if bitmode is 4IO/4O
  * 	  if you want to update paitial area of nand cache please call DATA_NAND_Page_Write_Data_Xfer() with
  *			Program load ramdom data command and DATA_NAND_Page_Write_Program_Execute
  */
u8 DATA_NAND_Page_Write(u32 PageAddr, u32 ByteAddr, u32 ByteLen, u8 *pData)
{
	u8 status;

	DATA_NAND_WriteEn();
	/* Use program laod X4 (reset buffer) commond if cur bitmode is 4IO/4O */
	if (data_flash_init_para.FLASH_cur_bitmode >= Spic4OBitMode) {
		DATA_NAND_TxData(NAND_CMD_QPP, ByteAddr, ByteLen, pData);
	} else {
		DATA_NAND_TxData(NAND_CMD_PP, ByteAddr, ByteLen, pData);
	}

	status = DATA_NAND_Page_Write_Program_Execute(PageAddr);

	return status;
}

/**
  * @brief  FLASH_GetPageSize used to get nand flash page size in bytes.
  * @retval valid return value only 0x800 (2KBytes) and 0x1000 (4KBytes), other value is invalid.
  */
int DATA_NAND_GetPageSize(u32 *page_size_byte)
{
	u8 NAND_MFG_GIGADEVICE = 0xC8;
	u8 NF_PARAMETER_PAGE_ADDR = 0x01;
	u8 NF_PARAMETER_PAGE_GIGADEVICE_ADDR = 0x04;
	u32 NF_PARAMETER_PAGE_OFFSET_BYTES_PER_PAGE = 80;

	u8 mid, did, reg, bk_reg;
	u8 ret = RTK_FAIL;
	u8 flash_ID[4] = {0};
	u32 addr;

	/* 1. get manufacture ID and device ID */
	DATA_NAND_RxCmd(NAND_CMD_RDID, 0, NULL, 4, flash_ID);
	mid = flash_ID[0];
	did = flash_ID[1];

	//  RTK_LOGI(TAG, "%02x, %02x, %02x, %02x\n", flash_ID[0], flash_ID[1], flash_ID[2], flash_ID[3]);
	/* 2. backup current status */
	reg = DATA_NAND_GetStatus(NAND_REG_CFG);
	//  RTK_LOGI(TAG, "reg:%02x\n", reg);
	bk_reg = reg;

	/* 3. enable OTP, disable ECC */
	reg |= NAND_CFG_OTP_ENABLE;
	reg &= ~NAND_CFG_ECC_ENABLE;
	DATA_NAND_SetStatus(NAND_REG_CFG, reg);
	if (reg != DATA_NAND_GetStatus(NAND_REG_CFG)) {
		//  RTK_LOGI(TAG, "DATA_NAND_SetStatus fail\n");
		goto exit;
	}

	/* 4. send read parameter page cmd */
	if ((mid == NAND_MFG_GIGADEVICE) &&
		((did == 0x21) || (did == 0x31) || (did == 0x41) || (did == 0x51) ||
		 (did == 0x22) || (did == 0x32) || (did == 0x42) || (did == 0x52) ||
		 (did == 0x25) || (did == 0x35) || (did == 0x45) || (did == 0x55))) {
		addr = NF_PARAMETER_PAGE_GIGADEVICE_ADDR;
	} else {
		addr = NF_PARAMETER_PAGE_ADDR;
	}

	ret = DATA_NAND_Page_Read(addr, NF_PARAMETER_PAGE_OFFSET_BYTES_PER_PAGE, 4, (u8 *)(page_size_byte));
	//  RTK_LOGI(TAG, "[%s] page_size_byte:%x\n", __func__, *page_size_byte);
	if (*page_size_byte == 0x800 || *page_size_byte == 0x1000) {
		ret = RTK_SUCCESS;
	} else {
		ret = RTK_FAIL;
	}

exit:
	DATA_NAND_SetStatus(NAND_REG_CFG, bk_reg);
	return ret;
}

/**
 * @brief  check the given block_id is bad block or not.
 * @param  block_id:
 * @retval
 */
u32 DATA_NAND_CheckBadBlock(u32 block_id)
{
	u8 oob[4];

	/*bad block: the first byte of spare area of the fisrt page of block is not 0xFF*/
	DATA_NAND_Page_Read(NAND_BLOCK_ID_TO_PAGE_ADDR(block_id), NAND_PAGE_SIZE_MAIN, 4, oob);
	if ((oob[0] != 0xFF) || (oob[1] != 0xFF)) {
		RTK_LOGS(NOTAG, RTK_LOG_ERROR, "Blk 0x%x is BB!\n", block_id);
		return TRUE;
	}
	return FALSE;
}

/**
  * @brief  This function is used to configure SPIC according to parameters in FLASH_InitStruct.
  * @param FLASH_InitStruct: pointer to a FLASH_InitTypeDef
  *         structure that contains the configuration information for SPIC.
  * @retval none
  */
void DATA_SPIC_Config(FLASH_InitTypeDef *FLASH_InitStruct)
{
	SPIC_Config(SPIC_COMBO, FLASH_InitStruct);
}

/**
* @brief  This function is used to set SPI mode.
* @param FLASH_InitStruct: pointer to a FLASH_InitTypeDef
*         structure that contains the configuration information for SPIC.
* @param SpicBitMode: SPI mode to be configured.
*		This parameter can be one of the following parameters:
*		@arg Spic1IOBitMode
*		@arg Spic2OBitMode
*		@arg Spic2IOBitMode
*		@arg Spic4OBitMode
*		@arg Spic4IOBitMode
* @retval none
*/
void DATA_NAND_SetSpiMode(FLASH_InitTypeDef *FLASH_InitStruct, u8 SpicBitMode)
{
	NAND_SetSpiMode(SPIC_COMBO, FLASH_InitStruct, SpicBitMode);
}

void DATA_NAND_StructInit_Micron(FLASH_InitTypeDef *FLASH_InitStruct)
{
	NAND_StructInit_Micron(FLASH_InitStruct);
}

void DATA_NAND_StructInit_MXIC(FLASH_InitTypeDef *FLASH_InitStruct)
{
	NAND_StructInit_MXIC(FLASH_InitStruct);
}

void DATA_NAND_StructInit_GD(FLASH_InitTypeDef *FLASH_InitStruct)
{
	NAND_StructInit_GD(FLASH_InitStruct);
}

void DATA_NAND_StructInit(FLASH_InitTypeDef *FLASH_InitStruct)
{
	NAND_StructInit(FLASH_InitStruct);
}

/**
* @brief  Init SPIC in specified SPI mode
* @param SpicBitMode: SPI mode to be configured.
*		This parameter can be one of the following parameters:
*		@arg Spic1IOBitMode
*		@arg Spic2OBitMode
*		@arg Spic2IOBitMode
*		@arg Spic4OBitMode
*		@arg Spic4IOBitMode
* @retval 1
*/
u8 DATA_NAND_Init(u8 SpicBitMode)
{
	/* config fixed parameters */
	DATA_SPIC_Config(&data_flash_init_para);

	/* Set SPI Mode */
	data_flash_init_para.FLASH_cur_bitmode = SpicBitMode;
	DATA_NAND_SetSpiMode(&data_flash_init_para, SpicBitMode);

	return TRUE;
}

void data_nand_get_vendor(void)
{
	u8 flash_ID[4];

	/* Read flash ID */
	DATA_NAND_RxCmd(data_flash_init_para.FLASH_cmd_rd_id, 0, NULL, 2, flash_ID);
	RTK_LOGI(TAG, "DATA Flash NAND ID: %x-%x\n", flash_ID[0], flash_ID[1]);

	/* Get flash chip information */
	FlashInfo_TypeDef *current_IC = data_flash_get_chip_info((flash_ID[2] << 16) | (flash_ID[1] << 8) | flash_ID[0]);
	if (current_IC == NULL) {
		RTK_LOGW(TAG, "This flash type is not supported!\n");
		assert_param(0);
	}

	/* Re-initialize flash init structure according to classification */
	switch (current_IC->flash_class) {
	case FlashClass1:
		DATA_NAND_StructInit(&data_flash_init_para);
		break;
	case FlashClass2:
		DATA_NAND_StructInit_GD(&data_flash_init_para);
		break;
	case FlashClass3:
		DATA_NAND_StructInit_MXIC(&data_flash_init_para);
		break;
	case FlashClass5:
		DATA_NAND_StructInit_Micron(&data_flash_init_para);
		break;
	case FlashClassUser:
		assert_param(current_IC->FlashInitHandler != NULL);
		current_IC->FlashInitHandler();
		break;
	default:
		break;
	}
}

/******************* (C) COPYRIGHT 2020 Realtek Semiconductor *****END OF FILE****/



