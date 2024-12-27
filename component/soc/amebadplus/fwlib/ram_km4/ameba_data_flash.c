/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "ameba_soc.h"

FLASH_InitTypeDef data_flash_init_para;

/**
* @brief  This function is used to enter User mode.
* @param none
* @retval none
*/
void DATA_FLASH_UserMode_Enter(void)
{
	SPIC_TypeDef *ComboSpic = SPIC_COMBO;
	ComboSpic->CTRLR0 |= BIT_USER_MODE;

	/* user mode is entered after auto cmd is done, which means if SPIC BIT_BUSY=1, HW will not write BIT_USER_MODE */
	while (!(ComboSpic->CTRLR0 & BIT_USER_MODE));
}

/**
* @brief  This function is used to Exit User mode.
* @param none
* @retval none
*/
void DATA_FLASH_UserMode_Exit(void)
{
	SPIC_TypeDef *ComboSpic = SPIC_COMBO;
	ComboSpic->CTRLR0 &= ~BIT_USER_MODE;
}

/**
* @brief  This function is used to send Rx command to flash to get status register or flash id.
* @param  cmd: command that need to be sent.
* @param  read_len: the number of bytes that will be read by SPIC after sending cmd.
* @param  read_data: pointer to a byte array which is used to save datas received.
* @note This function is only used for rx status/flash id ,not used for read flash data.
*		Only work in OneBitMode.
* @retval none
*/
void DATA_FLASH_RxCmd_InUserMode(u8 cmd, u32 read_len, u8 *read_data)
{
	SPIC_TypeDef *ComboSpic = SPIC_COMBO;
	FLASH_InitTypeDef *FLASH_InitStruct = &data_flash_init_para;
	u32 rx_num = 0;
	u32 value;

	/* Caller should Enter user mode first */

	/* Set OneBit Mode */
	DATA_FLASH_SetSpiMode(ComboSpic, FLASH_InitStruct, Spic1IOBitMode);

	/* Set CMD phase: 1 byte, ADDR phase: 0 byte */
	value = ComboSpic->USER_LENGTH & ~MASK_USER_ADDR_LENGTH;
	ComboSpic->USER_LENGTH = value | USER_ADDR_LENGTH(0);

	/* Set RX_NDF: frame number of receiving data. TX_NDF should be set in both transmit mode and receive mode.
		TX_NDF should be set to zero in receive mode to skip the TX_DATA phase. */
	ComboSpic->RX_NDF = RX_NDF(read_len);
	ComboSpic->TX_NDF = 0;

	/* set CTRLR0: RX_mode and one bit mode for data & addr*/
	ComboSpic->CTRLR0 = ((ComboSpic->CTRLR0 & 0xFFF0FFFF) | TMOD(3));

	/* set flash_cmd: write cmd to fifo */
	ComboSpic->DR[0].BYTE = cmd;

	/* Enable SSIENR to start the transfer */
	ComboSpic->SSIENR = BIT_SPIC_EN;

	/* read data */
	rx_num = 0;
	while (rx_num < read_len) {
		if (ComboSpic->SR & BIT_RFNE) {
			read_data[rx_num] = ComboSpic->DR[0].BYTE;
			rx_num += 1;
		}
	}

	/* Wait transfer complete. When complete, SSIENR.SPIC_EN are cleared by HW automatically. */
	DATA_FLASH_WaitBusy_InUserMode(WAIT_TRANS_COMPLETE);

	/* Recover */
	DATA_FLASH_SetSpiMode(ComboSpic, FLASH_InitStruct, FLASH_InitStruct->FLASH_cur_bitmode);

	/* Caller decide to Exit user mode */
}

void DATA_FLASH_RxCmd(u8 cmd, u32 read_len, u8 *read_data)
{
	/* Do Tx in user mode firstly */
	DATA_FLASH_UserMode_Enter();

	DATA_FLASH_RxCmd_InUserMode(cmd, read_len, read_data);

	/* Exit user mode and restore SPIC to auto mode */
	DATA_FLASH_UserMode_Exit();
}

