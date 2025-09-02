/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "ameba_soc.h"

static const char *const TAG = "SD";

SD_HdlTypeDef hsd0;

static u32 wait_for_sema = 0;

/* 32-Byte aligned for cache maintenance */
static u8 scratch[SD_BLOCK_SIZE]__attribute__((aligned(32))) = {0};

int (*sd_sema_take_fn)(u32);
int (*sd_sema_give_isr_fn)(u32);

static SD_RESULT SD_PowerON(SD_HdlTypeDef *hsd);
static SD_RESULT SD_InitCard(SD_HdlTypeDef *hsd);
static SD_RESULT SD_SendSDStatus(SD_HdlTypeDef *hsd, u32 *pSDstatus);
static u32 SD_SendCardStatus(SD_HdlTypeDef *hsd, u32 *pCardStatus);
static u32 SD_WideBus_Enable(SD_HdlTypeDef *hsd);
static u32 SD_WideBus_Disable(SD_HdlTypeDef *hsd);
static u32 SD_GetCardSCR(SD_HdlTypeDef *hsd);
static void SD_Write_IT(SD_HdlTypeDef *hsd);
static void SD_Read_IT(SD_HdlTypeDef *hsd);
static u32 SD_IRQHandler(void *pData);

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

int SD_CheckStatusTO(SD_HdlTypeDef *hsd, u32 timeout_us)
{
	u32 start_us = DTimestamp_Get();

	while (!is_timeout(start_us, timeout_us)) {
		if (SD_GetCardState(hsd) == SD_CARD_TRANSFER) {
			return 0;
		}
	}

	return -1;
}

static void SD_CardDetectHdl(u32 id, u32 event)
{
	(void)id;
	u32 level = event & 0x3;

	if (level == HAL_IRQ_FALL) { // high->low
		RTK_LOGI(TAG, "Card Detected!\n");
	} else if (level == HAL_IRQ_RISE) { // low->high
		RTK_LOGI(TAG, "Card Removed!\n");
	}
}

static void SDIOH_Pinmux(void)
{
	GPIO_InitTypeDef GPIO_InitStruct_CD;
	u8 idx;
	u8 port_num;

	/* Be consistent with order of SDIO_PAD[][]. */
	char *sdh_pin[] = {"CLK", "CMD", "D0", "D1", "D2", "D3"};
	IRQn_Type GPIO_IRQ[3] = {
		GPIOA_IRQ,
		GPIOB_IRQ,
		GPIOC_IRQ
	};

	/* CLK & CMD & D0 & D1 */
	for (idx = 0; idx < 4; idx++) {
		Pinmux_Config(SDIO_PAD[SDH_Pin_Grp][idx], PINMUX_FUNCTION_SDIO_MST);
		PAD_PullCtrl(SDIO_PAD[SDH_Pin_Grp][idx], GPIO_PuPd_UP);
		RTK_LOGI(TAG, "SD_%s --> P%c%d\n", sdh_pin[idx], 'A' + PORT_NUM(SDIO_PAD[SDH_Pin_Grp][idx]), PIN_NUM(SDIO_PAD[SDH_Pin_Grp][idx]));
	}

	/* D2 & D3 */
	if (sdioh_config.sdioh_bus_width == SDIOH_BUS_WIDTH_4BIT) {
		for (idx = 4; idx < 6; idx++) {
			Pinmux_Config(SDIO_PAD[SDH_Pin_Grp][idx], PINMUX_FUNCTION_SDIO_MST);
			PAD_PullCtrl(SDIO_PAD[SDH_Pin_Grp][idx], GPIO_PuPd_UP);
			RTK_LOGI(TAG, "SDH_%s --> P%c%d\n", sdh_pin[idx], 'A' + PORT_NUM(SDIO_PAD[SDH_Pin_Grp][idx]), PIN_NUM(SDIO_PAD[SDH_Pin_Grp][idx]));
		}
	}

	/* CD */
	if (sdioh_config.sdioh_cd_pin != _PNC) {
		port_num = PORT_NUM(sdioh_config.sdioh_cd_pin);
		GPIO_INTConfig(sdioh_config.sdioh_cd_pin, DISABLE);

		GPIO_InitStruct_CD.GPIO_Pin =  sdioh_config.sdioh_cd_pin;
		GPIO_InitStruct_CD.GPIO_Mode = GPIO_Mode_INT;
		GPIO_InitStruct_CD.GPIO_PuPd = GPIO_PuPd_UP;
		GPIO_InitStruct_CD.GPIO_ITTrigger = GPIO_INT_Trigger_BOTHEDGE;
		GPIO_Init(&GPIO_InitStruct_CD);

		InterruptRegister((IRQ_FUN)GPIO_INTHandler, GPIO_IRQ[port_num], (u32)GPIO_PORTx[port_num], INT_PRI_MIDDLE);
		InterruptEn(GPIO_IRQ[port_num], INT_PRI_MIDDLE);

		GPIO_UserRegIrq(sdioh_config.sdioh_cd_pin, (void *)SD_CardDetectHdl, (void *)NULL);
		GPIO_INTConfig(sdioh_config.sdioh_cd_pin, ENABLE);
		RTK_LOGI(TAG, "SDH_CD --> P%c%d\n", 'A' + port_num, PIN_NUM(sdioh_config.sdioh_cd_pin));
	}

	/* WP */
	if (sdioh_config.sdioh_wp_pin != _PNC) {
		RTK_LOGI(TAG, "SDH WP is not supported.\n");
	}
}

static void SD_PreDMATrans(SD_HdlTypeDef *hsd)
{
	if ((CPU_InInterrupt() == 0) && (rtos_sched_get_state() == RTOS_SCHED_RUNNING) && (sd_sema_take_fn != NULL)) {
		wait_for_sema = 1;
		SDIO_ConfigNormIntSig(hsd->Instance, SDIOHOST_BIT_XFER_COMPLETE_SIGNAL_EN, ENABLE);
	}
}

/**
  * @brief  Initialize SDIO host and card and create the associated handle.
  * @param  hsd: Pointer to the SD handle
  * @retval Result
  */
SD_RESULT SD_Init(void)
{
	SD_RESULT ret;
	u32 status;
	u32 errorstate;
	u8 val;
	u32 cnt = 0;

	SD_HdlTypeDef *hsd = &hsd0;

	if (hsd == NULL) {
		return SD_ERROR;
	}

	if (hsd->State == SD_STATE_RESET) {
		/* Allocate lock resource and initialize it */
		hsd->Lock = HAL_UNLOCKED;

		RCC_PeriphClockCmd(APBPeriph_SDH, APBPeriph_SDH_CLOCK, DISABLE);
		RCC_PeriphClockSourceSet(SDH, SYS_PLL);
		RCC_PeriphClockDividerFENSet(SYS_PLL_SDH, 1);
		RCC_PeriphClockCmd(APBPeriph_SDH, APBPeriph_SDH_CLOCK, ENABLE);

		/* Configure pinmux */
		SDIOH_Pinmux();
	}
	hsd->State = SD_STATE_READY;

	hsd->Instance = SDIO_HOST;

	status = SDIO_ResetAll(hsd->Instance);
	if (status != HAL_OK) {
		return SD_ERROR;
	}

	status = SDIO_CheckState(hsd->Instance);
	if (status != HAL_OK) {
		return SD_ERROR;
	}

	/* Initialize SDIO peripheral interface with default configuration */
	status = SDIOH_Init(hsd->Instance);
	if (status != HAL_OK) {
		return SD_ERROR;
	}

	/* register SDIO Host IRQ */
	InterruptRegister((IRQ_FUN)SD_IRQHandler, SDIO_HOST_IRQ, (u32)hsd, INT_PRI_MIDDLE);
	InterruptEn(SDIO_HOST_IRQ, INT_PRI_MIDDLE);

	/* Required power up waiting time before starting the SD initialization sequence */
	DelayUs(2);

	/* Initialize the Card parameters */
	/* CMD0->CMD8->CMD5->ACMD41)->(CMD2->)CMD3->(CMD9->)CMD7(->CMD16) */

	/* Identify card operating voltage */
	ret = SD_PowerON(hsd); // CMD0->CMD8->CMD5(->ACMD41)
	if (ret != SD_OK) {
		hsd->State = SD_STATE_READY;
		return ret;
	}

	/* Card initialization */
	ret = SD_InitCard(hsd); // (CMD2->)CMD3->(CMD9->)CMD7
	if (ret != SD_OK) {
		hsd->State = SD_STATE_READY;
		return ret;
	}

	/*  Set block size */
	if (hsd->Card.CardType != CARD_SDIO_ONLY) {
		/* CMD16: Set Block Size for Card */
		errorstate = SDMMC_CmdBlockLength(hsd->Instance, SD_BLOCK_SIZE); // CMD16
		if (errorstate != SD_ERROR_NONE) {
			/* Clear all the static flags */
			SDIO_ClearErrSts(hsd->Instance, SDIO_ERR_FLAG);

			hsd->ErrorCode |= errorstate;
			hsd->State = SD_STATE_READY;
			return SD_INITERR;
		}
	} else {
		/* CMD52: set func0 blocksize */
		ret = SD_IO_RW_Direct(hsd, BUS_WRITE, SDIO_FUNC0, SDIOD_CCCR_BLKSIZE_0, SD_BLOCK_SIZE & 0xFF, NULL);
		if (ret != SD_OK) {
			return ret;
		}

		/* CMD52: set func0 blocksize */
		ret = SD_IO_RW_Direct(hsd, BUS_WRITE, SDIO_FUNC0, SDIOD_CCCR_BLKSIZE_1, (SD_BLOCK_SIZE >> 8) & 0xFF, NULL);
		if (ret != SD_OK) {
			return ret;
		}

		/* CMD52: set func1 blocksize */
		ret = SD_IO_RW_Direct(hsd, BUS_WRITE, SDIO_FUNC0, SDIOD_CCCR_F1BLKSIZE_0, SD_BLOCK_SIZE & 0xFF, NULL);
		if (ret != SD_OK) {
			return ret;
		}

		/* CMD52: set func1 blocksize */
		ret = SD_IO_RW_Direct(hsd, BUS_WRITE, SDIO_FUNC0, SDIOD_CCCR_F1BLKSIZE_1, (SD_BLOCK_SIZE >> 8) & 0xFF, NULL);
		if (ret != SD_OK) {
			return ret;
		}
	}

	/* Set bus width */
	ret = SD_ConfigBusWidth(hsd, sdioh_config.sdioh_bus_width);
	if (ret != SD_OK) {
		return ret;
	}

	/* Set bus speed */
	ret = SD_ConfigBusSpeed(hsd, sdioh_config.sdioh_bus_speed);
	if (ret != SD_OK) {
		return ret;
	}

	if (hsd->Card.CardType != CARD_SDIO_ONLY) {
		if (SD_GetCardState(hsd) != SD_CARD_TRANSFER) {
			return SD_ERROR;
		}
	} else {
		/* Enable func1 */
		ret = SD_IO_RW_Direct(hsd, BUS_WRITE, SDIO_FUNC0, SDIOD_CCCR_IOEN, SDIO_FUNC_ENABLE_1, NULL);
		if (ret != SD_OK) {
			return SD_ERROR;
		}

		/* Wait until func1 is ready */
		while (1) {
			ret = SD_IO_RW_Direct(hsd, BUS_READ, SDIO_FUNC0, SDIOD_CCCR_IORDY, NULL, &val);
			if (ret != SD_OK) {
				return SD_ERROR;
			}

			if ((val & SDIO_FUNC_READY_1) != 0) {
				break;
			}
			DelayMs(1);

			if (cnt++ >= 500) {
				return SD_ERROR;
			}
		}

		/* Enable func1 interrupt */
		ret = SD_IO_RW_Direct(hsd, BUS_WRITE, SDIO_FUNC0, SDIOD_CCCR_INTEN, INTR_CTL_MASTER_EN | INTR_CTL_FUNC1_EN, NULL);
		if (ret != SD_OK) {
			return SD_ERROR;
		}
	}

	/* Initialize the error code */
	hsd->ErrorCode = SD_ERROR_NONE;

	/* Initialize the SD operation */
	hsd->Context = SD_CONTEXT_NONE;

	/* Initialize the SD state */
	hsd->State = SD_STATE_READY;

	return SD_OK;
}

/**
 * @brief  Enquires cards about their operating voltage and configures clock
 *         controls and stores SD information that will be needed in future
 *         in the SD handle.
 * @param  hsd: Pointer to SD handle
 * @retval error state
 */
static SD_RESULT SD_PowerON(SD_HdlTypeDef *hsd)
{
	SDIOHOST_TypeDef *SDIOx = hsd->Instance;
	u32 response = 0U, validvoltage = 0U;
	u32 errorstate;
	u32 count = 0U;

	/* CMD0: GO_IDLE_STATE */
	errorstate = SDMMC_CmdGoIdleState(SDIOx);
	if (errorstate != SD_ERROR_NONE) {
		hsd->ErrorCode |= errorstate;
		return SD_ERROR;
	}

	/* CMD8: SEND_IF_COND: Command available only on V2.0 cards */
	errorstate = SDMMC_CmdOperCond(SDIOx);
	if (errorstate == SD_ERROR_NONE) {
		RTK_LOGI(TAG, "Voltage match\n");
		hsd->Card.CardVersion = CARD_V2_X;
	} else if (errorstate == SD_ERROR_CMD_RSP_TIMEOUT) {
		RTK_LOGI(TAG, "No response to CMD8\n");
	} else {
		RTK_LOGI(TAG, "Voltage mismatch. Please check if SD card is inserted correctly\n");
		hsd->Card.CardVersion = CARD_V1_X;
		/* CMD0: GO_IDLE_STATE */
		errorstate = SDMMC_CmdGoIdleState(SDIOx);
		if (errorstate != SD_ERROR_NONE) {
			hsd->ErrorCode |= errorstate;
			return SD_ERROR;
		}
	}

	if ((hsd->Card.CardVersion != CARD_V2_X) && (hsd->Card.CardVersion != CARD_V1_X)) {
		/* 1st CMD5, arg is 0 */
		errorstate = SDIO_CmdSendOpCond(SDIOx, 0);
		if (errorstate == SD_ERROR_NONE) {

			response = SDIO_GetResponse(SDIOx, SDIO_RESP0);

			if (SD_RESP_R4_NUM_FUNCTIONS(response) == 0) {
				hsd->Card.CardType = CARD_UNSUPPORTED;
				hsd->ErrorCode |= SD_ERROR_UNSUPPORTED_FEATURE;
				RTK_LOGS(NOTAG, RTK_LOG_INFO, "Card is not supported!\n");
				return SD_ERROR;
			}

			if (response & SD_RESP_R4_MEM_PRESENT) {
				hsd->Card.CardType = CARD_COMBO;
				hsd->ErrorCode |= SD_ERROR_UNSUPPORTED_FEATURE;
				RTK_LOGS(NOTAG, RTK_LOG_INFO, "Combo card is not supported!\n");
				return SD_ERROR;
			} else {
				hsd->Card.CardType = CARD_SDIO_ONLY;
				RTK_LOGS(NOTAG, RTK_LOG_INFO, "This is a SDIO Card!\n");
			}

			/* get host ocr */
			/* 2nd CMD5: arg is not 0 */
			errorstate = SDIO_CmdSendOpCond(SDIOx, response & SD_RESP_R4_0CR_MASK);
			if (errorstate != SD_ERROR_NONE) {
				hsd->ErrorCode |= errorstate;
				return SD_ERROR;
			}

			return SD_OK;
		} else if (errorstate == SD_ERROR_CMD_RSP_TIMEOUT) {
			RTK_LOGI(TAG, "No response to CMD5\n");
		}
	}

	/* SD Memory CARD */
	/* Send ACMD41 SD_APP_OP_COND with Argument 0x80100000 */
	while ((count < SDMMC_MAX_VOLT_TRIAL) && (validvoltage == 0U)) {
		/* SEND CMD55 APP_CMD with RCA as 0 */
		errorstate = SDMMC_CmdAppCommand(SDIOx, 0);
		if (errorstate != SD_ERROR_NONE) {
			hsd->ErrorCode |= errorstate;
			return SD_ERROR;
		}

		/* Send CMD41 */
		/* host does not support UHS mode, no need to switch to 1.8V */
		errorstate = SDMMC_CmdAppOperCommand(SDIOx, SDMMC_VOLTAGE_WINDOW_SD | SDMMC_HIGH_CAPACITY);
		if (errorstate != SD_ERROR_NONE) {
			hsd->ErrorCode |= SD_ERROR_UNSUPPORTED_FEATURE;
			return SD_ERROR;
		}

		/* Get command response(ACMD41--R3) */
		response = SDIO_GetResponse(SDIOx, SDIO_RESP0);

		/* Get operating voltage*/
		validvoltage = (((response >> 31U) == 1U) ? 1U : 0U);

		count++;
	}

	if (count >= SDMMC_MAX_VOLT_TRIAL) {
		hsd->ErrorCode |= SD_ERROR_INVALID_VOLTRANGE;
		return SD_ERROR;
	}

	/* Check CCS(Card Capacity Status) bit, OCR bit30 */
	if ((response & SDMMC_HIGH_CAPACITY) == SDMMC_HIGH_CAPACITY) {
		RTK_LOGI(TAG, "This is a SDHC/SDXC card...\n");
		hsd->Card.CardType = CARD_SDHC_SDXC;
	} else {
		RTK_LOGI(TAG, "This is a SDSC card...\n");
		hsd->Card.CardType = CARD_SDSC;
	}

	return SD_OK;
}

