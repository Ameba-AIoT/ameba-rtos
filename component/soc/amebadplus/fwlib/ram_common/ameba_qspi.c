/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "ameba_soc.h"

static const char *TAG = "QSPI";

/** @addtogroup Ameba_Periph_Driver
  * @{
  */

/** @defgroup QSPI
* @brief QSPI driver modules
* @{
*/

/* Exported functions --------------------------------------------------------*/
/** @defgroup QSPI_Exported_Functions QSPI Exported Functions
  * @{
  */

/**
  * @brief  Initialize QSPI to user mode.
  * @param  None.
  * @retval None.
  */
void QSPI_Init(void)
{
	/* set cs and baudrate */
	QSPI->SSIENR = 0;
	QSPI->SER = BIT_SER;
	QSPI->BAUDR = 1;

	/* enter user mode */
	QSPI->CTRLR0 |= BIT_USER_MODE;
	QSPI->CTRLR0 |= BIT_SPI_DREIR_R_DIS;

	/* user mode is entered after auto cmd is done, which means if SPIC BUSY=1, HW will not write Ctrl0 */
	while (!(QSPI->CTRLR0 & BIT_USER_MODE));
}

/**
  * @brief  Initialize QSPI_CmdAddrInfo structure.
  * @param  info: cmd and addr info given by user.
  * @retval None.
  */
void QSPI_StructInit(QSPI_CmdAddrInfo *info)
{
	info->cmd_length = 1;
	info->addr_length = 0;
	info->cmd_ch = 0;
	info->addr_ch = 0;
	info->data_ch = 0;
	info->ddr_en = 0;
	info->rx_dummy_cycle = 0;
}

/**
  * @brief  Set baud value to divide clock.
  * @param  div: actual QSPI clock = bus_clk/(2 * div).
  * @retval None.
  */
void QSPI_SetBaud(u32 div)
{
	QSPI->BAUDR = div;
}


/**
  * @brief  Set SO_DUM value to determine if sdo & sdi use same line.
  * @param  val: val = 0 means use same line, other value means use different line.
  * @retval None.
  */
void QSPI_SO_DNUM(u32 val)
{
	if (val == 0) {
		QSPI->CTRLR2 &= ~(BIT_SO_DNUM);
	} else {
		QSPI->CTRLR2 |= BIT_SO_DNUM;
	}
}


/**
  * @brief  Write data by CPU according to the given parameter.
  * @param  info: cmd and addr info given by user.
  *         data: data to transmit.
  *         len: transmit data length.
  * @retval None.
  */
void QSPI_Write(QSPI_CmdAddrInfo *info, u8 *data, u32 len)
{
	u32 reg, i;
	QSPI->SER = BIT_SER;

	/* Disable QSPI */
	QSPI->SSIENR = 0;

	QSPI->CTRLR0 &= ~MASK_DDR_EN;
	QSPI->CTRLR0 |= DDR_EN(info->ddr_en);

	/* Set Channel number */
	QSPI->CTRLR0 &= ~(TMOD(3) | CMD_CH(3) | ADDR_CH(3) | DATA_CH(3));
	QSPI->CTRLR0 |= (CMD_CH(info->cmd_ch) | ADDR_CH(info->addr_ch) | DATA_CH(info->data_ch));

	/* Set tx mode */
	QSPI->CTRLR0 &= ~(TMOD(3));

	/* Set cmd and addr length */
	reg = QSPI->USER_LENGTH & ~(MASK_USER_ADDR_LENGTH | MASK_USER_CMD_LENGHT);
	QSPI->USER_LENGTH = reg | USER_ADDR_LENGTH(info->addr_length) | USER_CMD_LENGHT(info->cmd_length);

	/* Set tx length */
	QSPI->RX_NDF = 0;
	QSPI->TX_NDF = TX_NDF(len);

	/* Send cmd */
	for (i = 0; i < info->cmd_length; i++) {
		QSPI->DR[0].BYTE = info->cmd[i];
	}

	/* Send addr */
	for (i = 0; i < info->addr_length; i++) {
		QSPI->DR[0].BYTE = info->addr[i];
	}

	/* Enable QSPI */
	QSPI->SSIENR = BIT_SPIC_EN;

	/* Send data */
	for (i = 0; i < len;) {
		if (QSPI->SR & BIT_TFNF) {
			QSPI->DR[0].BYTE = data[i];
			i++;
		}
	}

	/* Wait bus busy */
	while (QSPI->SR & BIT_BUSY);

	/* Disable QSPI */
	QSPI->SSIENR = 0;
}

