/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "ameba_soc.h"

static const char *const TAG = "SDH";

static u8 is_timeout(u32 start_us, u32 timeout_us)
{
	u32 now_us = DTimestamp_Get();

	if (now_us < start_us) {
		if (0xffffffff - start_us + now_us > timeout_us) {
			return 1;
		} else {
			return 0;
		}
	} else {
		if (now_us - start_us > timeout_us) {
			return 1;
		} else {
			return 0;
		}
	}
}

/**
 * @brief  Initializes the SDMMC according to the specified
 *         parameters in the SDMMC_InitTypeDef and create the associated handle.
 * @param  SDIOx Pointer to SDIO host
 * @retval HAL status
 */
u32 SDIOH_Init(SDIOHOST_TypeDef *SDIOx)
{
	u32 ret = HAL_OK;
	u32 start_us;
	u32 timeout_us = SDIO_CMD_TIMEOUT_US;

	// set SDIO IP registers to default values
	/* TODO: expand SDIO_SRAM_CTL_CONFIG to specific bits */
	SDIOx->SDIOHOST_SRAM_CTL = SDIO_SRAM_CTL_CONFIG;
	SDIOx->SDIOHOST_CTL = SDIO_IP_CTL_CONFIG;
	SDIOx->SDIOHOST_HOST_CTL_1_REG = SDIO_HOST_CTL_CONFIG;
	SDIOx->SDIOHOST_PWR_CTL = ((0x7 << SDIOHOST_SHIFT_SD_BUS_VOLTAGE_SEL) | SDIOHOST_BIT_SD_BUS_PWR); // 3.3V bus voltage + power on

	SDIOx->SDIOHOST_SRAM_CTL |= SDIOHOST_BIT_MCU_SEL;					 // byte access
	SDIOx->SDIOHOST_SD_SAMPLE_POINT_CTL |= SDIOHOST_BIT_SD20_SAMP_POINT; // switch sampling point delay
	SDIOx->SDIOHOST_SRAM_CTL &= ~SDIOHOST_BIT_MCU_SEL;					 // word access

	SDIOx->SDIOHOST_CLK_CTL = 0x1; // turn on internal clock

	// wait for internal clock to be stable
	start_us = DTimestamp_Get();
	while ((SDIOx->SDIOHOST_CLK_CTL & SDIOHOST_BIT_INTERNAL_CLK_STABLE) != SDIOHOST_BIT_INTERNAL_CLK_STABLE) {
		if ((timeout_us != SDMMC_WAIT_FOREVER) && is_timeout(start_us, timeout_us)) {
			RTK_LOGE(TAG, "wait internal clock stable timeout !!\n");
			return HAL_TIMEOUT;
		}
	}

	ret = SDIO_ConfigClock(SDIOx, SD_INIT_400K);
	if (ret != HAL_OK) {
		return ret;
	}

	SDIOx->SDIOHOST_TIMEOUT_CTL = SDIO_TIMEOUT_CTL_CONFIG;
	SDIOx->SDIOHOST_ISREN = SDIO_ISREN_CONFIG;

	SDIOx->SDIOHOST_NORMAL_INT_STATUS_EN = SDIO_NORMAL_INT_STATUS_EN_CONFIG;
	SDIOx->SDIOHOST_ERR_INT_STATUS_EN = SDIO_ERR_INT_STATUS_EN_CONFIG;
	SDIOx->SDIOHOST_NORMAL_SIG_EN = SDIOHOST_BIT_CARD_INT_SIGNAL_EN;
	SDIOx->SDIOHOST_ERR_SIG_EN = SDIO_ERR_SIG_EN_CONFIG;

	// clear all int
	SDIOx->SDIOHOST_NORMAL_INT_STATUS = 0xFFFF;
	SDIOx->SDIOHOST_ERR_INT_STATUS = 0xFFFF;

	SDIOx->SDIOHOST_HOST_CTL = SDIO_HOST_CTL2_CONFIG;
	DelayMs(5); // regulator shall be stable within 5ms

	return ret;
}

/**
 * @brief  Perform software reset.
 * @param  SDIOx Pointer to SDIO host
 * @return SD result.
 */
u32 SDIO_ResetAll(SDIOHOST_TypeDef *SDIOx)
{
	u32 start_us;
	u32 timeout_us = SDIO_CMD_TIMEOUT_US;

	/* perform software reset*/
	SDIOx->SDIOHOST_SW_RST |= SDIOHOST_BIT_SW_RST_ALL;

	start_us = DTimestamp_Get();
	while (SDIOx->SDIOHOST_SW_RST != 0x0) {
		if ((timeout_us != SDMMC_WAIT_FOREVER) && is_timeout(start_us, timeout_us)) {
			RTK_LOGE(TAG, "%s: wait internal clock stable timeout !!\n", __func__);
			RTK_LOGE(TAG, "t_out=%u\n", timeout_us);
			RTK_LOGE(TAG, "now  =%u\n", DTimestamp_Get());
			return HAL_TIMEOUT;
		}
	}

	return HAL_OK;
}

u32 SDIO_CheckState(SDIOHOST_TypeDef *SDIOx)
{
	u32 status = SDIO_GetStatus(SDIOx);

	// check the SDIO bus status
	/* CMD line + DATA(4) line level */
	if ((status & (SDIOHOST_BIT_CMD_LINE_SIG_LVL | SDIOHOST_MASK_DAT_LINE_SIG_LVL)) !=
		(SDIOHOST_BIT_CMD_LINE_SIG_LVL | SDIOHOST_MASK_DAT_LINE_SIG_LVL)) {
		RTK_LOGE(TAG, "SDIO bus isn't in the idle state!!\n");
		RTK_LOGE(TAG, "SDIOHOST_PRESENT_STATE:0x%x\n", status);
		return HAL_BUSY;
	}

	// check the CMD & DATA state machine
	if (status & SDIOHOST_BIT_CMD_INHIBIT_CMD) {
		RTK_LOGE(TAG, "CMD state machine isn't in the idle state!!\n");
		return HAL_BUSY;
	}

	if (status & SDIOHOST_BIT_CMD_INHIBIT_DAT) {
		RTK_LOGE(TAG, "DATA state machine isn't in the idle state!!\n");
		return HAL_BUSY;
	}

	// check the SDIO card module state machine
	if (status & (SDIOHOST_BIT_WRITE_XFER_ACTIVE | SDIOHOST_BIT_READ_XFER_ACTIVE)) {
		RTK_LOGW(TAG, "SDIO card module state machine isn't in the idle state !!\n");
		return HAL_BUSY;
	}

	/* added by kw */
	/* check SDIO card is inserted or not */
	if (!(status & SDIOHOST_BIT_CARD_INSERTED)) {
		RTK_LOGE(TAG, "Reset or Debouncing or No Card!!\n");
		return HAL_ERR_HW;
	}

	if (!(status & SDIOHOST_BIT_WP_SWITCH_PIN_LVL)) {
		RTK_LOGE(TAG, "Card is write-protected!!\n");
		return HAL_ERR_HW;
	}

	return HAL_OK;
}

/**
 * @brief  Read data (word) from Rx FIFO in blocking mode (polling)
 * @param  SDIOx Pointer to SDIO host
 * @retval HAL status
 */
u32 SDIO_ReadFIFO(SDIOHOST_TypeDef *SDIOx)
{
	/* Read data from Rx FIFO */
	return (u32)SDIOx->SDIOHOST_BUFFER_PORT_DATA;
}

/**
 * @brief  Write data (word) to Tx FIFO in blocking mode (polling)
 * @param  SDIOx Pointer to SDIO host
 * @param  pWriteData: pointer to data to write
 * @retval HAL status
 */
void SDIO_WriteFIFO(SDIOHOST_TypeDef *SDIOx, u32 *pWriteData)
{
	/* Write data to FIFO */
	SDIOx->SDIOHOST_BUFFER_PORT_DATA = *pWriteData;
}

/**
 * @brief  Set SDMMC Power state to ON.
 * @param  SDIOx Pointer to SDIO host
 * @return NULL
 */
void SDIO_PowerState_ON(SDIOHOST_TypeDef *SDIOx)
{
	/* Set power state to ON */
	SDIOx->SDIOHOST_PWR_CTL |= SDIOHOST_BIT_SD_BUS_PWR;
}

/**
 * @brief  Set SDMMC Power state to OFF.
 * @param  SDIOx Pointer to SDIO host
 * @retval HAL status
 */
void SDIO_PowerState_OFF(SDIOHOST_TypeDef *SDIOx)
{
	/* Set power state to OFF */
	SDIOx->SDIOHOST_PWR_CTL = (u8)0x00;
}

/**
 * @brief  Get SDMMC Power state.
 * @param  SDIOx Pointer to SDIO host
 * @retval Power status of the controller. The returned value can be one of the
 *         following values:
 *            - 0x00: Power OFF
 *            - 0x01: Power ON
 */
u8 SDIO_GetPowerState(SDIOHOST_TypeDef *SDIOx)
{
	return (SDIOx->SDIOHOST_PWR_CTL & SDIOHOST_BIT_SD_BUS_PWR);
}

