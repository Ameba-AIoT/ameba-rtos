/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "ameba_soc.h"
static const char *const TAG = "SDIO";
SDIOH_InitTypeDef sdioh_init_para;
u32 wait_for_sema = 0;
extern int (*sd_sema_take_fn)(u32);

/** @addtogroup Ameba_Periph_Driver
  * @{
  */

/** @addtogroup SDHOST
  * @{
  */

/* Exported functions --------------------------------------------------------*/
/** @addtogroup SDHOST_Exported_Functions
  * @{
  */

/**
  * @brief  Check whether SD host is busy.
  * @return HAL operation result:
  *           - HAL_OK: Not busy.
  *           - HAL_BUSY: Busy.
  */
u32 SDIOH_Busy(void)
{
	SDIOH_TypeDef *psdioh = SDIOH_BASE;
	u8 idle_level = sdioh_init_para.SDIOH_idle_level;

	/* check the SD bus status */
	if ((psdioh->SD_BUS_STATUS & idle_level) == idle_level) {
		/* check the CMD & DATA state machine */
		if ((psdioh->SD_CMD_STATE & SDIOH_CMD_FSM_IDLE) && (psdioh->SD_DATA_STATE & SDIOH_DATA_FSM_IDLE)) {
			/* check the SD card module state machine */
			if ((psdioh->SD_TRANSFER & SDIOH_SD_MODULE_FSM_IDLE)) {
				return HAL_OK;
			} else {
				RTK_LOGS(TAG, RTK_LOG_WARN, "SD card module state machine isn't in the idle state !\r\n");
				return HAL_BUSY;
			}
		} else {
			RTK_LOGS(TAG, RTK_LOG_WARN, "CMD or DATA state machine isn't in the idle state !!\r\n");
			return HAL_BUSY;
		}
	} else {
		RTK_LOGS(TAG, RTK_LOG_WARN, "CMD or DAT[3:0] pin isn't in the idle state !!\r\n");
		return HAL_BUSY;
	}
}

/**
  * @brief  Check whether an error occurred during an SD host transfer.
  * @param  status Pointer to a 16-bit variable used to store the detailed error status.
  * @return HAL operation result:
  *           - HAL_OK: No error occurred during transfer.
  *           - HAL_ERR_UNKNOWN: An error occurred during transfer.
  */
u32 SDIOH_CheckTxError(u16 *status)
{
	SDIOH_TypeDef *psdioh = SDIOH_BASE;
	u8 TempVal = 0;

	if (psdioh->SD_TRANSFER & SDIOH_ERR_OCCUR) {
		if (status != NULL) {
			TempVal = psdioh->SD_STATUS2;
			*status = psdioh->SD_STATUS1 | (TempVal << 8);
		}

		return HAL_ERR_UNKNOWN;
	} else {
		return HAL_OK;
	}
}

/**
  * @brief  Wait for the SD host transfer to complete.
  * @param  timeout_us Timeout value in microseconds.
  * @return HAL operation result:
  *           - HAL_OK: SD host TX completed within the specified timeout.
  *           - HAL_TIMEOUT: SD host TX timeout.
  */
u32 SDIOH_WaitTxDone(u32 timeout_us)
{
	SDIOH_TypeDef *psdioh = SDIOH_BASE;
	u32 tmp;

	do {
		tmp = psdioh->SD_TRANSFER & (SDIOH_TRANSFER_END | SDIOH_SD_MODULE_FSM_IDLE);
		if (tmp == (SDIOH_TRANSFER_END | SDIOH_SD_MODULE_FSM_IDLE)) {
			return HAL_OK;
		}

		DelayUs(1);
	} while (timeout_us-- != 0);

	return HAL_TIMEOUT;
}

/**
  * @brief  Enable DMA completion interrupt and set semaphore wait flag before a DMA transfer.
  */