/**
 * @brief  Initializes the sd card.
 * @param  hsd: Pointer to SD handle
 * @retval SD Card error state
 */
static SD_RESULT SD_InitCard(SD_HdlTypeDef *hsd)
{
	SDIOHOST_TypeDef *SDIOx = hsd->Instance;
	SD_CardCIDTypeDef CID;
	SD_CardCSDTypeDef CSD;
	u32 errorstate;
	SD_RESULT ret;
	u16 sd_rca = 1U;

	/* Check the power State */
	if (SDIO_GetPowerState(SDIOx) == 0U) {
		/* Power off */
		hsd->ErrorCode |= SD_ERROR_REQUEST_NOT_APPLICABLE;
		return SD_ERROR;
	}

	if (hsd->Card.CardType != CARD_SDIO_ONLY) {
		/* CMD2: ALL_SEND_CID */
		errorstate = SDMMC_CmdSendCID(SDIOx);
		if (errorstate != SD_ERROR_NONE) {
			hsd->ErrorCode |= errorstate;
			return SD_ERROR;
		} else {
			/* Get Card identification number data */
			hsd->CID[0U] = SDIO_GetResponse(SDIOx, SDIO_RESP0);
			hsd->CID[1U] = SDIO_GetResponse(SDIOx, SDIO_RESP1);
			hsd->CID[2U] = SDIO_GetResponse(SDIOx, SDIO_RESP2);
			hsd->CID[3U] = SDIO_GetResponse(SDIOx, SDIO_RESP3);
		}

		/* Get CID parameters */
		SD_GetCardCID(hsd, &CID);
	}

	/* Send CMD3 SET_REL_ADDR with argument 0 */
	/* CMD3: SD Card publishes its RCA. */
	errorstate = SDMMC_CmdSetRelAdd(SDIOx, &sd_rca);
	if (errorstate != SD_ERROR_NONE) {
		hsd->ErrorCode |= errorstate;
		return SD_ERROR;
	}

	/* Get the SD card RCA */
	hsd->Card.RelCardAdd = sd_rca;
	RTK_LOGI(TAG, "RCA = %04X\n", hsd->Card.RelCardAdd);

	if (hsd->Card.CardType != CARD_SDIO_ONLY) {
		/* CMD9: SEND_CSD with argument as card's RCA */
		errorstate = SDMMC_CmdSendCSD(SDIOx, (u32)(hsd->Card.RelCardAdd << 16U));
		if (errorstate != SD_ERROR_NONE) {
			hsd->ErrorCode |= errorstate;
			return SD_ERROR;
		} else {
			/* Get Card Specific Data */
			hsd->CSD[0U] = SDIO_GetResponse(SDIOx, SDIO_RESP0);
			hsd->CSD[1U] = SDIO_GetResponse(SDIOx, SDIO_RESP1);
			hsd->CSD[2U] = SDIO_GetResponse(SDIOx, SDIO_RESP2);
			hsd->CSD[3U] = SDIO_GetResponse(SDIOx, SDIO_RESP3);
		}

		/* Get the Card Class */
		hsd->Card.Class = (u16)((hsd->CSD[2] & 0x00FFF000U) >> 12U);

		/* Get CSD parameters */
		/* kw: initialize members of hsd->Card */
		ret = SD_GetCardCSD(hsd, &CSD);
		if (ret != SD_OK) {
			hsd->ErrorCode |= SD_ERROR_UNSUPPORTED_FEATURE;
			return ret;
		}
	}

	/* CMD7: Select the Card */
	errorstate = SDMMC_CmdSelDesel(SDIOx, (u32)(((u32)hsd->Card.RelCardAdd) << 16U));
	if (errorstate != SD_ERROR_NONE) {
		hsd->ErrorCode |= errorstate;
		return SD_ERROR;
	}

	/* All cards are initialized */
	return SD_OK;
}

/**
 * @brief  De-Initializes the SD card.
 * @param  hsd: Pointer to SD handle
 * @retval HAL status
 */
SD_RESULT SD_DeInit(void)
{
	SD_HdlTypeDef *hsd = &hsd0;

	/* Check the SD handle allocation */
	if (hsd == NULL) {
		return SD_ERROR;
	}

	hsd->State = SD_STATE_BUSY;

	/* Disable SDIO IRQ */
	InterruptDis(SDIO_HOST_IRQ);
	/* Unregister IRQ handler */
	InterruptUnRegister(SDIO_HOST_IRQ);

	/* Set SD power state to off */
	SDIO_PowerState_OFF(hsd->Instance);

	RCC_PeriphClockCmd(APBPeriph_SDH, APBPeriph_SDH_CLOCK, DISABLE);

	hsd->ErrorCode = SD_ERROR_NONE;
	hsd->State = SD_STATE_RESET;

	return SD_OK;
}

SD_RESULT SD_TransPreCheck(SD_HdlTypeDef *hsd, u8 *pData, u32 BlockAdd, u32 NumberOfBlocks)
{
	SDIOHOST_TypeDef *SDIOx = hsd->Instance;

	if (NULL == pData) {
		hsd->ErrorCode |= SD_ERROR_INVALID_PARAMETER;
		return SD_ERROR;
	}

	if ((BlockAdd + NumberOfBlocks) > (hsd->Card.LogBlockNbr)) {
		hsd->ErrorCode |= SD_ERROR_ADDR_OUT_OF_RANGE;
		return SD_ERROR;
	}

	SDIO_ConfigNormIntSig(SDIOx, 0xFFFFFFFF, DISABLE); // disable all the normal int without err int
	SDIO_ClearNormSts(SDIOx, SDIO_GetNormSts(SDIOx)); // clear old flags

	hsd->ErrorCode = SD_ERROR_NONE;
	hsd->State = SD_STATE_BUSY;

	return SD_OK;
}

SD_RESULT SD_IO_RW_Direct(SD_HdlTypeDef *hsd, u8 RWFlag, u8 Func, u32 Addr, u8 In, u8 *Out)
{
	SDIOHOST_TypeDef *SDIOx = hsd->Instance;
	u32 errorstate;
	u32 arg = 0;
	u32 IntSigEn;

	if (Func > SD_ARG_CMD52_FUNC_MASK) {
		return SD_ERROR;
	}

	if (Addr & (~SD_ARG_CMD52_ADDR_MASK)) {
		return SD_ERROR;
	}

	if (!RWFlag && NULL == Out) {
		return SD_ERROR;
	}

	if (hsd->State == SD_STATE_READY) {
		hsd->ErrorCode = SD_ERROR_NONE;
		hsd->State = SD_STATE_BUSY;

		IntSigEn = SDIOx->SDIOHOST_NORMAL_SIG_EN;
		SDIO_ConfigNormIntSig(SDIOx, 0xFFFFFFFF, DISABLE); // disable all the normal int without err int
		SDIO_ClearNormSts(SDIOx, SDIO_GetNormSts(SDIOx)); // clear old flags

		if (RWFlag) {
			arg |= SD_ARG_CMD52_W_FLAG;
			arg |= In << SD_ARG_CMD52_DATA_SHIFT;

			if (NULL != Out) {
				arg |= SD_ARG_CMD52_RAW_FLAG;
			}
		}

		arg |= Func << SD_ARG_CMD52_FUNC_SHIFT;
		arg |= Addr << SD_ARG_CMD52_ADDR_SHIFT;

		errorstate = SDIO_CmdRWDirect(SDIOx, arg);
		if (errorstate != SD_ERROR_NONE) {
			hsd->ErrorCode |= errorstate;
			hsd->State = SD_STATE_READY;
			return SD_ERROR;
		}

		if (NULL != Out) {
			*Out = (u8)(SDIO_GetResponse(SDIOx, SDIO_RESP0) & 0xFF);
		}

		hsd->State = SD_STATE_READY;

		/* restore int signal config */
		if (IntSigEn != 0) {
			SDIO_ConfigNormIntSig(SDIOx, IntSigEn, ENABLE);
		}

		return SD_OK;
	} else {
		RTK_LOGW(TAG, "%s SDH NOT READY!\n", __FUNCTION__);
		return SD_ERROR;
	}
}

SD_RESULT SD_IO_RW_Extended(SD_HdlTypeDef *hsd, u8 RWFlag, u8 Func, u8 OpCode,
							u32 Addr, u8 *pData, u8 IsBlock, u16 Cnt)
{
	SDIOHOST_TypeDef *SDIOx = hsd->Instance;
	SDIO_DataInitTypeDef config;
	u32 status;
	u32 errorstate;
	u32 arg = 0;
	u32 IntSigEn;

	if (Func > SD_ARG_CMD53_FUNC_MASK) {
		return SD_ERROR;
	}

	if (Addr & (~SD_ARG_CMD53_ADDR_MASK)) {
		return SD_ERROR;
	}

	if (NULL == pData || Cnt == 0 || Cnt > SD_ARG_CMD53_COUNT_MASK) {
		return SD_ERROR;
	}

	if (hsd->State == SD_STATE_READY) {
		hsd->ErrorCode = SD_ERROR_NONE;
		hsd->State = SD_STATE_BUSY;
		hsd->Context = SD_CONTEXT_DMA;

		IntSigEn = SDIOx->SDIOHOST_NORMAL_SIG_EN;
		SDIO_ConfigNormIntSig(SDIOx, 0xFFFFFFFF, DISABLE); // disable all the normal int without err int
		SDIO_ClearNormSts(SDIOx, SDIO_GetNormSts(SDIOx)); // clear old flags

		/* Configure the SD DPSM (Data Path State Machine) */
		config.TransType = IsBlock ? SDIO_TRANS_MULTI_BLK : SDIO_TRANS_SINGLE_BLK;
		config.TransDir = RWFlag ? SDIO_TRANS_HOST_TO_CARD : SDIO_TRANS_CARD_TO_HOST;
		config.BlockSize = IsBlock ? SD_BLOCK_SIZE : Cnt;
		config.BlockCnt = IsBlock ? Cnt : 0x0;
		config.AutoCmdEn = SDIO_TRANS_AUTO_DIS;
		config.DmaEn = SDIO_TRANS_DMA_EN;
		SDIO_ConfigData(SDIOx, &config);

		status = SDIO_ConfigDMA(SDIOx, SDIO_SDMA_MODE, (u32)pData);
		if (status != HAL_OK) {
			hsd->ErrorCode |= SD_ERROR_INVALID_PARAMETER;
			hsd->State = SD_STATE_READY;
			return SD_ERROR;
		}

		arg = RWFlag ? SD_ARG_CMD53_W_FLAG : SD_ARG_CMD53_R_FLAG;
		arg |= Func << SD_ARG_CMD53_FUNC_SHIFT;
		arg |= IsBlock ? SD_ARG_CMD53_BLOCK_MODE : SD_ARG_CMD53_BYTE_MODE;
		arg |= OpCode ? SD_ARG_CMD53_OP_CODE : 0x0;
		arg |= Addr << SD_ARG_CMD53_ADDR_SHIFT;
		arg |= Cnt << SD_ARG_CMD53_COUNT_SHIFT;

		SD_PreDMATrans(hsd);

		errorstate = SDIO_CmdRWExtended(SDIOx, arg);
		if (errorstate != SD_ERROR_NONE) {
			hsd->ErrorCode |= errorstate;
			hsd->State = SD_STATE_READY;
			return SD_ERROR;
		}

		if (SD_WaitTransDone(hsd, SDMMC_DAT_TIMEOUT) != HAL_OK) {
			RTK_LOGE(TAG, "%s WaitTransDone error!\n", __FUNCTION__);
			return SD_ERROR;
		}

		SDIO_ConfigNormIntSig(SDIOx, SDIOHOST_BIT_CARD_INT_SIGNAL_EN, ENABLE);

		/* restore int signal config */
		if (IntSigEn != 0) {
			SDIO_ConfigNormIntSig(SDIOx, IntSigEn, ENABLE);
		}

		return SD_OK;
	} else {
		RTK_LOGW(TAG, "%s SDH NOT READY!\n", __FUNCTION__);
		return SD_ERROR;
	}
}

/**
 * @brief  Reads block(s) from a specified address in a card. The Data transfer
 *         is managed by polling mode.
 * @note   This API should be followed by a check on the card state through SD_GetCardState().
 * @param  hsd: Pointer to SD handle
 * @param  pData: pointer to the buffer that will contain the received data
 * @param  BlockAdd: Block Address from where data is to be read
 * @param  NumberOfBlocks: Number of SD blocks to read
 * @param  Timeout: Specify timeout value
 * @retval HAL status
 */