u32 SDIO_ConfigClock(SDIOHOST_TypeDef *SDIOx, u32 ClkKHz)
{
	u32 BaseClk;
	u32 div_cnt;
	u8 div_select;
	u32 power = 1;
	u16 temp;

	/* Disable SD clock */
	SDIOx->SDIOHOST_CLK_CTL &= ~SDIOHOST_BIT_SDCLK_EN;

	if (ClkKHz > 25000) { // 25MHz
		SDIOx->SDIOHOST_HOST_CTL_1_REG |= SDIOHOST_BIT_HS_EN;
	} else {
		SDIOx->SDIOHOST_HOST_CTL_1_REG &= ~SDIOHOST_BIT_HS_EN;
	}

	/* Get base clock frequency, unit: MHz */
	BaseClk = (SDIOx->SDIOHOST_CAPABILITIES0 & SDIOHOST_MASK_BASE_CLK_FREQ) >> SDIOHOST_SHIFT_BASE_CLK_FREQ;
	BaseClk /= 2; // actual baseclk is half of capabilities value
	/* Calculate divisor */
	div_cnt = BaseClk * 1000 / ClkKHz + (BaseClk * 1000 % ClkKHz ? 1 : 0);

	if (div_cnt == 0) {
		return HAL_ERR_HW;
	} else if ((div_cnt & (div_cnt - 1)) == 0) {
		div_select = div_cnt / 2;
	} else {
		while (power < div_cnt) {
			power <<= 1;
		}

		if (power > 256) {
			return HAL_ERR_PARA;
		}
		div_select = div_cnt / 2;
	}

	/* Set clock divisor */
	temp = SDIOx->SDIOHOST_CLK_CTL;
	temp &= ~SDIOHOST_MASK_SDCLK_FREQ_SEL;
	temp |= (div_select << SDIOHOST_SHIFT_SDCLK_FREQ_SEL) & SDIOHOST_MASK_SDCLK_FREQ_SEL;
	SDIOx->SDIOHOST_CLK_CTL = temp;

	/* Enable SD clock */
	SDIOx->SDIOHOST_CLK_CTL |= SDIOHOST_BIT_SDCLK_EN;

	return HAL_OK;
}

u32 SDIO_ConfigBusWidth(SDIOHOST_TypeDef *SDIOx, u8 BusWidth)
{
	assert_param(IS_SD_BUS_WIDTH(BusWidth));

	if (SDIOH_BUS_WIDTH_1BIT == BusWidth) {
		SDIOx->SDIOHOST_HOST_CTL_1_REG &= ~SDIOHOST_BIT_DATA_XFER_WIDTH;
	} else if (SDIOH_BUS_WIDTH_4BIT == BusWidth) {
		SDIOx->SDIOHOST_HOST_CTL_1_REG |= SDIOHOST_BIT_DATA_XFER_WIDTH;
	} else {
		return HAL_ERR_PARA;
	}

	return HAL_OK;
}

u32 SDIO_ConfigDMA(SDIOHOST_TypeDef *SDIOx, u8 DmaMode, u32 DmaAddr)
{
	u32 tmp0, tmp1;

	assert_param(IS_SD_DMA_MODE(DmaMode));

	tmp0 = SDIOx->SDIOHOST_CAPABILITIES0;

	switch (DmaMode) {
	case SDIO_SDMA_MODE:
		if (!(tmp0 & SDIOHOST_BIT_SDMA_SUPPORT)) {
			return HAL_ERR_PARA;
		}

		tmp1 = SDIOx->SDIOHOST_SDMA_SYS_ADDR;
		tmp1 &= ~SDIOHOST_MASK_SDMA_SYS_ADDR;
		tmp1 |= (DmaAddr << SDIOHOST_SHIFT_SDMA_SYS_ADDR);
		SDIOx->SDIOHOST_SDMA_SYS_ADDR = tmp1;
		break;

	case SDIO_ADMA2_32B_MODE:
		if (!(tmp0 & SDIOHOST_BIT_ADMA2_SUPPORT)) {
			return HAL_ERR_PARA;
		}

		tmp1 = SDIOx->SDIOHOST_SYS_ADDR0;
		tmp1 &= ~SDIOHOST_MASK_ADMA_SYS_ADDR;
		tmp1 |= (DmaAddr << SDIOHOST_SHIFT_ADMA_SYS_ADDR);
		SDIOx->SDIOHOST_SYS_ADDR0 = tmp1;
		break;

	case SDIO_ADMA2_64B_MODE:
	default:
		return HAL_ERR_PARA;
	}

	tmp1 = SDIOx->SDIOHOST_HOST_CTL_1_REG;
	tmp1 &= ~SDIOHOST_MASK_DMA_SEL;
	tmp1 |= (DmaMode << SDIOHOST_SHIFT_DMA_SEL);
	SDIOx->SDIOHOST_HOST_CTL_1_REG = tmp1;

	return HAL_OK;
}

/**
 * @brief  Configure the SDMMC data path according to the specified
 *         parameters in the SDIO_DataInitTypeDef.
 * @param  SDIOx: Pointer to SDIO register base
 * @param  Data : pointer to a SDIO_DataInitTypeDef structure
 *         that contains the configuration information for the SDMMC data.
 * @retval HAL status
 */
void SDIO_ConfigData(SDIOHOST_TypeDef *SDIOx, SDIO_DataInitTypeDef *Data)
{
	u32 tmp = 0;

	/* Check the parameters */
	assert_param(IS_SD_TRANSFER_TYPE(Data->TransType));
	assert_param(IS_SD_TRANSFER_DIR(Data->TransDir));
	assert_param(IS_SD_BLOCK_SIZE(Data->BlockSize));

	SDIOx->SDIOHOST_BLK_SZ = Data->BlockSize;

	/* Set the SDMMC XFER_MODE value */
	tmp = (Data->TransDir) << SDIOHOST_SHIFT_XFER_DIR_SEL |
		  (Data->DmaEn) << SDIOHOST_SHIFT_DMA_EN |
		  (Data->AutoCmdEn) << SDIOHOST_SHIFT_AUTO_CMD_EN;
	switch (Data->TransType) {
	case SDIO_TRANS_MULTI_BLK:
		tmp |= SDIOHOST_BIT_MULTI_SINGLE_BLK_SEL | SDIOHOST_BIT_BLK_CNT_EN;
		SDIOx->SDIOHOST_BLK_CNT = Data->BlockCnt;
		break;
	case SDIO_TRANS_INFIN_BLK:
		tmp |= SDIOHOST_BIT_MULTI_SINGLE_BLK_SEL;
		tmp &= ~SDIOHOST_BIT_BLK_CNT_EN;
		break;
	case SDIO_TRANS_MULTI_STOP:
		tmp |= SDIOHOST_BIT_MULTI_SINGLE_BLK_SEL | SDIOHOST_BIT_BLK_CNT_EN;
		SDIOx->SDIOHOST_BLK_CNT = 0;
		break;
	case SDIO_TRANS_SINGLE_BLK: // no need to set blk_cnt register
	default:
		tmp &= ~SDIOHOST_BIT_MULTI_SINGLE_BLK_SEL;
	}

	SDIOx->SDIOHOST_XFER_MODE = tmp;
}

/**
 * @brief  Configure the SDMMC command path according to the specified
 * parameters in SDIO_CmdInitTypeDef structure and send the command
 * @param  SDIOx Pointer to SDIO host
 * @param  Command: pointer to a SDIO_CmdInitTypeDef structure that contains
 *         the configuration information for the SDMMC command
 * @retval HAL status
 */
void SDIO_SendCommand(SDIOHOST_TypeDef *SDIOx, SDIO_CmdInitTypeDef *Command)
{
	u32 tmp = 0;

	/* Check the parameters */
	assert_param(IS_SD_CMD_INDEX(Command->CmdIndex));
	assert_param(IS_SD_CMD_TYPE(Command->CmdType));
	assert_param(IS_SD_RESP_TYPE(Command->RespType));
	assert_param(IS_SD_DATA_TYPE(Command->DataPresent));

	/* Set the SDMMC Argument value */
	SDIOx->SDIOHOST_CMD_ARG1 = Command->Argument;

	/* Set SDMMC command parameters */
	tmp |= (Command->CmdIndex) << SDIOHOST_SHIFT_CMD_IDX;

	tmp &= ~SDIOHOST_MASK_CMD_TYPE;
	tmp |= (Command->CmdType) << SDIOHOST_SHIFT_CMD_TYPE;

	if (Command->DataPresent == SDIO_TRANS_WITH_DATA) {
		tmp |= SDIOHOST_BIT_DATA_PRESENT_SEL;
	} else {
		tmp &= ~SDIOHOST_BIT_DATA_PRESENT_SEL;
	}

	switch (Command->RespType) {
	case SDMMC_RSP_NONE:
		tmp &= ~SDIOHOST_MASK_RESP_TYPE_SEL;
		tmp &= ~SDIOHOST_BIT_CMD_IDX_CHK_EN;
		tmp &= ~SDIOHOST_BIT_CMD_CRC_CHK_EN;
		break;
	case SDMMC_RSP_R1:
	case SDMMC_RSP_R5:
	case SDMMC_RSP_R6:
	case SDMMC_RSP_R7:
	default:
		tmp &= ~SDIOHOST_MASK_RESP_TYPE_SEL;
		tmp |= (SD_RESP_LEN48) << SDIOHOST_SHIFT_RESP_TYPE_SEL;
		tmp |= SDIOHOST_BIT_CMD_IDX_CHK_EN;
		tmp |= SDIOHOST_BIT_CMD_CRC_CHK_EN;
		break;
	case SDMMC_RSP_R2:
		tmp &= ~SDIOHOST_MASK_RESP_TYPE_SEL;
		tmp |= (SD_RESP_LEN136) << SDIOHOST_SHIFT_RESP_TYPE_SEL;
		tmp &= ~SDIOHOST_BIT_CMD_IDX_CHK_EN;
		tmp |= SDIOHOST_BIT_CMD_CRC_CHK_EN;
		break;
	case SDMMC_RSP_R3:
	case SDMMC_RSP_R4:
		tmp &= ~SDIOHOST_MASK_RESP_TYPE_SEL;
		tmp |= (SD_RESP_LEN48) << SDIOHOST_SHIFT_RESP_TYPE_SEL;
		tmp &= ~SDIOHOST_BIT_CMD_IDX_CHK_EN;
		tmp &= ~SDIOHOST_BIT_CMD_CRC_CHK_EN;
		break;
	case SDMMC_RSP_R1B:
	case SDMMC_RSP_R5B:
		tmp &= ~SDIOHOST_MASK_RESP_TYPE_SEL;
		tmp |= (SD_RESP_LEN48_BUSY) << SDIOHOST_SHIFT_RESP_TYPE_SEL;
		tmp |= SDIOHOST_BIT_CMD_IDX_CHK_EN;
		tmp |= SDIOHOST_BIT_CMD_CRC_CHK_EN;
		break;
	}
	SDIOx->SDIOHOST_CMD = tmp;
}