void SDIOH_PreDMATrans(void)
{
	if ((CPU_InInterrupt() == 0) && (rtos_sched_get_state() == RTOS_SCHED_RUNNING) && (sd_sema_take_fn != NULL)) {
		wait_for_sema = 1;
		SDIOH_INTConfig(SDIOH_DMA_CTL_INT_EN, ENABLE);
	}
}

/**
  * @brief  Wait some time for SD host DMA done.
  * @param  timeout_us Timeout value in microseconds.
  * @return HAL operation result:
  *           - HAL_OK: SD host DMA done within a specified time.
  *           - HAL_TIMEOUT: SD host DMA timeout.
  */
u32 SDIOH_WaitDMADone(u32 timeout_us)
{
	SDIOH_TypeDef *psdioh = SDIOH_BASE;

	/* If scheduling has already started, wait for sema to obtain the DMA done signal. */
	if (wait_for_sema == 1) {
		wait_for_sema = 0;
		if ((CPU_InInterrupt() == 0) && (rtos_sched_get_state() == RTOS_SCHED_RUNNING) && (sd_sema_take_fn != NULL)) {
			if (sd_sema_take_fn(MAX(timeout_us / 1000, SD_SEMA_MAX_DELAY)) != RTK_SUCCESS) {
				SDIOH_INTConfig(SDIOH_DMA_CTL_INT_EN, DISABLE);
				RTK_LOGS(TAG, RTK_LOG_ERROR, " SD Get Semaphore Timeout\r\n");
				return HAL_TIMEOUT;
			}
		}

		SDIOH_INTConfig(SDIOH_DMA_CTL_INT_EN, DISABLE);
	}

	/* If scheduling has already started, poll transfer status; otherwise, poll transfer and DMA_Xfree status. */
	do {
		if ((psdioh->SD_TRANSFER & SDIOH_TRANSFER_END) && (!(psdioh->DMA_CRL3 & SDIOH_DMA_XFER))) {
			SDIOH_DMAReset();
			return HAL_OK;
		}

		DelayUs(1);
	} while (timeout_us-- != 0);

	return HAL_TIMEOUT;
}

/**
  * @brief  Get SD host interrupt status.
  * @return SD host interrupt status.
  */
u32 SDIOH_GetISR(void)
{
	SDIOH_TypeDef *psdioh = SDIOH_BASE;

	return psdioh->SD_ISR;
}

/**
  * @brief  Configure SD host interrupt enable/disable.
  * @internal
  *         BIT[SDIO_IT] == 1 && BIT[0] == 1 -> SDIO_IT en will be set 1
  *         BIT[SDIO_IT] == 1 && BIT[0] == 0 -> SDIO_IT en will be set 0
  *         Bit0 can only be written and cannot be read, with a default read value of 0.
  * @endinternal
  * @param  SDIO_IT SD host interrupt source, which can be one or combination of the following values:
  *         @arg SDIOH_DMA_CTL_INT_EN
  *         @arg SDIOH_CARD_ERR_INT_EN
  *         @arg SDIOH_CARD_END_INT_EN
  * @param  newState New state of the interrupt, which can be ENABLE or DISABLE.
  */
void SDIOH_INTConfig(u8 SDIO_IT, u32 newState)
{
	SDIOH_TypeDef *psdioh = SDIOH_BASE;

	if (newState == ENABLE) {
		psdioh->SD_ISREN |= (SDIO_IT | SDIOH_WRITE_DATA);
	} else {
		/*Bit0 reads as 0*/
		psdioh->SD_ISREN = SDIO_IT;
	}
}

/**
  * @brief  Clear SD host pending interrupt status.
  * @param  SDIO_IT SD host interrupt pending bit, which can be one or combination of the following values:
  *         @arg SDIOH_DMA_TRANSFER_DONE
  *         @arg SDIOH_CARD_ERROR
  *         @arg SDIOH_CARD_END
  */
void SDIOH_INTClearPendingBit(u8 SDIO_IT)
{
	SDIOH_TypeDef *psdioh = SDIOH_BASE;

	psdioh->SD_ISR = SDIO_IT;
}