SD_RESULT SD_ReadBlocks_PIO(SD_HdlTypeDef *hsd, u8 *pData, u32 BlockAdd, u32 NumberOfBlocks, u32 Timeout)
{
	SDIOHOST_TypeDef *SDIOx = hsd->Instance;
	SDIO_DataInitTypeDef config;
	u32 errorstate;
	u32 tickstart = DTimestamp_Get();
	u32 data, dataremaining;
	u32 add = BlockAdd;
	u8 *tempbuff = pData;
	SD_RESULT ret;

	if (hsd->State == SD_STATE_READY) {
		ret = SD_TransPreCheck(hsd, pData, add, NumberOfBlocks);
		if (ret != SD_OK) {
			return ret;
		}

		/* Configure the SD DPSM (Data Path State Machine) */
		config.TransType = SDIO_TRANS_MULTI_BLK;
		config.TransDir = SDIO_TRANS_CARD_TO_HOST;
		config.BlockSize = SD_BLOCK_SIZE;
		config.BlockCnt = NumberOfBlocks;
		config.AutoCmdEn = SDIO_TRANS_AUTO_DIS;
		config.DmaEn = SDIO_TRANS_DMA_DIS;
		SDIO_ConfigData(SDIOx, &config);

		if (hsd->Card.CardType != CARD_SDHC_SDXC) {
			add *= SD_BLOCK_SIZE;
		}

		/* Read block(s) in polling mode */
		if (NumberOfBlocks > 1U) {
			hsd->Context = SD_CONTEXT_READ_MULTIPLE_BLOCK;

			/* Read Multi Block command */
			errorstate = SDMMC_CmdReadMultiBlock(SDIOx, add);
		} else {
			hsd->Context = SD_CONTEXT_READ_SINGLE_BLOCK;

			/* Read Single Block command */
			errorstate = SDMMC_CmdReadSingleBlock(SDIOx, add);
		}

		if (errorstate != SD_ERROR_NONE) {
			/* Clear all the static flags */
			SDIO_ClearErrSts(SDIOx, SDIO_ERR_FLAG);
			hsd->ErrorCode |= errorstate;
			hsd->State = SD_STATE_READY;
			hsd->Context = SD_CONTEXT_NONE;
			return SD_ERROR;
		}

		/* Poll on SDIO flags */
		dataremaining = config.BlockCnt * config.BlockSize;

		while (!((SDIO_GetErrSts(SDIOx) & SDIO_DAT_ERR) || (SDIO_GetNormSts(SDIOx) & SDIOHOST_BIT_XFER_COMPLETE))) {
			if ((SDIO_GetNormSts(SDIOx) & SDIOHOST_BIT_BUFF_READ_READY) && (dataremaining > 0U)) {
				/* Read data from SDIO Rx FIFO */
				SDIO_ClearNormSts(SDIOx, SDIOHOST_BIT_BUFF_READ_READY);

				while ((SDIO_GetStatus(SDIOx) & SDIOHOST_BIT_BUFF_READ_EN) && (dataremaining > 0U)) {
					data = SDIO_ReadFIFO(SDIOx);
					*tempbuff = (u8)(data & 0xFFU);
					tempbuff++;
					*tempbuff = (u8)((data >> 8U) & 0xFFU);
					tempbuff++;
					*tempbuff = (u8)((data >> 16U) & 0xFFU);
					tempbuff++;
					*tempbuff = (u8)((data >> 24U) & 0xFFU);
					tempbuff++;
					dataremaining -= 4U;
				}
			}

			if (is_timeout(tickstart, Timeout) || (Timeout == 0U)) {
				/* Clear all the static flags */
				SDIO_ClearErrSts(SDIOx, SDIO_ERR_FLAG);
				hsd->ErrorCode |= SD_ERROR_TIMEOUT;
				hsd->State = SD_STATE_READY;
				hsd->Context = SD_CONTEXT_NONE;
				return SD_ERROR;
			}
		}

		/* Send stop transmission command in case of multiblock read */
		if ((SDIO_GetNormSts(SDIOx) & SDIOHOST_BIT_XFER_COMPLETE) && (NumberOfBlocks > 1U)) {
			/* Send stop transmission command */
			errorstate = SDMMC_CmdStopTransfer(SDIOx);
			if (errorstate != SD_ERROR_NONE) {
				/* Clear all the static flags */
				SDIO_ClearErrSts(SDIOx, SDIO_ERR_FLAG);
				hsd->ErrorCode |= errorstate;
				hsd->State = SD_STATE_READY;
				hsd->Context = SD_CONTEXT_NONE;
				return SD_ERROR;
			}
		}

		/* Get error state */
		if (SDIO_GetErrSts(SDIOx) & SDIOHOST_BIT_DATA_TIMEOUT_ERR) {
			/* Clear all the static flags */
			SDIO_ClearErrSts(SDIOx, SDIO_ERR_FLAG);
			hsd->ErrorCode |= SD_ERROR_DATA_TIMEOUT;
			hsd->State = SD_STATE_READY;
			hsd->Context = SD_CONTEXT_NONE;
			return SD_ERROR;
		} else if (SDIO_GetErrSts(SDIOx) & SDIOHOST_BIT_DATA_CRC_ERR) {
			/* Clear all the static flags */
			SDIO_ClearErrSts(SDIOx, SDIO_ERR_FLAG);
			hsd->ErrorCode |= SD_ERROR_DATA_CRC_FAIL;
			hsd->State = SD_STATE_READY;
			hsd->Context = SD_CONTEXT_NONE;
			return SD_ERROR;
		} else {
			/* Nothing to do */
		}

#if 1
		/* Empty FIFO if there is still any data */
		while (SDIO_GetStatus(SDIOx) & SDIOHOST_BIT_BUFF_READ_EN) {
			data = SDIO_ReadFIFO(SDIOx);
			*tempbuff = (u8)(data & 0xFFU);
			tempbuff++;
			*tempbuff = (u8)((data >> 8U) & 0xFFU);
			tempbuff++;
			*tempbuff = (u8)((data >> 16U) & 0xFFU);
			tempbuff++;
			*tempbuff = (u8)((data >> 24U) & 0xFFU);
			tempbuff++;
			dataremaining -= 4U;
			if (is_timeout(tickstart, Timeout) || (Timeout == 0U)) {
				/* Clear all the static flags */
				SDIO_ClearErrSts(SDIOx, SDIO_ERR_FLAG);
				hsd->ErrorCode |= SD_ERROR_TIMEOUT;
				hsd->State = SD_STATE_READY;
				hsd->Context = SD_CONTEXT_NONE;
				return SD_ERROR;
			}
		}
#endif

		SDIO_ClearNormSts(SDIOx, SDIOHOST_BIT_XFER_COMPLETE);
		/* Clear all the static flags */
		SDIO_ClearErrSts(SDIOx, SDIO_DAT_ERR);

		hsd->State = SD_STATE_READY;

		SDIO_ConfigNormIntSig(SDIOx, SDIOHOST_BIT_CARD_INT_SIGNAL_EN, ENABLE);

		return SD_OK;
	} else {
		RTK_LOGW(TAG, "%s SDH NOT READY!\n", __FUNCTION__);
		hsd->ErrorCode |= SD_ERROR_BUSY;
		return SD_ERROR;
	}
}

/**
 * @brief  Allows to write block(s) to a specified address in a card. The Data
 *         transfer is managed by polling mode.
 * @note   This API should be followed by a check on the card state through SD_GetCardState().
 * @param  hsd: Pointer to SD handle
 * @param  pData: pointer to the buffer that will contain the data to transmit
 * @param  BlockAdd: Block Address where data will be written
 * @param  NumberOfBlocks: Number of SD blocks to write
 * @param  Timeout: Specify timeout value
 * @retval HAL status
 */
SD_RESULT SD_WriteBlocks_PIO(SD_HdlTypeDef *hsd, u8 *pData, u32 BlockAdd, u32 NumberOfBlocks, u32 Timeout)
{
	SDIOHOST_TypeDef *SDIOx = hsd->Instance;
	SDIO_DataInitTypeDef config;
	u32 errorstate;
	u32 tickstart = DTimestamp_Get();
	u32 data, dataremaining;
	u32 add = BlockAdd;
	u8 *tempbuff = pData;
	SD_RESULT ret;

	if (hsd->State == SD_STATE_READY) {
		ret = SD_TransPreCheck(hsd, pData, add, NumberOfBlocks);
		if (ret != SD_OK) {
			return ret;
		}

		/* Configure the SD DPSM (Data Path State Machine) */
		config.TransType = SDIO_TRANS_MULTI_BLK;
		config.TransDir = SDIO_TRANS_HOST_TO_CARD;
		config.BlockSize = SD_BLOCK_SIZE;
		config.BlockCnt = NumberOfBlocks;
		config.AutoCmdEn = SDIO_TRANS_AUTO_DIS;
		config.DmaEn = SDIO_TRANS_DMA_DIS;
		SDIO_ConfigData(SDIOx, &config);

		if (hsd->Card.CardType != CARD_SDHC_SDXC) {
			add *= SD_BLOCK_SIZE;
		}

		/* Write Blocks in Polling mode */
		if (NumberOfBlocks > 1U) {
			hsd->Context = SD_CONTEXT_WRITE_MULTIPLE_BLOCK;

			/* Send CMD55 APP_CMD with argument as card's RCA.*/
			errorstate = SDMMC_CmdAppCommand(SDIOx, (u32)(hsd->Card.RelCardAdd << 16U));
			if (errorstate != SD_ERROR_NONE) {
				return errorstate;
			}

			/* Send ACMD23 with argument as NumberOfBlocks for faster multiple block WR */
			errorstate = SDMMC_CmdSetWrBlkEraseCnt(SDIOx, NumberOfBlocks);
			if (errorstate != SD_ERROR_NONE) {
				return errorstate;
			}

			/* Write Multi Block command */
			errorstate = SDMMC_CmdWriteMultiBlock(SDIOx, add);
		} else {
			hsd->Context = SD_CONTEXT_WRITE_SINGLE_BLOCK;

			/* Write Single Block command */
			errorstate = SDMMC_CmdWriteSingleBlock(SDIOx, add);
		}

		if (errorstate != SD_ERROR_NONE) {
			/* Clear all the static flags */
			SDIO_ClearErrSts(SDIOx, SDIO_ERR_FLAG);
			hsd->ErrorCode |= errorstate;
			hsd->State = SD_STATE_READY;
			hsd->Context = SD_CONTEXT_NONE;
			return SD_ERROR;
		}

		/* Write block(s) in polling mode */
		dataremaining = config.BlockCnt * config.BlockSize;

		while (!((SDIO_GetErrSts(SDIOx) & SDIO_DAT_ERR) || (SDIO_GetNormSts(SDIOx) & SDIOHOST_BIT_XFER_COMPLETE))) {
			if ((SDIO_GetNormSts(SDIOx) & SDIOHOST_BIT_BUFF_WRITE_READY) && (dataremaining > 0U)) {
				/* Write data to SDIO Tx FIFO */
				SDIO_ClearNormSts(SDIOx, SDIOHOST_BIT_BUFF_WRITE_READY);

				while ((SDIO_GetStatus(SDIOx) & SDIOHOST_BIT_BUFF_WRITE_EN) && (dataremaining > 0U)) {
					data = (u32)(*tempbuff);
					tempbuff++;
					data |= ((u32)(*tempbuff) << 8U);
					tempbuff++;
					data |= ((u32)(*tempbuff) << 16U);
					tempbuff++;
					data |= ((u32)(*tempbuff) << 24U);
					tempbuff++;
					dataremaining -= 4U;
					SDIO_WriteFIFO(SDIOx, &data);
				}
			}

			if (is_timeout(tickstart, Timeout) || (Timeout == 0U)) {
				/* Clear all the static flags */
				SDIO_ClearErrSts(SDIOx, SDIO_ERR_FLAG);
				hsd->ErrorCode |= SD_ERROR_TIMEOUT;
				hsd->State = SD_STATE_READY;
				hsd->Context = SD_CONTEXT_NONE;
				return SD_ERROR;
			}
		}

		/* Send stop transmission command in case of multiblock write */
		if ((SDIO_GetNormSts(SDIOx) & SDIOHOST_BIT_XFER_COMPLETE) && (NumberOfBlocks > 1U)) {
			/* Send stop transmission command */
			errorstate = SDMMC_CmdStopTransfer(SDIOx);
			if (errorstate != SD_ERROR_NONE) {
				/* Clear all the static flags */
				SDIO_ClearErrSts(SDIOx, SDIO_ERR_FLAG);
				hsd->ErrorCode |= errorstate;
				hsd->State = SD_STATE_READY;
				hsd->Context = SD_CONTEXT_NONE;
				return SD_ERROR;
			}
		}

		/* Get error state */
		if (SDIO_GetErrSts(SDIOx) & SDIOHOST_BIT_DATA_TIMEOUT_ERR) {
			/* Clear all the static flags */
			SDIO_ClearErrSts(SDIOx, SDIO_ERR_FLAG);
			hsd->ErrorCode |= SD_ERROR_DATA_TIMEOUT;
			hsd->State = SD_STATE_READY;
			hsd->Context = SD_CONTEXT_NONE;
			return SD_ERROR;
		} else if (SDIO_GetErrSts(SDIOx) & SDIOHOST_BIT_DATA_CRC_ERR) {
			/* Clear all the static flags */
			SDIO_ClearErrSts(SDIOx, SDIO_ERR_FLAG);
			hsd->ErrorCode |= SD_ERROR_DATA_CRC_FAIL;
			hsd->State = SD_STATE_READY;
			hsd->Context = SD_CONTEXT_NONE;
			return SD_ERROR;
		} else {
			/* Nothing to do */
		}

		SDIO_ClearNormSts(SDIOx, SDIOHOST_BIT_XFER_COMPLETE);
		/* Clear all the static flags */
		SDIO_ClearErrSts(SDIOx, SDIO_DAT_ERR);

		hsd->State = SD_STATE_READY;

		SDIO_ConfigNormIntSig(SDIOx, SDIOHOST_BIT_CARD_INT_SIGNAL_EN, ENABLE);

		return SD_OK;
	} else {
		RTK_LOGW(TAG, "%s SDH NOT READY!\n", __FUNCTION__);
		hsd->ErrorCode |= SD_ERROR_BUSY;
		return SD_ERROR;
	}
}

/**
 * @brief  Reads block(s) from a specified address in a card. The Data transfer
 *         is managed in interrupt mode.
 * @note   This API should be followed by a check on the card state through SD_GetCardState().
 * @note   You could also check the IT transfer process through the SD Rx
 *         interrupt event.
 * @param  hsd: Pointer to SD handle
 * @param  pData: Pointer to the buffer that will contain the received data
 * @param  BlockAdd: Block Address from where data is to be read
 * @param  NumberOfBlocks: Number of blocks to read.
 * @retval HAL status
 */
SD_RESULT SD_ReadBlocks_IT(SD_HdlTypeDef *hsd, u8 *pData, u32 BlockAdd, u32 NumberOfBlocks)
{
	SDIOHOST_TypeDef *SDIOx = hsd->Instance;
	SDIO_DataInitTypeDef config;
	u32 errorstate;
	u32 add = BlockAdd;
	SD_RESULT ret;

	if (hsd->State == SD_STATE_READY) {
		ret = SD_TransPreCheck(hsd, pData, add, NumberOfBlocks);
		if (ret != SD_OK) {
			return ret;
		}

		hsd->pRxBuffPtr = pData;
		hsd->RxXferSize = SD_BLOCK_SIZE * NumberOfBlocks;

		/* Enable transfer interrupts */
		SDIO_ConfigNormIntSig(SDIOx, SDIOHOST_BIT_BUFF_READ_READY, ENABLE);
		SDIO_ConfigErrIntSig(SDIOx, SDIO_DAT_ERR, ENABLE);

		/* Configure the SD DPSM (Data Path State Machine) */
		config.TransType = SDIO_TRANS_MULTI_BLK;
		config.TransDir = SDIO_TRANS_CARD_TO_HOST;
		config.BlockSize = SD_BLOCK_SIZE;
		config.BlockCnt = NumberOfBlocks;
		config.AutoCmdEn = SDIO_TRANS_AUTO_DIS;
		config.DmaEn = SDIO_TRANS_DMA_DIS;
		SDIO_ConfigData(SDIOx, &config);

		if (hsd->Card.CardType != CARD_SDHC_SDXC) {
			add *= SD_BLOCK_SIZE;
		}

		SD_PreDMATrans(hsd);

		/* Read Blocks in IT mode */
		if (NumberOfBlocks > 1U) {
			hsd->Context = (SD_CONTEXT_READ_MULTIPLE_BLOCK | SD_CONTEXT_IT);

			/* Read Multi Block command */
			errorstate = SDMMC_CmdReadMultiBlock(SDIOx, add);
		} else {
			hsd->Context = (SD_CONTEXT_READ_SINGLE_BLOCK | SD_CONTEXT_IT);

			/* Read Single Block command */
			errorstate = SDMMC_CmdReadSingleBlock(SDIOx, add);
		}

		if (errorstate != SD_ERROR_NONE) {
			/* Clear all the static flags */
			SDIO_ClearErrSts(SDIOx, SDIO_ERR_FLAG);
			hsd->ErrorCode |= errorstate;
			hsd->State = SD_STATE_READY;
			hsd->Context = SD_CONTEXT_NONE;
			return SD_ERROR;
		}

		if (SD_WaitTransDone(hsd, SDMMC_DAT_TIMEOUT) != HAL_OK) {
			RTK_LOGE(TAG, "%s WaitTransDone error!\n", __FUNCTION__);
			return SD_ERROR;
		}

		SDIO_ConfigNormIntSig(SDIOx, SDIOHOST_BIT_CARD_INT_SIGNAL_EN, ENABLE);

		return SD_OK;
	} else {
		RTK_LOGW(TAG, "%s SDH NOT READY!\n", __FUNCTION__);
		return SD_ERROR;
	}
}

/**
 * @brief  Writes block(s) to a specified address in a card. The Data transfer
 *         is managed in interrupt mode.
 * @note   This API should be followed by a check on the card state through SD_GetCardState().
 * @note   You could also check the IT transfer process through the SD Tx
 *         interrupt event.
 * @param  hsd: Pointer to SD handle
 * @param  pData: Pointer to the buffer that will contain the data to transmit
 * @param  BlockAdd: Block Address where data will be written
 * @param  NumberOfBlocks: Number of blocks to write
 * @retval HAL status
 */