/**
 * @brief  Return the response received from the card for the last command
 * @param  SDIOx Pointer to SDIO host
 * @param  Response: Specifies the SDMMC response register.
 *          This parameter can be one of the following values:
 *            @arg SDIO_RESP0: Response Register 0
 *            @arg SDIO_RESP1: Response Register 1
 *            @arg SDIO_RESP2: Response Register 2
 *            @arg SDIO_RESP3: Response Register 3
 * @retval The Corresponding response register value
 */
u32 SDIO_GetResponse(SDIOHOST_TypeDef *SDIOx, u8 Response)
{
	u32 tmp;

	/* Check the parameters */
	assert_param(IS_SD_RESP(Response));

	/* Get the response */
	tmp = (u32)(&(SDIOx->SDIOHOST_RESP0)) + Response;

	return (*(u32 *)tmp);
}

u32 SDIO_CmdSendOpCond(SDIOHOST_TypeDef *SDIOx, u32 Ocr)
{
	SDIO_CmdInitTypeDef sdmmc_cmdinit;
	u32 errorstate;

	sdmmc_cmdinit.Argument = (u32)Ocr;
	sdmmc_cmdinit.CmdIndex = SDMMC_IO_SEND_OP_COND; // CMD5
	sdmmc_cmdinit.CmdType = SDMMC_CMD_NORMAL;
	sdmmc_cmdinit.RespType = SDMMC_RSP_R4;
	sdmmc_cmdinit.DataPresent = SDIO_TRANS_NO_DATA;
	SDIO_SendCommand(SDIOx, &sdmmc_cmdinit);

	/* Check for error conditions */
	errorstate = SDMMC_GetCmdResp4(SDIOx);

	return errorstate;
}

u32 SDIO_CmdRWDirect(SDIOHOST_TypeDef *SDIOx, u32 Argument)
{
	SDIO_CmdInitTypeDef sdmmc_cmdinit;
	u32 errorstate;

	sdmmc_cmdinit.Argument = Argument;
	sdmmc_cmdinit.CmdIndex = SDMMC_IO_RW_DIRECT; // CMD52
	sdmmc_cmdinit.CmdType = SDMMC_CMD_NORMAL;
	sdmmc_cmdinit.RespType = SDMMC_RSP_R5;
	sdmmc_cmdinit.DataPresent = SDIO_TRANS_NO_DATA;
	SDIO_SendCommand(SDIOx, &sdmmc_cmdinit);

	/* Check for error conditions */
	errorstate = SDMMC_GetCmdResp5(SDIOx);

	return errorstate;
}

u32 SDIO_CmdRWExtended(SDIOHOST_TypeDef *SDIOx, u32 Argument)
{
	SDIO_CmdInitTypeDef sdmmc_cmdinit;
	u32 errorstate;

	sdmmc_cmdinit.Argument = Argument;
	sdmmc_cmdinit.CmdIndex = SDMMC_IO_RW_EXTENDED; // CMD53
	sdmmc_cmdinit.CmdType = SDMMC_CMD_NORMAL;
	sdmmc_cmdinit.RespType = SDMMC_RSP_R5;
	sdmmc_cmdinit.DataPresent = SDIO_TRANS_WITH_DATA;
	SDIO_SendCommand(SDIOx, &sdmmc_cmdinit);

	/* Check for error conditions */
	errorstate = SDMMC_GetCmdResp5(SDIOx);

	return errorstate;
}

/**
 * @brief Get SDIOx present status.
 * @param SDIOx Pointer to SDIO host.
 * @return SDIOx present status.
 */
u32 SDIO_GetStatus(SDIOHOST_TypeDef *SDIOx)
{
	return SDIOx->SDIOHOST_PRESENT_STATE;
}

/**
 * @brief Enable or disable specified SDIOx normal interrupt signal.
 * @param SDIOx Pointer to SDIO host.
 * @param SDIO_IT Specified SDIOx normal interrupt sources to be enabled or disabled.
 * @param NewState New state of the specified SDIOx normal interrupt signal.
 * 		This parameter can be ENABLE or DISABLE.
 * @return None
 */
void SDIO_ConfigNormIntSig(SDIOHOST_TypeDef *SDIOx, u32 SDIO_IT, u32 NewState)
{
	if (NewState == ENABLE) {
		/* Enable selected SDIOx interrupt signal */
		SDIOx->SDIOHOST_NORMAL_SIG_EN |= SDIO_IT;
	} else {
		/* Disable selected SDIOx interrupt signal */
		SDIOx->SDIOHOST_NORMAL_SIG_EN &= (u32)~SDIO_IT;
	}
}

/**
 * @brief Enable or disable specified SDIOx error interrupt signal.
 * @param SDIOx Pointer to SDIO host.
 * @param SDIO_IT Specified SDIOx error interrupt sources to be enabled or disabled.
 * @param NewState New state of the specified SDIOx error interrupt signal.
 * 		This parameter can be ENABLE or DISABLE.
 * @return None
 */
void SDIO_ConfigErrIntSig(SDIOHOST_TypeDef *SDIOx, u32 SDIO_IT, u32 NewState)
{
	if (NewState == ENABLE) {
		/* Enable selected SDIOx interrupt signal */
		SDIOx->SDIOHOST_ERR_SIG_EN |= SDIO_IT;
	} else {
		/* Disable selected SDIOx interrupt signal */
		SDIOx->SDIOHOST_ERR_SIG_EN &= (u32)~SDIO_IT;
	}
}

/**
 * @brief Enable or disable specified SDIOx normal interrupt status.
 * @param SDIOx Pointer to SDIO host.
 * @param SDIO_IT Specified SDIOx normal interrupt sources to be enabled or disabled.
 * @param NewState New state of the specified SDIOx normal interrupt status.
 * 		This parameter can be ENABLE or DISABLE.
 * @return None
 */
void SDIO_ConfigNormIntSts(SDIOHOST_TypeDef *SDIOx, u32 SDIO_IT, u32 NewState)
{
	if (NewState == ENABLE) {
		/* Enable selected SDIOx interrupt status */
		SDIOx->SDIOHOST_NORMAL_INT_STATUS_EN |= SDIO_IT;
	} else {
		/* Disable selected SDIOx interrupt status */
		SDIOx->SDIOHOST_NORMAL_INT_STATUS_EN &= (u32)~SDIO_IT;
	}
}

/**
 * @brief Enable or disable specified SDIOx error interrupt status.
 * @param SDIOx Pointer to SDIO host.
 * @param SDIO_IT Specified SDIOx error interrupt sources to be enabled or disabled.
 * @param NewState New state of the specified SDIOx error interrupt status.
 * 		This parameter can be ENABLE or DISABLE.
 * @return None
 */
void SDIO_ConfigErrIntSts(SDIOHOST_TypeDef *SDIOx, u32 SDIO_IT, u32 NewState)
{
	if (NewState == ENABLE) {
		/* Enable selected SDIOx interrupt status */
		SDIOx->SDIOHOST_ERR_INT_STATUS_EN |= SDIO_IT;
	} else {
		/* Disable selected SDIOx interrupt status */
		SDIOx->SDIOHOST_ERR_INT_STATUS_EN &= (u32)~SDIO_IT;
	}
}

/**
 * @brief Get SDIOx normal status.
 * @param SDIOx Pointer to SDIO host.
 * @return SDIOx normal status.
 */
u32 SDIO_GetNormSts(SDIOHOST_TypeDef *SDIOx)
{
	return SDIOx->SDIOHOST_NORMAL_INT_STATUS;
}

/**
 * @brief Get SDIOx error status.
 * @param SDIOx Pointer to SDIO host.
 * @return SDIOx error status.
 */
u32 SDIO_GetErrSts(SDIOHOST_TypeDef *SDIOx)
{
	return SDIOx->SDIOHOST_ERR_INT_STATUS;
}

/**
 * @brief Clear SDIOx normal status.
 * @param SDIOx Pointer to SDIO host.
 * @param SDIO_IT Specified SDIOx normal status to be cleared.
 */
void SDIO_ClearNormSts(SDIOHOST_TypeDef *SDIOx, u32 SDIO_IT)
{
	SDIOx->SDIOHOST_NORMAL_INT_STATUS = SDIO_IT;
}

/**
 * @brief Clear SDIOx error status.
 * @param SDIOx Pointer to SDIO host.
 * @param SDIO_IT Specified SDIOx error status to be cleared.
 */
void SDIO_ClearErrSts(SDIOHOST_TypeDef *SDIOx, u32 SDIO_IT)
{
	SDIOx->SDIOHOST_ERR_INT_STATUS = SDIO_IT;
}

/**
 * @brief  Send the Data Block Length command and check the response
 * @param  SDIOx: Pointer to SDIO register base
 * @retval HAL status
 */
u32 SDMMC_CmdBlockLength(SDIOHOST_TypeDef *SDIOx, u32 BlockSize)
{
	SDIO_CmdInitTypeDef sdmmc_cmdinit;
	u32 errorstate;

	/* TODO: init struct */
	// CMD_Init(&sdmmc_cmdinit);

	/* Set Block Size for Card */
	sdmmc_cmdinit.Argument = (u32)BlockSize;
	sdmmc_cmdinit.CmdIndex = SDMMC_SET_BLOCKLEN; // CMD16
	sdmmc_cmdinit.CmdType = SDMMC_CMD_NORMAL;
	sdmmc_cmdinit.RespType = SDMMC_RSP_R1;
	sdmmc_cmdinit.DataPresent = SDIO_TRANS_NO_DATA;
	SDIO_SendCommand(SDIOx, &sdmmc_cmdinit);

	/* Check for error conditions */
	errorstate = SDMMC_GetCmdResp1(SDIOx, SDMMC_CMD_TIMEOUT);

	return errorstate;
}

/**
 * @brief  Send the Read Single Block command and check the response
 * @param  SDIOx: Pointer to SDIO register base
 * @retval HAL status
 */