/**
  * @brief  Wait until the SD host bus transitions to the specified logic state within a given timeout.
  * @param  status SD host bus state, which can be 0 or 1:
  *         @arg 0: wait for bus to go low
  *         @arg 1: wait for bus to go high
  * @param  timeout_us Timeout value in microseconds.
  * @return HAL operation result:
  *           - HAL_OK: Bus switched to specified state within timeout.
  *           - HAL_TIMEOUT: Timeout waiting for bus state.
  */
u32 SDIOH_CheckBusState(u8 status, u32 timeout_us)
{
	SDIOH_TypeDef *psdioh = SDIOH_BASE;
	u8 idle_level = sdioh_init_para.SDIOH_idle_level;
	u32 tmp;

	do {
		tmp = psdioh->SD_BUS_STATUS & idle_level;
		if (status) {
			if (tmp == idle_level) {
				return HAL_OK;
			}
		} else {
			if (tmp == 0) {
				return HAL_OK;
			}
		}

		DelayUs(1);
	} while (timeout_us-- != 0);

	return HAL_TIMEOUT;
}

/**
  * @brief  Get SD host bus width.
  * @return Bus width:
  *           - 0: 1-bit bus.
  *           - 1: 4-bit bus.
  */
u8 SDIOH_GetBusWidth(void)
{
	SDIOH_TypeDef *psdioh = SDIOH_BASE;

	return psdioh->SD_CONFIG1 & SDIOH_MASK_BUS_WIDTH;
}

/**
  * @brief  Set SD host bus width.
  * @param  width Bus width to set, which can be SDIOH_BUS_WIDTH_1BIT or SDIOH_BUS_WIDTH_4BIT.
  */
void SDIOH_SetBusWidth(u8 width)
{
	SDIOH_TypeDef *psdioh = SDIOH_BASE;
	u8 tmp;

	tmp = psdioh->SD_CONFIG1;
	tmp &= ~SDIOH_MASK_BUS_WIDTH;
	tmp |= (width & SDIOH_MASK_BUS_WIDTH);

	psdioh->SD_CONFIG1 = tmp;
}

/**
  * @brief  Set SD host DMA configuration.
  * @param  dma_ctl Pointer to an SDIOH_DmaCtl structure which contains the DMA configuration parameters.
  */
void SDIOH_DMAConfig(SDIOH_DmaCtl *dma_ctl)
{
	SDIOH_TypeDef *psdioh = SDIOH_BASE;
	u32 tmp = 0;

	/* set block length and count */
	if ((dma_ctl->type) == SDIOH_DMA_NORMAL) {
		/* 512 bytes (one block size) */
		psdioh->SD_BYTE_CNT_L = 0;
		psdioh->SD_BYTE_CNT_H = 2;
	} else {
		/* 64 bytes( CMD6, R2 response...) */
		psdioh->SD_BYTE_CNT_L = SDIOH_C6R2_BUF_LEN;
		psdioh->SD_BYTE_CNT_H = 0;
	}
	psdioh->SD_BLOCK_CNT_L = (dma_ctl->blk_cnt) & SDIOH_MASK_BLOCL_CNT_L;
	psdioh->SD_BLOCK_CNT_H = ((dma_ctl->blk_cnt) >> 8) & SDIOH_MASK_BLOCL_CNT_H;

	/* set the DMA control register */
	psdioh->DMA_CRL1 = (dma_ctl->start_addr) & SDIOH_MASK_DRAM_SA;	/* DMA start address (unit: 8 Bytes) */
	psdioh->DMA_CRL2 = (dma_ctl->blk_cnt) & SDIOH_MASK_DMA_LEN;		/* DMA transfer length (unit: 512 Bytes) */

	if ((dma_ctl->type) == SDIOH_DMA_64B) {
		tmp = SDIOH_DAT64_SEL;
	} else if ((dma_ctl->type) == SDIOH_DMA_R2) {
		tmp = SDIOH_RSP17_SEL;
	}

	tmp |= (dma_ctl->op << SDIOH_SHIFT_DDR_WR);
	psdioh->DMA_CRL3 = tmp;

	/* clear pending interrupt */
	SDIOH_INTClearPendingBit(SDIOH_DMA_TRANSFER_DONE | SDIOH_CARD_ERROR | SDIOH_CARD_END);

	/* enable DMA transfer */
	tmp = psdioh->DMA_CRL3;
	tmp |= SDIOH_DMA_XFER;
	psdioh->DMA_CRL3 = tmp;
}