/**
* @brief  This function is used to read data from flash in User Mode.
* @param  cmd: Read data command.
* @param  StartAddr: Start address in flash from which SPIC wants to read.
* @param  read_len: the number of bytes that SPIC wants to read.
* @param  read_data: pointer to a byte array which is used to save received data.
* @retval none
*/
void DATA_FLASH_RxData(u8 cmd, u32 StartAddr, u32 read_len, u8 *read_data)
{
	SPIC_TypeDef *ComboSpic = SPIC_COMBO;
	FLASH_InitTypeDef *FLASH_InitStruct = &data_flash_init_para;
	u8 rd_cmd = cmd;
	u32 rx_num = 0;
	u32 value;

	if (rd_cmd == 0) {
		rd_cmd = FLASH_InitStruct->FLASH_cur_cmd;
	}

	/* Enter user mode firstly */
	DATA_FLASH_UserMode_Enter();

	/* set CTRLR0: RX_mode */
	ComboSpic->CTRLR0 |= TMOD(3);

	/* Set RX_NDF: frame number of receiving data. TX_NDF should be set in both transmit mode and receive mode.
		TX_NDF should be set to zero in receive mode to skip the TX_DATA phase. */
	ComboSpic->RX_NDF = RX_NDF(read_len);
	ComboSpic->TX_NDF = 0;

	/* Set ADDR length */
	value = ComboSpic->USER_LENGTH & ~MASK_USER_ADDR_LENGTH;
	if (FLASH_InitStruct->FLASH_dum_en) {
		value |= USER_ADDR_LENGTH(FLASH_InitStruct->FLASH_addr_phase_len + 1);
	} else {
		value |= USER_ADDR_LENGTH(FLASH_InitStruct->FLASH_addr_phase_len);
	}

	ComboSpic->USER_LENGTH = value;

	/* Set target slave */
	ComboSpic->SER = 1;

	/* set flash_cmd: write cmd & address to fifo & addr is MSB */
	ComboSpic->DR[0].BYTE = rd_cmd;
	if (FLASH_InitStruct->FLASH_addr_phase_len == ADDR_4_BYTE) {
		ComboSpic->DR[0].BYTE = (u8)((StartAddr & 0xFF000000) >> 24);
	}

	ComboSpic->DR[0].BYTE = (u8)((StartAddr & 0xFF0000) >> 16);
	ComboSpic->DR[0].BYTE = (u8)((StartAddr & 0xFF00) >> 8);
	ComboSpic->DR[0].BYTE = (u8)(StartAddr & 0xFF);

	/* Push dummy byte*/
	if (FLASH_InitStruct->FLASH_dum_en) {
		ComboSpic->DR[0].BYTE = 0x0;
	}

	/* Enable SSIENR to start the transfer */
	ComboSpic->SSIENR = BIT_SPIC_EN;

	rx_num = 0;
	while (rx_num < read_len) {
		if (ComboSpic->SR & BIT_RFNE) {
			read_data[rx_num] = ComboSpic->DR[0].BYTE;
			rx_num += 1;
		}
	}

	/* Wait transfer complete. When complete, SSIENR.SPIC_EN are cleared by HW automatically. */
	DATA_FLASH_WaitBusy_InUserMode(WAIT_TRANS_COMPLETE);

	/* Exit user mode and restore SPIC to auto mode */
	DATA_FLASH_UserMode_Exit();
}

/**
* @brief  This function is used to wait until SPIC/Flash Busy done or WLE bit set.
* @param  WaitType:  wait type, which can be one of the following parameters:
*			@arg WAIT_SPIC_BUSY: wait until SPIC transmission done.
*			@arg WAIT_TRANS_COMPLETE: wait until user transfer done.
*			@arg WAIT_FLASH_BUSY: wait until Flash busy done.
*			@arg WAIT_WRITE_EN: wait until flash status WLE(Write Enbale Latch) bit is set.
* @retval none
*/
void DATA_FLASH_WaitBusy_InUserMode(u32 WaitType)
{
	SPIC_TypeDef *ComboSpic = SPIC_COMBO;
	FLASH_InitTypeDef *FLASH_InitStruct = &data_flash_init_para;
	u32 BusyCheck = 0;
	u8 status = 0;

	do {
		if (WaitType == WAIT_SPIC_BUSY) {
			BusyCheck = (ComboSpic->SR & BIT_BUSY);

		} else if (WaitType == WAIT_TRANS_COMPLETE) {
			/* When transfer completes, SSIENR.SPIC_EN are cleared by HW automatically. */
			BusyCheck = (ComboSpic->SSIENR & BIT_SPIC_EN);

		} else if (WaitType == WAIT_FLASH_BUSY) {
			DATA_FLASH_RxCmd_InUserMode(FLASH_InitStruct->FLASH_cmd_rd_status, 1, &status);

			BusyCheck = (status & FLASH_InitStruct->FLASH_Busy_bit);
		} else if (WaitType == WAIT_WRITE_EN) {
			DATA_FLASH_RxCmd_InUserMode(FLASH_InitStruct->FLASH_cmd_rd_status, 1, &status);

			/* write enable ok & flash not busy */
			BusyCheck = (!(status & FLASH_InitStruct->FLASH_WLE_bit)) || (status & FLASH_InitStruct->FLASH_Busy_bit);
		}

		if (!BusyCheck) {
			break;
		}
	} while (1);
}