u32 SDMMC_CmdReadSingleBlock(SDIOHOST_TypeDef *SDIOx, u32 ReadAdd)
{
	SDIO_CmdInitTypeDef sdmmc_cmdinit;
	u32 errorstate;

	/* Set Block Size for Card */
	sdmmc_cmdinit.Argument = (u32)ReadAdd;
	sdmmc_cmdinit.CmdIndex = SDMMC_READ_BLOCK_SINGLE; // CMD17
	sdmmc_cmdinit.CmdType = SDMMC_CMD_NORMAL;
	sdmmc_cmdinit.RespType = SDMMC_RSP_R1;
	sdmmc_cmdinit.DataPresent = SDIO_TRANS_WITH_DATA;
	SDIO_SendCommand(SDIOx, &sdmmc_cmdinit);

	/* Check for error conditions */
	errorstate = SDMMC_GetCmdResp1(SDIOx, SDMMC_CMD_TIMEOUT);

	return errorstate;
}

/**
 * @brief  Send the Read Multi Block command and check the response
 * @param  SDIOx: Pointer to SDIO register base
 * @retval HAL status
 */
u32 SDMMC_CmdReadMultiBlock(SDIOHOST_TypeDef *SDIOx, u32 ReadAdd)
{
	SDIO_CmdInitTypeDef sdmmc_cmdinit;
	u32 errorstate;

	/* Set Block Size for Card */
	sdmmc_cmdinit.Argument = (u32)ReadAdd;
	sdmmc_cmdinit.CmdIndex = SDMMC_READ_BLOCK_MULTIPLE; // CMD18
	sdmmc_cmdinit.CmdType = SDMMC_CMD_NORMAL;
	sdmmc_cmdinit.RespType = SDMMC_RSP_R1;
	sdmmc_cmdinit.DataPresent = SDIO_TRANS_WITH_DATA;
	SDIO_SendCommand(SDIOx, &sdmmc_cmdinit);

	/* Check for error conditions */
	errorstate = SDMMC_GetCmdResp1(SDIOx, SDMMC_CMD_TIMEOUT);

	return errorstate;
}

/**
 * @brief  Send the Write Single Block command and check the response
 * @param  SDIOx: Pointer to SDIO register base
 * @retval HAL status
 */
u32 SDMMC_CmdWriteSingleBlock(SDIOHOST_TypeDef *SDIOx, u32 WriteAdd)
{
	SDIO_CmdInitTypeDef sdmmc_cmdinit;
	u32 errorstate;

	/* Set Block Size for Card */
	sdmmc_cmdinit.Argument = (u32)WriteAdd;
	sdmmc_cmdinit.CmdIndex = SDMMC_WRITE_BLOCK_SINGLE; // CMD24
	sdmmc_cmdinit.CmdType = SDMMC_CMD_NORMAL;
	sdmmc_cmdinit.RespType = SDMMC_RSP_R1;
	sdmmc_cmdinit.DataPresent = SDIO_TRANS_WITH_DATA;
	SDIO_SendCommand(SDIOx, &sdmmc_cmdinit);

	/* Check for error conditions */
	errorstate = SDMMC_GetCmdResp1(SDIOx, SDMMC_CMD_TIMEOUT);

	return errorstate;
}

/**
 * @brief  Send the Write Multi Block command and check the response
 * @param  SDIOx: Pointer to SDIO register base
 * @retval HAL status
 */
u32 SDMMC_CmdWriteMultiBlock(SDIOHOST_TypeDef *SDIOx, u32 WriteAdd)
{
	SDIO_CmdInitTypeDef sdmmc_cmdinit;
	u32 errorstate;

	/* Set Block Size for Card */
	sdmmc_cmdinit.Argument = (u32)WriteAdd;
	sdmmc_cmdinit.CmdIndex = SDMMC_WRITE_BLOCK_MULTIPLE; // CMD25
	sdmmc_cmdinit.CmdType = SDMMC_CMD_NORMAL;
	sdmmc_cmdinit.RespType = SDMMC_RSP_R1;
	sdmmc_cmdinit.DataPresent = SDIO_TRANS_WITH_DATA;
	SDIO_SendCommand(SDIOx, &sdmmc_cmdinit);

	/* Check for error conditions */
	errorstate = SDMMC_GetCmdResp1(SDIOx, SDMMC_CMD_TIMEOUT);

	return errorstate;
}

/**
 * @brief  Send the Start Address Erase command for SD and check the response
 * @param  SDIOx: Pointer to SDIO register base
 * @retval HAL status
 */
u32 SDMMC_CmdSDEraseStartAdd(SDIOHOST_TypeDef *SDIOx, u32 StartAdd)
{
	SDIO_CmdInitTypeDef sdmmc_cmdinit;
	u32 errorstate;

	/* Set Block Size for Card */
	sdmmc_cmdinit.Argument = (u32)StartAdd;
	sdmmc_cmdinit.CmdIndex = SDMMC_SD_ERASE_GROUP_START; // 32
	sdmmc_cmdinit.CmdType = SDMMC_CMD_NORMAL;
	sdmmc_cmdinit.RespType = SDMMC_RSP_R1;
	sdmmc_cmdinit.DataPresent = SDIO_TRANS_NO_DATA;
	SDIO_SendCommand(SDIOx, &sdmmc_cmdinit);

	/* Check for error conditions */
	errorstate = SDMMC_GetCmdResp1(SDIOx, SDMMC_CMD_TIMEOUT);

	return errorstate;
}

/**
 * @brief  Send the End Address Erase command for SD and check the response
 * @param  SDIOx: Pointer to SDIO register base
 * @retval HAL status
 */
u32 SDMMC_CmdSDEraseEndAdd(SDIOHOST_TypeDef *SDIOx, u32 EndAdd)
{
	SDIO_CmdInitTypeDef sdmmc_cmdinit;
	u32 errorstate;

	/* Set Block Size for Card */
	sdmmc_cmdinit.Argument = (u32)EndAdd;
	sdmmc_cmdinit.CmdIndex = SDMMC_SD_ERASE_GROUP_END; // 33
	sdmmc_cmdinit.CmdType = SDMMC_CMD_NORMAL;
	sdmmc_cmdinit.RespType = SDMMC_RSP_R1;
	sdmmc_cmdinit.DataPresent = SDIO_TRANS_NO_DATA;
	SDIO_SendCommand(SDIOx, &sdmmc_cmdinit);

	/* Check for error conditions */
	errorstate = SDMMC_GetCmdResp1(SDIOx, SDMMC_CMD_TIMEOUT);

	return errorstate;
}

/**
 * @brief  Send the Erase command and check the response
 * @param  SDIOx: Pointer to SDIO register base
 * @retval HAL status
 */
u32 SDMMC_CmdErase(SDIOHOST_TypeDef *SDIOx)
{
	SDIO_CmdInitTypeDef sdmmc_cmdinit;
	u32 errorstate;

	/* Set Block Size for Card */
	sdmmc_cmdinit.Argument = 0U;
	sdmmc_cmdinit.CmdIndex = SDMMC_ERASE; // CMD38
	sdmmc_cmdinit.CmdType = SDMMC_CMD_NORMAL;
	sdmmc_cmdinit.RespType = SDMMC_RSP_R1B;
	sdmmc_cmdinit.DataPresent = SDIO_TRANS_NO_DATA;
	SDIO_SendCommand(SDIOx, &sdmmc_cmdinit);

	/* Check for error conditions */
	errorstate = SDMMC_GetCmdResp1B(SDIOx, SDMMC_ERASE_TIMEOUT);

	return errorstate;
}

/**
 * @brief  Send the Stop Transfer command and check the response.
 * @param  SDIOx: Pointer to SDIO register base
 * @retval HAL status
 */
u32 SDMMC_CmdStopTransfer(SDIOHOST_TypeDef *SDIOx)
{
	SDIO_CmdInitTypeDef sdmmc_cmdinit;
	u32 errorstate;

	/* Send CMD12 STOP_TRANSMISSION  */
	sdmmc_cmdinit.Argument = 0U;
	sdmmc_cmdinit.CmdIndex = SDMMC_STOP_TRANSMISSION; // CMD12
	sdmmc_cmdinit.CmdType = SDMMC_CMD_ABORT;
	sdmmc_cmdinit.RespType = SDMMC_RSP_R1B;
	sdmmc_cmdinit.DataPresent = SDIO_TRANS_NO_DATA;
	SDIO_SendCommand(SDIOx, &sdmmc_cmdinit);

	/* Check for error conditions */
	errorstate = SDMMC_GetCmdResp1B(SDIOx, SDMMC_STOP_TRANS_TIMEOUT);

	return errorstate;
}

/**
 * @brief  Send the Select Deselect command and check the response.
 * @param  SDIOx: Pointer to SDIO register base
 * @param  addr: Address of the card to be selected
 * @retval HAL status
 */
u32 SDMMC_CmdSelDesel(SDIOHOST_TypeDef *SDIOx, u32 Addr)
{
	SDIO_CmdInitTypeDef sdmmc_cmdinit;
	u32 errorstate;

	/* Send CMD7 SDMMC_SEL_DESEL_CARD */
	sdmmc_cmdinit.Argument = Addr;
	sdmmc_cmdinit.CmdIndex = SDMMC_SELECT_CARD; // 7
	sdmmc_cmdinit.CmdType = SDMMC_CMD_NORMAL;
	sdmmc_cmdinit.RespType = SDMMC_RSP_R1B;
	sdmmc_cmdinit.DataPresent = SDIO_TRANS_NO_DATA;
	SDIO_SendCommand(SDIOx, &sdmmc_cmdinit);

	/* Check for error conditions */
	errorstate = SDMMC_GetCmdResp1(SDIOx, SDMMC_CMD_TIMEOUT);

	return errorstate;
}

/**
 * @brief  Send the Go Idle State command and check the response.
 * @param  SDIOx: Pointer to SDIO register base
 * @retval HAL status
 */
u32 SDMMC_CmdGoIdleState(SDIOHOST_TypeDef *SDIOx)
{
	SDIO_CmdInitTypeDef sdmmc_cmdinit;
	u32 errorstate;

	sdmmc_cmdinit.Argument = 0U;
	sdmmc_cmdinit.CmdIndex = SDMMC_GO_IDLE_STATE; // CMD0
	sdmmc_cmdinit.CmdType = SDMMC_CMD_NORMAL;
	sdmmc_cmdinit.RespType = SDMMC_RSP_NONE;
	sdmmc_cmdinit.DataPresent = SDIO_TRANS_NO_DATA;
	SDIO_SendCommand(SDIOx, &sdmmc_cmdinit);

	/* Check for error conditions */
	errorstate = SDMMC_GetCmdError(SDIOx);

	return errorstate;
}

/**
 * @brief  Send the Operating Condition command and check the response.
 * @param  SDIOx: Pointer to SDIO register base
 * @retval HAL status
 */
u32 SDMMC_CmdOperCond(SDIOHOST_TypeDef *SDIOx)
{
	SDIO_CmdInitTypeDef sdmmc_cmdinit;
	u32 errorstate;

	/* Send CMD8 to verify SD card interface operating condition */
	/* Argument: - [31:12]: Reserved (shall be set to '0')
	- [11:8]: Supply Voltage (VHS) 0x1 (Range: 2.7-3.6 V)
	- [7:0]: Check Pattern (recommended 0xAA) */
	/* CMD Response: R7 */
	sdmmc_cmdinit.Argument = SDMMC_CHECK_PATTERN;
	sdmmc_cmdinit.CmdIndex = SDMMC_SEND_EXT_CSD; // CMD8
	sdmmc_cmdinit.CmdType = SDMMC_CMD_NORMAL;
	sdmmc_cmdinit.RespType = SDMMC_RSP_R7;
	sdmmc_cmdinit.DataPresent = SDIO_TRANS_NO_DATA;
	SDIO_SendCommand(SDIOx, &sdmmc_cmdinit);

	/* Check for error conditions */
	errorstate = SDMMC_GetCmdResp7(SDIOx); // R7

	return errorstate;
}

/**
 * @brief  Send the Application command to verify that that the next command
 *         is an application specific com-mand rather than a standard command
 *         and check the response.
 * @param  SDIOx: Pointer to SDIO register base
 * @param  Argument: Command Argument
 * @retval HAL status
 */
u32 SDMMC_CmdAppCommand(SDIOHOST_TypeDef *SDIOx, u32 Argument)
{
	SDIO_CmdInitTypeDef sdmmc_cmdinit;
	u32 errorstate;

	sdmmc_cmdinit.Argument = (u32)Argument;
	sdmmc_cmdinit.CmdIndex = SDMMC_APP_CMD; // CMD55
	sdmmc_cmdinit.CmdType = SDMMC_CMD_NORMAL;
	sdmmc_cmdinit.RespType = SDMMC_RSP_R1;
	sdmmc_cmdinit.DataPresent = SDIO_TRANS_NO_DATA;
	SDIO_SendCommand(SDIOx, &sdmmc_cmdinit);

	/* Check for error conditions */
	/* If there is a HAL_ERR_PARA, it is a MMC card, else
	it is a SD card: SD card 2.0 (voltage range mismatch)
	   or SD card 1.x */
	errorstate = SDMMC_GetCmdResp1(SDIOx, SDMMC_CMD_TIMEOUT);

	return errorstate;
}

/**
 * @brief  Send the command asking the accessed card to send its operating
 *         condition register (OCR)
 * @param  SDIOx: Pointer to SDIO register base
 * @param  Argument: Command Argument
 * @retval HAL status
 */
u32 SDMMC_CmdAppOperCommand(SDIOHOST_TypeDef *SDIOx, u32 Argument)
{
	SDIO_CmdInitTypeDef sdmmc_cmdinit;
	u32 errorstate;

	sdmmc_cmdinit.Argument = SDMMC_VOLTAGE_WINDOW_SD | Argument;
	sdmmc_cmdinit.CmdIndex = SDMMC_APP_OP_COND; // ACMD41(CMD55 + CMD41)
	sdmmc_cmdinit.CmdType = SDMMC_CMD_NORMAL;
	sdmmc_cmdinit.RespType = SDMMC_RSP_R3;
	sdmmc_cmdinit.DataPresent = SDIO_TRANS_NO_DATA;
	SDIO_SendCommand(SDIOx, &sdmmc_cmdinit);

	/* Check for error conditions */
	errorstate = SDMMC_GetCmdResp3(SDIOx);

	return errorstate;
}

/**
 * @brief  Send the Bus Width command and check the response.
 * @param  SDIOx: Pointer to SDIO register base
 * @param  BusWidth: BusWidth
 * @retval HAL status
 */
u32 SDMMC_CmdBusWidth(SDIOHOST_TypeDef *SDIOx, u32 BusWidth)
{
	SDIO_CmdInitTypeDef sdmmc_cmdinit;
	u32 errorstate;

	sdmmc_cmdinit.Argument = (u32)BusWidth;
	sdmmc_cmdinit.CmdIndex = SDMMC_APP_SET_BUS_WIDTH; // ACMD6(CMD55 + CMD6)
	sdmmc_cmdinit.CmdType = SDMMC_CMD_NORMAL;
	sdmmc_cmdinit.RespType = SDMMC_RSP_R1;
	sdmmc_cmdinit.DataPresent = SDIO_TRANS_NO_DATA;
	SDIO_SendCommand(SDIOx, &sdmmc_cmdinit);

	/* Check for error conditions */
	errorstate = SDMMC_GetCmdResp1(SDIOx, SDMMC_CMD_TIMEOUT);

	return errorstate;
}

/**
 * @brief  Send the set write block erase count command and check the response.
 * @param  SDIOx: Pointer to SDIO register base
 * @param  BlockCnt: Number of write blocks to be pre-erased before writing.
 * @retval HAL status
 */
u32 SDMMC_CmdSetWrBlkEraseCnt(SDIOHOST_TypeDef *SDIOx, u32 BlockCnt)
{
	SDIO_CmdInitTypeDef sdmmc_cmdinit;
	u32 errorstate;

	sdmmc_cmdinit.Argument = (u32)BlockCnt;
	sdmmc_cmdinit.CmdIndex = SDMMC_APP_SET_WR_BLK_ERASE_CNT; // ACMD23(CMD55 + CMD23)
	sdmmc_cmdinit.CmdType = SDMMC_CMD_NORMAL;
	sdmmc_cmdinit.RespType = SDMMC_RSP_R1;
	sdmmc_cmdinit.DataPresent = SDIO_TRANS_NO_DATA;
	SDIO_SendCommand(SDIOx, &sdmmc_cmdinit);

	/* Check for error conditions */
	errorstate = SDMMC_GetCmdResp1(SDIOx, SDMMC_CMD_TIMEOUT);

	return errorstate;
}

/**
 * @brief  Send the Send SCR command and check the response.
 * @param  SDIOx: Pointer to SDIO register base
 * @retval HAL status
 */
u32 SDMMC_CmdSendSCR(SDIOHOST_TypeDef *SDIOx)
{
	SDIO_CmdInitTypeDef sdmmc_cmdinit;
	u32 errorstate;

	/* Send CMD51 SDMMC_APP_SEND_SCR */
	sdmmc_cmdinit.Argument = 0U;
	sdmmc_cmdinit.CmdIndex = SDMMC_APP_SEND_SCR; // ACMD51(CMD55 + CMD51)
	sdmmc_cmdinit.CmdType = SDMMC_CMD_NORMAL;
	sdmmc_cmdinit.RespType = SDMMC_RSP_R1;
	sdmmc_cmdinit.DataPresent = SDIO_TRANS_WITH_DATA;
	SDIO_SendCommand(SDIOx, &sdmmc_cmdinit);

	/* Check for error conditions */
	errorstate = SDMMC_GetCmdResp1(SDIOx, SDMMC_CMD_TIMEOUT);

	return errorstate;
}

/**
 * @brief  Send the Send CID command and check the response.
 * @param  SDIOx: Pointer to SDIO register base
 * @retval HAL status
 */
u32 SDMMC_CmdSendCID(SDIOHOST_TypeDef *SDIOx)
{
	SDIO_CmdInitTypeDef sdmmc_cmdinit;
	u32 errorstate;

	/* Send CMD2 ALL_SEND_CID */
	sdmmc_cmdinit.Argument = 0U;
	sdmmc_cmdinit.CmdIndex = SDMMC_ALL_SEND_CID; // CMD2
	sdmmc_cmdinit.CmdType = SDMMC_CMD_NORMAL;
	sdmmc_cmdinit.RespType = SDMMC_RSP_R2;
	sdmmc_cmdinit.DataPresent = SDIO_TRANS_NO_DATA;
	SDIO_SendCommand(SDIOx, &sdmmc_cmdinit);

	/* Check for error conditions */
	errorstate = SDMMC_GetCmdResp2(SDIOx);

	return errorstate;
}

/**
 * @brief  Send the Send CSD command and check the response.
 * @param  SDIOx: Pointer to SDIO register base
 * @param  Argument: Command Argument
 * @retval HAL status
 */
u32 SDMMC_CmdSendCSD(SDIOHOST_TypeDef *SDIOx, u32 Argument)
{
	SDIO_CmdInitTypeDef sdmmc_cmdinit;
	u32 errorstate;

	/* Send CMD9 SEND_CSD */
	sdmmc_cmdinit.Argument = Argument;
	sdmmc_cmdinit.CmdIndex = SDMMC_SEND_CSD; // CMD9
	sdmmc_cmdinit.CmdType = SDMMC_CMD_NORMAL;
	sdmmc_cmdinit.RespType = SDMMC_RSP_R2;
	sdmmc_cmdinit.DataPresent = SDIO_TRANS_NO_DATA;
	SDIO_SendCommand(SDIOx, &sdmmc_cmdinit);

	/* Check for error conditions */
	errorstate = SDMMC_GetCmdResp2(SDIOx);

	return errorstate;
}

/**
 * @brief  Send the Send CSD command and check the response.
 * @param  SDIOx: Pointer to SDIO register base
 * @param  pRCA: Card RCA
 * @retval HAL status
 */
u32 SDMMC_CmdSetRelAdd(SDIOHOST_TypeDef *SDIOx, u16 *pRCA)
{
	SDIO_CmdInitTypeDef sdmmc_cmdinit;
	u32 errorstate;

	/* Send CMD3 SDMMC_CMD_SET_REL_ADDR */
	sdmmc_cmdinit.Argument = 0U;
	sdmmc_cmdinit.CmdIndex = SDMMC_SET_RELATIVE_ADDR; // CMD3
	sdmmc_cmdinit.CmdType = SDMMC_CMD_NORMAL;
	sdmmc_cmdinit.RespType = SDMMC_RSP_R6;
	sdmmc_cmdinit.DataPresent = SDIO_TRANS_NO_DATA;
	SDIO_SendCommand(SDIOx, &sdmmc_cmdinit);

	/* Check for error conditions */
	errorstate = SDMMC_GetCmdResp6(SDIOx, pRCA);

	return errorstate;
}

/**
 * @brief  Send the Status command and check the response.
 * @param  SDIOx: Pointer to SDIO register base
 * @param  Argument: Command Argument
 * @retval HAL status
 */
u32 SDMMC_CmdSendStatus(SDIOHOST_TypeDef *SDIOx, u32 Argument)
{
	SDIO_CmdInitTypeDef sdmmc_cmdinit;
	u32 errorstate;

	sdmmc_cmdinit.Argument = Argument;
	sdmmc_cmdinit.CmdIndex = SDMMC_SEND_STATUS; // CMD13
	sdmmc_cmdinit.CmdType = SDMMC_CMD_NORMAL;
	sdmmc_cmdinit.RespType = SDMMC_RSP_R1;
	sdmmc_cmdinit.DataPresent = SDIO_TRANS_NO_DATA;
	SDIO_SendCommand(SDIOx, &sdmmc_cmdinit);

	/* Check for error conditions */
	errorstate = SDMMC_GetCmdResp1(SDIOx, SDMMC_CMD_TIMEOUT);

	return errorstate;
}

/**
 * @brief  Send the Status register command and check the response.
 * @param  SDIOx: Pointer to SDIO register base
 * @retval HAL status
 */
u32 SDMMC_CmdStatusRegister(SDIOHOST_TypeDef *SDIOx)
{
	SDIO_CmdInitTypeDef sdmmc_cmdinit;
	u32 errorstate;

	sdmmc_cmdinit.Argument = 0U;
	sdmmc_cmdinit.CmdIndex = SDMMC_APP_SDMMC_STATUS; // ACMD13(CMD55 + CMD13)
	sdmmc_cmdinit.CmdType = SDMMC_CMD_NORMAL;
	sdmmc_cmdinit.RespType = SDMMC_RSP_R1;
	sdmmc_cmdinit.DataPresent = SDIO_TRANS_WITH_DATA;
	SDIO_SendCommand(SDIOx, &sdmmc_cmdinit);

	/* Check for error conditions */
	errorstate = SDMMC_GetCmdResp1(SDIOx, SDMMC_CMD_TIMEOUT);

	return errorstate;
}

/**
 * @brief  Sends host capacity support information and activates the card's
 *         initialization process. Send MMC_SEND_OP_COND command
 * @param  SDIOx: Pointer to SDIO register base
 * @parame Argument: Argument used for the command
 * @retval HAL status
 */
u32 SDMMC_CmdOpCondition(SDIOHOST_TypeDef *SDIOx, u32 Argument)
{
	SDIO_CmdInitTypeDef sdmmc_cmdinit;
	u32 errorstate;

	sdmmc_cmdinit.Argument = Argument;
	sdmmc_cmdinit.CmdIndex = SDMMC_SEND_OP_COND; // CMD1
	sdmmc_cmdinit.CmdType = SDMMC_CMD_NORMAL;
	sdmmc_cmdinit.RespType = SDMMC_RSP_R2;
	sdmmc_cmdinit.DataPresent = SDIO_TRANS_NO_DATA;
	SDIO_SendCommand(SDIOx, &sdmmc_cmdinit);

	/* Check for error conditions */
	errorstate = SDMMC_GetCmdResp3(SDIOx);

	return errorstate;
}

/**
 * @brief  Checks switchable function and switch card function.
 * MMC_SWITCH command
 * @param  SDIOx: Pointer to SDIO register base
 * @parame Argument: Argument used for the command
 * @retval HAL status
 */
u32 SDMMC_CmdSwitch(SDIOHOST_TypeDef *SDIOx, u32 Argument)
{
	SDIO_CmdInitTypeDef sdmmc_cmdinit;
	u32 errorstate;

	/* Send CMD6 to activate SDR50 Mode and Power Limit 1.44W */
	/* CMD Response: R1 */
	sdmmc_cmdinit.Argument = Argument;	 /* SDMMC_SDR25_SWITCH_PATTERN */
	sdmmc_cmdinit.CmdIndex = SDMMC_SWITCH; // 6
	sdmmc_cmdinit.CmdType = SDMMC_CMD_NORMAL;
	sdmmc_cmdinit.RespType = SDMMC_RSP_R1;
	sdmmc_cmdinit.DataPresent = SDIO_TRANS_WITH_DATA;
	SDIO_SendCommand(SDIOx, &sdmmc_cmdinit);

	/* Check for error conditions */
	errorstate = SDMMC_GetCmdResp1(SDIOx, SDMMC_CMD_TIMEOUT);

	return errorstate;
}

/**
 * @brief  Send the Send EXT_CSD command and check the response.
 * @param  SDIOx Pointer to SDIO host
 * @param  Argument Command Argument
 * @retval HAL status
 */
u32 SDMMC_CmdSendEXTCSD(SDIOHOST_TypeDef *SDIOx, u32 Argument)
{
	SDIO_CmdInitTypeDef sdmmc_cmdinit;
	u32 errorstate;

	/* Send CMD9 SEND_CSD */
	sdmmc_cmdinit.Argument = Argument;
	sdmmc_cmdinit.CmdIndex = SDMMC_SEND_EXT_CSD; // cmd8 SD CARD
	sdmmc_cmdinit.CmdType = SDMMC_CMD_NORMAL;
	sdmmc_cmdinit.RespType = SDMMC_RSP_R2;
	sdmmc_cmdinit.DataPresent = SDIO_TRANS_NO_DATA;
	SDIO_SendCommand(SDIOx, &sdmmc_cmdinit);

	/* Check for error conditions */
	errorstate = SDMMC_GetCmdResp1(SDIOx, SDMMC_CMD_TIMEOUT);

	return errorstate;
}

/**
 * @brief  Checks for error conditions for CMD0.
 * @param  SDIOx Pointer to SDIO host
 * @retval SD Card error state
 */
u32 SDMMC_GetCmdError(SDIOHOST_TypeDef *SDIOx)
{
	/* 8 is the number of required instruction cycles for the below loop
	statement. The SDMMC_CMD_TIMEOUT is expressed in us */
	u32 count = SDMMC_CMD_TIMEOUT * (SystemCoreClock / 8U / 1000000U);

	do {
		if (count-- == 0U) {
			return SD_ERROR_TIMEOUT;
		}

	} while (!(SDIO_GetNormSts(SDIOx) & SDIOHOST_BIT_CMD_COMPLETE));

	SDIO_ClearNormSts(SDIOx, SDIOHOST_BIT_CMD_COMPLETE);

	return SD_ERROR_NONE;
}

/**
 * @brief  Checks for error conditions for R1 response.
 * @param  SDIOx Pointer to SDIO host
 * @param  Timeout: Timoout value in ms.
 * @retval SD Card error state
 */
u32 SDMMC_GetCmdResp1(SDIOHOST_TypeDef *SDIOx, u32 Timeout)
{
	u32 response_r0;
	u32 norm_int, err_int;

	/* 8 is the number of required instruction cycles for the below loop
	statement. The Timeout is expressed in us */
	u32 count = Timeout * (SystemCoreClock / 8U / 1000000U);

	do {
		if (count-- == 0U) {
			return SD_ERROR_TIMEOUT;
		}
		norm_int = SDIO_GetNormSts(SDIOx);
		err_int = SDIO_GetErrSts(SDIOx);
	} while (!(norm_int & SDIOHOST_BIT_CMD_COMPLETE || err_int & SDIO_CMD_ERR));

	if (norm_int & SDIOHOST_BIT_CMD_COMPLETE) {
		SDIO_ClearNormSts(SDIOx, SDIOHOST_BIT_CMD_COMPLETE);
	}

	if (err_int & SDIO_CMD_ERR) {
		SDIO_ClearErrSts(SDIOx, SDIO_CMD_ERR);
		if (err_int & SDIOHOST_BIT_CMD_TIMEOUT_ERR) {
			return SD_ERROR_CMD_RSP_TIMEOUT;
		} else {
			return SD_ERROR_CMD_CRC_FAIL;
		}
	}

	/* We have received response, retrieve it for analysis  */
	response_r0 = SDIO_GetResponse(SDIOx, SDIO_RESP0);

	if ((response_r0 & SDMMC_R1_ERRORBITS) == SDMMC_ALLZERO) {
		return SD_ERROR_NONE;
	} else if ((response_r0 & SDMMC_R1_ADDR_OUT_OF_RANGE) == SDMMC_R1_ADDR_OUT_OF_RANGE) {
		return SD_ERROR_ADDR_OUT_OF_RANGE;
	} else if ((response_r0 & SDMMC_R1_ADDR_MISALIGNED) == SDMMC_R1_ADDR_MISALIGNED) {
		return SD_ERROR_ADDR_MISALIGNED;
	} else if ((response_r0 & SDMMC_R1_BLOCK_LEN_ERR) == SDMMC_R1_BLOCK_LEN_ERR) {
		return SD_ERROR_BLOCK_LEN_ERR;
	} else if ((response_r0 & SDMMC_R1_ERASE_SEQ_ERR) == SDMMC_R1_ERASE_SEQ_ERR) {
		return SD_ERROR_ERASE_SEQ_ERR;
	} else if ((response_r0 & SDMMC_R1_BAD_ERASE_PARAM) == SDMMC_R1_BAD_ERASE_PARAM) {
		return SD_ERROR_BAD_ERASE_PARAM;
	} else if ((response_r0 & SDMMC_R1_WRITE_PROT_VIOLATION) == SDMMC_R1_WRITE_PROT_VIOLATION) {
		return SD_ERROR_WRITE_PROT_VIOLATION;
	} else if ((response_r0 & SDMMC_R1_LOCK_UNLOCK_FAILED) == SDMMC_R1_LOCK_UNLOCK_FAILED) {
		return SD_ERROR_LOCK_UNLOCK_FAILED;
	} else if ((response_r0 & SDMMC_R1_COM_CRC_FAILED) == SDMMC_R1_COM_CRC_FAILED) {
		return SD_ERROR_COM_CRC_FAILED;
	} else if ((response_r0 & SDMMC_R1_ILLEGAL_CMD) == SDMMC_R1_ILLEGAL_CMD) {
		return SD_ERROR_ILLEGAL_CMD;
	} else if ((response_r0 & SDMMC_R1_CARD_ECC_FAILED) == SDMMC_R1_CARD_ECC_FAILED) {
		return SD_ERROR_CARD_ECC_FAILED;
	} else if ((response_r0 & SDMMC_R1_CC_ERROR) == SDMMC_R1_CC_ERROR) {
		return SD_ERROR_CC_ERR;
	} else if ((response_r0 & SDMMC_R1_STREAM_READ_UNDERRUN) == SDMMC_R1_STREAM_READ_UNDERRUN) {
		return SD_ERROR_STREAM_READ_UNDERRUN;
	} else if ((response_r0 & SDMMC_R1_STREAM_WRITE_OVERRUN) == SDMMC_R1_STREAM_WRITE_OVERRUN) {
		return SD_ERROR_STREAM_WRITE_OVERRUN;
	} else if ((response_r0 & SDMMC_R1_CID_CSDMMC_OVERWRITE) == SDMMC_R1_CID_CSDMMC_OVERWRITE) {
		return SD_ERROR_CID_CSDMMC_OVERWRITE;
	} else if ((response_r0 & SDMMC_R1_WP_ERASE_SKIP) == SDMMC_R1_WP_ERASE_SKIP) {
		return SD_ERROR_WP_ERASE_SKIP;
	} else if ((response_r0 & SDMMC_R1_CARD_ECC_DISABLED) == SDMMC_R1_CARD_ECC_DISABLED) {
		return SD_ERROR_CARD_ECC_DISABLED;
	} else if ((response_r0 & SDMMC_R1_ERASE_RESET) == SDMMC_R1_ERASE_RESET) {
		return SD_ERROR_ERASE_RESET;
	} else if ((response_r0 & SDMMC_R1_AKE_SEQ_ERROR) == SDMMC_R1_AKE_SEQ_ERROR) {
		return SD_ERROR_AKE_SEQ_ERR;
	} else {
		return SD_ERROR_GENERAL_UNKNOWN_ERR;
	}
}

/**
 * @brief  Checks for error conditions for R1 response.
 * @param  SDIOx Pointer to SDIO host
 * @param  Timeout: Timoout value in ms.
 * @retval SD Card error state
 */
u32 SDMMC_GetCmdResp1B(SDIOHOST_TypeDef *SDIOx, u32 Timeout)
{
	u32 response_r0;
	u32 norm_int, err_int;

	/* 8 is the number of required instruction cycles for the below loop
	statement. The Timeout is expressed in us */
	u32 count = Timeout * (SystemCoreClock / 8U / 1000000U);

	do {
		if (count-- == 0U) {
			return SD_ERROR_TIMEOUT;
		}
		norm_int = SDIO_GetNormSts(SDIOx);
		err_int = SDIO_GetErrSts(SDIOx);
		/* KW: SDIOHOST_BIT_XFER_COMPLETE is set when a command with busy is completed */
		/* KW: SDIOHOST_BIT_DATA_TIMEOUT_ERR is set when busy timeout for r1b,r5b type */
	} while (!(norm_int & SDIOHOST_BIT_XFER_COMPLETE || err_int & (SDIO_CMD_ERR | SDIOHOST_BIT_DATA_TIMEOUT_ERR)));

	if (norm_int & SDIOHOST_BIT_CMD_COMPLETE) {
		SDIO_ClearNormSts(SDIOx, SDIOHOST_BIT_CMD_COMPLETE);
	}

	if (err_int & SDIO_CMD_ERR) {
		SDIO_ClearErrSts(SDIOx, SDIO_CMD_ERR);
		if (err_int & SDIOHOST_BIT_CMD_TIMEOUT_ERR) {
			return SD_ERROR_CMD_RSP_TIMEOUT;
		} else {
			return SD_ERROR_CMD_CRC_FAIL;
		}
	}

	/* We have received response, retrieve it for analysis  */
	response_r0 = SDIO_GetResponse(SDIOx, SDIO_RESP0);

	if ((response_r0 & SDMMC_R1_ERRORBITS) == SDMMC_ALLZERO) {
		return SD_ERROR_NONE;
	} else if ((response_r0 & SDMMC_R1_ADDR_OUT_OF_RANGE) == SDMMC_R1_ADDR_OUT_OF_RANGE) {
		return SD_ERROR_ADDR_OUT_OF_RANGE;
	} else if ((response_r0 & SDMMC_R1_ADDR_MISALIGNED) == SDMMC_R1_ADDR_MISALIGNED) {
		return SD_ERROR_ADDR_MISALIGNED;
	} else if ((response_r0 & SDMMC_R1_BLOCK_LEN_ERR) == SDMMC_R1_BLOCK_LEN_ERR) {
		return SD_ERROR_BLOCK_LEN_ERR;
	} else if ((response_r0 & SDMMC_R1_ERASE_SEQ_ERR) == SDMMC_R1_ERASE_SEQ_ERR) {
		return SD_ERROR_ERASE_SEQ_ERR;
	} else if ((response_r0 & SDMMC_R1_BAD_ERASE_PARAM) == SDMMC_R1_BAD_ERASE_PARAM) {
		return SD_ERROR_BAD_ERASE_PARAM;
	} else if ((response_r0 & SDMMC_R1_WRITE_PROT_VIOLATION) == SDMMC_R1_WRITE_PROT_VIOLATION) {
		return SD_ERROR_WRITE_PROT_VIOLATION;
	} else if ((response_r0 & SDMMC_R1_LOCK_UNLOCK_FAILED) == SDMMC_R1_LOCK_UNLOCK_FAILED) {
		return SD_ERROR_LOCK_UNLOCK_FAILED;
	} else if ((response_r0 & SDMMC_R1_COM_CRC_FAILED) == SDMMC_R1_COM_CRC_FAILED) {
		return SD_ERROR_COM_CRC_FAILED;
	} else if ((response_r0 & SDMMC_R1_ILLEGAL_CMD) == SDMMC_R1_ILLEGAL_CMD) {
		return SD_ERROR_ILLEGAL_CMD;
	} else if ((response_r0 & SDMMC_R1_CARD_ECC_FAILED) == SDMMC_R1_CARD_ECC_FAILED) {
		return SD_ERROR_CARD_ECC_FAILED;
	} else if ((response_r0 & SDMMC_R1_CC_ERROR) == SDMMC_R1_CC_ERROR) {
		return SD_ERROR_CC_ERR;
	} else if ((response_r0 & SDMMC_R1_STREAM_READ_UNDERRUN) == SDMMC_R1_STREAM_READ_UNDERRUN) {
		return SD_ERROR_STREAM_READ_UNDERRUN;
	} else if ((response_r0 & SDMMC_R1_STREAM_WRITE_OVERRUN) == SDMMC_R1_STREAM_WRITE_OVERRUN) {
		return SD_ERROR_STREAM_WRITE_OVERRUN;
	} else if ((response_r0 & SDMMC_R1_CID_CSDMMC_OVERWRITE) == SDMMC_R1_CID_CSDMMC_OVERWRITE) {
		return SD_ERROR_CID_CSDMMC_OVERWRITE;
	} else if ((response_r0 & SDMMC_R1_WP_ERASE_SKIP) == SDMMC_R1_WP_ERASE_SKIP) {
		return SD_ERROR_WP_ERASE_SKIP;
	} else if ((response_r0 & SDMMC_R1_CARD_ECC_DISABLED) == SDMMC_R1_CARD_ECC_DISABLED) {
		return SD_ERROR_CARD_ECC_DISABLED;
	} else if ((response_r0 & SDMMC_R1_ERASE_RESET) == SDMMC_R1_ERASE_RESET) {
		return SD_ERROR_ERASE_RESET;
	} else if ((response_r0 & SDMMC_R1_AKE_SEQ_ERROR) == SDMMC_R1_AKE_SEQ_ERROR) {
		return SD_ERROR_AKE_SEQ_ERR;
	} else {
		return SD_ERROR_GENERAL_UNKNOWN_ERR;
	}
}

/**
 * @brief  Checks for error conditions for R2 (CID or CSD) response.
 * @param  SDIOx Pointer to SDIO host
 * @retval SD Card error state
 */
u32 SDMMC_GetCmdResp2(SDIOHOST_TypeDef *SDIOx)
{
	u32 norm_int, err_int;
	/* 8 is the number of required instruction cycles for the below loop
	statement. The SDMMC_CMD_TIMEOUT is expressed in us */
	u32 count = SDMMC_CMD_TIMEOUT * (SystemCoreClock / 8U / 1000000U);

	do {
		if (count-- == 0U) {
			return SD_ERROR_TIMEOUT;
		}
		norm_int = SDIO_GetNormSts(SDIOx);
		err_int = SDIO_GetErrSts(SDIOx);
	} while (!(norm_int & SDIOHOST_BIT_CMD_COMPLETE || err_int & SDIO_CMD_ERR));

	if (norm_int & SDIOHOST_BIT_CMD_COMPLETE) {
		SDIO_ClearNormSts(SDIOx, SDIOHOST_BIT_CMD_COMPLETE);
	}

	if (err_int & SDIO_CMD_ERR) {
		SDIO_ClearErrSts(SDIOx, SDIO_CMD_ERR);
		if (err_int & SDIOHOST_BIT_CMD_TIMEOUT_ERR) {
			return SD_ERROR_CMD_RSP_TIMEOUT;
		} else {
			return SD_ERROR_CMD_CRC_FAIL;
		}
	}

	return SD_ERROR_NONE;
}

/**
 * @brief  Checks for error conditions for R3 (OCR) response.
 * @param  SDIOx Pointer to SDIO host
 * @retval SD Card error state
 */
u32 SDMMC_GetCmdResp3(SDIOHOST_TypeDef *SDIOx)
{
	u32 norm_int, err_int;
	/* 8 is the number of required instruction cycles for the below loop
	statement. The SDMMC_CMD_TIMEOUT is expressed in us */
	u32 count = SDMMC_CMD_TIMEOUT * (SystemCoreClock / 8U / 1000000U);

	do {
		if (count-- == 0U) {
			return SD_ERROR_TIMEOUT;
		}
		norm_int = SDIO_GetNormSts(SDIOx);
		err_int = SDIO_GetErrSts(SDIOx);
	} while (!(norm_int & SDIOHOST_BIT_CMD_COMPLETE || err_int & SDIO_CMD_ERR));

	if (norm_int & SDIOHOST_BIT_CMD_COMPLETE) {
		SDIO_ClearNormSts(SDIOx, SDIOHOST_BIT_CMD_COMPLETE);
	}

	if (err_int & SDIO_CMD_ERR) {
		SDIO_ClearErrSts(SDIOx, SDIO_CMD_ERR);
		if (err_int & SDIOHOST_BIT_CMD_TIMEOUT_ERR) {
			return SD_ERROR_CMD_RSP_TIMEOUT;
		}
	}

	return SD_ERROR_NONE;
}

/**
 * @brief  Checks for error conditions for R3 (OCR) response.
 * @param  SDIOx Pointer to SDIO host
 * @retval SD Card error state
 */
u32 SDMMC_GetCmdResp4(SDIOHOST_TypeDef *SDIOx)
{
	u32 norm_int, err_int;
	/* 8 is the number of required instruction cycles for the below loop
	statement. The SDMMC_CMD_TIMEOUT is expressed in us */
	u32 count = SDMMC_CMD_TIMEOUT * (SystemCoreClock / 8U / 1000000U);

	do {
		if (count-- == 0U) {
			return SD_ERROR_TIMEOUT;
		}
		norm_int = SDIO_GetNormSts(SDIOx);
		err_int = SDIO_GetErrSts(SDIOx);
	} while (!(norm_int & SDIOHOST_BIT_CMD_COMPLETE || err_int & SDIO_CMD_ERR));

	if (norm_int & SDIOHOST_BIT_CMD_COMPLETE) {
		SDIO_ClearNormSts(SDIOx, SDIOHOST_BIT_CMD_COMPLETE);
	}

	if (err_int & SDIO_CMD_ERR) {
		SDIO_ClearErrSts(SDIOx, SDIO_CMD_ERR);
		if (err_int & SDIOHOST_BIT_CMD_TIMEOUT_ERR) {
			return SD_ERROR_CMD_RSP_TIMEOUT;
		}
	}

	return SD_ERROR_NONE;
}

/**
 * @brief  Checks for error conditions for R1 response.
 * @param  SDIOx Pointer to SDIO host
 * @retval SD Card error state
 */
u32 SDMMC_GetCmdResp5(SDIOHOST_TypeDef *SDIOx)
{
	u32 response_r0;
	u32 norm_int, err_int;

	/* 8 is the number of required instruction cycles for the below loop
	statement. The SDMMC_CMD_TIMEOUT is expressed in us */
	u32 count = SDMMC_CMD_TIMEOUT * (SystemCoreClock / 8U / 1000000U);

	do {
		if (count-- == 0U) {
			return SD_ERROR_TIMEOUT;
		}
		norm_int = SDIO_GetNormSts(SDIOx);
		err_int = SDIO_GetErrSts(SDIOx);
	} while (!(norm_int & SDIOHOST_BIT_CMD_COMPLETE || err_int & SDIO_CMD_ERR));

	if (norm_int & SDIOHOST_BIT_CMD_COMPLETE) {
		SDIO_ClearNormSts(SDIOx, SDIOHOST_BIT_CMD_COMPLETE);
	}

	if (err_int & SDIO_CMD_ERR) {
		SDIO_ClearErrSts(SDIOx, SDIO_CMD_ERR);
		if (err_int & SDIOHOST_BIT_CMD_TIMEOUT_ERR) {
			return SD_ERROR_CMD_RSP_TIMEOUT;
		} else {
			return SD_ERROR_CMD_CRC_FAIL;
		}
	}

	/* We have received response, retrieve it.  */
	response_r0 = SDIO_GetResponse(SDIOx, SDIO_RESP0);

	if ((response_r0 & SDMMC_R5_ERRORBITS) == SDMMC_ALLZERO) {
		return SD_ERROR_NONE;
	} else if ((response_r0 & SDMMC_R5_COM_CRC_ERROR) == SDMMC_R5_COM_CRC_ERROR) {
		return SD_ERROR_CMD_CRC_FAIL;
	} else if ((response_r0 & SDMMC_R5_ILLEGAL_COMMAND) == SDMMC_R5_ILLEGAL_COMMAND) {
		return SD_ERROR_ILLEGAL_CMD;
	} else if ((response_r0 & SDMMC_R5_OUT_OF_RANGE) == SDMMC_R5_OUT_OF_RANGE) {
		return SD_ERROR_ADDR_OUT_OF_RANGE;
	} else {
		return SD_ERROR_GENERAL_UNKNOWN_ERR;
	}
}

/**
 * @brief  Checks for error conditions for R6 (RCA) response.
 * @param  SDIOx Pointer to SDIO host
 * @param  SDMMC_CMD: The sent command index
 * @param  pRCA: Pointer to the variable that will contain the SD card relative
 *         address RCA
 * @retval SD Card error state
 */
u32 SDMMC_GetCmdResp6(SDIOHOST_TypeDef *SDIOx, u16 *pRCA)
{
	u32 response_r0;
	u32 norm_int, err_int;

	/* 8 is the number of required instruction cycles for the below loop
	statement. The SDMMC_CMD_TIMEOUT is expressed in us */
	u32 count = SDMMC_CMD_TIMEOUT * (SystemCoreClock / 8U / 1000000U);

	do {
		if (count-- == 0U) {
			return SD_ERROR_TIMEOUT;
		}
		norm_int = SDIO_GetNormSts(SDIOx);
		err_int = SDIO_GetErrSts(SDIOx);
	} while (!(norm_int & SDIOHOST_BIT_CMD_COMPLETE || err_int & SDIO_CMD_ERR));

	if (norm_int & SDIOHOST_BIT_CMD_COMPLETE) {
		SDIO_ClearNormSts(SDIOx, SDIOHOST_BIT_CMD_COMPLETE);
	}

	if (err_int & SDIO_CMD_ERR) {
		SDIO_ClearErrSts(SDIOx, SDIO_CMD_ERR);
		if (err_int & SDIOHOST_BIT_CMD_TIMEOUT_ERR) {
			return SD_ERROR_CMD_RSP_TIMEOUT;
		} else {
			return SD_ERROR_CMD_CRC_FAIL;
		}
	}

	/* We have received response, retrieve it.  */
	response_r0 = SDIO_GetResponse(SDIOx, SDIO_RESP0);

	if ((response_r0 & (SDMMC_R6_GENERAL_UNKNOWN_ERROR | SDMMC_R6_ILLEGAL_CMD |
						SDMMC_R6_COM_CRC_FAILED)) == SDMMC_ALLZERO) {
		*pRCA = (u16)(response_r0 >> 16);

		return SD_ERROR_NONE;
	} else if ((response_r0 & SDMMC_R6_ILLEGAL_CMD) == SDMMC_R6_ILLEGAL_CMD) {
		return SD_ERROR_ILLEGAL_CMD;
	} else if ((response_r0 & SDMMC_R6_COM_CRC_FAILED) == SDMMC_R6_COM_CRC_FAILED) {
		return SD_ERROR_COM_CRC_FAILED;
	} else {
		return SD_ERROR_GENERAL_UNKNOWN_ERR;
	}
}

/**
 * @brief  Checks for error conditions for R7 response.
 * @param  SDIOx Pointer to SDIO host
 * @retval SD Card error state
 */
u32 SDMMC_GetCmdResp7(SDIOHOST_TypeDef *SDIOx)
{
	u32 norm_int, err_int;
	/* 8 is the number of required instruction cycles for the below loop
	statement. The SDMMC_CMD_TIMEOUT is expressed in us */
	u32 count = SDMMC_CMD_TIMEOUT * (SystemCoreClock / 8U / 1000000U);

	do {
		if (count-- == 0U) {
			return SD_ERROR_TIMEOUT;
		}
		norm_int = SDIO_GetNormSts(SDIOx);
		err_int = SDIO_GetErrSts(SDIOx);
	} while (!(norm_int & SDIOHOST_BIT_CMD_COMPLETE || err_int & SDIO_CMD_ERR));

	if (norm_int & SDIOHOST_BIT_CMD_COMPLETE) {
		SDIO_ClearNormSts(SDIOx, SDIOHOST_BIT_CMD_COMPLETE);
	}

	if (err_int & SDIO_CMD_ERR) {
		DelayUs(10); // avoid cmd_err sts is set again after clear
		SDIO_ClearErrSts(SDIOx, SDIO_CMD_ERR);
		if (err_int & SDIOHOST_BIT_CMD_TIMEOUT_ERR) {
			return SD_ERROR_CMD_RSP_TIMEOUT;
		} else {
			return SD_ERROR_CMD_CRC_FAIL;
		}
	}

	return SD_ERROR_NONE;
}