/**
  * @brief  Reset SD host DMA configuration.
  */
void SDIOH_DMAReset(void)
{
	SDIOH_TypeDef *psdioh = SDIOH_BASE;

	psdioh->SD_BYTE_CNT_L = 0;
	psdioh->SD_BYTE_CNT_H = 0;
	psdioh->SD_BLOCK_CNT_L = 0;
	psdioh->SD_BLOCK_CNT_H = 0;
	psdioh->DMA_CRL1 = 0;
	psdioh->DMA_CRL2 = 0;
	psdioh->DMA_CRL3 = 0;
}

/**
  * @brief  Send a command to the SD card via SD host.
  * @param  cmd_attrib Pointer to an SDIOH_CmdTypeDef structure containing the command attributes.
  * @param  timeout_us Timeout value in microseconds.
  * @return HAL operation result:
  *           - HAL_OK: Command sent successfully.
  *           - Others: Error occurred.
  */
u32 SDIOH_SendCommand(SDIOH_CmdTypeDef *cmd_attrib, u32 timeout_us)
{
	SDIOH_TypeDef *psdioh = SDIOH_BASE;
	u8 cmd = cmd_attrib->idx, cmd_code, tmp;
	u32 val0 = 0, ret;

	ret = SDIOH_Busy();
	if (ret != HAL_OK) {
		return ret;
	}

	/* set SD_CONFIGURE2 (0x581) */
	val0 = SDIOH_CRC7_CAL_EN | (cmd_attrib->rsp_type);

	if (cmd_attrib->rsp_crc_chk) {
		val0 |= SDIOH_CRC7_CHK_EN;
	} else {
		val0 |= SDIOH_CRC7_CHK_DIS;
	}

	if ((cmd == SD_CMD_RdSingleBlk) || (cmd == SD_CMD_RdMulBlk) || (cmd == SD_CMD_SendTuningBlk)) {
		val0 |= SDIOH_CRC16_CHK_EN;
	} else {
		val0 |= SDIOH_CRC16_CHK_DIS;
	}

	if ((cmd == SD_CMD_WrBlk) || (cmd == SD_CMD_WrMulBlk)) {
		val0 |= SDIOH_WAIT_WR_CRCSTA_TO_EN | SDIOH_IGNORE_WR_CRC_ERR_EN;
	} else {
		val0 |= SDIOH_WAIT_WR_CRCSTA_TO_DIS | SDIOH_IGNORE_WR_CRC_ERR_DIS;
	}

	if (cmd == SD_CMD_VolSwitch) {
		val0 |= SDIOH_WAIT_BUSY_END_DIS;
	} else {
		val0 |= SDIOH_WAIT_BUSY_END_EN;
	}

	psdioh->SD_CONFIG2 = val0;

	/* set SD_CONFIGURE3 (0x582) */
	val0 = SDIOH_STOP_STA_WAIT_BUSY_EN | SDIOH_SD30_CLK_STOP_DIS | SDIOH_SD20_CLK_STOP_DIS | \
		   SDIOH_SD_CMD_RESP_CHK_DIS | SDIOH_ADDR_MODE_SECTOR;

	if ((cmd == SD_CMD_RdMulBlk) || (cmd == SD_CMD_WrMulBlk) || (cmd == SD_CMD_StopXsmission)) {
		val0 |= SDIOH_CMD_STA_WAIT_BUSY_DIS;
	} else {
		val0 |= SDIOH_CMD_STA_WAIT_BUSY_EN;
	}

	if ((cmd == SD_CMD_VolSwitch) || (cmd == SD_CMD_StopXsmission)) {
		val0 |= SDIOH_DATA_PHA_WAIT_BUSY_DIS;
	} else {
		val0 |= SDIOH_DATA_PHA_WAIT_BUSY_EN;
	}

	if ((cmd_attrib->rsp_type) == SDIOH_NO_RESP) {
		val0 |= SDIOH_CMD_RESP_TO_DIS;
	} else {
		val0 |= SDIOH_CMD_RESP_TO_EN;
	}

	psdioh->SD_CONFIG3 = val0;

	/* fill the command register */
	psdioh->SD_CMD[0] = HOST_COMMAND | (cmd & 0x3F);
	psdioh->SD_CMD[1] = ((cmd_attrib->arg) >> 24) & 0xFF;
	psdioh->SD_CMD[2] = ((cmd_attrib->arg) >> 16) & 0xFF;
	psdioh->SD_CMD[3] = ((cmd_attrib->arg) >> 8) & 0xFF;
	psdioh->SD_CMD[4] = (cmd_attrib->arg) & 0xFF;
	psdioh->SD_CMD[5] = 0x0;

	/* set the command code */
	switch (cmd) {
	case SD_CMD_SwitchFunc:
	case SD_CMD_SendSts:
		cmd_code = ((cmd_attrib->data_present) == SDIOH_NO_DATA) ? \
				   SDIOH_SEND_CMD_GET_RSP : SDIOH_NORMAL_READ;
		break;
	case SD_CMD_SendTuningBlk:
		cmd_code = SDIOH_TUNING;
		break;
	case SD_CMD_RdSingleBlk:
#if defined(SDIO) && (SDIO == EMMC)
	case EMMC_CMD_SendExtCsd:
#endif
		cmd_code = SDIOH_AUTO_READ2;
		break;
	case SD_CMD_RdMulBlk:
		cmd_code = SDIOH_AUTO_READ1;
		break;
	case SD_CMD_WrBlk:
		cmd_code = SDIOH_AUTO_WRITE2;
		break;
	case SD_CMD_WrMulBlk:
		cmd_code = SDIOH_AUTO_WRITE1;
		break;
	case SD_CMD_SendScr:
		cmd_code = SDIOH_NORMAL_READ;
		break;
	default:
		cmd_code = SDIOH_SEND_CMD_GET_RSP;
	}

	tmp = psdioh->SD_TRANSFER;
	tmp &= ~SDIOH_MASK_COM_CODE;
	tmp |= cmd_code;
	psdioh->SD_TRANSFER = tmp;

	/* start to transfer */
	psdioh->SD_TRANSFER |= SDIOH_START_TRANSFER;

	if (timeout_us != 0) {
		ret = SDIOH_WaitTxDone(timeout_us);
	}

	return ret;
}