/**
* @brief  This function is used to send WREN(Write Enable) cmd to set WLE bit
*		in flash status register.
* @param  none
* @note This function must be used prior to every Page Program,
	Sector/Block/Chip Erase, Write Status Register instruction.
* @retval none
*/
void DATA_FLASH_WriteEn_InUserMode(void)
{
	/* Wait for flash busy done */
	DATA_FLASH_WaitBusy_InUserMode(WAIT_FLASH_BUSY);

	/* Set flash_cmd: WREN to FIFO */
	DATA_FLASH_TxCmd_InUserMode(data_flash_init_para.FLASH_cmd_wr_en, 0, 0);

	/* Wait for write enable done*/
	DATA_FLASH_WaitBusy_InUserMode(WAIT_WRITE_EN);
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
void DATA_FLASH_TxCmd_InUserMode(u8 cmd, u8 DataPhaseLen, u8 *pData)
{
	SPIC_TypeDef *ComboSpic = SPIC_COMBO;
	u32 tx_num = 0;
	u32 addr_length;
	u32 ctrl0;
	u32 value;

	/* Caller should Enter user mode first */

	/* backup bitmode & addr_len */
	addr_length = ComboSpic->USER_LENGTH;
	ctrl0 = ComboSpic->CTRLR0;

	/* set CTRLR0: TX mode and one bit mode */
	ComboSpic->CTRLR0 &= ~(TMOD(3) | CMD_CH(3) | ADDR_CH(3) | DATA_CH(3));

	/* Set ADDR length: 0 byte, skip ADDR phase */
	value = ComboSpic->USER_LENGTH & ~MASK_USER_ADDR_LENGTH;
	ComboSpic->USER_LENGTH = value | USER_ADDR_LENGTH(0);

	/* Set TX_NDF: frame number of Tx data. */
	ComboSpic->RX_NDF = 0;
	ComboSpic->TX_NDF = TX_NDF(DataPhaseLen);

	/* set flash_cmd: write cmd to fifo */
	ComboSpic->DR[0].BYTE = cmd;

	/* fill data */
	for (tx_num = 0; tx_num < DataPhaseLen; tx_num++) {
		ComboSpic->DR[0].BYTE = (u8) * (pData + tx_num);
	}

	/* Enable SSIENR to start the transfer */
	ComboSpic->SSIENR = BIT_SPIC_EN;

	/* Wait transfer complete. When complete, SSIENR.SPIC_EN are cleared by HW automatically. */
	DATA_FLASH_WaitBusy_InUserMode(WAIT_TRANS_COMPLETE);

	/* Restore bitmode & addr_len */
	ComboSpic->USER_LENGTH = addr_length;
	ComboSpic->CTRLR0 = ctrl0;

	/* Call FLASH_WaitBusy(WAIT_FLASH_BUSY) after this function:
		1) wait flash busy done (wip=0)
		2) restore SPIC to auto mode */

}

/**
  * @brief  This function is used to write data to flash in OneBitMode and User Mode.
  * @param  StartAddr: Start address in flash from which SPIC writes.
  * @param  DataPhaseLen: the number of bytes that SPIC sends in Data Phase.
  * @param  pData: pointer to a byte array that is to be sent.
  * @note
  *		- RXI312_SPIC Tx data frames number can be 0~2^24-1 bytes.
  *			TX_NDF can prevent TX FIFO underflow, when TX FIFO empty and Tx num<TX_NDF, clock
  *			would be gated and CS keep low.
  *			TX_NDF can't prevent TX FIFO overflow, so driver should check SR.TFNF before push data to FIFO.
  * @retval none
  */
void DATA_FLASH_TxData(u32 StartAddr, u32 DataPhaseLen, u8 *pData)
{
	SPIC_TypeDef *ComboSpic = SPIC_COMBO;
	FLASH_InitTypeDef *FLASH_InitStruct = &data_flash_init_para;
	u32 tx_num = 0;
	u32 ctrl0;
	u32 value;

	assert_param(((StartAddr & 0xFF) + DataPhaseLen) <= 0x100); /* addr should page(256) align */

	/* Do Tx in user mode firstly */
	DATA_FLASH_UserMode_Enter();

	/* write enable cmd */
	DATA_FLASH_WriteEn_InUserMode();

	/* set CTRLR0: TX mode and one bit mode */
	ctrl0 = ComboSpic->CTRLR0;
	ComboSpic->CTRLR0 &= ~(TMOD(3) | CMD_CH(3) | ADDR_CH(3) | DATA_CH(3));

	/* Set ADDR length */
	value = ComboSpic->USER_LENGTH & ~MASK_USER_ADDR_LENGTH;
	ComboSpic->USER_LENGTH = value | USER_ADDR_LENGTH(FLASH_InitStruct->FLASH_addr_phase_len);

	/* Set TX_NDF: frame number of Tx data. */
	ComboSpic->RX_NDF = 0;
	ComboSpic->TX_NDF = TX_NDF(DataPhaseLen);

	/* set flash_cmd: write cmd & address to fifo & addr is MSB */
	ComboSpic->DR[0].BYTE = FLASH_CMD_PP;
	if (FLASH_InitStruct->FLASH_addr_phase_len == ADDR_4_BYTE) {
		ComboSpic->DR[0].BYTE = (u8)((StartAddr & 0xFF000000) >> 24);
	}

	ComboSpic->DR[0].BYTE = (u8)((StartAddr & 0xFF0000) >> 16);
	ComboSpic->DR[0].BYTE = (u8)((StartAddr & 0xFF00) >> 8);
	ComboSpic->DR[0].BYTE = (u8)(StartAddr & 0xFF);

	/* write some data to fill the fifo before set SSIENR. */
	tx_num = 0;
	while (tx_num < DataPhaseLen) {
		if (ComboSpic->SR & BIT_TFNF) {
			ComboSpic->DR[0].BYTE = (u8) * (pData + tx_num);
			tx_num += 1;
		} else {
			break;
		}
	}

	/* Enable SSIENR to start the transfer */
	ComboSpic->SSIENR = BIT_SPIC_EN;

	/* write the remaining data into fifo */
	while (tx_num < DataPhaseLen) {
		if (ComboSpic->SR & BIT_TFNF) {
			ComboSpic->DR[0].BYTE = (u8) * (pData + tx_num);
			tx_num += 1;
		}
	}

	/* Wait transfer complete. When complete, SSIENR.SPIC_EN are cleared by HW automatically. */
	DATA_FLASH_WaitBusy_InUserMode(WAIT_TRANS_COMPLETE);

	/* Restore bitmode */
	ComboSpic->CTRLR0 = ctrl0;

	DATA_FLASH_WaitBusy_InUserMode(WAIT_FLASH_BUSY);

	/* Exit user mode and restore SPIC to auto mode */
	DATA_FLASH_UserMode_Exit();
}

/**
  * @brief  DATA_FLASH_SetStatus used to set register status. FLASH_WriteEn & FLASH_WaitBusy
  *  		are included in this function to avoid hardfault when TxCmd in XIP
  * @param    Cmd: command to be sent
  * @param    Len: the number of bytes to be sent after Cmd
  * @param    Status: pointer to byte array to be sent
  * @retval     none
  */
void DATA_FLASH_SetStatus(u8 Cmd, u32 Len, u8 *Status)
{
	/* Do Tx in user mode firstly */
	DATA_FLASH_UserMode_Enter();

	DATA_FLASH_WriteEn_InUserMode();
	DATA_FLASH_TxCmd_InUserMode(Cmd, Len, Status);
	DATA_FLASH_WaitBusy_InUserMode(WAIT_FLASH_BUSY);

	/* Exit user mode and restore SPIC to auto mode */
	DATA_FLASH_UserMode_Exit();
}

/**
  * @brief  DATA_FLASH_SetStatusBits set or clear status bits., used to set protect bit or quad enable bit
  * @param    SetBits: 16bits valid, SetBits[7:0] is status1 & SetBits[15:8] is status2
  * @param    NewState: ENABLE/DISABLE
  * @retval none
  */
void DATA_FLASH_SetStatusBits(u32 SetBits, u32 NewState)
{
	u8 status[2];
	u8 status1_mask = (u8)(SetBits & 0xFF);
	u8 status2_mask = (u8)((SetBits >> 8) & 0xFF);
	u8 StatusLen = 1;
	FLASH_InitTypeDef *FLASH_InitStruct = &data_flash_init_para;

	/* Do Set in user mode firstly */
	DATA_FLASH_UserMode_Enter();

	/* status1 */
	DATA_FLASH_RxCmd_InUserMode(FLASH_InitStruct->FLASH_cmd_rd_status, 1, &status[0]);
	if (NewState == ENABLE) {
		status[0] |=  status1_mask;
	} else {
		status[0] &=  ~status1_mask;
	}

	/* check if status2 */
	if (FLASH_InitStruct->FLASH_Status2_exist) {
		StatusLen = 2;
		DATA_FLASH_RxCmd_InUserMode(FLASH_InitStruct->FLASH_cmd_rd_status2, 1, &status[1]);

		if (NewState == ENABLE) {
			status[1] |=  status2_mask;
		} else {
			status[1] &=  ~status2_mask;
		}
	}

	if (!FLASH_InitStruct->FLASH_cmd_wr_status2) {
		DATA_FLASH_WriteEn_InUserMode();
		DATA_FLASH_TxCmd_InUserMode(FLASH_InitStruct->FLASH_cmd_wr_status, StatusLen, status);
		DATA_FLASH_WaitBusy_InUserMode(WAIT_FLASH_BUSY);
	} else {
		DATA_FLASH_WriteEn_InUserMode();
		DATA_FLASH_TxCmd_InUserMode(FLASH_InitStruct->FLASH_cmd_wr_status, 1, &status[0]);
		DATA_FLASH_WaitBusy_InUserMode(WAIT_FLASH_BUSY);

		DATA_FLASH_WriteEn_InUserMode();
		DATA_FLASH_TxCmd_InUserMode(FLASH_InitStruct->FLASH_cmd_wr_status2, 1, &status[1]);
		DATA_FLASH_WaitBusy_InUserMode(WAIT_FLASH_BUSY);
	}

	/* Exit user mode and restore SPIC to auto mode */
	DATA_FLASH_UserMode_Exit();
}

/**
  * @brief  This function is used to erase flash.
  * @param EraseType: can be one of the following  parameters:
  		@arg EraseChip: Erase the whole chip.
  		@arg EraseBlock: Erase specified block(64KB)
  		@arg EraseSector: Erase specified sector(4KB)
  * @param    Address should 4 byte align.The block/sector which
  * 		the address in will be erased.
  * @retval none
  */
void DATA_FLASH_Erase(u32 EraseType, u32 Address)
{
	u8 Addr[4];
	u8 len = 0;
	FLASH_InitTypeDef *FLASH_InitStruct = &data_flash_init_para;

	assert_param(EraseType <= EraseSector); /* max len is 2 */

	if (FLASH_InitStruct->FLASH_addr_phase_len == ADDR_4_BYTE) {
		len = 4;
		Addr[0] = (Address >> 24) & 0xFF;
		Addr[1] = (Address >> 16) & 0xFF;
		Addr[2] = (Address >> 8) & 0xFF;
		Addr[3] = Address & 0xFF;
	} else {
		len = 3;
		Addr[0] = (Address >> 16) & 0xFF;
		Addr[1] = (Address >> 8) & 0xFF;
		Addr[2] = Address & 0xFF;
	}

	/* Do Erase in user mode firstly */
	DATA_FLASH_UserMode_Enter();

	DATA_FLASH_WriteEn_InUserMode();

	if (EraseType == EraseChip) {
		DATA_FLASH_TxCmd_InUserMode(FLASH_InitStruct->FLASH_cmd_chip_e, 0, 0);
	} else if (EraseType == EraseBlock) {
		DATA_FLASH_TxCmd_InUserMode(FLASH_InitStruct->FLASH_cmd_block_e, len, Addr);
	} else if (EraseType == EraseSector) {
		DATA_FLASH_TxCmd_InUserMode(FLASH_InitStruct->FLASH_cmd_sector_e, len, Addr);
	}

	DATA_FLASH_WaitBusy_InUserMode(WAIT_FLASH_BUSY);

	/* Exit user mode and restore SPIC to auto mode */
	DATA_FLASH_UserMode_Exit();
}

void DATA_FLASH_Addr4ByteEn(void)
{
	DATA_FLASH_UserMode_Enter();
	DATA_FLASH_WriteEn_InUserMode();
	DATA_FLASH_TxCmd_InUserMode(FLASH_CMD_ENT_ADDR4B, 0, NULL);
	DATA_FLASH_WaitBusy_InUserMode(WAIT_FLASH_BUSY);
	DATA_FLASH_UserMode_Exit();
}

/**
  * @brief  This function is used to make flash enter/release from deep power down mode.
  * @param NewState: ENABLE or DISABLE
  * @retval none
  * @note Release from power-down will take the time duration of tRES1 before the device will resume normal
  *    operation and other instructions are accepted. The value of tRES1 is as following:
  *    Winbond: 3us, MXIC: 8.8/20/30/100 us, Micron: 30us, GD: 20/30 us
  */
void DATA_FLASH_DeepPowerDown(u32 NewState)
{
	FLASH_InitTypeDef *FLASH_InitStruct = &data_flash_init_para;

	if (NewState == ENABLE) {
		if (SYSCFG_OTP_FlashDSleepEn()) {
			DATA_FLASH_UserMode_Enter();
			DATA_FLASH_WriteEn_InUserMode();
			DATA_FLASH_TxCmd_InUserMode(FLASH_InitStruct->FLASH_cmd_pwdn, 0, 0);
			DATA_FLASH_UserMode_Exit();
		}
	} else {
		DATA_FLASH_UserMode_Enter();
		DATA_FLASH_TxCmd_InUserMode(FLASH_InitStruct->FLASH_cmd_pwdn_release, 0, 0);
		DelayUs(100);
		DATA_FLASH_WaitBusy_InUserMode(WAIT_FLASH_BUSY);
		DATA_FLASH_UserMode_Exit();
	}
}

/**
  * @brief  This function is used to configure SPIC according to parameters in FLASH_InitStruct.
  * @param FLASH_InitStruct: pointer to a FLASH_InitTypeDef
  *         structure that contains the configuration information for SPIC.
  * @retval none
  */
void DATA_FLASH_Config(SPIC_TypeDef *ComboSpic, FLASH_InitTypeDef *FLASH_InitStruct)
{
	FLASH_Config(ComboSpic, FLASH_InitStruct);
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
void DATA_FLASH_SetSpiMode(SPIC_TypeDef *ComboSpic, FLASH_InitTypeDef *FLASH_InitStruct, u8 SpicBitMode)
{
	FLASH_SetSpiMode(ComboSpic, FLASH_InitStruct, SpicBitMode);
}

/**
* @brief  Initializes the parameters in the FLASH_InitTypeDef with
*		its default values according to Micron Spec.
* @param FLASH_InitStruct: pointer to a FLASH_InitTypeDef
*		structure which will be initialized.
* @retval none
*/
void DATA_FLASH_StructInit_Micron(FLASH_InitTypeDef *FLASH_InitStruct)
{
	FLASH_StructInit_Micron(FLASH_InitStruct);
}

/**
* @brief  Initializes the parameters in the FLASH_InitTypeDef with
*		its default values according to MXIC Spec.
* @param FLASH_InitStruct: pointer to a FLASH_InitTypeDef
*		structure which will be initialized.
* @retval none
*/
void DATA_FLASH_StructInit_MXIC(FLASH_InitTypeDef *FLASH_InitStruct)
{
	FLASH_StructInit_MXIC(FLASH_InitStruct);
}

/**
* @brief  Initializes the parameters in the FLASH_InitTypeDef with
*		its default values according to Gigadevice spec.
* @param FLASH_InitStruct: pointer to a FLASH_InitTypeDef
*		structure which will be initialized.
* @retval none
*/
void DATA_FLASH_StructInit_GD(FLASH_InitTypeDef *FLASH_InitStruct)
{
	FLASH_StructInit_GD(FLASH_InitStruct);
}

/**
* @brief  Initializes the parameters in the FLASH_InitTypeDef with
*		its default values according to WINBOND_W25Q256FV_Spec.
* @param FLASH_InitStruct: pointer to a FLASH_InitTypeDef
*		structure which will be initialized.
* @retval none
*/
void DATA_FLASH_StructInit(FLASH_InitTypeDef *FLASH_InitStruct)
{
	FLASH_StructInit(FLASH_InitStruct);
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
u8 DATA_FLASH_Init(u8 SpicBitMode)
{
	SPIC_TypeDef *ComboSpic = SPIC_COMBO;
	FLASH_InitTypeDef *FLASH_InitStruct = &data_flash_init_para;

	/* config fixed parameters */
	DATA_FLASH_Config(ComboSpic, FLASH_InitStruct);

	/* Set SPI Mode */
	FLASH_InitStruct->FLASH_cur_bitmode = SpicBitMode;
	DATA_FLASH_SetSpiMode(ComboSpic, FLASH_InitStruct, SpicBitMode);

	return TRUE;
}