/**
  * @brief  Read data by CPU according to the given parameter.
  * @param  info: cmd and addr info given by user.
  *         data: pointer to recevie data.
  *         len: receive data length.
  * @retval None.
  */
void QSPI_Read(QSPI_CmdAddrInfo *info, u8 *data, u32 len)
{
	u32 reg, i;
	u32 rd_num = 0;

	/* Disable QSPI */
	QSPI->SSIENR = 0;

	/* Set Channel number */
	QSPI->CTRLR0 &= ~(CMD_CH(3) | ADDR_CH(3) | DATA_CH(3));
	QSPI->CTRLR0 |= (CMD_CH(info->cmd_ch) | ADDR_CH(info->addr_ch) | DATA_CH(info->data_ch));

	/* Set rx mode */
	QSPI->CTRLR0 |= TMOD(3);

	/* Set rx length */
	QSPI->RX_NDF = RX_NDF(len);
	QSPI->TX_NDF = 0;

	/* Set cmd and addr and dummy cycle length */
	reg = QSPI->USER_LENGTH & ~(MASK_USER_ADDR_LENGTH | MASK_USER_CMD_LENGHT);
	QSPI->USER_LENGTH = reg | USER_ADDR_LENGTH(info->addr_length) | USER_CMD_LENGHT(info->cmd_length)
						| USER_RD_DUMMY_LENGTH(info->rx_dummy_cycle * 2);

	/* Send cmd */
	for (i = 0; i < info->cmd_length; i++) {
		QSPI->DR[0].BYTE = info->cmd[i];
	}

	/* Send addr */
	for (i = 0; i < info->addr_length; i++) {
		QSPI->DR[0].BYTE = info->addr[i];
	}

	/* Enable QSPI */
	QSPI->SSIENR = BIT_SPIC_EN;

	while (rd_num < len) {
		if (QSPI->SR & BIT_RFNE) {
			data[rd_num] = QSPI->DR[0].BYTE;
			rd_num ++;
		}
	}

	/* Disable QSPI */
	QSPI->SSIENR = 0;
}

/**
  * @brief  Write data according to the given parameter through PPE or GDMA. User should
  *            enable PPE to QSPI or GDMA to QSPI handshake to start data transfer.
  * @param  info: cmd and addr info given by user.
  *         len: transmit data length.
  * @retval None.
  */
void QSPI_WriteStart(QSPI_CmdAddrInfo *info, u32 len)
{
	u32 reg, i;
	QSPI->SER = BIT_SER;

	/* Disable QSPI */
	QSPI->SSIENR = 0;

	QSPI->CTRLR0 &= ~MASK_DDR_EN;
	QSPI->CTRLR0 |= DDR_EN(info->ddr_en);

	/* Set Channel number */
	QSPI->CTRLR0 &= ~(TMOD(3) | CMD_CH(3) | ADDR_CH(3) | DATA_CH(3));
	QSPI->CTRLR0 |= (CMD_CH(info->cmd_ch) | ADDR_CH(info->addr_ch) | DATA_CH(info->data_ch));

	/* Set tx mode */
	QSPI->CTRLR0 &= ~(TMOD(3));

	/* Set cmd and addr length */
	reg = QSPI->USER_LENGTH & ~(MASK_USER_ADDR_LENGTH | MASK_USER_CMD_LENGHT);
	QSPI->USER_LENGTH = reg | USER_ADDR_LENGTH(info->addr_length) | USER_CMD_LENGHT(info->cmd_length);

	/* Set tx length */
	QSPI->RX_NDF = 0;
	QSPI->TX_NDF = TX_NDF(len);

	/* DMA configuration*/
	QSPI->DMACR = 2;
	QSPI->DMATDLR = 16;

	/* Send cmd */
	for (i = 0; i < info->cmd_length; i++) {
		QSPI->DR[0].BYTE = info->cmd[i];
	}

	/* Send addr */
	for (i = 0; i < info->addr_length; i++) {
		QSPI->DR[0].BYTE = info->addr[i];
	}

	/* Enable QSPI */
	QSPI->SSIENR = BIT_SPIC_EN;
}