/**
  * @brief  Get a byte from the SD host command response register.
  * @param  byte_index Index of the response byte to read.
  * @return The value of the specified response byte.
  */
u8 SDIOH_GetResponse(u8 byte_index)
{
	SDIOH_TypeDef *psdioh = SDIOH_BASE;

	return psdioh->SD_CMD[byte_index];
}

/**
  * @brief  Set the SD host clock divider and bus speed mode.
  * @param  clk_div Clock divider, which can be one of the following values:
  *         @arg SDIOH_CLK_DIV1:  divide by 1
  *         @arg SDIOH_CLK_DIV2:  divide by 2
  *         @arg SDIOH_CLK_DIV4:  divide by 4
  *         @arg SDIOH_CLK_DIV8:  divide by 8
  * @param  mode Bus speed mode, which can be SDIOH_SD20_MODE.
  * @note  Only SD 2.0 mode (SDIOH_SD20_MODE) is supported.
  */
void SDIOH_SwitchSpeed(u8 clk_div, u8 mode)
{
	SDIOH_TypeDef *psdioh = SDIOH_BASE;
	u32 value32;
	u8 value8;

	value32 = psdioh->CKGEN_CTL;
	value32 &= ~SDIOH_MASK_CLKDIV;
	value32 |= clk_div;
	psdioh->CKGEN_CTL = value32;

	value8 = psdioh->SD_CONFIG1;
	value8 &= ~SDIOH_MASK_MODE_SEL;
	value8 |= (mode << SDIOH_SHIFT_MODE_SEL);
	psdioh->SD_CONFIG1 = value8;
}

/**
  * @brief  Enable or disable SD host initialization (card identification) mode.
  * @param  NewState New state of the initialization mode, which can be ENABLE or DISABLE.
  * @param  Level Signal level, which can be one of the following values:
  *           @arg SDIOH_SIG_VOL_33
  *           @arg SDIOH_SIG_VOL_18
  * @return HAL operation result:
  *           - HAL_OK: SD host configured successfully.
  *           - Others: SD host failed to configure.
  */
u32 SDIOH_InitialModeCmd(u8 NewState, u8 Level)
{
	SDIOH_TypeDef *psdioh = SDIOH_BASE;
	u32 ret = HAL_OK;

	ret = SDIOH_Busy();
	if (ret != HAL_OK) {
		return ret;
	}

	if (NewState) {
		psdioh->CKGEN_CTL = SDIOH_SD30_SAMP_CLK_VP1 | SDIOH_SD30_PUSH_CLK_VP0 | \
							SDIOH_CRC_CLK_SSC | SDIOH_CLK_DIV4;

		/* SDCLK (Card Identification mode) : 390.625 KHz */
		psdioh->SD_CONFIG1 = (SDIOH_SD20_MODE << SDIOH_SHIFT_MODE_SEL) | \
							 SDIOH_CLK_DIV_BY_128 | SDIOH_INITIAL_MODE | SDIOH_SD30_ASYNC_FIFO_RST_N | \
							 (SDIOH_BUS_WIDTH_1BIT << SDIOH_SHIFT_BUS_WIDTH);
	} else {
		/* Switch to DS mode (3.3V) or SDR12 (1.8V). SDCLK (Data Transfer mode) : 25 MHz */
		if (Level == SDIOH_SIG_VOL_18) {
			psdioh->CKGEN_CTL = SDIOH_SD30_SAMP_CLK_VP1 | SDIOH_SD30_PUSH_CLK_VP0 | \
								SDIOH_CRC_CLK_SSC | SDIOH_CLK_DIV4;

			psdioh->SD_CONFIG1 = (SDIOH_SD30_MODE << SDIOH_SHIFT_MODE_SEL) | \
								 SDIOH_CLK_DIV_BY_128 | SDIOH_SD30_ASYNC_FIFO_RST_N | \
								 (SDIOH_BUS_WIDTH_1BIT << SDIOH_SHIFT_BUS_WIDTH);
		} else {
			psdioh->CKGEN_CTL = SDIOH_SD30_SAMP_CLK_VP1 | SDIOH_SD30_PUSH_CLK_VP0 | \
								SDIOH_CRC_CLK_SSC | (SDIOH_CLK_DIV4);

			psdioh->SD_CONFIG1 = (SDIOH_SD20_MODE << SDIOH_SHIFT_MODE_SEL) | \
								 SDIOH_CLK_DIV_BY_128 | (SDIOH_BUS_WIDTH_1BIT << SDIOH_SHIFT_BUS_WIDTH);
		}
	}

	return ret;
}

/**
  * @brief  Initialize the SD host controller and enable the SD clock.
  * @param  BusBitMode Bus width mode, which can be SDIOH_BUS_WIDTH_1BIT or SDIOH_BUS_WIDTH_4BIT.
  * @return HAL operation result:
  *           - HAL_OK: SD host initialized successfully.
  *           - Others: SD host failed to initialize.
  */
u32 SDIOH_Init(u8 BusBitMode)
{
	SDIOH_TypeDef *psdioh = SDIOH_BASE;
	u32 ret = HAL_OK;

	/* Set pin idle level */
	if (BusBitMode == SDIOH_BUS_WIDTH_4BIT) {
		sdioh_init_para.SDIOH_idle_level = 0x1F;
	} else {
		sdioh_init_para.SDIOH_idle_level = 0x03;
	}

	/* SD host func & clock enable */
	RCC_PeriphClockCmd(APBPeriph_SDH, APBPeriph_SDH_CLOCK, ENABLE);

	/* Enable direct access mode */
	psdioh->SRAM_CRL = SDIOH_LX_BURST_SIZE_64B | SDIOH_MAP_SEL_DEC | SDIOH_DIRECT_ACCESS_SEL;

	/* Enable interrupt */
	psdioh->CARD_INT_EN = SDIOH_SDMMC_INT_EN;

	psdioh->CARD_SELECT = SDIOH_CARD_SEL_SD_MODULE;
	psdioh->CARD_STOP = SDIOH_TARGET_MODULE_SD;

	/* Set sample delay */
//	psdioh->SD_SAMPLE_POINT_CTL |= SDIOH_SD_SAMPLE_POINT_SEL;

	/* Initial mode */
	ret = SDIOH_InitialModeCmd(ENABLE, 0);
	if (ret != HAL_OK) {
		return ret;
	}

	psdioh->CARD_CLK_EN_CTL = SDIOH_SD_CARD_MOUDLE_EN;

	return ret;
}

/**
  * @brief  De-initialize SD host peripheral.
  */
void SDIOH_DeInit(void)
{
	SDIOH_TypeDef *psdioh = SDIOH_BASE;

	/* disable interrupt & clear all pending interrupts */
	psdioh->CARD_INT_PEND |= SDIOH_SDMMC_INT_PEND;
	psdioh->CARD_INT_EN &= ~ SDIOH_SDMMC_INT_EN;
	SDIOH_INTClearPendingBit(SDIOH_SD_ISR_ALL);
	SDIOH_INTConfig(SDIOH_SD_ISR_ALL, DISABLE);
}

/**
  * @brief  Enable or disable the card-detect debounce function.
  * @param  NewState ENABLE or DISABLE.
  * @note   Only insertion debounce is implemented.
  */
void SDIOH_DebounceCmd(u8 NewState)
{
	u32 temp = 0;
	temp = HAL_READ32(SYSTEM_CTRL_BASE_HP, REG_HSYS_SDH_CTRL);

	if (NewState != DISABLE) {
		temp |= HSYS_BIT_SDCD_DBNC_EN;
	} else {
		temp &= ~(HSYS_BIT_SDCD_DBNC_EN);
	}

	HAL_WRITE32(SYSTEM_CTRL_BASE_HP, REG_HSYS_SDH_CTRL, temp);
}

/**
  * @brief  Set the card-detect debounce count.
  * @param  debouncevalue Debounce count value (unit: 1 ms).
  */
void SDIOH_DebounceSet(u32 debouncevalue)
{
	u32 temp = 0;
	temp = HAL_READ32(SYSTEM_CTRL_BASE_HP, REG_HSYS_SDH_CTRL);

	temp &= ~(HSYS_MASK_SDCD_DBNC_CNT);
	temp |= HSYS_SDCD_DBNC_CNT(debouncevalue);

	HAL_WRITE32(SYSTEM_CTRL_BASE_HP, REG_HSYS_SDH_CTRL, temp);
}

/**
  * @brief  Get the card-detect debounce count value.
  * @return Debounce count value (unit: 1 ms).
  */
u32 SDIOH_DebounceGet(void)
{
	u32 temp = 0;
	temp = HAL_READ32(SYSTEM_CTRL_BASE_HP, REG_HSYS_SDH_CTRL);

	return HSYS_GET_SDCD_DBNC_CNT(temp);
}

/**
  * @}
  */

/**
  * @}
  */

/** @} */