SD_RESULT SD_WriteBlocks_IT(SD_HdlTypeDef *hsd, u8 *pData, u32 BlockAdd, u32 NumberOfBlocks)
{
	SDIOHOST_TypeDef *SDIOx = hsd->Instance;
	SDIO_DataInitTypeDef config;
	u32 errorstate;
	u32 add = BlockAdd;
	SD_RESULT ret;

	if (hsd->State == SD_STATE_READY) {
		ret = SD_TransPreCheck(hsd, pData, add, NumberOfBlocks);
		if (ret != SD_OK) {
			return ret;
		}

		hsd->pTxBuffPtr = pData;
		hsd->TxXferSize = SD_BLOCK_SIZE * NumberOfBlocks;

		/* Enable transfer interrupts */
		SDIO_ConfigNormIntSig(SDIOx, SDIOHOST_BIT_BUFF_WRITE_READY, ENABLE);
		SDIO_ConfigErrIntSig(SDIOx, SDIO_DAT_ERR, ENABLE);

		config.TransType = SDIO_TRANS_MULTI_BLK;
		config.TransDir = SDIO_TRANS_HOST_TO_CARD;
		config.BlockSize = SD_BLOCK_SIZE; // 512Byte
		config.BlockCnt = NumberOfBlocks;
		config.AutoCmdEn = SDIO_TRANS_AUTO_DIS;
		config.DmaEn = SDIO_TRANS_DMA_DIS;
		SDIO_ConfigData(SDIOx, &config);

		if (hsd->Card.CardType != CARD_SDHC_SDXC) {
			add *= SD_BLOCK_SIZE;
		}

		/* Write Blocks in Polling mode */
		if (NumberOfBlocks > 1U) {
			hsd->Context = (SD_CONTEXT_WRITE_MULTIPLE_BLOCK | SD_CONTEXT_IT);

			/* Send CMD55 APP_CMD with argument as card's RCA.*/
			errorstate = SDMMC_CmdAppCommand(SDIOx, (u32)(hsd->Card.RelCardAdd << 16U));
			if (errorstate != SD_ERROR_NONE) {
				return errorstate;
			}

			/* Send ACMD23 with argument as NumberOfBlocks for faster multiple block WR */
			errorstate = SDMMC_CmdSetWrBlkEraseCnt(SDIOx, NumberOfBlocks);
			if (errorstate != SD_ERROR_NONE) {
				return errorstate;
			}

			SD_PreDMATrans(hsd);

			/* Write Multi Block command */
			errorstate = SDMMC_CmdWriteMultiBlock(SDIOx, add);
		} else {
			hsd->Context = (SD_CONTEXT_WRITE_SINGLE_BLOCK | SD_CONTEXT_IT);

			SD_PreDMATrans(hsd);

			/* Write Single Block command */
			errorstate = SDMMC_CmdWriteSingleBlock(SDIOx, add);
		}

		if (errorstate != SD_ERROR_NONE) {
			/* Clear all the static flags */
			SDIO_ClearErrSts(SDIOx, SDIO_ERR_FLAG);
			hsd->ErrorCode |= errorstate;
			hsd->State = SD_STATE_READY;
			hsd->Context = SD_CONTEXT_NONE;
			return SD_ERROR;
		}

		if (SD_WaitTransDone(hsd, SDMMC_DAT_TIMEOUT) != HAL_OK) {
			RTK_LOGE(TAG, "%s WaitTransDone error!\n", __FUNCTION__);
			return SD_ERROR;
		}

		SDIO_ConfigNormIntSig(SDIOx, SDIOHOST_BIT_CARD_INT_SIGNAL_EN, ENABLE);

		return SD_OK;
	} else {
		RTK_LOGW(TAG, "%s SDH NOT READY!\n", __FUNCTION__);
		return SD_ERROR;
	}
}

/**
 * @brief  Reads block(s) from a specified address in a card. The Data transfer
 *         is managed by DMA mode.
 * @note   This API should be followed by a check on the card state through SD_GetCardState().
 * @note   You could also check the DMA transfer process through the SD Rx
 *         interrupt event.
 * @param  hsd: Pointer SD handle
 * @param  pData: Pointer to the buffer that will contain the received data
 * @param  BlockAdd: Block Address from where data is to be read
 * @param  NumberOfBlocks: Number of blocks to read.
 * @retval HAL status
 */
SD_RESULT SD_ReadBlocks_DMA(SD_HdlTypeDef *hsd, u8 *pData, u32 BlockAdd, u32 NumberOfBlocks)
{
	SDIOHOST_TypeDef *SDIOx = hsd->Instance;
	SDIO_DataInitTypeDef config;
	u32 status;
	u32 errorstate;
	u32 add = BlockAdd;
	SD_RESULT ret;

	if (hsd->State == SD_STATE_READY) {
		ret = SD_TransPreCheck(hsd, pData, add, NumberOfBlocks);
		if (ret != SD_OK) {
			return ret;
		}

		/* Configure the SD DPSM (Data Path State Machine) */
		config.TransType = SDIO_TRANS_MULTI_BLK;
		config.TransDir = SDIO_TRANS_CARD_TO_HOST;
		config.BlockSize = SD_BLOCK_SIZE;
		config.BlockCnt = NumberOfBlocks;
		config.AutoCmdEn = SDIO_TRANS_AUTO_DIS;
		config.DmaEn = SDIO_TRANS_DMA_EN;
		SDIO_ConfigData(SDIOx, &config);

		status = SDIO_ConfigDMA(SDIOx, SDIO_SDMA_MODE, (u32)pData);
		if (status != HAL_OK) {
			hsd->ErrorCode |= SD_ERROR_INVALID_PARAMETER;
			hsd->State = SD_STATE_READY;
			return SD_ERROR;
		}

		if (hsd->Card.CardType != CARD_SDHC_SDXC) {
			add *= SD_BLOCK_SIZE;
		}

		SD_PreDMATrans(hsd);

		/* Read Blocks in DMA mode */
		if (NumberOfBlocks > 1U) {
			hsd->Context = (SD_CONTEXT_READ_MULTIPLE_BLOCK | SD_CONTEXT_DMA);

			/* Read Multi Block command */
			errorstate = SDMMC_CmdReadMultiBlock(SDIOx, add);
		} else {
			hsd->Context = (SD_CONTEXT_READ_SINGLE_BLOCK | SD_CONTEXT_DMA);

			/* Read Single Block command */
			errorstate = SDMMC_CmdReadSingleBlock(SDIOx, add);
		}
		if (errorstate != SD_ERROR_NONE) {
			/* Clear all the static flags */
			SDIO_ClearErrSts(SDIOx, SDIO_ERR_FLAG);
			hsd->ErrorCode |= errorstate;
			hsd->State = SD_STATE_READY;
			hsd->Context = SD_CONTEXT_NONE;
			return SD_ERROR;
		}

		if (SD_WaitTransDone(hsd, SDMMC_READ_TIMEOUT * NumberOfBlocks) != HAL_OK) {
			RTK_LOGE(TAG, "%s WaitTransDone error!\n", __FUNCTION__);
			return SD_ERROR;
		}

		SDIO_ConfigNormIntSig(SDIOx, SDIOHOST_BIT_CARD_INT_SIGNAL_EN, ENABLE);

		return SD_OK;
	} else {
		RTK_LOGW(TAG, "%s SDH NOT READY!\n", __FUNCTION__);
		return SD_ERROR;
	}
}

/**
 * @brief  Writes block(s) to a specified address in a card. The Data transfer
 *         is managed by DMA mode.
 * @note   This API should be followed by a check on the card state through SD_GetCardState().
 * @note   You could also check the DMA transfer process through the SD Tx
 *         interrupt event.
 * @param  hsd: Pointer to SD handle
 * @param  pData: Pointer to the buffer that will contain the data to transmit
 * @param  BlockAdd: Block Address where data will be written
 * @param  NumberOfBlocks: Number of blocks to write
 * @retval HAL status
 */
SD_RESULT SD_WriteBlocks_DMA(SD_HdlTypeDef *hsd, u8 *pData, u32 BlockAdd, u32 NumberOfBlocks)
{
	SDIOHOST_TypeDef *SDIOx = hsd->Instance;
	SDIO_DataInitTypeDef config;
	u32 status;
	u32 errorstate;
	u32 add = BlockAdd;
	SD_RESULT ret;

	if (hsd->State == SD_STATE_READY) {
		ret = SD_TransPreCheck(hsd, pData, add, NumberOfBlocks);
		if (ret != SD_OK) {
			return ret;
		}

		/* Configure the SD DPSM (Data Path State Machine) */
		config.TransType = SDIO_TRANS_MULTI_BLK;
		config.TransDir = SDIO_TRANS_HOST_TO_CARD;
		config.BlockSize = SD_BLOCK_SIZE;
		config.BlockCnt = NumberOfBlocks;
		config.AutoCmdEn = SDIO_TRANS_AUTO_DIS;
		config.DmaEn = SDIO_TRANS_DMA_EN;
		SDIO_ConfigData(SDIOx, &config);

		status = SDIO_ConfigDMA(SDIOx, SDIO_SDMA_MODE, (u32)pData);
		if (status != HAL_OK) {
			hsd->ErrorCode |= SD_ERROR_INVALID_PARAMETER;
			hsd->State = SD_STATE_READY;
			return SD_ERROR;
		}

		if (hsd->Card.CardType != CARD_SDHC_SDXC) {
			add *= SD_BLOCK_SIZE;
		}

		/* Write Blocks in Polling mode */
		if (NumberOfBlocks > 1U) {
			hsd->Context = (SD_CONTEXT_WRITE_MULTIPLE_BLOCK | SD_CONTEXT_DMA);

			/* Send CMD55 APP_CMD with argument as card's RCA.*/
			errorstate = SDMMC_CmdAppCommand(SDIOx, (u32)(hsd->Card.RelCardAdd << 16U));
			if (errorstate != SD_ERROR_NONE) {
				return errorstate;
			}

			/* Send ACMD23 with argument as NumberOfBlocks for faster multiple block WR */
			errorstate = SDMMC_CmdSetWrBlkEraseCnt(SDIOx, NumberOfBlocks);
			if (errorstate != SD_ERROR_NONE) {
				return errorstate;
			}

			SD_PreDMATrans(hsd);

			/* Write Multi Block command */
			errorstate = SDMMC_CmdWriteMultiBlock(SDIOx, add);
		} else {
			hsd->Context = (SD_CONTEXT_WRITE_SINGLE_BLOCK | SD_CONTEXT_DMA);

			SD_PreDMATrans(hsd);

			/* Write Single Block command */
			errorstate = SDMMC_CmdWriteSingleBlock(SDIOx, add);
		}

		if (errorstate != SD_ERROR_NONE) {
			/* Clear all the static flags */
			SDIO_ClearErrSts(SDIOx, SDIO_ERR_FLAG);
			hsd->ErrorCode |= errorstate;
			hsd->State = SD_STATE_READY;
			hsd->Context = SD_CONTEXT_NONE;
			return SD_ERROR;
		}

		if (SD_WaitTransDone(hsd, SDMMC_WRITE_TIMEOUT * NumberOfBlocks) != HAL_OK) {
			RTK_LOGE(TAG, "%s WaitTransDone error!\n", __FUNCTION__);
			return SD_ERROR;
		}

		SDIO_ConfigNormIntSig(SDIOx, SDIOHOST_BIT_CARD_INT_SIGNAL_EN, ENABLE);

		return SD_OK;
	} else {
		RTK_LOGW(TAG, "%s SDH NOT READY!\n", __FUNCTION__);
		return SD_ERROR;
	}
}

/* copied from pro2 -  hal_rtl_sdiohost_create_adma_entry */
void SD_CreateADMAEntry(u32 *pBuf, u16 Length, sdioh_adma_line_end_t End, u32 *pDescTable)
{
	u32 *adma_desc_table_ptr = pDescTable;
	adma_descriptor_line_t adma_desc;

	// create adma entry based on parameters passed in
	adma_desc.length = Length;
	adma_desc.stuff_1 = 0x0;
	adma_desc.act = 0x2; // 10b-Transfer Data
	adma_desc.stuff_0 = 0x0;
	adma_desc.interrupt = 0x0;
	adma_desc.end = End;
	adma_desc.valid = 0x1;

	// load adma descriptor line in address space specified
	adma_desc_table_ptr[0] = *(u32 *)&adma_desc;
	adma_desc_table_ptr[1] = (u32)pBuf;

	DCache_CleanInvalidate((u32)adma_desc_table_ptr, 2 * sizeof(u32));
}

SD_RESULT SD_ReadBlocks_ADMA(SD_HdlTypeDef *hsd, u8 *pData, u32 BlockAdd, u32 *pDescTbl, u32 DescItemNum)
{
	SDIOHOST_TypeDef *SDIOx = hsd->Instance;
	SDIO_DataInitTypeDef config;
	u32 status;
	u32 errorstate;
	u32 add = BlockAdd;
	u32 i;
	u32 blockcnt;
	u32 total_len = 0;
	SD_RESULT ret;

	if (hsd->State == SD_STATE_READY) {
		if (NULL == pDescTbl) {
			hsd->ErrorCode |= SD_ERROR_INVALID_PARAMETER;
			return SD_ERROR;
		}

		for (i = 0; i < DescItemNum; i++) {
			total_len += (*(pDescTbl + 2 * i) & 0xffff0000) >> 16; // [31:16]
		}

		if (total_len % SD_BLOCK_SIZE) {
			hsd->ErrorCode |= SD_ERROR_INVALID_PARAMETER;
			return SD_ERROR;
		} else {
			blockcnt = total_len / SD_BLOCK_SIZE;
		}

		ret = SD_TransPreCheck(hsd, pData, add, blockcnt);
		if (ret != SD_OK) {
			return ret;
		}

		/* Configure the SD DPSM (Data Path State Machine) */
		config.TransType = SDIO_TRANS_MULTI_BLK;
		config.TransDir = SDIO_TRANS_CARD_TO_HOST;
		config.BlockSize = SD_BLOCK_SIZE;
		config.BlockCnt = blockcnt;
		config.AutoCmdEn = SDIO_TRANS_AUTO_DIS;
		config.DmaEn = SDIO_TRANS_DMA_EN;
		SDIO_ConfigData(SDIOx, &config);

		status = SDIO_ConfigDMA(SDIOx, SDIO_ADMA2_32B_MODE, (u32)pDescTbl);
		if (status != HAL_OK) {
			hsd->ErrorCode |= SD_ERROR_INVALID_PARAMETER;
			hsd->State = SD_STATE_READY;
			return SD_ERROR;
		}

		if (hsd->Card.CardType != CARD_SDHC_SDXC) {
			add *= SD_BLOCK_SIZE;
		}

		SD_PreDMATrans(hsd);

		/* Read Blocks in DMA mode */
		if (blockcnt > 1U) {
			hsd->Context = (SD_CONTEXT_READ_MULTIPLE_BLOCK | SD_CONTEXT_DMA);

			/* Read Multi Block command */
			errorstate = SDMMC_CmdReadMultiBlock(SDIOx, add);
		} else {
			hsd->Context = (SD_CONTEXT_READ_SINGLE_BLOCK | SD_CONTEXT_DMA);

			/* Read Single Block command */
			errorstate = SDMMC_CmdReadSingleBlock(SDIOx, add);
		}

		if (errorstate != SD_ERROR_NONE) {
			/* Clear all the static flags */
			SDIO_ClearErrSts(SDIOx, SDIO_ERR_FLAG);
			hsd->ErrorCode |= errorstate;
			hsd->State = SD_STATE_READY;
			hsd->Context = SD_CONTEXT_NONE;
			return SD_ERROR;
		}

		if (SD_WaitTransDone(hsd, SDMMC_READ_TIMEOUT * blockcnt) != HAL_OK) {
			RTK_LOGE(TAG, "%s WaitTransDone error!\n", __FUNCTION__);
			return SD_ERROR;
		}

		SDIO_ConfigNormIntSig(SDIOx, SDIOHOST_BIT_CARD_INT_SIGNAL_EN, ENABLE);

		return SD_OK;
	} else {
		RTK_LOGW(TAG, "%s SDH NOT READY!\n", __FUNCTION__);
		return SD_ERROR;
	}
}

SD_RESULT SD_WriteBlocks_ADMA(SD_HdlTypeDef *hsd, u8 *pData, u32 BlockAdd, u32 *pDescTbl,
							  u32 DescItemNum)
{
	SDIOHOST_TypeDef *SDIOx = hsd->Instance;
	SDIO_DataInitTypeDef config;
	u32 status;
	u32 errorstate;
	u32 add = BlockAdd;
	u32 i;
	u32 total_len = 0;
	u32 blockcnt;
	SD_RESULT ret;

	if (hsd->State == SD_STATE_READY) {
		if (NULL == pDescTbl) {
			hsd->ErrorCode |= SD_ERROR_INVALID_PARAMETER;
			return SD_ERROR;
		}

		for (i = 0; i < DescItemNum; i++) {
			total_len += (*(pDescTbl + 2 * i) & 0xffff0000) >> 16; // [31:16]
		}

		if (total_len % SD_BLOCK_SIZE) {
			hsd->ErrorCode |= SD_ERROR_INVALID_PARAMETER;
			return SD_ERROR;
		} else {
			blockcnt = total_len / SD_BLOCK_SIZE;
		}

		ret = SD_TransPreCheck(hsd, pData, add, blockcnt);
		if (ret != SD_OK) {
			return ret;
		}

		/* Configure the SD DPSM (Data Path State Machine) */
		config.TransType = SDIO_TRANS_MULTI_BLK;
		config.TransDir = SDIO_TRANS_HOST_TO_CARD;
		config.BlockSize = SD_BLOCK_SIZE;
		config.BlockCnt = blockcnt;
		config.AutoCmdEn = SDIO_TRANS_AUTO_DIS;
		config.DmaEn = SDIO_TRANS_DMA_EN;
		SDIO_ConfigData(SDIOx, &config);

		status = SDIO_ConfigDMA(SDIOx, SDIO_ADMA2_32B_MODE, (u32)pDescTbl);
		if (status != HAL_OK) {
			hsd->ErrorCode |= SD_ERROR_INVALID_PARAMETER;
			hsd->State = SD_STATE_READY;
			return SD_ERROR;
		}

		if (hsd->Card.CardType != CARD_SDHC_SDXC) {
			add *= SD_BLOCK_SIZE;
		}

		/* Write Blocks in DMA mode */
		if (blockcnt > 1U) {
			hsd->Context = (SD_CONTEXT_WRITE_MULTIPLE_BLOCK | SD_CONTEXT_DMA);

			/* Send CMD55 APP_CMD with argument as card's RCA.*/
			errorstate = SDMMC_CmdAppCommand(SDIOx, (u32)(hsd->Card.RelCardAdd << 16U));
			if (errorstate != SD_ERROR_NONE) {
				return errorstate;
			}

			/* Send ACMD23 with argument as blockcnt for faster multiple block WR */
			errorstate = SDMMC_CmdSetWrBlkEraseCnt(SDIOx, blockcnt);
			if (errorstate != SD_ERROR_NONE) {
				return errorstate;
			}

			SD_PreDMATrans(hsd);

			/* Write Multi Block command */
			errorstate = SDMMC_CmdWriteMultiBlock(SDIOx, add);
		} else {
			hsd->Context = (SD_CONTEXT_WRITE_SINGLE_BLOCK | SD_CONTEXT_DMA);

			SD_PreDMATrans(hsd);

			/* Write Single Block command */
			errorstate = SDMMC_CmdWriteSingleBlock(SDIOx, add);
		}

		if (errorstate != SD_ERROR_NONE) {
			/* Clear all the static flags */
			SDIO_ClearErrSts(SDIOx, SDIO_ERR_FLAG);
			hsd->ErrorCode |= errorstate;
			hsd->State = SD_STATE_READY;
			hsd->Context = SD_CONTEXT_NONE;
			return SD_ERROR;
		}

		if (SD_WaitTransDone(hsd, SDMMC_WRITE_TIMEOUT * blockcnt) != HAL_OK) {
			RTK_LOGE(TAG, "%s WaitTransDone error!\n", __FUNCTION__);
			return SD_ERROR;
		}

		SDIO_ConfigNormIntSig(SDIOx, SDIOHOST_BIT_CARD_INT_SIGNAL_EN, ENABLE);

		return SD_OK;
	} else {
		RTK_LOGW(TAG, "%s SDH NOT READY!\n", __FUNCTION__);
		return SD_ERROR;
	}
}

/**
 * @brief  Erases the specified memory area of the given SD card.
 * @note   This API should be followed by a check on the card state through SD_GetCardState().
 * @param  hsd: Pointer to SD handle
 * @param  BlockStartAdd: Start Block address
 * @param  BlockEndAdd: End Block address
 * @retval HAL status
 */
SD_RESULT SD_Erase(SD_HdlTypeDef *hsd, u32 BlockStartAdd, u32 BlockEndAdd)
{
	SDIOHOST_TypeDef *SDIOx = hsd->Instance;
	u32 errorstate;
	u32 start_add = BlockStartAdd;
	u32 end_add = BlockEndAdd;

	if (hsd->State == SD_STATE_READY) {
		hsd->ErrorCode = SD_ERROR_NONE;

		if (end_add < start_add) {
			hsd->ErrorCode |= SD_ERROR_INVALID_PARAMETER;
			return SD_ERROR;
		}

		if (end_add > (hsd->Card.LogBlockNbr)) {
			hsd->ErrorCode |= SD_ERROR_ADDR_OUT_OF_RANGE;
			return SD_ERROR;
		}

		SDIO_ConfigNormIntSig(SDIOx, 0xFFFFFFFF, DISABLE); // disable all the normal int without err int
		SDIO_ClearNormSts(SDIOx, SDIO_GetNormSts(SDIOx)); // clear old flags

		hsd->State = SD_STATE_BUSY;

		/* Check if the card command class supports erase command */
		if (((hsd->Card.Class) & SDMMC_CCC_ERASE) == 0U) {
			/* Clear all the static flags */
			SDIO_ClearErrSts(SDIOx, SDIO_ERR_FLAG);
			hsd->ErrorCode |= SD_ERROR_REQUEST_NOT_APPLICABLE;
			hsd->State = SD_STATE_READY;
			return SD_ERROR;
		}

		if ((SDIO_GetResponse(SDIOx, SDIO_RESP0) & SDMMC_R1_CARD_IS_LOCKED) == SDMMC_R1_CARD_IS_LOCKED) {
			/* Clear all the static flags */
			SDIO_ClearErrSts(SDIOx, SDIO_ERR_FLAG);
			hsd->ErrorCode |= SD_ERROR_LOCK_UNLOCK_FAILED;
			hsd->State = SD_STATE_READY;
			return SD_ERROR;
		}

		/* Get start and end block for high capacity cards */
		if (hsd->Card.CardType != CARD_SDHC_SDXC) {
			start_add *= SD_BLOCK_SIZE;
			end_add *= SD_BLOCK_SIZE;
		}

		/* According to sd-card spec 1.0 ERASE_GROUP_START (CMD32) and erase_group_end(CMD33) */
		/* Send CMD32 SD_ERASE_GRP_START with argument as addr  */
		errorstate = SDMMC_CmdSDEraseStartAdd(SDIOx, start_add);
		if (errorstate != SD_ERROR_NONE) {
			/* Clear all the static flags */
			SDIO_ClearErrSts(SDIOx, SDIO_ERR_FLAG);
			hsd->ErrorCode |= errorstate;
			hsd->State = SD_STATE_READY;
			return SD_ERROR;
		}

		/* Send CMD33 SD_ERASE_GRP_END with argument as addr  */
		errorstate = SDMMC_CmdSDEraseEndAdd(SDIOx, end_add);
		if (errorstate != SD_ERROR_NONE) {
			/* Clear all the static flags */
			SDIO_ClearErrSts(SDIOx, SDIO_ERR_FLAG);
			hsd->ErrorCode |= errorstate;
			hsd->State = SD_STATE_READY;
			return SD_ERROR;
		}

		/* Send CMD38 ERASE */
		errorstate = SDMMC_CmdErase(SDIOx, end_add - start_add + 1);
		if (errorstate != SD_ERROR_NONE) {
			/* Clear all the static flags */
			SDIO_ClearErrSts(SDIOx, SDIO_ERR_FLAG);
			hsd->ErrorCode |= errorstate;
			hsd->State = SD_STATE_READY;
			return SD_ERROR;
		}

		SDIO_ConfigNormIntSig(SDIOx, SDIOHOST_BIT_CARD_INT_SIGNAL_EN, ENABLE);

		return SD_OK;
	} else {
		RTK_LOGW(TAG, "%s SDH NOT READY!\n", __FUNCTION__);
		return SD_ERROR;
	}
}

__weak void SD_IRQ_NOTIFY(void)
{
	return;
}

/**
 * @brief  This function handles SD card interrupt request.
 * @param  hsd: Pointer to SD handle
 * @retval None
 */
static u32 SD_IRQHandler(void *pData)
{
	SD_HdlTypeDef *hsd = (SD_HdlTypeDef *)pData;
	SDIOHOST_TypeDef *SDIOx = hsd->Instance;
	u32 errorstate;
	u32 context = hsd->Context;
	u32 norm_sts, err_sts;

	norm_sts = SDIO_GetNormSts(SDIOx);
	err_sts = SDIO_GetErrSts(SDIOx);

	/* clear interrupts */
	SDIO_ClearNormSts(SDIOx, norm_sts);
	SDIO_ClearErrSts(SDIOx, err_sts);

	/* get masked sts */
	norm_sts &= SDIOx->SDIOHOST_NORMAL_SIG_EN;
	err_sts &= SDIOx->SDIOHOST_ERR_SIG_EN;

	if (norm_sts & SDIOHOST_BIT_CARD_INT) {
		/* note1: Writing 1 to card int status bit does not clear card int status. */
		/* note2: Set STATUS_EN to 0 to clear card int status. */
		// SDIO_ConfigNormIntSts(SDIOx, SDIOHOST_BIT_CARD_INT_STATUS_EN, DISABLE);
		SD_IRQ_NOTIFY();
	}

	/* Check for SDIO interrupt flags */
	if ((norm_sts & SDIOHOST_BIT_BUFF_READ_READY) && ((context & SD_CONTEXT_IT) != 0U)) {
		SD_Read_IT(hsd);
	} else if (norm_sts & SDIOHOST_BIT_XFER_COMPLETE) {

		if ((context & SD_CONTEXT_IT) != 0U || (context & SD_CONTEXT_DMA) != 0U) {

			if (((context & SD_CONTEXT_READ_MULTIPLE_BLOCK) != 0U) ||
				((context & SD_CONTEXT_WRITE_MULTIPLE_BLOCK) != 0U)) {
				errorstate = SDMMC_CmdStopTransfer(SDIOx);
				if (errorstate != SD_ERROR_NONE) {
					hsd->ErrorCode |= errorstate;
					// TODO: ErrorCallback
				}
			}
		} else {
			/* Nothing to do */
		}

		if (sd_sema_give_isr_fn != NULL) {
			sd_sema_give_isr_fn(SD_SEMA_MAX_DELAY);
		}

	} else if ((norm_sts & SDIOHOST_BIT_BUFF_WRITE_READY) && ((context & SD_CONTEXT_IT) != 0U)) {
		SD_Write_IT(hsd);
	} else if (err_sts & SDIO_DAT_ERR) {
		/* Set Error code */
		if (err_sts & SDIOHOST_BIT_DATA_CRC_ERR) {
			hsd->ErrorCode |= SD_ERROR_DATA_CRC_FAIL;
		}
		if (err_sts & SDIOHOST_BIT_DATA_TIMEOUT_ERR) {
			hsd->ErrorCode |= SD_ERROR_DATA_TIMEOUT;
		}

		hsd->ErrorCode |= SDMMC_CmdStopTransfer(SDIOx);

		if ((context & SD_CONTEXT_IT) != 0U) {
			/* Set the SD state to ready to be able to start again the process */
			hsd->State = SD_STATE_READY;
			hsd->Context = SD_CONTEXT_NONE;
			// TODO: ErrorCallback
		} else if ((context & SD_CONTEXT_DMA) != 0U) {

			hsd->ErrorCode = SD_ERROR_NONE;
			hsd->State = SD_STATE_READY;
			hsd->Context = SD_CONTEXT_NONE;
			// TODO: AbortCpltCallback

		} else {
			/* Nothing to do */
		}
	} else {
		/* Nothing to do */
	}

	return 0;
}

/**
 * @brief  Returns information the information of the card which are stored on
 *         the CID register.
 * @param  hsd: Pointer to SD handle
 * @param  pCID: Pointer to a SD_CardCIDTypeDef structure that
 *         contains all CID register parameters
 * @retval HAL status
 */
void SD_GetCardCID(SD_HdlTypeDef *hsd, SD_CardCIDTypeDef *pCID)
{
	pCID->ManufacturerID = (u8)((hsd->CID[3] & 0x00FF0000U) >> 16U);

	pCID->OEM_AppliID = (u16)(hsd->CID[3] & 0x0000FFFFU);

	pCID->ProdName1 = (u32)(hsd->CID[2]);

	pCID->ProdName2 = (u8)((hsd->CID[1] & 0xFF000000U) >> 24U);

	pCID->ProdRev = (u8)((hsd->CID[1] & 0x00FF0000U) >> 16U);

	pCID->ProdSN = (((hsd->CID[1] & 0x0000FFFFU) << 16U) | ((hsd->CID[0] & 0xFFFF0000U) >> 16U));

	pCID->Reserved1 = (u8)((hsd->CID[0] & 0x0000F000U) >> 12U);

	pCID->ManufactDate = (u16)(hsd->CID[0] & 0x00000FFFU);

	RTK_LOGI(TAG, "Manufacturer ID: %d\n", pCID->ManufacturerID);
	RTK_LOGI(TAG, "OEM/Application ID: %c%c\n", (pCID->OEM_AppliID & 0x0000FF00U) >> 8U, pCID->OEM_AppliID & 0x000000FFU);
	RTK_LOGI(TAG, "Product name: %c%c%c%c%c\n", (pCID->ProdName1 & 0xFF000000U) >> 24U, (pCID->ProdName1 & 0x00FF0000U) >> 16U,
			 (pCID->ProdName1 & 0x0000FF00U) >> 8U, pCID->ProdName1 & 0x000000FFU, pCID->ProdName2);
	RTK_LOGI(TAG, "Product serial number: %08x\n", pCID->ProdSN);
	RTK_LOGI(TAG, "Manufacturing date: %d/%d\n", 2000 + ((pCID->ManufactDate & 0x00000FF0U) >> 4U), pCID->ManufactDate & 0x0000000FU);
}

/**
 * @brief  Returns information the information of the card which are stored on
 *         the CSD register.
 * @param  hsd: Pointer to SD handle
 * @param  pCSD: Pointer to a SD_CardCSDTypeDef structure that
 *         contains all CSD register parameters
 * @retval HAL status
 */
SD_RESULT SD_GetCardCSD(SD_HdlTypeDef *hsd, SD_CardCSDTypeDef *pCSD)
{
	SDIOHOST_TypeDef *SDIOx = hsd->Instance;

	pCSD->CSDStruct = (u8)((hsd->CSD[3] & 0x00C00000U) >> 22U);

	pCSD->Reserved1 = (u8)((hsd->CSD[3] & 0x003F0000U) >> 16U);

	pCSD->TAAC = (u8)((hsd->CSD[3] & 0x0000FF00U) >> 8U);

	pCSD->NSAC = (u8)((hsd->CSD[3] & 0x000000FFU) >> 8U);

	pCSD->MaxBusClkFrec = (u8)((hsd->CSD[2] & 0xFF000000U) >> 24U);

	pCSD->CardComdClasses = (u16)((hsd->CSD[2] & 0x00FFF000U) >> 12U);

	pCSD->RdBlockLen = (u8)((hsd->CSD[2] & 0x00000F00U) >> 8U);

	pCSD->PartBlockRead = (u8)((hsd->CSD[2] & 0x00000080U) >> 7U);

	pCSD->WrBlockMisalign = (u8)((hsd->CSD[2] & 0x00000040U) >> 6U);

	pCSD->RdBlockMisalign = (u8)((hsd->CSD[2] & 0x00000020U) >> 5U);

	pCSD->DSRImpl = (u8)((hsd->CSD[2] & 0x00000010U) >> 4U);

	pCSD->Reserved2 = 0U; /*!< Reserved */

	if (hsd->Card.CardType == CARD_SDSC) {
		pCSD->DeviceSize = (((hsd->CSD[2] & 0x00000003U) << 20U) | ((hsd->CSD[1] & 0xFFC00000U) >> 22U));

		pCSD->MaxRdCurrentVDDMin = (u8)((hsd->CSD[1] & 0x0038000000U) >> 19U);

		pCSD->MaxRdCurrentVDDMax = (u8)((hsd->CSD[1] & 0x00070000U) >> 16U);

		pCSD->MaxWrCurrentVDDMin = (u8)((hsd->CSD[1] & 0x0000E000U) >> 13U);

		pCSD->MaxWrCurrentVDDMax = (u8)((hsd->CSD[1] & 0x00001C00U) >> 10U);

		pCSD->DeviceSizeMul = (u8)((hsd->CSD[1] & 0x00000380U) >> 7U);

		hsd->Card.BlockNbr = (u32)(pCSD->DeviceSize + 1U);
		hsd->Card.BlockNbr *= (1UL << ((pCSD->DeviceSizeMul & 0x07U) + 2U));
		hsd->Card.BlockSize = (1UL << (pCSD->RdBlockLen & 0x0FU));

		hsd->Card.LogBlockNbr = (hsd->Card.BlockNbr) * ((hsd->Card.BlockSize) / SD_BLOCK_SIZE);
		hsd->Card.LogBlockSize = SD_BLOCK_SIZE;
		RTK_LOGI(TAG, "CSD Version:1.0\n");
		RTK_LOGI(TAG, "User data area capacity: %d MB\n", hsd->Card.LogBlockNbr / 1024 * hsd->Card.LogBlockSize / 1024);
	} else if (hsd->Card.CardType == CARD_SDHC_SDXC) {
		/* Byte 7 */
		pCSD->DeviceSize = ((hsd->CSD[1] & 0x3FFFFF00U) >> 8U);
		hsd->Card.BlockNbr = ((pCSD->DeviceSize + 1U) * 1024U);
		hsd->Card.LogBlockNbr = hsd->Card.BlockNbr;
		hsd->Card.BlockSize = SD_BLOCK_SIZE;
		hsd->Card.LogBlockSize = hsd->Card.BlockSize;
		RTK_LOGI(TAG, "CSD Version:2.0\n");
		RTK_LOGI(TAG, "User data area capacity: %d GB\n", hsd->Card.LogBlockNbr / 1024 / 1024 * hsd->Card.LogBlockSize / 1024);
	} else {
		/* Clear all the static flags */
		SDIO_ClearErrSts(SDIOx, SDIO_ERR_FLAG);
		hsd->ErrorCode |= SD_ERROR_UNSUPPORTED_FEATURE;
		hsd->State = SD_STATE_READY;
		return SD_ERROR;
	}

	pCSD->EraseGrSize = (u8)((hsd->CSD[1] & 0x00000040U) >> 6U);

	pCSD->EraseGrMul = (((hsd->CSD[1] & 0x0000003FU) << 1U) | ((hsd->CSD[0] & 0x80000000U) >> 31U));

	pCSD->WrProtectGrSize = (u8)((hsd->CSD[0] & 0x7F000000U) >> 24);

	pCSD->WrProtectGrEnable = (u8)((hsd->CSD[0] & 0x00800000U) >> 23U);

	pCSD->ManDeflECC = (u8)((hsd->CSD[0] & 0x00600000U) >> 21U);

	pCSD->WrSpeedFact = (u8)((hsd->CSD[0] & 0x001C0000U) >> 18U);

	pCSD->MaxWrBlockLen = (u8)((hsd->CSD[0] & 0x0003C000U) >> 14U);

	pCSD->WriteBlockPaPartial = (u8)((hsd->CSD[0] & 0x00002000U) >> 13U);

	pCSD->Reserved3 = 0;

	pCSD->ContentProtectAppli = (u8)((hsd->CSD[0] & 0x00000100U) >> 8U);

	pCSD->FileFormatGroup = (u8)((hsd->CSD[0] & 0x00000080U) >> 7U);

	pCSD->CopyFlag = (u8)((hsd->CSD[0] & 0x00000040U) >> 6U);

	pCSD->PermWrProtect = (u8)((hsd->CSD[0] & 0x00000020U) >> 5U);

	pCSD->TempWrProtect = (u8)((hsd->CSD[0] & 0x00000010U) >> 4U);

	pCSD->FileFormat = (u8)((hsd->CSD[0] & 0x0000000CU) >> 2U);

	pCSD->ECC = (u8)(hsd->CSD[0] & 0x00000003U);

	RTK_LOGI(TAG, "Max. read data block length: %d Bytes\n", 1 << (pCSD->RdBlockLen));
	RTK_LOGI(TAG, "Max. write data block length: %d Bytes\n", 1 << (pCSD->MaxWrBlockLen));

	return SD_OK;
}

/**
 * @brief  Gets the SD card info.
 * @param  hsd: Pointer to SD handle
 * @param  pCardInfo: Pointer to the SDIO_CardInfoTypeDef structure that
 *         will contain the SD card status information
 * @retval HAL status
 */
void SD_GetCardInfo(SD_HdlTypeDef *hsd, SDIO_CardInfoTypeDef *pCardInfo)
{
	pCardInfo->CardType = (u32)(hsd->Card.CardType);
	pCardInfo->CardVersion = (u32)(hsd->Card.CardVersion);
	pCardInfo->Class = (u32)(hsd->Card.Class);
	pCardInfo->RelCardAdd = (u32)(hsd->Card.RelCardAdd);
	pCardInfo->BlockNbr = (u32)(hsd->Card.BlockNbr);
	pCardInfo->BlockSize = (u32)(hsd->Card.BlockSize);
	pCardInfo->LogBlockNbr = (u32)(hsd->Card.LogBlockNbr);
	pCardInfo->LogBlockSize = (u32)(hsd->Card.LogBlockSize);
}

/**
 * @brief  Gets the SD status info.
 * @param  hsd: Pointer to SD handle
 * @param  pStatus: Pointer to the SD_StatusTypeDef structure that
 *         will contain the SD card status information
 * @retval HAL status
 */
SD_RESULT SD_GetSDStatus(SD_HdlTypeDef *hsd, SD_StatusTypeDef *pStatus)
{
	SDIOHOST_TypeDef *SDIOx = hsd->Instance;
	u32 sd_status[16];
	u32 errorstate;
	SD_RESULT ret;

	/* Get SD status by ACMD13 */
	ret = SD_SendSDStatus(hsd, sd_status);
	if (ret != SD_OK) {
		/* Clear all the static flags */
		SDIO_ClearErrSts(SDIOx, SDIO_ERR_FLAG);
		hsd->State = SD_STATE_READY;
		return SD_ERROR;
	} else {
		pStatus->DataBusWidth = (u8)((sd_status[0] & 0xC0U) >> 6U);

		pStatus->SecuredMode = (u8)((sd_status[0] & 0x20U) >> 5U);

		pStatus->CardType = (u16)(((sd_status[0] & 0x00FF0000U) >> 8U) | ((sd_status[0] & 0xFF000000U) >> 24U));

		pStatus->ProtectedAreaSize = (((sd_status[1] & 0xFFU) << 24U) | ((sd_status[1] & 0xFF00U) << 8U) |
									  ((sd_status[1] & 0xFF0000U) >> 8U) | ((sd_status[1] & 0xFF000000U) >> 24U));

		pStatus->SpeedClass = (u8)(sd_status[2] & 0xFFU);

		pStatus->PerformanceMove = (u8)((sd_status[2] & 0xFF00U) >> 8U);

		pStatus->AllocationUnitSize = (u8)((sd_status[2] & 0xF00000U) >> 20U);

		pStatus->EraseSize = (u16)(((sd_status[2] & 0xFF000000U) >> 16U) | (sd_status[3] & 0xFFU));

		pStatus->EraseTimeout = (u8)((sd_status[3] & 0xFC00U) >> 10U);

		pStatus->EraseOffset = (u8)((sd_status[3] & 0x0300U) >> 8U);
	}

	/* Set Block Size for Card */
	if (SD_BLOCK_SIZE != 512U) {
		/* CMD16 */
		errorstate = SDMMC_CmdBlockLength(SDIOx, SD_BLOCK_SIZE);
		if (errorstate != SD_ERROR_NONE) {
			/* Clear all the static flags */
			SDIO_ClearErrSts(SDIOx, SDIO_ERR_FLAG);
			hsd->ErrorCode |= errorstate;
			hsd->State = SD_STATE_READY;
			return SD_ERROR;
		}
	}

	return SD_OK;
}

/**
 * @brief  Gets the current sd card data state.
 * @param  hsd: pointer to SD handle
 * @retval Card state
 */
SD_CardStateTypeDef SD_GetCardState(SD_HdlTypeDef *hsd)
{
	u32 cardstate;
	u32 errorstate;
	u32 resp1 = 0;

	errorstate = SD_SendCardStatus(hsd, &resp1); // CMD13
	if (errorstate != SD_ERROR_NONE) {
		hsd->ErrorCode |= errorstate;
	}

	/* [12:9]CURRENT_STATE idle/ready/ident... */
	cardstate = ((resp1 >> 9U) & 0x0FU);

	return (SD_CardStateTypeDef)cardstate;
}

/**
 * @brief  Send Status info command.
 * @param  hsd: pointer to SD handle
 * @param  pSDstatus: Pointer to the buffer that will contain the SD card status
 *         SD Status register)
 * @retval error state
 */
static SD_RESULT SD_SendSDStatus(SD_HdlTypeDef *hsd, u32 *pSDstatus)
{
	SDIOHOST_TypeDef *SDIOx = hsd->Instance;
	SDIO_DataInitTypeDef config;
	u32 errorstate;
	u32 tickstart = DTimestamp_Get();
	u32 count;
	u32 *pData = pSDstatus;

	/* Check SD response */
	if ((SDIO_GetResponse(SDIOx, SDIO_RESP0) & SDMMC_R1_CARD_IS_LOCKED) == SDMMC_R1_CARD_IS_LOCKED) {
		hsd->ErrorCode |= SD_ERROR_LOCK_UNLOCK_FAILED;
		return SD_ERROR;
	}

	/* Set block size for card if it is not equal to current block size for card */
	if (SD_BLOCK_SIZE != 512U) {
		// CMD16
		errorstate = SDMMC_CmdBlockLength(SDIOx, 512U);
		if (errorstate != SD_ERROR_NONE) {
			hsd->ErrorCode |= errorstate;
			return SD_ERROR;
		}
	}

	/* Send CMD55 */
	errorstate = SDMMC_CmdAppCommand(SDIOx, (u32)(hsd->Card.RelCardAdd << 16U));
	if (errorstate != SD_ERROR_NONE) {
		hsd->ErrorCode |= errorstate;
		return SD_ERROR;
	}

	/* Configure the SD DPSM (Data Path State Machine) */
	config.TransType = SDIO_TRANS_SINGLE_BLK;
	config.TransDir = SDIO_TRANS_CARD_TO_HOST;
	config.BlockSize = 512U;
	config.BlockCnt = 1U; // DON'T CARE
	config.AutoCmdEn = SDIO_TRANS_AUTO_DIS;
	config.DmaEn = SDIO_TRANS_DMA_DIS;
	SDIO_ConfigData(SDIOx, &config);

	/* Send ACMD13 (SD_APP_STAUS)  with argument as card's RCA */
	errorstate = SDMMC_CmdStatusRegister(SDIOx);
	if (errorstate != SD_ERROR_NONE) {
		hsd->ErrorCode |= errorstate;
		return SD_ERROR;
	}

	/* Get status data */
	/* Read STA register */
	while (!((SDIO_GetErrSts(SDIOx) & SDIO_DAT_ERR) || (SDIO_GetNormSts(SDIOx) & SDIOHOST_BIT_XFER_COMPLETE))) {
		if (SDIO_GetNormSts(SDIOx) & SDIOHOST_BIT_BUFF_READ_READY) {
			for (count = 0U; count < 8U; count++) {
				*pData = SDIO_ReadFIFO(SDIOx);
				pData++;
			}
		}

		if (is_timeout(tickstart, SDMMC_DAT_TIMEOUT)) {
			hsd->ErrorCode |= SD_ERROR_TIMEOUT;
			return SD_ERROR;
		}
	}

	if (SDIO_GetErrSts(SDIOx) & SDIOHOST_BIT_DATA_TIMEOUT_ERR) {
		hsd->ErrorCode |= SD_ERROR_DATA_TIMEOUT;
		return SD_ERROR;
	} else if (SDIO_GetErrSts(SDIOx) & SDIOHOST_BIT_DATA_CRC_ERR) {
		hsd->ErrorCode |= SD_ERROR_DATA_CRC_FAIL;
		return SD_ERROR;
	} else {
		/* Nothing to do */
	}

	while (SDIO_GetStatus(SDIOx) & SDIOHOST_BIT_BUFF_READ_EN) {
		*pData = SDIO_ReadFIFO(SDIOx);
		pData++;

		if (is_timeout(tickstart, SDMMC_DAT_TIMEOUT)) {
			hsd->ErrorCode |= SD_ERROR_TIMEOUT;
			return SD_ERROR;
		}
	}

	/* Clear all the static status flags*/
	SDIO_ClearErrSts(SDIOx, SDIO_DAT_ERR);

	return SD_OK;
}

/**
 * @brief  Returns the current card's status.
 * @param  hsd: Pointer to SD handle
 * @param  pCardStatus: pointer to the buffer that will contain the SD card
 *         status (Card Status register)
 * @retval error state
 */
static u32 SD_SendCardStatus(SD_HdlTypeDef *hsd, u32 *pCardStatus)
{
	SDIOHOST_TypeDef *SDIOx = hsd->Instance;
	u32 errorstate;

	if (pCardStatus == NULL) {
		return SD_ERROR_INVALID_PARAMETER;
	}

	/* Send Status command */
	/* CMD13 */
	errorstate = SDMMC_CmdSendStatus(SDIOx, (u32)(hsd->Card.RelCardAdd << 16U));
	if (errorstate != SD_ERROR_NONE) {
		return errorstate;
	}

	/* Get SD card status */
	*pCardStatus = SDIO_GetResponse(SDIOx, SDIO_RESP0);

	return SD_ERROR_NONE;
}

/**
  * @brief  Check switchable function(mode 0) or switch card function(mode 1).
  * @param  mode Operation mode, which can be a value of @ref SD_CMD6_OpMode.
  * @param  grp Function group, which can be a value of @ref SD_CMD6_FuncGroup.
  * @param  func Function, which can be a value of 0x0-0xF.
  * @param  pData pointer to a buffer to save the switch function status, which should be 32byte aligned.
  * @retval  SD_OK or SD_ERROR.
  */
SD_RESULT SD_SwitchFunction(SD_HdlTypeDef *hsd, u8 mode, u8 grp, u8 func, u8 *pData)
{
	SDIOHOST_TypeDef *SDIOx = hsd->Instance;
	SDIO_DataInitTypeDef config;
	u32 errorstate;
	u32 status;
	u32 arg = 0xFFFFFF;
	u32 grp_shift = (grp - 1) << 2;

	assert_param(((u32)pData & 0x1F) == 0); // cache_line(32) aligned

	/* check card spec version and card commmand class */
	if ((hsd->Card.CardSpecVer < SD_SPEC_V110) || ((hsd->Card.Class & SDMMC_CCC_SWITCH) == 0)) {
		RTK_LOGW(TAG, "This card doesn't support CMD6 and can't switch bus speed!\n");
		return SD_ERROR;
	}

	SDIO_ConfigNormIntSig(SDIOx, 0xFFFFFFFF, DISABLE); // disabLe alLl the normal int without err int
	SDIO_ClearNormSts(SDIOx, SDIO_GetNormSts(SDIOx)); // clear old flags

	config.TransType = SDIO_TRANS_SINGLE_BLK;
	config.TransDir = SDIO_TRANS_CARD_TO_HOST;
	config.BlockSize = 64U; // 512bit/8
	config.BlockCnt = 0x0; // DON'T CARE when TRANS_SINGLE_BLK
	config.AutoCmdEn = SDIO_TRANS_AUTO_DIS;
	config.DmaEn = SDIO_TRANS_DMA_EN;
	SDIO_ConfigData(SDIOx, &config);

	status = SDIO_ConfigDMA(SDIOx, SDIO_SDMA_MODE, (u32)pData);
	if (status != HAL_OK) {
		hsd->ErrorCode |= SD_ERROR_INVALID_PARAMETER;
		hsd->State = SD_STATE_READY;
		return SD_ERROR;
	}

	arg &= ~(u32)(0xF << grp_shift);
	arg |= func << grp_shift;
	arg |= mode << 31;

	SD_PreDMATrans(hsd);

	/* Send CMD6 */
	errorstate = SDMMC_CmdSwitch(SDIOx, arg);
	if (errorstate != SD_ERROR_NONE) {
		hsd->ErrorCode |= errorstate;
		hsd->State = SD_STATE_READY;
		return SD_ERROR;
	}

	if (SD_WaitTransDone(hsd, SDMMC_DAT_TIMEOUT) != HAL_OK) {
		RTK_LOGE(TAG, "%s WaitTransDone error!\n", __FUNCTION__);
		return SD_ERROR;
	}

	SDIO_ConfigNormIntSig(SDIOx, SDIOHOST_BIT_CARD_INT_SIGNAL_EN, ENABLE);

	DCache_Invalidate((u32)pData, 64); // 512bit/8

	return SD_OK;
}

SD_RESULT SD_SwitchBusSpeed(SD_HdlTypeDef *hsd, u8 BusSpeed)
{
	u8 cmd6_resp[64]__attribute__((aligned(32))); // 512bit/8
	SD_RESULT ret;

	/* Send CMD6 to check(mode0) whether target mode is supported  */
	ret = SD_SwitchFunction(hsd, SD_CMD6_CHECK_MODE, SD_ACCESS_MODE, SD_KEEP_CUR_SPEED, cmd6_resp); // check group1
	if (ret != SD_OK) {
		return ret;
	}

	if ((cmd6_resp[13] & BIT(BusSpeed)) == 0) { // Group1 information [407:400]
		RTK_LOGW(TAG, "This card does not support target speed mode(%d)!\n", BusSpeed);
		return SD_ERROR;
	}

	if ((cmd6_resp[16] & 0xF) == BusSpeed) { // Group1 [383:376]
		RTK_LOGI(TAG, "Current speed mode is already the target setting.\n");
	} else {
		/* Send CMD6 to check(mode0) whether target mode is ready  */
		ret = SD_SwitchFunction(hsd, SD_CMD6_CHECK_MODE, SD_ACCESS_MODE, BusSpeed, cmd6_resp); // check group1
		if (ret != SD_OK) {
			return ret;
		}

		if ((cmd6_resp[16] & 0xF) != BusSpeed) { // Group1 [383:376]
			RTK_LOGW(TAG, "The target speed mode(%d) cannot be switched!\n", BusSpeed);
			return SD_ERROR;
		} else {
			/* Send CMD6 to switch(mode1) access_mode to target mode  */
			ret = SD_SwitchFunction(hsd, SD_CMD6_SWITCH_MODE, SD_ACCESS_MODE, BusSpeed, cmd6_resp); // switch group1
			if (ret != SD_OK) {
				return ret;
			}

			if ((cmd6_resp[16] & 0xF) == BusSpeed) { // Group1 [383:376]
				RTK_LOGI(TAG, "SD card has been changed to target speed mode(%d).\n", BusSpeed);
				return SD_OK;
			} else {
				RTK_LOGW(TAG, "The switch request is cancelled!\n");
				return SD_ERROR;
			}
		}
	}

	return SD_OK;
}

SD_RESULT SD_ConfigBusSpeed(SD_HdlTypeDef *hsd, u8 BusSpeed)
{
	SDIOHOST_TypeDef *SDIOx = hsd->Instance;
	SD_RESULT ret;
	u32 status;
	u8 val;

	/* Check the parameters */
	assert_param(IS_SD_BUS_SPEED(BusSpeed));

	if (BusSpeed == SD_SPEED_HS) {
		if (hsd->Card.CardType == CARD_SDIO_ONLY) {
			ret = SD_IO_RW_Direct(hsd, BUS_READ, SDIO_FUNC0, SDIOD_CCCR_SPEED_CONTROL, NULL, &val);
			if (ret != SD_OK) {
				return ret;
			}

			if ((val & SDIO_SPEED_SHS) == 0) {
				RTK_LOGE(TAG, "SHS is 0! High speed is not supported!\n");
				hsd->ErrorCode |= SD_ERROR_UNSUPPORTED_FEATURE;
				return SD_ERROR;
			}

			ret = SD_IO_RW_Direct(hsd, BUS_WRITE, SDIO_FUNC0, SDIOD_CCCR_SPEED_CONTROL, SDIO_SPEED_EHS, &val);
			if (ret != SD_OK) {
				return ret;
			}
		} else {
			ret = SD_SwitchBusSpeed(hsd, SD_SPEED_HS);
			if (ret != SD_OK) {
				return ret;
			}
		}

		status = SDIO_ConfigClock(SDIOx, SD_TRANS_HS_50MHZ);
		if (status != HAL_OK) {
			hsd->ErrorCode |= SD_ERROR_INVALID_PARAMETER;
			return SD_ERROR;
		}
	} else if (BusSpeed == SD_SPEED_DS) {
		if (hsd->Card.CardType == CARD_SDIO_ONLY) {
			ret = SD_IO_RW_Direct(hsd, BUS_WRITE, SDIO_FUNC0, SDIOD_CCCR_SPEED_CONTROL, 0x0, &val);
			if (ret != SD_OK) {
				return ret;
			}
		} else {
			ret = SD_SwitchBusSpeed(hsd, SD_SPEED_DS);
			if (ret != SD_OK) {
				return ret;
			}
		}

		status = SDIO_ConfigClock(SDIOx, SD_TRANS_DS_25MHZ);
		if (status != SD_ERROR_NONE) {
			hsd->ErrorCode |= SD_ERROR_INVALID_PARAMETER;
			return SD_ERROR;
		}
	} else {
		hsd->ErrorCode |= SD_ERROR_INVALID_PARAMETER;
		return SD_ERROR;
	}

	/* Change State */
	hsd->State = SD_STATE_READY;

	return SD_OK;
}

/**
 * @brief  Enable wide bus for the requested card if supported by card.
 * @param  hsd: Pointer to SD handle
 * @param  WideMode: Specifies the SD card wide bus mode
 *          This parameter can be one of the following values:
 *            @arg SDIOH_BUS_WIDTH_8BIT: 8-bit data transfer
 *            @arg SDIOH_BUS_WIDTH_4BIT: 4-bit data transfer
 *            @arg SDIOH_BUS_WIDTH_1BIT: 1-bit data transfer
 * @retval HAL status
 */
SD_RESULT SD_ConfigBusWidth(SD_HdlTypeDef *hsd, u8 WideMode)
{
	SDIOHOST_TypeDef *SDIOx = hsd->Instance;
	u32 errorstate;

	/* Check the parameters */
	assert_param(IS_SD_BUS_WIDTH(WideMode));

	if (WideMode == SDIOH_BUS_WIDTH_8BIT) {
		hsd->ErrorCode |= SD_ERROR_UNSUPPORTED_FEATURE;
		return SD_ERROR;
	}

	/* Get SCR Register */
	if (hsd->Card.CardType != CARD_SDIO_ONLY) {
		errorstate = SD_GetCardSCR(hsd);
		if (errorstate != SD_ERROR_NONE) {
			return errorstate;
		}
	}

	/* Change State */
	if (WideMode == SDIOH_BUS_WIDTH_4BIT) {
		errorstate = SD_WideBus_Enable(hsd);
		if (errorstate != SD_ERROR_NONE) {
			hsd->ErrorCode |= errorstate;
			return SD_ERROR;
		}
	} else if (WideMode == SDIOH_BUS_WIDTH_1BIT) {
		errorstate = SD_WideBus_Disable(hsd);
		if (errorstate != SD_ERROR_NONE) {
			hsd->ErrorCode |= errorstate;
			return SD_ERROR;
		}
	} else {
		/* WideMode is not a valid argument*/
		hsd->ErrorCode |= SD_ERROR_INVALID_PARAMETER;
		return SD_ERROR;
	}

	/* Configure sdio host bus width. */
	errorstate = SDIO_ConfigBusWidth(SDIOx, WideMode);
	if (errorstate != SD_ERROR_NONE) {
		hsd->ErrorCode |= errorstate;
		return SD_ERROR;
	}

#ifdef AMEBAGREEN2_TODO
	/* kw: need to set block size again after setting bus width? */
	/* CMD16: Set Block Size for Card */
	errorstate = SDMMC_CmdBlockLength(SDIOx, SD_BLOCK_SIZE);
	if (errorstate != SD_ERROR_NONE) {
		hsd->ErrorCode |= errorstate;
		return SD_ERROR;
	}
#endif

	return SD_OK;
}

/**
 * @brief  Enables the SDIO wide bus mode.
 * @param  hsd: pointer to SD handle
 * @retval error state
 */
static u32 SD_WideBus_Enable(SD_HdlTypeDef *hsd)
{
	SDIOHOST_TypeDef *SDIOx = hsd->Instance;
	u32 errorstate;
	SD_RESULT ret;
	u8 val;

	if ((SDIO_GetResponse(SDIOx, SDIO_RESP0) & SDMMC_R1_CARD_IS_LOCKED) == SDMMC_R1_CARD_IS_LOCKED) {
		return SD_ERROR_LOCK_UNLOCK_FAILED;
	}

	if (hsd->Card.CardType == CARD_SDIO_ONLY) {

		ret = SD_IO_RW_Direct(hsd, BUS_READ, SDIO_FUNC0, SDIOD_CCCR_BICTRL, NULL, &val);
		if (ret != SD_OK) {
			return SD_ERROR;
		}

		val &= ~BUS_SD_DATA_WIDTH_MASK;
		val |= BUS_SD_DATA_WIDTH_4BIT;

		ret = SD_IO_RW_Direct(hsd, BUS_WRITE, SDIO_FUNC0, SDIOD_CCCR_BICTRL, val, NULL);
		if (ret != SD_OK) {
			return SD_ERROR;
		}
	} else {
		/* If requested card supports wide bus operation */
		if ((hsd->SCR[1] & SDMMC_WIDE_BUS_SUPPORT) != SDMMC_ALLZERO) {

			/* Send CMD55 APP_CMD with argument as card's RCA.*/
			errorstate = SDMMC_CmdAppCommand(SDIOx, (u32)(hsd->Card.RelCardAdd << 16U));
			if (errorstate != SD_ERROR_NONE) {
				return errorstate;
			}

			/* Send ACMD6 APP_CMD with argument as 2 for wide bus mode */
			errorstate = SDMMC_CmdBusWidth(SDIOx, 2U);
			if (errorstate != SD_ERROR_NONE) {
				return errorstate;
			}
		} else {
			return SD_ERROR_REQUEST_NOT_APPLICABLE;
		}
	}

	return SD_ERROR_NONE;
}

/**
 * @brief  Disables the SDIO wide bus mode.
 * @param  hsd: Pointer to SD handle
 * @retval error state
 */
static u32 SD_WideBus_Disable(SD_HdlTypeDef *hsd)
{
	SDIOHOST_TypeDef *SDIOx = hsd->Instance;
	u32 errorstate;
	SD_RESULT ret;
	u8 val;

	if ((SDIO_GetResponse(SDIOx, SDIO_RESP0) & SDMMC_R1_CARD_IS_LOCKED) == SDMMC_R1_CARD_IS_LOCKED) {
		return SD_ERROR_LOCK_UNLOCK_FAILED;
	}

	if (hsd->Card.CardType == CARD_SDIO_ONLY) {

		ret = SD_IO_RW_Direct(hsd, BUS_READ, SDIO_FUNC0, SDIOD_CCCR_BICTRL, NULL, &val);
		if (ret != SD_OK) {
			return SD_ERROR;
		}

		val &= ~BUS_SD_DATA_WIDTH_MASK;
		val |= BUS_SD_DATA_WIDTH_1BIT;

		ret = SD_IO_RW_Direct(hsd, BUS_WRITE, SDIO_FUNC0, SDIOD_CCCR_BICTRL, val, NULL);
		if (ret != SD_OK) {
			return SD_ERROR;
		}
	} else {
		/* If requested card supports 1 bit mode operation */
		if ((hsd->SCR[1] & SDMMC_SINGLE_BUS_SUPPORT) != SDMMC_ALLZERO) {
			/* Send CMD55 APP_CMD with argument as card's RCA */
			errorstate = SDMMC_CmdAppCommand(SDIOx, (u32)(hsd->Card.RelCardAdd << 16U));
			if (errorstate != SD_ERROR_NONE) {
				return errorstate;
			}

			/* Send ACMD6 APP_CMD with argument as 0 for single bus mode */
			errorstate = SDMMC_CmdBusWidth(SDIOx, 0U);
			if (errorstate != SD_ERROR_NONE) {
				return errorstate;
			}
		} else {
			return SD_ERROR_REQUEST_NOT_APPLICABLE;
		}
	}

	return SD_ERROR_NONE;
}

/**
 * @brief  Get the SD card SCR register value.
 * @param  hsd: Pointer to SD handle
 * @retval error state
 */
static u32 SD_GetCardSCR(SD_HdlTypeDef *hsd)
{
	SDIOHOST_TypeDef *SDIOx = hsd->Instance;
	SDIO_DataInitTypeDef config;
	u32 errorstate;
	u32 tickstart = DTimestamp_Get();
	u32 index = 0U;
	u32 tempscr[2U] = {0U, 0U};
	u32 dataremaining;

	// avoid crc_err_sts is set during ACMD51
	SDIO_ConfigErrIntSts(SDIOx, SDIOHOST_BIT_DATA_CRC_ERR_STATUS_EN, DISABLE);

	/* Send CMD55 APP_CMD with argument as card's RCA */
	errorstate = SDMMC_CmdAppCommand(SDIOx, (u32)((hsd->Card.RelCardAdd) << 16U));
	if (errorstate != SD_ERROR_NONE) {
		return errorstate;
	}

	config.TransType = SDIO_TRANS_SINGLE_BLK;
	config.TransDir = SDIO_TRANS_CARD_TO_HOST;
	config.BlockSize = 8U; // 64bit/8
	config.BlockCnt = 0U; // DON'T CARE when TRANS_SINGLE_BLK
	config.AutoCmdEn = SDIO_TRANS_AUTO_DIS;
	config.DmaEn = SDIO_TRANS_DMA_DIS;
	SDIO_ConfigData(SDIOx, &config);

	/* Send ACMD51 SD_APP_SEND_SCR with argument as 0 */
	errorstate = SDMMC_CmdSendSCR(SDIOx);
	if (errorstate != SD_ERROR_NONE) {
		return errorstate;
	}

	dataremaining = 8U; // size of SCR: 8Bytes

	/* SDIOHOST_BIT_XFER_COMPLETE will not be set under this case */
	while (!((SDIO_GetErrSts(SDIOx) & SDIO_DAT_ERR))) {

		if (dataremaining  == 0U) {
			break;
		}

		if ((SDIO_GetStatus(SDIOx) & SDIOHOST_BIT_BUFF_READ_EN) && (dataremaining > 0U)) {
			*(tempscr + index) = SDIO_ReadFIFO(SDIOx);
			index++;
			dataremaining -= 4U;
		}

		if (is_timeout(tickstart, SDMMC_DAT_TIMEOUT)) {
			return SD_ERROR_TIMEOUT;
		}
	}

	if (SDIO_GetErrSts(SDIOx) & SDIOHOST_BIT_DATA_TIMEOUT_ERR) {
		SDIO_ClearErrSts(SDIOx, SDIOHOST_BIT_DATA_TIMEOUT_ERR);
		return SD_ERROR_DATA_TIMEOUT;
	} else if (SDIO_GetErrSts(SDIOx) & SDIOHOST_BIT_DATA_CRC_ERR) {
		SDIO_ClearErrSts(SDIOx, SDIOHOST_BIT_DATA_CRC_ERR);
		return SD_ERROR_DATA_CRC_FAIL;
	} else {
		/* No error flag set */
		/* Clear all the static flags */
		SDIO_ClearErrSts(SDIOx, SDIO_DAT_ERR);

		hsd->SCR[0] = (((tempscr[1] & SDMMC_0TO7BITS) << 24) | ((tempscr[1] & SDMMC_8TO15BITS) << 8) |
					   ((tempscr[1] & SDMMC_16TO23BITS) >> 8) | ((tempscr[1] & SDMMC_24TO31BITS) >> 24));
		hsd->SCR[1] = (((tempscr[0] & SDMMC_0TO7BITS) << 24) | ((tempscr[0] & SDMMC_8TO15BITS) << 8) |
					   ((tempscr[0] & SDMMC_16TO23BITS) >> 8) | ((tempscr[0] & SDMMC_24TO31BITS) >> 24));
	}
	SDIOx->SDIOHOST_ERR_INT_STATUS_EN |= SDIOHOST_BIT_DATA_CRC_ERR_STATUS_EN;

	hsd->Card.CardSpecVer = SDMMC_GET_SPEC_VER(hsd->SCR[1]);

	return SD_ERROR_NONE;
}

/**
 * @brief  Wrap up reading in non-blocking mode.
 * @param  hsd: pointer to a SD_HdlTypeDef structure that contains
 *              the configuration information.
 * @retval None
 */
static void SD_Read_IT(SD_HdlTypeDef *hsd)
{
	SDIOHOST_TypeDef *SDIOx = hsd->Instance;
	u32 data, dataremaining;
	u8 *tmp;

	tmp = hsd->pRxBuffPtr;
	dataremaining = hsd->RxXferSize;

	while ((SDIO_GetStatus(SDIOx) & SDIOHOST_BIT_BUFF_READ_EN) && (dataremaining > 0U)) {
		/* Read data from SDIO Rx FIFO */
		data = SDIO_ReadFIFO(SDIOx);
		*tmp = (u8)(data & 0xFFU);
		tmp++;
		*tmp = (u8)((data >> 8U) & 0xFFU);
		tmp++;
		*tmp = (u8)((data >> 16U) & 0xFFU);
		tmp++;
		*tmp = (u8)((data >> 24U) & 0xFFU);
		tmp++;
		dataremaining -= 4U;
	}

	hsd->pRxBuffPtr = tmp;
	hsd->RxXferSize = dataremaining;

	if (hsd->RxXferSize == 0) {
		SDIO_ConfigNormIntSig(SDIOx, SDIOHOST_BIT_BUFF_READ_READY, DISABLE);
	}
}

/**
 * @brief  Wrap up writing in non-blocking mode.
 * @param  hsd: pointer to a SD_HdlTypeDef structure that contains
 *              the configuration information.
 * @retval None
 */
static void SD_Write_IT(SD_HdlTypeDef *hsd)
{
	SDIOHOST_TypeDef *SDIOx = hsd->Instance;
	u32 data, dataremaining;
	u8 *tmp;

	tmp = hsd->pTxBuffPtr;
	dataremaining = hsd->TxXferSize;

	while ((SDIO_GetStatus(SDIOx) & SDIOHOST_BIT_BUFF_WRITE_EN) && (dataremaining > 0U)) {
		/* Write data to SDIO Tx FIFO */
		data = (u32)(*tmp);
		tmp++;
		data |= ((u32)(*tmp) << 8U);
		tmp++;
		data |= ((u32)(*tmp) << 16U);
		tmp++;
		data |= ((u32)(*tmp) << 24U);
		tmp++;
		dataremaining -= 4U;
		SDIO_WriteFIFO(SDIOx, &data);
	}

	hsd->pTxBuffPtr = tmp;
	hsd->TxXferSize = dataremaining;

	if (hsd->TxXferSize == 0) {
		SDIO_ConfigNormIntSig(SDIOx, SDIOHOST_BIT_BUFF_WRITE_READY, DISABLE);
	}
}

/**
  *  @brief Sema stub function for sd transfer. If this funtcion is not called, polling mode will be used
  *         when waiting for transfer dma done.
  *  @param sema_take_fn: semaphore take function instance given by user.
  *         sema_give_isr_fn: semaphore give function instance given by user.
  *  @retval  None.
  */
void SD_SetSema(int (*sema_take_fn)(u32), int (*sema_give_isr_fn)(u32))
{
	sd_sema_take_fn = sema_take_fn;
	sd_sema_give_isr_fn = sema_give_isr_fn;
}
/**
  * @brief  Wait until transfer is done before timeout.
  * @param  hsd: pointer to a SD_HdlTypeDef structure that contains
  *              the configuration information.
  * @param  timeout_us: timeout value in microseconds.
  * @return wait status:
  * 		- HAL_TIMEOUT: Timeout.
  * 		- HAL_OK: Transfer is done within a specified time.
  */
u32 SD_WaitTransDone(SD_HdlTypeDef *hsd, u32 timeout_us)
{
	SDIOHOST_TypeDef *SDIOx = hsd->Instance;
	u32 context = hsd->Context;
	u32 errorstate;

	/* If scheduling has already started, wait for sema to obtain the trans done signal. */
	if (wait_for_sema == 1) {
		wait_for_sema = 0;
		if ((CPU_InInterrupt() == 0) && (rtos_sched_get_state() == RTOS_SCHED_RUNNING) && (sd_sema_take_fn != NULL)) {
			if (sd_sema_take_fn(MAX(timeout_us / 1000, SD_SEMA_MAX_DELAY)) != RTK_SUCCESS) {
				SDIO_ConfigNormIntSig(SDIOx, SDIOHOST_BIT_XFER_COMPLETE_SIGNAL_EN, DISABLE);
				RTK_LOGE(TAG, "SD Get Semaphore Timeout\n");
				return HAL_TIMEOUT;
			} else {
				SDIO_ConfigNormIntSig(SDIOx, SDIOHOST_BIT_XFER_COMPLETE_SIGNAL_EN, DISABLE);
				hsd->State = SD_STATE_READY;
				hsd->Context = SD_CONTEXT_NONE;
				return HAL_OK;
			}
		}
	}

	while (!(SDIO_GetNormSts(SDIOx) & SDIOHOST_BIT_XFER_COMPLETE)) {
		if (timeout_us-- == 0) {
			return HAL_TIMEOUT;
		}
		DelayUs(1);
	}
	SDIO_ClearNormSts(SDIOx, SDIOHOST_BIT_XFER_COMPLETE);

	if ((context & SD_CONTEXT_IT) != 0U || (context & SD_CONTEXT_DMA) != 0U) {
		if ((context & SD_CONTEXT_READ_MULTIPLE_BLOCK) || (context & SD_CONTEXT_WRITE_MULTIPLE_BLOCK)) {
			errorstate = SDMMC_CmdStopTransfer(SDIOx);
			if (errorstate != SD_ERROR_NONE) {
				hsd->ErrorCode |= errorstate;
			}
		}
	}

	hsd->State = SD_STATE_READY;
	hsd->Context = SD_CONTEXT_NONE;
	return HAL_OK;
}

SD_RESULT SD_Status(void)
{
	// TODO
	// return card_info.sd_status;
	return SD_OK;
}

SD_RESULT SD_ReadBlocks(u32 sector, u8 *data, u32 count)
{
	SD_RESULT ret = SD_ERROR;
	u32 i;

	if (SD_CheckStatusTO(&hsd0, SD_FATFS_TIMEOUT) < 0) {
		return ret; // SD card did not turn ready before timeout
	}

	if (!((u32)data & 0x1F) && !((count * SD_BLOCK_SIZE) & 0x1F)) { // 32Byte-aligned
		if (SD_ReadBlocks_DMA(&hsd0, (u8 *)data, (u32)(sector), count) == SD_OK) {
			if (SD_CheckStatusTO(&hsd0, SD_FATFS_TIMEOUT) >= 0) {
				DCache_Invalidate((u32)data, count * SD_BLOCK_SIZE);
				ret = SD_OK;
			}
		}
	} else {
		/* Slow path, fetch each sector a part and _memcpy to destination buffer */
		for (i = 0; i < count; i++) {
			if (SD_ReadBlocks_DMA(&hsd0, (u8 *)scratch, (u32)sector++, 1) == SD_OK) {
				if (SD_CheckStatusTO(&hsd0, SD_FATFS_TIMEOUT) >= 0) {
					/* invalidate the scratch buffer before the next read to get the actual data instead of the cached one */
					DCache_Invalidate((u32)scratch, SD_BLOCK_SIZE);
					_memcpy(data, scratch, SD_BLOCK_SIZE);
					data += SD_BLOCK_SIZE;
				} else {
					break;
				}
			} else {
				break;
			}
		}

		if (i == count) {
			ret = SD_OK;
		}
	}

	return ret;
}

SD_RESULT SD_WriteBlocks(u32 sector, const u8 *data, u32 count)
{
	SD_RESULT ret = SD_ERROR;
	u32 i;

	if (SD_CheckStatusTO(&hsd0, SD_FATFS_TIMEOUT) < 0) {
		return ret; // SD card did not turn ready before timeout
	}

	if (!((u32)data & 0x1F) && !((count * SD_BLOCK_SIZE) & 0x1F)) { // 32Byte-aligned
		/* the SCB_CleanDCache_by_Addr() requires a 32-Byte aligned address adjust the address and the D-Cache size to clean accordingly. */
		DCache_CleanInvalidate((u32)data, count * SD_BLOCK_SIZE);

		if (SD_WriteBlocks_DMA(&hsd0, (u8 *)data, (u32)sector, count) == SD_OK) {
			/* Wait that writing process is completed or a timeout occurs */

			if (SD_CheckStatusTO(&hsd0, SD_FATFS_TIMEOUT) >= 0) {
				ret = SD_OK;
			}
		}
	} else {
		/* Slow path, fetch each sector a part and _memcpy to destination buffer */
		for (i = 0; i < count; i++) {
			_memcpy((void *)scratch, (void *)data, SD_BLOCK_SIZE);
			data += SD_BLOCK_SIZE;

			DCache_CleanInvalidate((u32)scratch, SD_BLOCK_SIZE);

			if (SD_WriteBlocks_DMA(&hsd0, (u8 *)scratch, (u32)sector++, 1) == SD_OK) {
				/* Wait that writing process is completed or a timeout occurs */

				if (SD_CheckStatusTO(&hsd0, SD_FATFS_TIMEOUT) < 0) {
					break;
				}
			} else {
				break;
			}
		}

		if (i == count) {
			ret = SD_OK;
		}
	}

	return ret;
}

SD_RESULT SD_GetCapacity(u32 *sector_count)
{
	SDIO_CardInfoTypeDef CardInfo;

	SD_GetCardInfo(&hsd0, &CardInfo);
	*(u32 *)sector_count = CardInfo.LogBlockNbr;

	if (*sector_count) {
		return SD_OK;
	}
	return SD_ERROR;
}

SD_RESULT SD_GetSectorSize(u32 *sector_size)
{
	SDIO_CardInfoTypeDef CardInfo;

	SD_GetCardInfo(&hsd0, &CardInfo);
	*(u32 *)sector_size = CardInfo.LogBlockSize;

	if (*sector_size) {
		return SD_OK;
	}
	return SD_ERROR;
}

SD_RESULT SD_GetBlockSize(u32 *block_size)
{
	SDIO_CardInfoTypeDef CardInfo;

	SD_GetCardInfo(&hsd0, &CardInfo);
	*(u32 *)block_size = CardInfo.LogBlockSize / 512;

	if (*block_size) {
		return SD_OK;
	}
	return SD_ERROR;
}

SD_RESULT SD_IO_ReadBytes(u8 FuncNum, u32 Addr, u8 *pData, u16 ByteCnt)
{
	assert_param(ByteCnt <= SD_BLOCK_SIZE);

	if (!((u32)pData & 0x1F) && !(ByteCnt & 0x1F)) { // 32Byte-aligned

		if (SD_IO_RW_Extended(&hsd0, BUS_READ, FuncNum, 0x1, Addr, pData, 0, ByteCnt) == SD_OK) {
			DCache_Invalidate((u32)pData, ByteCnt);
			return SD_OK;
		}
	} else {
		if (SD_IO_RW_Extended(&hsd0, BUS_READ, FuncNum, 0x1, Addr, scratch, 0, ByteCnt) == SD_OK) {
			DCache_Invalidate((u32)scratch, ByteCnt);
			_memcpy(pData, scratch, ByteCnt);
			return SD_OK;
		}
	}

	return SD_ERROR;
}

SD_RESULT SD_IO_ReadBlocks(u8 FuncNum, u32 Addr, u8 *pData, u16 BlockCnt)
{
	u32 i;

	if (!((u32)pData & 0x1F) && !((BlockCnt * SD_BLOCK_SIZE) & 0x1F)) { // 32Byte-aligned
		if (SD_IO_RW_Extended(&hsd0, BUS_READ, FuncNum, 0x1, Addr, pData, 1, BlockCnt) == SD_OK) {
			DCache_Invalidate((u32)pData, BlockCnt * SD_BLOCK_SIZE);
			return SD_OK;
		}
	} else {
		/* Slow path, fetch each sector a part and _memcpy to destination buffer */
		for (i = 0; i < BlockCnt; i++) {
			if (SD_IO_RW_Extended(&hsd0, BUS_READ, FuncNum, 0x1, Addr, scratch, 1, 1) == SD_OK) {
				/* invalidate the scratch buffer before the next read to get the actual data instead of the cached one */
				DCache_Invalidate((u32)scratch, SD_BLOCK_SIZE);

				_memcpy(pData, scratch, SD_BLOCK_SIZE);
				pData += SD_BLOCK_SIZE;
			} else {
				break;
			}
		}

		if (i == BlockCnt) {
			return SD_OK;
		}
	}

	return SD_ERROR;
}

SD_RESULT SD_IO_WriteBytes(u8 FuncNum, u32 Addr, u8 *pData, u16 ByteCnt)
{
	assert_param(ByteCnt <= SD_BLOCK_SIZE);

	if (!((u32)pData & 0x1F) && !(ByteCnt & 0x1F)) { // 32Byte-aligned

		DCache_CleanInvalidate((u32)pData, ByteCnt);

		if (SD_IO_RW_Extended(&hsd0, BUS_WRITE, FuncNum, 0x1, Addr, pData, 0, ByteCnt) == SD_OK) {
			return SD_OK;
		}
	} else {
		_memcpy(scratch, pData, ByteCnt);
		DCache_CleanInvalidate((u32)scratch, ByteCnt);

		if (SD_IO_RW_Extended(&hsd0, BUS_WRITE, FuncNum, 0x1, Addr, scratch, 0, ByteCnt) == SD_OK) {
			return SD_OK;
		}
	}

	return SD_ERROR;
}

SD_RESULT SD_IO_WriteBlocks(u8 FuncNum, u32 Addr, u8 *pData, u16 BlockCnt)
{
	u32 i;

	if (!((u32)pData & 0x1F) && !((BlockCnt * SD_BLOCK_SIZE) & 0x1F)) { // 32Byte-aligned
		DCache_CleanInvalidate((u32)pData, BlockCnt * SD_BLOCK_SIZE);

		if (SD_IO_RW_Extended(&hsd0, BUS_WRITE, FuncNum, 0x1, Addr, pData, 1, BlockCnt) == SD_OK) {
			/* Wait that writing process is completed or a timeout occurs */
			return SD_OK;
		}
	} else {
		/* Slow path, fetch each sector a part and _memcpy to destination buffer */
		for (i = 0; i < BlockCnt; i++) {
			_memcpy(scratch, pData, SD_BLOCK_SIZE);
			pData += SD_BLOCK_SIZE;

			DCache_CleanInvalidate((u32)scratch, SD_BLOCK_SIZE);

			if (SD_IO_RW_Extended(&hsd0, BUS_WRITE, FuncNum, 0x1, Addr, scratch, 1, 1) != SD_OK) {
				break;
			}
		}

		if (i == BlockCnt) {
			return SD_OK;
		}
	}

	return SD_ERROR;
}