/**
  * @brief  User could use this to disable QSPI After PPE or GDMA transfer complete.
  * @param  None.
  * @retval None.
  */
void QSPI_WriteEnd(void)
{
	/* Wait bus busy */
	while (QSPI->SR & BIT_BUSY);

	/* Disable QSPI */
	QSPI->SSIENR = 0;
}


/**
  * @brief  Init data transfer from GDMA to QSPI.
  * @param  GDMA_InitStruct: pointer to a GDMA_InitTypeDef structure that contains
  *         the configuration information for the GDMA peripheral.
  * @param  CallbackData: GDMA callback data.
  * @param  CallbackFunc: GDMA callback function.
  * @param  pbuf: data buffer send to QSPI. Must be 4-bytes aligned.
  * @param  size: buffer size;
  * @retval   TRUE/FLASE
  */
int QSPI_GDMAInit(GDMA_InitTypeDef *GDMA_InitStruct,
				  IRQ_FUN CallbackFunc,
				  void *CallbackData,
				  u8 *Pbuf,
				  u32 Size)
{
	u8 GdmaChnl;

	GdmaChnl = GDMA_ChnlAlloc(0, (IRQ_FUN)CallbackFunc, (u32)CallbackData, INT_PRI_HIGHEST);
	if (GdmaChnl == 0xFF) {
		/* No Available DMA channel */
		RTK_LOGE(TAG, "Alloc dma channel fail\n");
		return _FALSE;
	}

	/* Set QSPI hw handshake to GDMA */
	HAL_WRITE32(SYSTEM_CTRL_BASE, REG_LSYS_ECO_CTRL, HAL_READ32(SYSTEM_CTRL_BASE, REG_LSYS_ECO_CTRL) | BIT_QSPI_HW_HANDSHAKE_SEL);

	_memset((void *)GDMA_InitStruct, 0, sizeof(GDMA_InitTypeDef));
	GDMA_InitStruct->MuliBlockCunt = 0;
	GDMA_InitStruct->MaxMuliBlock  = 1;
	GDMA_InitStruct->GDMA_DIR     = TTFCMemToPeri;
	GDMA_InitStruct->GDMA_DstHandshakeInterface = GDMA_HANDSHAKE_INTERFACE_SPIC_TX;
	GDMA_InitStruct->GDMA_DstAddr = (u32) & (QSPI->DR[0].BYTE);
	GDMA_InitStruct->GDMA_Index   = 0;
	GDMA_InitStruct->GDMA_ChNum   = GdmaChnl;
	GDMA_InitStruct->GDMA_IsrType = (BlockType | TransferType | ErrType);
	GDMA_InitStruct->GDMA_DstMsize = MsizeFour;
	GDMA_InitStruct->GDMA_DstDataWidth = TrWidthFourBytes;
	GDMA_InitStruct->GDMA_DstInc = NoChange;
	GDMA_InitStruct->GDMA_SrcInc = IncType;
	/* 4-bytes aligned, move 4 bytes each transfer */
	GDMA_InitStruct->GDMA_SrcMsize   = MsizeFour;
	GDMA_InitStruct->GDMA_SrcDataWidth = TrWidthFourBytes;
	GDMA_InitStruct->GDMA_BlockSize = Size >> 2;
	GDMA_InitStruct->GDMA_SrcAddr = (u32)(Pbuf);

	GDMA_Init(GDMA_InitStruct->GDMA_Index, GDMA_InitStruct->GDMA_ChNum, GDMA_InitStruct);

	return _TRUE;
}


/**
  * @brief  start data transfer from GDMA to QSPI.
  * @param  GDMA_InitStruct: pointer to a GDMA_InitTypeDef structure that contains
  *         the configuration information for the GDMA peripheral.
  * @retval None.
  */
void QSPI_GDMAStart(GDMA_InitTypeDef *GDMA_InitStruct)
{
	GDMA_Cmd(GDMA_InitStruct->GDMA_Index, GDMA_InitStruct->GDMA_ChNum, ENABLE);
}

/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */